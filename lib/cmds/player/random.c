/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** random.c -- invoke a random emote
**
** 950826, Deathblade: created.
*/

//: PLAYERCOMMAND
//$$ see: feelings, adverbs, semote
//
// USAGE
//   |  ``random``
//   |  ``random <name>``
//
// This command will randomly select a feeling to execute.
//
// .. TAGS: RST

#include <daemons.h>

inherit CMD;

private
void main(string str)
{
   string *emotes = SOUL_D->list_emotes();
   string name = lower_case(trim(str || ""));
   string rule;
   string extra;

   if (find_body(name))
   {
      rule = "LIV";
      extra = " " + name;
   }
   else
   {
      rule = extra = "";
   }

   emotes = filter(emotes, (: SOUL_D->query_emote($1)[$(rule)] :));
   if (!sizeof(emotes))
   {
      out("No suitable emotes.\n");
      return;
   }

   this_body()->do_game_command(choice(emotes) + extra);
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
