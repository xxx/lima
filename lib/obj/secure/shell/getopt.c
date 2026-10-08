/* Do not remove the headers from this file! see /USAGE for more info. */
//  Command line processing

#include <mudlib.h>

private
int classify(int opt, string options)
{
   int i;

   i = member_array(opt, options);
   if (i == -1 || opt == ':')
   {
      printf("option -%c not recognized\n", opt);
      return -1;
   }
   if (++i != strlen(options))
      return options[i] == ':';
   return 0;
}

//: FUNCTION getopt
// Takes an array and parses flags from it. Returns an array, the first
// element being a mapping of flag : value, the second element being an
// array of the remaining args, all flag information removed.  The second
// argument should be a string of single character flags that the command
// accepts, followed by a colon (:) if the flag can take an argument.
mixed getopt(mixed args, string options)
{
   mapping res;
   int kind;
   mixed optstring, opt, optarg;

   res = ([]);

   if (!options)
      options = "";

   while (sizeof(args) && stringp(args[0]) && strlen(args[0]) && args[0][0] == '-' && args[0] != "-")
   {
      if (args[0] == "--")
      {
         args = (sizeof(args) > 1) ? args[1..] : ({});
         break;
      }
      optstring = strlen(args[0]) == 1 ? "" : args[0][1..];
      args = sizeof(args) == 1 ? ({}) : args[1..];
      while (optstring != "")
      {
         opt = optstring[0];
         optstring = strlen(optstring) == 1 ? "" : optstring[1..];
         kind = classify(opt, options);
         if (kind < 0)
            return -1;
         if (kind)
         {
            if (optstring == "")
            {
               if (!sizeof(args))
               {
                  printf("option -%c requires an argument.\n", opt);
                  return -1;
               }
               optstring = args[0];
               args = sizeof(args) > 1 ? args[1..] : ({});
            }
            optarg = optstring;
            optstring = "";
            res[chr(opt)] = optarg;
         }
         else
         {
            optarg = "";
            res[chr(opt)] = optarg;
         }
      }
   }
   return ({res, args});
}

private string argument_span(string text, int start, int end)
{
   return start < end ? text[start..end - 1] : "";
}

//: FUNCTION argument_explode
// Returns tokens and the exact spacing before, between and after them.
mixed *argument_explode(string text)
{
   string *tokens = ({});
   string *spacing = ({});
   int i = 0, end = 0, length = strlen(text);

   while (i < length)
   {
      int start, quote;
      while (i < length && text[i] == ' ')
         i++;
      if (i == length)
         break;
      start = i;
      if (text[i] == '|' || text[i] == '<' || text[i] == '>')
      {
         if (text[i] == '>' && i + 1 < length && text[i + 1] == '>')
            i++;
         i++;
      }
      else if (text[i] == '"' || text[i] == '`')
      {
         quote = text[i++];
         while (i < length)
         {
            int character = text[i];
            if (character == '\\' && i + 1 < length)
               i += 2;
            else
            {
               i++;
               if (character == quote)
                  break;
            }
         }
      }
      else
      {
         while (i < length)
         {
            int character = text[i];
            if (character == '\\' && i + 1 < length)
               i += 2;
            else if (character == ' ' || character == '|' || character == '<' || character == '>' ||
                     character == '"' || character == '`')
               break;
            else
               i++;
         }
      }
      spacing += ({argument_span(text, end, start)});
      tokens += ({argument_span(text, start, i)});
      end = i;
   }
   return ({tokens, spacing + ({argument_span(text, end, length)})});
}

//: FUNCTION parse_argument
// calls argument_explode() and then getopt(), returning the
// value of the getopt() call.
mixed parse_argument(string input, string options)
{
   return getopt(argument_explode(input), options);
}
