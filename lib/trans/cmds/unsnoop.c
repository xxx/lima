/* Do not remove the headers from this file! see /USAGE for more info. */

// specification is: unsnoop [user]
// we receive: ({ user })

//: COMMAND
//$$ see: snoop, snoops
// USAGE: ``unsnoop [user]``
//
// Stops snooping the target, or everyone when no target is supplied.
//
// .. TAGS: RST

#include <log.h>

inherit CMD;

private
void main(mixed *arg)
{
   int rc;
   if (!arg[0])
   {
      rc = SNOOP_D->unsnoop_all(this_body());
      out(rc ? "Snoop someone first, dumbass.\n" : "Okay.\n");
      return;
   }
   else
   {
      rc = SNOOP_D->unsnoop(this_body(), arg[0]);
      out(rc ? "Like you're snooping 'em, duh!\n" : "Okay.\n");
      return;
   }
}

protected varargs void dispatch_main(mixed first, mixed second, mixed third, mixed fourth)
{
   main(first);
}
