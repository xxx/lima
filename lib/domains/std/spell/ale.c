
/* Do not remove the headers from this file! see /USAGE for more info. */

#include <spells.h>
#include <move.h>

inherit SPELL;

void setup()
{
   set_name("conjure ale");
   set_category("conjuration");
   set_targets(TARGET_ROOM);
   set_level(0);
   set_description("This spell conjures a small amount of ale, typically on the floor.");
}

void cast_spell(object ob, object reagent, int success)
{
   object caster = this_body();
   object destination;
   object ale;

   if (!caster || !(destination = environment(caster)))
      return;
   ale = clone_object ("/domains/std/ale");
   if (ale->move(destination) != MOVE_OK)
   {
      ale->remove();
      write("The ale cannot appear here.\n");
      return;
   }
   ale->start_evaporation();
   caster->simple_action("A streak of ale appears at the wave of $p hand.");
}
