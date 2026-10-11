/* Do not remove the headers from this file! see /USAGE for more info. */

//: ADMINCOMMAND
//$$see: objfind
// USAGE ``objpurge <filename>``
//
// Removes the objects that would be returned from objfind
// ie those with the specified filename
//
// Admin Only
//
// .. TAGS: RST

/*
** objpurge.c
**
** Remove instances od the given class
**
** 07-Nov-94. Created. Deathblade.
**
** Modified from objfind.c by someone at sometime
**
*/

#include <mudlib.h>

inherit CMD;

private
object *get_obs(string arg)
{
   return filter(objects(), ( : base_name($1) == $(arg) :));
}

private
void main(string arg)
{
   object *obs;

   if (!check_privilege(1))
      error("Must be an admin to use objpurge.\n");

   obs = get_obs(arg);
   outf("Removing %d objects of class %s\n", sizeof(obs), arg);
   obs->remove();

   obs = filter(obs, ( : $1:)); /* remove zeros */
   outf("Destructing %d objects of class %s\n", sizeof(obs), arg);
   map(obs, destruct);
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
