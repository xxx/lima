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
private nosave int ed_scroll = 20;
private nosave mapping ed_marks = ([]);
private nosave string ed_pattern = "";
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
      if (ch == '\\' && ed_scan_pos < sizeof(ed_scan) &&
          ed_scan[ed_scan_pos] == delimiter)
         pattern += sprintf("%c", ed_scan[ed_scan_pos++]);
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

private string ed_print(int first, int last, int list)
{
   string output = "";
   ed_range(first, last);
   for (int line = first; line <= last; line++)
   {
      string text = ed_lines[line-1];
      if (list)
      {
         text = replace_string(text, "\\", "\\\\");
         text = replace_string(text, "\t", "\\t");
         text = replace_string(text, "\r", "\\r");
         text = replace_string(text, sprintf("%c", 0), "\\000");
         text += "$";
      }
      if (ed_number)
         text = sprintf("%d\t%s", line, text);
      if (ed_bytes(output) + ed_bytes(text) + 1 > ED_MAX_BYTES)
         ed_fail("output exceeds 8192 UTF-8 bytes; use a smaller range");
      output += text + "\n";
   }
   ed_cursor = last;
   return output;
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
   ed_filename = ed_pattern = "";
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
   if (!operation)
   {
      if (!addressed)
         first = last = ed_cursor + 1;
      return ed_print(first, last, 0);
   }
   switch (operation)
   {
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
      return "";
   case 'j':
      if (sizeof(argument)) ed_fail("unexpected command argument");
      if (addressed < 2) last = first < sizeof(ed_lines) ? first + 1 : first;
      ed_range(first, last);
      if (first != last)
         ed_replace(first, last, ({ implode(ed_lines[first-1..last-1], "") }));
      ed_cursor = first;
      return "";
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
         return "";
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
   if (ed_mode != -1) ed_fail("editor already active");
   if (!stringp(path)) ed_fail("filename must be a string");
   if (restricted != 0 && restricted != 1) ed_fail("restricted must be 0 or 1");
   if (scroll < 1 || scroll > 100) ed_fail("scroll must be between 1 and 100");
   if (ed_bytes(path) > ED_MAX_COMMAND_BYTES) ed_fail("filename too long");
   loaded = sizeof(path) ? ed_read(path, 1) : ({});
   ed_lines = loaded;
   ed_filename = path;
   ed_marks = ([]);
   ed_pattern = "";
   ed_cursor = sizeof(loaded) ? 1 : 0;
   ed_mode = ed_dirty = ed_number = 0;
   ed_restricted = restricted;
   ed_scroll = scroll;
   return sizeof(loaded) ? sprintf("%d lines\n", sizeof(loaded)) : "[New or empty file]\n";
}

//: FUNCTION query_ed_mode
// Return -1 when inactive, 0 for commands, or the next input line number.
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
   string saved_filename = ed_filename;
   string saved_pattern = ed_pattern;
   mixed failure;
   string output = "";
   if (ed_mode == -1) ed_fail("editor is inactive");
   ed_marks = ed_marks + ([]);
   failure = catch {
      if (strindex(command, "\n") >= 0) ed_fail("input must be one line");
      if (ed_mode > 0)
      {
         if (command == ".") ed_mode = 0;
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
      ed_filename = saved_filename;
      ed_pattern = saved_pattern;
      return "? " + failure + "\n";
   }
   return output;
}
