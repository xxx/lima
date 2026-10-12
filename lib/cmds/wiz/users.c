/* Do not remove the headers from this file! see /USAGE for more info. */

/* List the user names of all people listed in the "users()" array
 *
 * Peregrin@ZorkMUD
 */

//: COMMAND
// USAGE:  ``users``
//
// This command lists the names of all users currently connected.
// It is convenient for those who don't wish to clutter their screen with excess
// information such as what comes with the "``who``" command.
//
// .. TAGS: RST

inherit CMD;

private
void main()
{
   object *ulist = users();
   string ustring;
   string *names;

   if (sizeof(ulist) <= 0)
   {
      out(sprintf("NO ONE LOGGED IN!!!   (not even you!)\n"));
      return;
   }
   if ((sizeof(ulist) == 1) && (ulist[0] == this_user()))
   {
      out(sprintf("You are the only person logged in!\n"));
      return;
   }
   names = map(ulist->query_userid(), (: $1 ? $1 : "(login)" :));
   if (end_of_pipeline())
      out(implode(names, " ") + "\n");
   else
      out((implode(names, "\n") + "\n"));
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main();
}
