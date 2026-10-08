// Object-owned line editing; file effects remain in the caller's transaction.
#define ED_MAX_BYTES 8192
#define ED_MAX_LINE_BYTES 2048
#define ED_MAX_LINES 1000
#define ED_MAX_COMMAND_BYTES 1024

private nosave string *ed_lines = ({});
private nosave int ed_cursor;
private nosave int ed_mode = -1;
private nosave string ed_filename = "";
private nosave int ed_dirty;
private nosave int ed_restricted;
private nosave int ed_number;
private nosave int ed_list;
private nosave int ed_auto_print;
private nosave int ed_shiftwidth = 4;
private nosave int ed_scroll = 20;
private nosave string *ed_help = ({});
private nosave int ed_help_pos;
private nosave mapping ed_marks = ([]);
private nosave string ed_pattern = "";
private nosave int ed_global;
private nosave int *ed_selected = ({});
private nosave string ed_scan = "";
private nosave int ed_scan_pos;

private void ed_fail(string message)
{
   throw("ed: " + message);
}

private int ed_bytes(string text)
{
   return sizeof(to_bytes(text, "UTF-8"));
}

private void ed_check_lines(string *lines)
{
   int total = 0;
   if (sizeof(lines) > ED_MAX_LINES)
      ed_fail("buffer exceeds 1000 lines");
   foreach (string line in lines)
   {
      int size = ed_bytes(line);
      if (size > ED_MAX_LINE_BYTES)
         ed_fail("line exceeds 2048 UTF-8 bytes");
      total += size + 1;
   }
   if (total > ED_MAX_BYTES)
      ed_fail("buffer exceeds 8192 UTF-8 bytes");
}

private string *ed_read(string path, int missing_ok)
{
   string text;
   string *lines;
   int size = efun::file_size(path);
   if (size == -1 && missing_ok)
   {
      int slash = strindex(path, "/", 1);
      string pattern = slash >= 0 ? path[0..slash] : "";
      // Escaping the entire leaf selects literal matching in get_dir's pattern mode.
      for (int index = slash + 1; index < sizeof(path); index++)
         pattern += "\\" + sprintf("%c", path[index]);
      if (!sizeof(efun::get_dir(pattern)))
         return ({});
      ed_fail("file exists but metadata is unavailable");
   }
   if (size == -2)
      ed_fail("filename is a directory");
   if (size > ED_MAX_BYTES)
      ed_fail("file exceeds 8192 UTF-8 bytes");
   text = efun::read_file(path);
   lines = sizeof(text) ? explode(text, "\n") : ({});
   if (sizeof(text) && text[<1] == '\n')
      lines = lines[0..<2];
   ed_check_lines(lines);
   return lines;
}

private string ed_text(int first, int last)
{
   if (first > last)
      return "";
   return implode(ed_lines[first-1..last-1], "\n") + "\n";
}

private void ed_replace(int first, int last, string *replacement)
{
   string *next = first > 1 ? ed_lines[0..first-2] : ({});
   int delta = sizeof(replacement) - (last - first + 1);
   next += replacement;
   if (last < sizeof(ed_lines))
      next += ed_lines[last..];
   ed_check_lines(next);
   foreach (string mark in keys(ed_marks))
   {
      int line = ed_marks[mark];
      if (line > last)
         ed_marks[mark] = line + delta;
      else if (line >= first)
         m_delete(ed_marks, mark);
   }
   ed_lines = next;
   if (ed_global)
   {
      int *pending = ({});
      foreach (int line in ed_selected)
         if (line < first)
            pending += ({ line });
         else if (line > last)
            pending += ({ line + delta });
      ed_selected = pending;
   }
   ed_cursor = first + sizeof(replacement) - 1;
   ed_dirty = 1;
}

private void ed_skip_space()
{
   while (ed_scan_pos < sizeof(ed_scan) &&
          (ed_scan[ed_scan_pos] == ' ' || ed_scan[ed_scan_pos] == '\t'))
      ed_scan_pos++;
}

private int ed_decimal()
{
   int value = 0;
   while (ed_scan_pos < sizeof(ed_scan) &&
          ed_scan[ed_scan_pos] >= '0' && ed_scan[ed_scan_pos] <= '9')
   {
      value = value * 10 + ed_scan[ed_scan_pos++] - '0';
      if (value > ED_MAX_LINES)
         ed_fail("line number out of range");
   }
   return value;
}

private string ed_search_pattern(int delimiter)
{
   string pattern = "";
   int closed;
   ed_scan_pos++;
   while (ed_scan_pos < sizeof(ed_scan))
   {
      int ch = ed_scan[ed_scan_pos++];
      if (ch == delimiter)
      {
         closed = 1;
         break;
      }
      if (ch == '\\' && ed_scan_pos < sizeof(ed_scan))
         pattern += "\\" + sprintf("%c", ed_scan[ed_scan_pos++]);
      else
         pattern += sprintf("%c", ch);
   }
   if (!closed)
      ed_fail("unterminated search address");
   if (!sizeof(pattern))
   {
      if (!sizeof(ed_pattern))
         ed_fail("no previous search pattern");
      pattern = ed_pattern;
   }
   ed_pattern = pattern;
   return pattern;
}

private int ed_search(string pattern, int base, int direction)
{
   int count = sizeof(ed_lines);
   for (int offset = 1; offset <= count; offset++)
   {
      int line = (base - 1 + direction * offset + count * 2) % count + 1;
      if (stringp(efun::regmatch(ed_lines[line-1], pattern)))
         return line;
   }
   ed_fail("search failed");
   return 0;
}

