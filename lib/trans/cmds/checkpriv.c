/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** checkpriv.c -- check if you are operating at a particular privilege
**
** 950816, Deathblade: created.
*/

//: COMMAND
// USAGE:  ``checkpriv < privilege >``
//
// This command will tell you whether or not you have a certain privilege.
//
// ``checkpriv 1``
// will check if you have priv 1 (ie admin)
//
// .. TAGS: RST

#include <security.h>

inherit CMD;

private
void main(mixed arg)
{
   arg = arg ? trim(arg) : "";
   if (arg == "")
   {
      out("Usage: checkpriv <privilege>\n");
      return;
   }

   if (arg == "1")
      arg = 1;
   else if (arg == "0")
      arg = 0;

   if (!SECURE_D->valid_privilege(arg))
   {
      outf("'%s' is not a valid privilege.\n", arg + "");
      return;
   }

   if (check_privilege(arg))
      outf("Verified.  You have privilege '%s'\n", arg + "");
   else
      outf("Sorry.  You do not have privilege '%s'\n", arg + "");
}

protected varargs void dispatch_main(mixed first, mixed second, mixed third, mixed fourth)
{
   main(first);
}
