/* Do not remove the headers from this file! see /USAGE for more info. */
//
// By Ohara.

#include <mudlib.h>

inherit ADVERSARY;
inherit M_BLOCKEXITS;

private
nosave string guard_for = "<no guild assigned yet>";

private
int allow_passage(string dir, object who)
{
   string *allies;

   if (who->query_member_guild(guard_for))
      return 1;

   allies = GUILD_D->query_guild_allies(guard_for);
   if (allies && sizeof(who->guilds_belong() & allies) != 0)
      return 1;

   targetted_action("The $n $vpush $t back. \"Guild members only\", $n $vgrowl.", who, dir, this_object());
   return 0;
}

void mudlib_setup(string guild)
{
   ::mudlib_setup();

   guard_for = guild;

   set_name("guard");
   set_in_room_desc("A guard is here.");
   set_gender(1); // default male.
   set_block_action(allow_passage);
}