private int ed_address(int base)
{
   int value = -1;
   int ch;
   ed_skip_space();
   if (ed_scan_pos == sizeof(ed_scan))
      return -1;
   ch = ed_scan[ed_scan_pos];
   if (ch >= '0' && ch <= '9')
      value = ed_decimal();
   else if (ch == '.' || ch == '$')
   {
      value = ch == '.' ? base : sizeof(ed_lines);
      ed_scan_pos++;
   }
   else if (ch == '\'')
   {
      string mark;
      ed_scan_pos++;
      if (ed_scan_pos == sizeof(ed_scan))
         ed_fail("missing mark name");
      mark = sprintf("%c", ed_scan[ed_scan_pos++]);
      if (!has_key(ed_marks, mark))
         ed_fail("unknown mark");
      value = ed_marks[mark];
   }
   else if (ch == '/' || ch == '?')
      value = ed_search(ed_search_pattern(ch), base, ch == '/' ? 1 : -1);
   else if (ch == '+' || ch == '-')
      value = base;
   else
      return -1;
   while (1)
   {
      int direction;
      int offset;
      ed_skip_space();
      if (ed_scan_pos == sizeof(ed_scan))
         break;
      ch = ed_scan[ed_scan_pos];
      if (ch != '+' && ch != '-')
         break;
      direction = ch == '+' ? 1 : -1;
      ed_scan_pos++;
      ed_skip_space();
      offset = ed_scan_pos < sizeof(ed_scan) &&
               ed_scan[ed_scan_pos] >= '0' && ed_scan[ed_scan_pos] <= '9' ?
               ed_decimal() : 1;
      value += direction * offset;
   }
   if (value < 0 || value > sizeof(ed_lines))
      ed_fail("line number out of range");
   return value;
}

private int *ed_addresses(string command)
{
   int first;
   int last;
   int count;
   ed_scan = command;
   ed_scan_pos = 0;
   ed_skip_space();
   if (ed_scan_pos < sizeof(ed_scan) && ed_scan[ed_scan_pos] == '%')
   {
      ed_scan_pos++;
      return ({ 1, sizeof(ed_lines), 2 });
   }
   first = ed_address(ed_cursor);
   if (first >= 0)
   {
      last = first;
      count = 1;
   }
   else
      first = last = ed_cursor;
   ed_skip_space();
   if (ed_scan_pos < sizeof(ed_scan) &&
       (ed_scan[ed_scan_pos] == ',' || ed_scan[ed_scan_pos] == ';'))
   {
      int separator = ed_scan[ed_scan_pos++];
      if (!count)
         first = separator == ',' ? 1 : ed_cursor;
      if (separator == ';')
         ed_cursor = first;
      last = ed_address(separator == ';' ? first : ed_cursor);
      if (last < 0)
         last = sizeof(ed_lines);
      count = 2;
   }
   return ({ first, last, count });
}

private void ed_range(int first, int last)
{
   if (first < 1 || last < first || last > sizeof(ed_lines))
      ed_fail("invalid line range");
}

private int ed_delimiter()
{
   int delimiter;
   ed_skip_space();
   if (ed_scan_pos == sizeof(ed_scan))
      ed_fail("missing pattern delimiter");
   delimiter = ed_scan[ed_scan_pos];
   if (delimiter < 33 || delimiter > 126 || delimiter == '\\' ||
       (delimiter >= '0' && delimiter <= '9') ||
       (delimiter >= 'a' && delimiter <= 'z') ||
       (delimiter >= 'A' && delimiter <= 'Z'))
      ed_fail("delimiter must be printable punctuation other than backslash");
   return delimiter;
}

private string ed_replacement(int delimiter)
{
   string replacement = "";
   while (ed_scan_pos < sizeof(ed_scan))
   {
      int ch = ed_scan[ed_scan_pos++];
      if (ch == delimiter)
         return replacement;
      if (ch == '\\')
      {
         if (ed_scan_pos == sizeof(ed_scan)) ed_fail("unterminated replacement");
         replacement += "\\";
         ch = ed_scan[ed_scan_pos++];
         if (ch != delimiter && strindex("\\&123456789nrtb", sprintf("%c", ch)) < 0)
            ed_fail("unsupported replacement escape");
      }
      replacement += sprintf("%c", ch);
   }
   ed_fail("unterminated replacement");
   return "";
}

private string ed_output(string output, string addition)
{
   if (ed_bytes(output) + ed_bytes(addition) > ED_MAX_BYTES)
      ed_fail("output exceeds 8192 UTF-8 bytes; use a smaller range");
   return output + addition;
}

private string ed_expand(string replacement, mixed *captures, int delimiter)
{
   string output = "";
   for (int index = 0; index < sizeof(replacement); index++)
   {
      int ch = replacement[index];
      string addition;
      if (ch == '&')
         addition = captures[0];
      else if (ch == '\\')
      {
         ch = replacement[++index];
         if (ch == delimiter)
            addition = sprintf("%c", ch);
         else if (ch >= '1' && ch <= '9')
         {
            int group = ch - '0';
            if (group >= sizeof(captures) - 1) ed_fail("replacement capture does not exist");
            addition = stringp(captures[group]) ? captures[group] : "";
         }
         else if (ch == 'n') addition = "\n";
         else if (ch == 'r') addition = "\r";
         else if (ch == 't') addition = "\t";
         else if (ch == 'b') addition = sprintf("%c", 8);
         else addition = sprintf("%c", ch);
      }
      else
         addition = sprintf("%c", ch);
      output = ed_output(output, addition);
   }
   return output;
}

private string ed_render(string text, int line, int list)
{
   if (list || ed_list)
   {
      text = replace_string(text, "\\", "\\\\");
      text = replace_string(text, "\t", "\\t");
      text = replace_string(text, "\r", "\\r");
      text = replace_string(text, sprintf("%c", 0), "\\000");
      text += "$";
   }
   if (ed_number) text = sprintf("%d\t%s", line, text);
   return text + "\n";
}

