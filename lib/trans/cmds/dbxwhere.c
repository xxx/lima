/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE:
//     ``dbxwhere [<who>]``
//
// Dump error stack for yourself or ``who``. See 'help dbx'.
//
// .. TAGS: RST

/* Beek - 10/4/94 */
/* ported to Zork 1/15/95 */
#include <security.h>

inherit CMD;

private
mapping get_the_error(string str)
{
   return unguarded(1, (: master()->query_error($(str)) :));
}

private
void main(string str)
{
   mapping record;

   // We do not have any ANSI here, but also don't want emojis.
   set_output_flags(NO_ANSI);

   if (!str || str == "")
   {
      record = get_the_error(this_user()->query_userid());
   }
   else
   {
      record = get_the_error(str);
      if (!record && !find_body(str) && str != "last")
      {
         out("No such player.\n");
         return;
      }
   }
   if (!record)
   {
      out("No error.\n");
   }
   else
   {
      outf("%s\n", master()->standard_trace(record, 1));
   }
   return;
}

protected varargs void dispatch_main(mixed first, mixed second, mixed third, mixed fourth)
{
   main(first);
}
