/* Do not remove the headers from this file! see /USAGE for more info. */

//  Grep - Locate files containing a particular regular expression.
//  Created by Rust, Jul 10, 1995

//: COMMAND
// USAGE ``grep [-inv] <target> <files>``
//
// Locates and displays lines matching specified pattern within specified files.
//
// Flags:
//
//   | -i for case insensitive comparison
//   | -n to display line numbers
//   | -v to invert slection - ie display lines NOT matching.
//
// .. TAGS: RST

inherit CMD;
inherit M_REGEX;
inherit M_GLOB;
inherit M_ED;

private
void stdin_grep(string pattern, mapping flags, string stdin)
{
   string *input_lines;
   mixed *lines;
   int bits = 0;
   int i;

   if (!stdin)
   {
      out("Too few arguments.\nUsage: grep -inv string file(s)\n");
      return;
   }

   if (flags["i"])
      pattern = insensitive_pattern(pattern);
   if (flags["n"])
      bits |= 1;
   if (flags["v"])
      bits |= 2;

   pattern = translate(pattern);
   input_lines = strlen(stdin) ? explode(stdin, "\n") : ({});
   if (strlen(stdin) && stdin[<1] == '\n')
      input_lines = input_lines[0..<2];
   lines = regexp(input_lines, pattern, bits);
   if (!sizeof(lines))
   {
      out("No matches.\n");
   }
   else
   {
      if (flags["n"])
      {
         for (i = 0; i < sizeof(lines); i += 2)
            outf(" %d  %s\n", lines[i], lines[i + 1]);
      }
      else
      {
         map(map(lines, ( : $1 + "\n" :)), out);
      }
   }
}

private
void main(mixed argv, mapping flags, string stdin)
{
   string file;
   string this_output;
   string pattern;

   pattern = replace_string(argv[0], "/", "\\/");
   if (!argv[1])
   {
      stdin_grep(pattern, flags, stdin);
      return;
   }
   if (flags["v"])
   {
      map(argv[1], ( : stdin_grep($(pattern), $(flags), read_file($1)) :));
      return;
   }

   argv[1] = decompose(map(argv[1], ( : is_directory($1) ? glob($1 + ($1[ < 1] == '/' ? "*" : "/*")) : $1:)));

   if (flags["i"])
      pattern = insensitive_pattern(pattern);
   pattern = translate(pattern);
   foreach (file in argv[1])
   {
      if (file_size(file) < 1)
         continue;
      if (file_size(file) > MAX_FILE_SIZE)
      {
         outf("Ignoring file %s - too large!\n", file);
         continue;
      }
      mixed start_error = catch(ed_start(file));
      if (start_error)
      {
         outf("Cannot grep %s: %s\n", file, start_error);
         continue;
      }
      if (flags["n"])
         ed_cmd("n");
      if (catch (this_output = ed_cmd("1,$g/" + pattern + "/p")))
      {
         printf("Warning: %s was too large for LPC to grep.\n", file);
         continue;
      }
      if (this_output && strlen(this_output))
         outf("[%s]:\n%s\n\n", file, this_output);
      ed_cmd("q");
   }
   if (!strlen(get_output()))
      out("No matches found.\n");
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg, flags, input);
}