private string ed_substitute(int first, int last, int selected_only = 0)
{
   int delimiter = ed_delimiter();
   string pattern = ed_search_pattern(delimiter);
   string replacement = ed_replacement(delimiter);
   int all;
   int print;
   int changed;
   int total_bytes;
   int total_lines = first - 1 + sizeof(ed_lines) - last;
   int selected_index;
   int cursor = ed_cursor;
   string *next = first > 1 ? ed_lines[0..first-2] : ({});
   mapping positions = ([]);
   mapping pending_positions = ([]);
   string output = "";
   ed_skip_space();
   while (ed_scan_pos < sizeof(ed_scan))
   {
      int flag = ed_scan[ed_scan_pos++];
      if (flag == 'g' && !all) all = 1;
      else if (flag == 'p' && !print) print = 1;
      else ed_fail("unsupported or repeated substitution flag");
   }
   ed_range(first, last);
   efun::regmatch("", pattern, 0x1000);
   for (int line = 1; line <= sizeof(ed_lines); line++)
      if (line < first || line > last) total_bytes += ed_bytes(ed_lines[line-1]) + 1;
   for (int original = first; original <= last; original++)
   {
      int line = sizeof(next) + 1;
      int selected = !selected_only ||
                     (selected_index < sizeof(ed_selected) && ed_selected[selected_index] == original);
      string text = ed_lines[original-1];
      string result = "";
      int position;
      int start;
      int matches;
      int previous_nonempty;
      positions[original] = line;
      if (selected_only && selected)
      {
         selected_index++;
         cursor = line;
      }
      while (selected && start <= sizeof(text))
      {
         mixed found = efun::regmatch(text, pattern, 0x1000, start);
         mixed *captures;
         int end;
         int begin;
         if (!arrayp(found)) break;
         captures = found;
         end = captures[<1] - (sizeof(captures[0]) == 0 ? 1 : 0);
         begin = end - sizeof(captures[0]);
         start = captures[<1];
         // An empty match at the end of a nonempty match must not replace it twice.
         if (!sizeof(captures[0]) && matches && previous_nonempty && end == position)
            continue;
         if (begin > position)
            result = ed_output(result, text[position..begin-1]);
         result = ed_output(result, ed_expand(replacement, captures, delimiter));
         position = end;
         previous_nonempty = sizeof(captures[0]) != 0;
         matches++;
         if (!all) break;
      }
      if (!matches)
      {
         result = text;
         pending_positions[original] = line;
      }
      else if (position < sizeof(text)) result = ed_output(result, text[position..]);
      {
         string *lines = sizeof(result) ? explode(result, "\n") : ({ "" });
         ed_check_lines(lines);
         total_bytes += ed_bytes(result) + 1;
         total_lines += sizeof(lines);
         if (total_bytes > ED_MAX_BYTES) ed_fail("buffer exceeds 8192 UTF-8 bytes");
         if (total_lines > ED_MAX_LINES) ed_fail("buffer exceeds 1000 lines");
         next += lines;
         if (matches)
         {
            cursor = sizeof(next);
            if (print)
               foreach (string item in lines) output = ed_output(output, ed_render(item, line++, 0));
         }
      }
      if (matches) changed++;
   }
   if (!changed && !ed_global) ed_fail("pattern not found");
   if (last < sizeof(ed_lines)) next += ed_lines[last..];
   if (!print && changed && ed_auto_print && !ed_global && cursor > 0)
      output = ed_output("", ed_render(next[cursor-1], cursor, 0));
   {
      int delta = sizeof(next) - sizeof(ed_lines);
      foreach (string mark in keys(ed_marks))
      {
         int line = ed_marks[mark];
         if (line >= first && line <= last) ed_marks[mark] = positions[line];
         else if (line > last) ed_marks[mark] = line + delta;
      }
      if (ed_global)
      {
         int *pending = ({});
         if (!selected_only)
            foreach (int line in ed_selected)
               if (line < first) pending += ({ line });
               else if (line > last) pending += ({ line + delta });
               else if (has_key(pending_positions, line)) pending += ({ pending_positions[line] });
         ed_selected = pending;
      }
   }
   ed_lines = next;
   ed_cursor = cursor;
   if (changed) ed_dirty = 1;
   return output;
}

private void ed_delete_selected()
{
   string *next = allocate(sizeof(ed_lines));
   mapping positions = ([]);
   int index;
   int kept;
   int cursor = ed_selected[<1] - sizeof(ed_selected) + 1;
   for (int line = 1; line <= sizeof(ed_lines); line++)
      if (index < sizeof(ed_selected) && ed_selected[index] == line)
         index++;
      else
      {
         next[kept++] = ed_lines[line-1];
         positions[line] = kept;
      }
   foreach (string mark in keys(ed_marks))
      if (has_key(positions, ed_marks[mark])) ed_marks[mark] = positions[ed_marks[mark]];
      else m_delete(ed_marks, mark);
   ed_lines = kept ? next[0..kept-1] : ({});
   ed_cursor = cursor < kept ? cursor : kept;
   ed_dirty = 1;
   ed_selected = ({});
}

private string ed_select(int first, int last, int inverse)
{
   int delimiter = ed_delimiter();
   string pattern = ed_search_pattern(delimiter);
   string command = efun::trim(ed_scan_pos < sizeof(ed_scan) ? ed_scan[ed_scan_pos..] : "");
   string output = "";
   ed_range(first, last);
   efun::regmatch("", pattern);
   ed_selected = ({});
   for (int line = first; line <= last; line++)
      if (stringp(efun::regmatch(ed_lines[line-1], pattern)) != inverse)
         ed_selected += ({ line });
   if (!sizeof(command)) command = "p";
   ed_global = 1;
   if (sizeof(ed_selected) && command == "d")
      ed_delete_selected();
   else if (sizeof(ed_selected) && command[0] == 's')
   {
      ed_scan = command;
      ed_scan_pos = 1;
      output = ed_substitute(first, last, 1);
   }
   // Pending line numbers follow edits; inserted lines never join the original selection.
   while (sizeof(ed_selected))
   {
      ed_cursor = ed_selected[0];
      ed_selected = ed_selected[1..];
      output = ed_output(output, ed_execute(command));
   }
   ed_global = 0;
   return output;
}

private string ed_print(int first, int last, int list)
{
   string output = "";
   ed_range(first, last);
   for (int line = first; line <= last; line++)
   {
      output = ed_output(output, ed_render(ed_lines[line-1], line, list));
   }
   ed_cursor = last;
   return output;
}

private string ed_after_change()
{
   return ed_auto_print && !ed_global && ed_cursor > 0 ? ed_print(ed_cursor, ed_cursor, 0) : "";
}

