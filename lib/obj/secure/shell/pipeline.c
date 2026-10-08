/* Do not remove the headers from this file! see /USAGE for more info. */

private int pipeline_operator(mixed word)
{
   return stringp(word) && (word == "|" || word == "<" || word == ">" || word == ">>");
}

private int pipeline_word_complete(mixed word)
{
   int i, quote;
   if (!stringp(word))
      return 1;
   for (i = 0; i < strlen(word); i++)
   {
      if (word[i] == '\\')
      {
         if (++i == strlen(word))
            return 0;
      }
      else if (word[i] == '"' || word[i] == '`')
      {
         if (!quote)
            quote = word[i];
         else if (quote == word[i])
            quote = 0;
      }
   }
   return !quote;
}

private string pipeline_path(mixed *words)
{
   string result = "";
   int i, j;
   for (i = 0; i < sizeof(words); i++)
   {
      string word = words[i];
      if (i)
         result += " ";
      if (strlen(word) >= 2 && word[0] == '"' && word[<1] == '"')
         word = strlen(word) == 2 ? "" : word[1..<2];
      for (j = 0; j < strlen(word); j++)
      {
         if (word[j] == '\\' && j + 1 < strlen(word))
            j++;
         result += word[j..j];
      }
   }
   return result;
}

private string *pipeline_argument_spacing(string *spacing, int start, int count)
{
   string *result = count > 1 ? spacing[start + 1..start + count - 1] + ({""}) : ({""});
   if (strlen(result[0]) && result[0][0] == ' ')
      result[0] = result[0][1..];
   return result;
}

protected string pipeline_raw_suffix(mixed *words, string *spacing, int start)
{
   string result = implode_by_arr(map(words[start + 1..], (: stringp($1) ? $1 : sprintf("%O", $1) :)),
                                  spacing[start + 1..]);
   if (strlen(result) && result[0] == ' ')
      result = result[1..];
   return result;
}

// A string is a syntax error, an integer is the first lookup's fallback, and an array is a prepared pipeline.
protected mixed prepare_pipeline(mixed *words, string *spacing, function lookup)
{
   mixed *stages = ({});
   int start = 0, length = sizeof(words);
   if (sizeof(spacing) != length + 1)
      return "command expansion changed token spacing";
   if (!length)
      return ({});

   while (start < length)
   {
      int end = start, cursor;
      mixed info;
      mapping stage;
      string *arg_spacing;
      while (end < length && !pipeline_operator(words[end]))
         end++;
      if (end == start)
         return "missing command before " + words[start];
      arg_spacing = pipeline_argument_spacing(spacing, start, end - start);
      info = evaluate(lookup, words[start..end - 1], arg_spacing);
      if (!arrayp(info))
      {
         if (!sizeof(stages))
            return info;
         return "cannot resolve pipeline command " + words[start];
      }
      stage = (["command":info, "spacing":arg_spacing,
                "raw":pipeline_raw_suffix(words, spacing, start),
                "input":nil, "output":nil, "remaining_spacing":nil]);
      if (info[0]->query_no_pipeline() &&
          (sizeof(stages) || (end < length && member_array("|", words[end..]) != -1)))
         return "command " + words[start] + " does not support pipelines";
      if (info[0]->query_no_redirection())
         return stages + ({stage});

      for (cursor = start; cursor < end; cursor++)
         if (!pipeline_word_complete(words[cursor]))
            return "unfinished quote or escape";
      cursor = end;
      while (cursor < length && words[cursor] != "|")
      {
         string op = words[cursor++];
         int first = cursor;
         string path;
         if (op == "<" && (sizeof(stages) || stage["input"]))
            return "input redirection requires the first stage and a single file";
         if (op != "<" && stage["output"])
            return "duplicate output redirection";
         while (cursor < length && !pipeline_operator(words[cursor]))
         {
            if (!stringp(words[cursor]) || !pipeline_word_complete(words[cursor]))
               return "invalid redirect filename";
            cursor++;
         }
         if (cursor == first)
            return "missing filename after " + op;
         path = pipeline_path(words[first..cursor - 1]);
         if (!strlen(path))
            return "empty redirect filename";
         if (op == "<")
            stage["input"] = ({path});
         else
            stage["output"] = ({op, path});
      }
      if (cursor < length)
      {
         if (stage["output"])
            return "output redirection conflicts with a pipe";
         if (cursor + 1 == length)
            return "missing command after pipe";
         stage["output"] = words[cursor..];
         stage["remaining_spacing"] = spacing[cursor + 1..];
      }
      stages += ({stage});
      start = cursor + 1;
   }
   return stages;
}

protected void execute_pipeline(mixed *stages)
{
   mixed input = nil;
   mapping stage;
   foreach (stage in stages)
   {
      mixed command = stage["command"];
      mixed result = command[0]->call_main(command[2], command[1],
                                          stage["input"] || input, stage["output"], stage["spacing"],
                                          stage["remaining_spacing"], stage["raw"]);
      if (!result)
         return;
      input = result[1];
   }
}
