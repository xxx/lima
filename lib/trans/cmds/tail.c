/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE:
//
//     |  ``tail <file(s)>``
//     |  ``tail -n<number> <files>``
//
//  Prints out the last few lines of a file.
// Optional -n flag for number of lines to display (default 10).
//
// .. TAGS: RST

#include <mudlib.h>
inherit CMD;

private string tail_text(string text, int nlines)
{
   if (nlines < 1)
      nlines = 10;

   bytes data = to_bytes(text, "UTF-8");
   int pos = sizeof(data) - 1;
   if (pos >= 0 && data[pos] == '\n')
      pos--;
   for (; pos >= 0; pos--)
   {
      if (data[pos] == '\n' && --nlines == 0)
         return to_text(data[pos + 1..], "UTF-8");
   }
   return text;
}

private
void main(mixed *arg, mapping flags, string stdin)
{
   int i;
   int n = 10;

   if (flags["n"])
      n = to_int(flags["n"]) || 10;
   if (!nilp(stdin))
   {
      more(tail_text(stdin, n));
      return;
   }
   for (i = 0; i < sizeof(arg[0]); i++)
   {
      more(tail(arg[0][i], n));
   }
}