private mapping ed_indent_lex()
{
   string text = implode(ed_lines, "\n");
   string pattern = "[ \\t\\r\\n]+|//[^\\n]*|(?s:/\\*.*?\\*/)|\"(?:\\\\[\\s\\S]|[^\"\\\\])*\"|'''|'(?:\\\\.|[^'])'|[\\p{Alphabetic}_]\\w*|::|(?s:.)";
   string *tokens = allocate(sizeof(text) + 1);
   int *rows = allocate(sizeof(text) + 1);
   int *prefix = allocate(sizeof(ed_lines));
   int *keep = allocate(sizeof(ed_lines));
   int *columns = allocate(sizeof(ed_lines), -1);
   int count;
   int pos;
   int row;
   int col;
   for (int line = 0; line < sizeof(ed_lines); line++)
      prefix[line] = sizeof(regmatch(ed_lines[line], "^[ \\t]*"));
   while (pos < sizeof(text))
   {
      mixed *match;
      string piece;
      string token;
      string *parts;
      int start_row = row;
      int extra_semicolon;
      if (text[pos] == '#' && col == prefix[row])
      {
         int last = row;
         while (sizeof(ed_lines[last]) && ed_lines[last][<1] == '\\')
         {
            if (++last == sizeof(ed_lines)) ed_fail("unfinished preprocessor continuation");
         }
         piece = ed_lines[row][col..];
         for (int line = row; line <= last; line++)
         {
            keep[line] = 1;
            if (line > row) piece += "\n" + ed_lines[line];
         }
         pos += sizeof(piece);
      }
      else if (text[pos] == '@' &&
               (!count || strindex("=(:,{[?;", tokens[count-1]) >= 0 || tokens[count-1] == "return") &&
               arrayp(match = regmatch(text, "@{1,2}([A-Za-z_][A-Za-z_0-9]*)[ \\t]*\\n", 0x1040, pos)))
      {
         string marker = match[1];
         int last = row + 1;
         while (last < sizeof(ed_lines) && ed_lines[last] != marker && ed_lines[last] != marker + ";") last++;
         if (last == sizeof(ed_lines)) ed_fail("unterminated text block");
         piece = ed_lines[row][col..];
         for (int line = row + 1; line <= last; line++)
         {
            keep[line] = 1;
            piece += "\n" + ed_lines[line];
         }
         token = "<literal>";
         extra_semicolon = ed_lines[last] == marker + ";";
         pos += sizeof(piece);
      }
      else
      {
         match = regmatch(text, pattern, 0x1040, pos);
         if (!arrayp(match)) ed_fail("unable to tokenize indentation input");
         piece = match[0];
         pos = match[<1];
         if (piece == "\"" || piece == "'" ||
             (piece == "/" && pos < sizeof(text) && text[pos] == '*'))
            ed_fail("unterminated string, character or comment");
         if (piece[0] != ' ' && piece[0] != '\t' && piece[0] != '\r' && piece[0] != '\n') token = piece;
      }
      parts = explode(piece, "\n");
      if (sizeof(parts) > 1)
      {
         row += sizeof(parts) - 1;
         col = sizeof(parts[<1]);
         // Continuation-line whitespace can be part of a literal or comment payload.
         if (sizeof(token) && (token[0] == '"' || strindex(token, "/*") == 0))
            for (int line = start_row + 1; line <= row; line++) keep[line] = 1;
      }
      else col += sizeof(piece);
      if (sizeof(token))
      {
         tokens[count] = token;
         rows[count++] = start_row;
         if (extra_semicolon)
         {
            tokens[count] = ";";
            rows[count++] = row;
         }
      }
   }
   return ([ "tokens":count ? tokens[0..count-1] : ({}),
             "rows":count ? rows[0..count-1] : ({}), "prefix":prefix,
             "keep":keep, "columns":columns, "pos":0, "nest":0 ]);
}

private string ed_indent_peek(mapping work)
{
   return work["pos"] < sizeof(work["tokens"]) ? work["tokens"][work["pos"]] : "";
}

private void ed_indent_take(mapping work, int column)
{
   int row = work["rows"][work["pos"]];
   if (work["columns"][row] < 0) work["columns"][row] = column;
   work["pos"]++;
}

private void ed_indent_comments(mapping work, int column)
{
   string token = ed_indent_peek(work);
   while (strindex(token, "//") == 0 || strindex(token, "/*") == 0)
   {
      ed_indent_take(work, column);
      token = ed_indent_peek(work);
   }
}

private void ed_indent_enter(mapping work)
{
   if (++work["nest"] > 64) ed_fail("indentation nesting exceeds 64");
}

private string ed_indent_close(string token)
{
   switch (token)
   {
   case "(": return ")";
   case "[": return "]";
   }
   if (token == "({") return "})";
   if (token == "([") return "])";
   if (token == "(:") return ":)";
   return "";
}

private int ed_indent_closer(string token)
{
   return token == ")" || token == "]" || token == "}" ||
          token == "})" || token == "])" || token == ":)";
}

private void ed_indent_group(mapping work, int column)
{
   string close = ed_indent_close(ed_indent_peek(work));
   int inner = column + (ed_shiftwidth > 1 ? ed_shiftwidth / 2 : 1);
   ed_indent_enter(work);
   ed_indent_take(work, column);
   ed_indent_comments(work, inner);
   if (close == ")" && (ed_indent_peek(work) == "{" || ed_indent_peek(work) == "[" || ed_indent_peek(work) == ":"))
   {
      close = ed_indent_peek(work) == "{" ? "})" : ed_indent_peek(work) == "[" ? "])" : ":)";
      ed_indent_take(work, column);
   }
   if (close == ":)")
   {
      ed_indent_comments(work, inner);
      if (ed_indent_peek(work) == "[") ed_indent_group(work, inner);
      for (;;)
      {
         ed_indent_comments(work, inner);
         if (ed_indent_peek(work) == ":")
         {
            ed_indent_take(work, column);
            ed_indent_comments(work, column);
            if (ed_indent_peek(work) != ")") ed_fail("mismatched closure delimiter");
            ed_indent_take(work, column);
            work["nest"]--;
            return;
         }
         if (!sizeof(ed_indent_peek(work))) ed_fail("unclosed closure delimiter");
         ed_indent_statement(work, inner, 0, ":");
      }
   }
   for (;;)
   {
      string token;
      ed_indent_comments(work, inner);
      token = ed_indent_peek(work);
      if (!sizeof(token)) ed_fail("unclosed indentation delimiter");
      if (token == close || (sizeof(close) == 2 && token == close[0..0]))
      {
         ed_indent_take(work, column);
         if (sizeof(close) == 2)
         {
            ed_indent_comments(work, column);
            if (ed_indent_peek(work) != ")") ed_fail("mismatched literal delimiter");
            ed_indent_take(work, column);
         }
         work["nest"]--;
         return;
      }
      if (ed_indent_closer(token)) ed_fail("mismatched indentation delimiter");
      if (sizeof(ed_indent_close(token))) ed_indent_group(work, inner);
      else if (token == "{") ed_indent_block(work, inner, 0);
      else ed_indent_take(work, inner);
   }
}

