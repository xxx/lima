/* Do not remove the headers from this file! see /USAGE for more info. */

// Originally a 5 line hack by someone :)
// Modified by Beek when he successfully more'd /std/player (a directory)

inherit CMD;

//: COMMAND
// USAGE :
//
//       |  ``more <object(s)>``
//       |  ``more <filename(s)>``
//
//  Prints out the lines of a file
//
// we will recieve  : ({ array of files })
//
// .. TAGS: RST

private
void main(mixed *arg, mapping flags, string stdin)
{
   mixed *files = arg[0];
   if (!files)
   {
      if (!stdin)
      {
         out("Too few arguments.\n");
         out("Usage: more file(s)\n");
      }
      else
         more(stdin, 0, nil, NO_ANSI);
      return;
   }

   files = filter(
       files, (: [string file] {
          if (!stringp(file))
             return 0;
          if (is_directory(file))
          {
             out(file + ": Is a directory.\n");
             return 0;
          }
          return 1;
       } :));
   more_file(files, 0, nil, NO_ANSI);
}

void help()
{
   write("Usage: more ( <filename>* | -c <chunksize> )\n"
         "The more command prints a file on the screen in managable chunks. "
         "Type '?' while within more for detailed information.\n");
   return;
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg, flags, input);
}
