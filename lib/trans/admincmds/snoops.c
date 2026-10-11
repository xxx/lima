/* Do not remove the headers from this file! see /USAGE for more info. */

// This could print a tree, but that would take effort. -- Rust

//: ADMINCOMMAND
// USAGE: ``snoops``
//
// Displays a list of wizards currently snooping, together with their targets.
//
// Admin only
//
// .. TAGS: RST

inherit CMD;

private
void main()
{
   mapping raw_data;
   mapping str_data = ([]);
   string *snoopers;

   if (!adminp(this_body()))
   {
      out("This command is for admins only, sorry :(\n");
      return;
   }

   raw_data = SNOOP_D->get_snoop_info();

   if (!sizeof(raw_data))
   {
      out("Nobody is being snooped.\n");
      return;
   }
   foreach (object snoop_ob in keys(raw_data))
   {
      // Ignore destroyed targets when formatting each subscriber's list.
      str_data[capitalize(snoop_ob->query_userid())] =
          implode(sort_array(map(filter(raw_data[snoop_ob], objectp), (
                                                                 : capitalize($1->query_userid())
                                                                 :)),
                             0),
                  ", ");
   }

   snoopers = sort_array(keys(str_data), 0);
   foreach (string snooper in snoopers)
   {
      outf("%s: %s\n", snooper, str_data[snooper]);
   }
}

protected varargs void dispatch_main(mixed first, mixed second, mixed third, mixed fourth)
{
   main();
}