private void ed_indent_block(mapping work, int column, int labels)
{
   ed_indent_enter(work);
   ed_indent_take(work, column);
   for (;;)
   {
      string token;
      ed_indent_comments(work, column + ed_shiftwidth);
      token = ed_indent_peek(work);
      if (!sizeof(token)) ed_fail("unclosed indentation block");
      if (token == "}")
      {
         ed_indent_take(work, column);
         work["nest"]--;
         return;
      }
      ed_indent_statement(work, column + ed_shiftwidth, labels);
   }
}

private void ed_indent_body(mapping work, int column, int labels, string end)
{
   string token;
   int first_row;
   int continuation = column + (ed_shiftwidth > 1 ? ed_shiftwidth / 2 : 1);
   ed_indent_comments(work, column);
   token = ed_indent_peek(work);
   if (!sizeof(token)) ed_fail("missing control-statement body");
   first_row = work["rows"][work["pos"]];
   if (token == "{")
   {
      ed_indent_block(work, column, 0);
      return;
   }
   if (token == "if" || token == "for" || token == "foreach" || token == "while" || token == "switch" || token == "do")
   {
      string control = token;
      ed_indent_take(work, column);
      if (control != "do")
      {
         ed_indent_comments(work, column);
         if (ed_indent_peek(work) != "(") ed_fail("control statement requires parentheses");
         ed_indent_group(work, column);
      }
      ed_indent_comments(work, column);
      if (ed_indent_peek(work) == "{") ed_indent_block(work, column, control == "switch");
      else
      {
         if (control == "switch") ed_fail("switch requires a block");
         ed_indent_statement(work, column + ed_shiftwidth, 0);
      }
      ed_indent_comments(work, column);
      if (control == "if" && ed_indent_peek(work) == "else")
      {
         ed_indent_take(work, column);
         ed_indent_comments(work, column);
         if (ed_indent_peek(work) == "if" || ed_indent_peek(work) == "{")
            ed_indent_statement(work, column, 0);
         else ed_indent_statement(work, column + ed_shiftwidth, 0);
      }
      if (control == "do")
      {
         if (ed_indent_peek(work) != "while") ed_fail("do requires a trailing while");
         ed_indent_take(work, column);
         ed_indent_comments(work, column);
         if (ed_indent_peek(work) != "(") ed_fail("do/while requires parentheses");
         ed_indent_group(work, column);
         ed_indent_comments(work, column);
         if (ed_indent_peek(work) != ";") ed_fail("do/while requires a semicolon");
         ed_indent_take(work, column);
      }
      return;
   }
   if (token == "case" || token == "default")
   {
      int ternary;
      if (!labels) ed_fail("case label outside switch");
      column -= ed_shiftwidth;
      ed_indent_take(work, column);
      for (;;)
      {
         ed_indent_comments(work, column);
         token = ed_indent_peek(work);
         if (!sizeof(token) || ed_indent_closer(token)) ed_fail("unfinished case label");
         if (token == ":" && !ternary)
         {
            ed_indent_take(work, column);
            return;
         }
         if (token == "?") ternary++;
         else if (token == ":") ternary--;
         if (sizeof(ed_indent_close(token))) ed_indent_group(work, column);
         else ed_indent_take(work, column);
      }
   }
   for (;;)
   {
      int target;
      ed_indent_comments(work, continuation);
      token = ed_indent_peek(work);
      if (!sizeof(token)) return;
      if (end == ":" && token == ":" && work["pos"] + 1 < sizeof(work["tokens"]) &&
          work["tokens"][work["pos"]+1] == ")") return;
      if (ed_indent_closer(token)) ed_fail("mismatched or unfinished indentation statement");
      if (token == "else" || token == "case" || token == "default") ed_fail("misplaced control keyword");
      target = work["rows"][work["pos"]] == first_row ? column : continuation;
      if (token == ";")
      {
         ed_indent_take(work, column);
         return;
      }
      if (sizeof(ed_indent_close(token))) ed_indent_group(work, target);
      else if (token == "{")
      {
         ed_indent_block(work, column, 0);
         ed_indent_comments(work, column);
         token = ed_indent_peek(work);
         if (token == ";") ed_indent_take(work, column);
         else if (sizeof(token) == 1 && strindex("([,+-*/%&|^<>=!?:.@", token) >= 0 && token != end) continue;
         return;
      }
      else ed_indent_take(work, target);
   }
}

private void ed_indent_statement(mapping work, int column, int labels, string end = "")
{
   ed_indent_enter(work);
   ed_indent_body(work, column, labels, end);
   work["nest"]--;
}

private string ed_indent()
{
   mapping work = ed_indent_lex();
   string *next = allocate(sizeof(ed_lines));
   int changed;
   while (sizeof(ed_indent_peek(work)))
   {
      ed_indent_comments(work, 0);
      if (sizeof(ed_indent_peek(work))) ed_indent_statement(work, 0, 0);
   }
   for (int row = 0; row < sizeof(ed_lines); row++)
   {
      int column = work["columns"][row];
      if (work["keep"][row]) next[row] = ed_lines[row];
      else if (work["prefix"][row] == sizeof(ed_lines[row])) next[row] = "";
      else
      {
         if (column < 0 || column > ED_MAX_LINE_BYTES) ed_fail("invalid indentation column");
         next[row] = " " * column + ed_lines[row][work["prefix"][row]..];
      }
      if (next[row] != ed_lines[row]) changed = 1;
   }
   ed_check_lines(next);
   ed_lines = next;
   if (changed) ed_dirty = 1;
   return sprintf("Indented %d lines.\n", sizeof(next));
}

private int ed_setup_code()
{
   // The legacy eightbit bit is always set because native strings retain Unicode.
   return ed_shiftwidth | 0x80 | (ed_number ? 0x10 : 0) |
          (ed_list ? 0x20 : 0) | (ed_auto_print ? 0x40 : 0);
}

private int ed_retrieve_setup()
{
   mixed code;
   if (!efun::function_exists("query_ed_setup", this_object())) return 0x84;
   code = this_object()->query_ed_setup();
   if (!intp(code) || code < 0 || code > 0xff)
      ed_fail("unsupported saved setup code");
   if (!code) return 0x84;
   if (!(code & 0x0f)) ed_fail("saved shiftwidth must be between 1 and 15");
   return code;
}

private string ed_settings(int addressed)
{
   string option;
   int begin;
   int kind;
   int value;
   if (addressed || ed_global) ed_fail("set takes no address and is not a global command");
   if (ed_scan_pos < sizeof(ed_scan) && ed_scan[ed_scan_pos] != ' ' && ed_scan[ed_scan_pos] != '\t')
      ed_fail("expected space after set");
   ed_skip_space();
   if (ed_scan_pos == sizeof(ed_scan))
      return sprintf("number:%s list:%s print:%s\nshiftwidth:%d scroll:%d\n",
                     ed_number ? "on" : "off", ed_list ? "on" : "off",
                     ed_auto_print ? "on" : "off", ed_shiftwidth, ed_scroll);
   begin = ed_scan_pos;
   while (ed_scan_pos < sizeof(ed_scan) && ed_scan[ed_scan_pos] != ' ' && ed_scan[ed_scan_pos] != '\t')
      ed_scan_pos++;
   option = ed_scan[begin..ed_scan_pos-1];
   ed_skip_space();
   if (option == "shiftwidth") kind = 1;
   else if (option == "scroll") kind = 2;
   if (kind)
   {
      if (ed_scan_pos == sizeof(ed_scan) || ed_scan[ed_scan_pos] < '0' || ed_scan[ed_scan_pos] > '9')
         ed_fail("option requires an integer");
      value = ed_decimal();
      ed_skip_space();
      if (ed_scan_pos != sizeof(ed_scan) || value < 1 || value > (kind == 1 ? 15 : 100))
         ed_fail("option value out of range");
      if (kind == 1) ed_shiftwidth = value;
      else ed_scroll = value;
      return "";
   }
   if (ed_scan_pos != sizeof(ed_scan)) ed_fail("unexpected option argument");
   if (option == "number") ed_number = 1;
   else if (option == "nonumber") ed_number = 0;
   else if (option == "list") ed_list = 1;
   else if (option == "nolist") ed_list = 0;
   else if (option == "print") ed_auto_print = 1;
   else if (option == "noprint") ed_auto_print = 0;
   else if (option == "save")
   {
      if (ed_restricted) ed_fail("setup saving is restricted");
      if (!efun::function_exists("query_ed_setup", this_object()) ||
          !efun::function_exists("set_ed_setup", this_object()))
         ed_fail("setup persistence is unavailable");
      this_object()->set_ed_setup(ed_setup_code());
      return "Setup handed to owner.\n";
   }
   else ed_fail("unsupported editor option");
   return "";
}

private string ed_help_page()
{
   string output = "";
   int end = ed_help_pos + ed_scroll;
   if (end > sizeof(ed_help)) end = sizeof(ed_help);
   while (ed_help_pos < end) output = ed_output(output, ed_help[ed_help_pos++] + "\n");
   if (ed_help_pos < sizeof(ed_help))
   {
      ed_mode = -2;
      output = ed_output(output, "--Return for more; q to leave help--\n");
   }
   else
   {
      ed_mode = 0;
      ed_help = ({});
      ed_help_pos = 0;
   }
   return output;
}

private string ed_show_help(string topic)
{
   if (!sizeof(topic))
      ed_help = ({
         "Line editor", "Addresses: numbers, ., $, +/-, marks 'a and /re/ or ?re?.",
         "Ranges: first,last or first;last; % selects the whole buffer.",
         "p prints; l lists controls; n toggles line numbers; = reports a count.",
         "a appends, i inserts, c changes; a lone . ends line input.",
         "d deletes; j joins; m moves; t copies; k sets a lowercase mark.",
         "g selects matching lines; v selects nonmatching lines.",
         "Global bodies support p, l, =, d and s; omitted bodies print.",
         "s/re/replacement/ replaces the first match on each addressed line.",
         "Substitution flags: g replaces all; p prints changed lines.",
         "Replacement: & is the full match; \\1 through \\9 are captures.",
         "Escaped delimiters are literal; empty patterns recall the last search.",
         "e opens; E forces open; r reads into the buffer; f selects a filename.",
         "w stages overwrite; W stages append; x stages overwrite and exits.",
         "q refuses modified buffers; Q discards the buffer and exits.",
         "z scrolls; set scroll changes the page size from 1 through 100.",
         "set number/nonumber, list/nolist and print/noprint change output.",
         "I indents the whole buffer; set shiftwidth chooses 1 through 15 spaces.",
         "set save forwards preferences to optional owner setup methods.",
         "Restricted editors cannot load other files or save preferences.",
         "Limits: 8192 UTF-8 buffer bytes, 1000 lines, 2048 bytes per line.",
         "Commands/filenames are capped at 1024 UTF-8 bytes; output at 8192.",
         "File saves acknowledge staging; commit-time delivery may still fail.",
         "h s, h g, h set, h files or h limits gives focused help.",
         "Return advances help; q leaves help without exiting the editor.",
         "Autoindent and legacy regex options are not implemented yet."
      });
   else if (topic == "s")
      ed_help = ({ "[range]s/re/replacement/[gp]", "g replaces all occurrences per line; p prints changed lines.",
                  "& is the full match; \\1 through \\9 are captures; \\& is literal.",
                  "\\n, \\r, \\t and \\b insert controls; \\n can split lines.",
                  "Patterns use native Rust regex; empty patterns recall the last search." });
   else if (topic == "g" || topic == "v")
      ed_help = ({ "[range]g/re/command or [range]v/re/command", "Select original matching or nonmatching lines once.",
                  "Bodies support p, l, =, d and s; inserted lines are not selected.",
                  "Nested globals and file/input/exit commands are refused." });
   else if (topic == "set")
      ed_help = ({ "set reports current editor options.", "set number/nonumber, list/nolist or print/noprint changes output.",
                  "set scroll 1..100 controls scrolling and help pages.", "set shiftwidth 1..15 controls LPC indentation.",
                  "set save uses owner query_ed_setup/set_ed_setup methods.", "I indents the whole buffer; autoindent is unavailable; Unicode is retained." });
   else if (topic == "I")
      ed_help = ({ "I indents the whole buffer without addresses or arguments.", "Blocks use shiftwidth spaces; continuations use half, with a minimum of one.",
                  "Only leading whitespace changes; multiline literal/directive text is retained.", "Malformed delimiters and excessive growth/nesting leave the buffer intact.",
                  "Cursor, marks and lines are preserved; changed indentation marks the buffer modified." });
   else if (topic == "files")
      ed_help = ({ "e/E open, r reads, f selects a filename.", "w stages overwrite; W stages append; x stages overwrite and exits.",
                  "Partial writes and append preserve the modified flag.", "Native file policy authorizes the owning object and module.",
                  "Staging does not acknowledge durable file delivery." });
   else if (topic == "limits")
      ed_help = ({ "8192 UTF-8 buffer bytes including LF per line; 1000 lines.", "2048 UTF-8 bytes per line; 1024 per command/filename.",
                  "Output is capped at 8192 bytes after numbering/listing expansion.", "Refused commands restore editor state without truncation." });
   else
      ed_fail("unknown help topic");
   ed_help_pos = 0;
   return ed_help_page();
}

private string ed_name(string argument)
{
   if (sizeof(argument))
   {
      if (ed_restricted)
         ed_fail("filename changes are restricted");
      return argument;
   }
   if (!sizeof(ed_filename))
      ed_fail("no filename");
   return ed_filename;
}

private void ed_quit()
{
   ed_lines = ({});
   ed_marks = ([]);
   ed_cursor = ed_dirty = ed_number = ed_restricted = 0;
   ed_list = ed_auto_print = ed_help_pos = 0;
   ed_help = ({});
   ed_shiftwidth = 4;
   ed_filename = ed_pattern = "";
   ed_global = 0;
   ed_selected = ({});
   ed_mode = -1;
}

private string ed_execute(string command)
{
   int *range = ed_addresses(command);
   int first = range[0];
   int last = range[1];
   int addressed = range[2];
   int operation;
   string argument;
   ed_skip_space();
   operation = ed_scan_pos < sizeof(command) ? command[ed_scan_pos++] : 0;
   argument = ed_scan_pos < sizeof(command) ? efun::trim(command[ed_scan_pos..]) : "";
   if (ed_global && operation != 0 && operation != 'p' && operation != 'l' &&
       operation != '=' && operation != 'd' && operation != 's')
      ed_fail("command is not supported inside global selection");
   if (!operation)
   {
      if (!addressed)
         first = last = ed_cursor + 1;
      return ed_print(first, last, 0);
   }
   switch (operation)
   {
   case 'I':
      if (addressed || sizeof(argument)) ed_fail("I takes no address or argument");
      return ed_indent();
   case 'h':
      if (addressed) ed_fail("help takes no address");
      return ed_show_help(argument);
   case 'g':
   case 'v':
      if (!addressed) { first = 1; last = sizeof(ed_lines); }
      return ed_select(first, last, operation == 'v');
   case 's':
      if (ed_scan_pos + 1 < sizeof(ed_scan) && ed_scan[ed_scan_pos] == 'e' && ed_scan[ed_scan_pos+1] == 't')
      {
         ed_scan_pos += 2;
         return ed_settings(addressed);
      }
      return ed_substitute(first, last);
   case 'p':
   case 'l':
      if (sizeof(argument)) ed_fail("unexpected command argument");
      return ed_print(first, last, operation == 'l');
   case '=':
      if (sizeof(argument)) ed_fail("unexpected command argument");
      return sprintf("%d\n", addressed ? last : sizeof(ed_lines));
   case 'n':
      if (addressed || sizeof(argument)) ed_fail("n takes no address or argument");
      ed_number = !ed_number;
      return "";
   case 'a':
   case 'i':
   case 'c':
      if (sizeof(argument)) ed_fail("unexpected command argument");
      if (operation == 'a' && addressed > 1) ed_fail("append takes one address");
      if (operation == 'i' && addressed > 1) ed_fail("insert takes one address");
      if (operation == 'a')
         ed_cursor = last;
      else if (operation == 'i')
      {
         if (first < 1 && sizeof(ed_lines)) ed_fail("invalid insertion address");
         ed_cursor = first > 0 ? first - 1 : 0;
      }
      else
      {
         ed_range(first, last);
         ed_replace(first, last, ({}));
      }
      ed_mode = ed_cursor + 1;
      return "";
   case 'd':
      if (sizeof(argument)) ed_fail("unexpected command argument");
      ed_range(first, last);
      ed_replace(first, last, ({}));
      ed_cursor = first < sizeof(ed_lines) ? first : sizeof(ed_lines);
      return ed_after_change();
   case 'j':
      if (sizeof(argument)) ed_fail("unexpected command argument");
      if (addressed < 2) last = first < sizeof(ed_lines) ? first + 1 : first;
      ed_range(first, last);
      if (first != last)
         ed_replace(first, last, ({ implode(ed_lines[first-1..last-1], "") }));
      ed_cursor = first;
      return ed_after_change();
   case 'k':
      if (addressed > 1 || sizeof(argument) != 1 || argument[0] < 'a' || argument[0] > 'z')
         ed_fail("mark requires one line and a lowercase letter");
      ed_range(last, last);
      ed_marks[argument] = last;
      return "";
   case 'm':
   case 't':
      {
         int destination;
         string *selected;
         mapping moved = ([]);
         ed_range(first, last);
         destination = ed_address(ed_cursor);
         ed_skip_space();
         if (destination < 0 || ed_scan_pos != sizeof(command))
            ed_fail("move/copy requires a destination address");
         if (operation == 'm' && destination >= first && destination <= last)
            ed_fail("cannot move lines into themselves");
         if (operation == 'm' && destination == first - 1)
            return "";
         selected = ed_lines[first-1..last-1];
         if (operation == 'm')
         {
            foreach (string mark in keys(ed_marks))
               if (ed_marks[mark] >= first && ed_marks[mark] <= last)
                  moved[mark] = ed_marks[mark] - first;
            ed_replace(first, last, ({}));
            if (destination > last) destination -= sizeof(selected);
         }
         ed_replace(destination+1, destination, selected);
         foreach (string mark in keys(moved))
            ed_marks[mark] = destination + 1 + moved[mark];
         return ed_after_change();
      }
   case 'e':
   case 'E':
      {
         string path;
         string *loaded;
         if (ed_restricted) ed_fail("file loading is restricted");
         if (addressed) ed_fail("edit takes no address");
         if (operation == 'e' && ed_dirty) ed_fail("buffer modified; use w or E");
         path = ed_name(argument);
         loaded = ed_read(path, 1);
         ed_lines = loaded;
         ed_filename = path;
         ed_cursor = sizeof(loaded) ? 1 : 0;
         ed_dirty = 0;
         ed_marks = ([]);
         return sizeof(loaded) ? sprintf("%d lines\n", sizeof(loaded)) : "[New or empty file]\n";
      }
   case 'r':
      {
         string *loaded;
         if (ed_restricted) ed_fail("file reading is restricted");
         if (addressed > 1) ed_fail("read takes one address");
         loaded = ed_read(ed_name(argument), 0);
         if (sizeof(loaded)) ed_replace(last+1, last, loaded);
         return sprintf("%d lines\n", sizeof(loaded));
      }
   case 'f':
      if (addressed) ed_fail("filename takes no address");
      if (sizeof(argument)) ed_filename = ed_name(argument);
      return ed_filename + "\n";
   case 'w':
   case 'W':
   case 'x':
      {
         string path;
         string text;
         string output;
         if (operation == 'x' && (addressed || sizeof(argument)))
            ed_fail("save-and-exit takes no address or argument");
         path = ed_name(argument);
         if (!addressed) { first = 1; last = sizeof(ed_lines); }
         if (sizeof(ed_lines) || addressed) ed_range(first, last);
         text = ed_text(first, last);
         output = sprintf("Staged %d lines to %s.\n", last-first+1, path);
         efun::write_file(path, text, operation == 'W' ? 0 : 1);
         if (!sizeof(ed_filename)) ed_filename = path;
         if (operation != 'W' && path == ed_filename && first == 1 && last == sizeof(ed_lines))
            ed_dirty = 0;
         if (operation == 'x') ed_quit();
         return output;
      }
   case 'q':
   case 'Q':
      if (addressed || sizeof(argument)) ed_fail("quit takes no address or argument");
      if (operation == 'q' && ed_dirty) ed_fail("buffer modified; use w or Q");
      ed_quit();
      return "";
   case 'z':
      if (addressed > 1 || sizeof(argument)) ed_fail("scroll takes at most one address");
      first = addressed ? first : ed_cursor + 1;
      last = first + ed_scroll - 1;
      if (last > sizeof(ed_lines)) last = sizeof(ed_lines);
      return ed_print(first, last, 0);
   default:
      ed_fail("unsupported command");
   }
   return "";
}

//: FUNCTION ed_start
// Start an owner-local session; omitted filenames create an unnamed empty buffer.
protected string ed_start(string path = "", int restricted = 0, int scroll = 20)
{
   string *loaded;
   int setup;
   if (ed_mode != -1) ed_fail("editor already active");
   if (!stringp(path)) ed_fail("filename must be a string");
   if (restricted != 0 && restricted != 1) ed_fail("restricted must be 0 or 1");
   if (scroll < 1 || scroll > 100) ed_fail("scroll must be between 1 and 100");
   if (ed_bytes(path) > ED_MAX_COMMAND_BYTES) ed_fail("filename too long");
   loaded = sizeof(path) ? ed_read(path, 1) : ({});
   setup = ed_retrieve_setup();
   ed_lines = loaded;
   ed_filename = path;
   ed_marks = ([]);
   ed_pattern = "";
   ed_global = 0;
   ed_selected = ({});
   ed_cursor = sizeof(loaded) ? 1 : 0;
   ed_mode = ed_dirty = ed_help_pos = 0;
   ed_help = ({});
   ed_number = (setup & 0x10) != 0;
   ed_list = (setup & 0x20) != 0;
   ed_auto_print = (setup & 0x40) != 0;
   ed_shiftwidth = setup & 0x0f;
   ed_restricted = restricted;
   ed_scroll = scroll;
   return sizeof(loaded) ? sprintf("%d lines\n", sizeof(loaded)) : "[New or empty file]\n";
}

//: FUNCTION query_ed_mode
// Return -2 during paged help, -1 when inactive, 0 for commands, or the next input line.
protected int query_ed_mode()
{
   return ed_mode;
}

//: FUNCTION ed_cmd
// Return command output or a diagnostic, retaining the buffer after a refused command.
protected string ed_cmd(string command)
{
   string *saved_lines = ed_lines;
   mapping saved_marks = ed_marks;
   int saved_cursor = ed_cursor;
   int saved_mode = ed_mode;
   int saved_dirty = ed_dirty;
   int saved_number = ed_number;
   int saved_list = ed_list;
   int saved_print = ed_auto_print;
   int saved_shiftwidth = ed_shiftwidth;
   int saved_scroll = ed_scroll;
   string *saved_help = ed_help;
   int saved_help_pos = ed_help_pos;
   string saved_filename = ed_filename;
   string saved_pattern = ed_pattern;
   int saved_global = ed_global;
   int *saved_selected = ed_selected;
   mixed failure;
   string output = "";
   if (ed_mode == -1) ed_fail("editor is inactive");
   ed_marks = ed_marks + ([]);
   failure = catch {
      if (strindex(command, "\n") >= 0) ed_fail("input must be one line");
      if (ed_mode == -2)
      {
         if (ed_bytes(command) > ED_MAX_COMMAND_BYTES) ed_fail("command exceeds 1024 UTF-8 bytes");
         if (!sizeof(command)) output = ed_help_page();
         else if (command == "q")
         {
            ed_help = ({});
            ed_help_pos = 0;
            ed_mode = 0;
         }
         else ed_fail("Return advances help; q leaves help");
      }
      else if (ed_mode > 0)
      {
         if (command == ".")
         {
            ed_mode = 0;
            output = ed_after_change();
         }
         else
         {
            if (ed_bytes(command) > ED_MAX_LINE_BYTES) ed_fail("line exceeds 2048 UTF-8 bytes");
            ed_replace(ed_cursor+1, ed_cursor, ({ command }));
            ed_mode = ed_cursor + 1;
         }
      }
      else
      {
         if (ed_bytes(command) > ED_MAX_COMMAND_BYTES) ed_fail("command exceeds 1024 UTF-8 bytes");
         output = ed_execute(command);
      }
   };
   if (failure)
   {
      ed_lines = saved_lines;
      ed_marks = saved_marks;
      ed_cursor = saved_cursor;
      ed_mode = saved_mode;
      ed_dirty = saved_dirty;
      ed_number = saved_number;
      ed_list = saved_list;
      ed_auto_print = saved_print;
      ed_shiftwidth = saved_shiftwidth;
      ed_scroll = saved_scroll;
      ed_help = saved_help;
      ed_help_pos = saved_help_pos;
      ed_filename = saved_filename;
      ed_pattern = saved_pattern;
      ed_global = saved_global;
      ed_selected = saved_selected;
      return "? " + failure + "\n";
   }
   return output;
}
