/* Do not remove the headers from this file! see /USAGE for more info. */

#include <spells.h>
#include <move.h>

#define DANCING_LIGHTS_OBJ "/domains/std/spell/item/dancing_lights"

inherit COMBAT_SPELL;
inherit M_DICE;

// Inspired by D&D SRD, but modified for an interactive MUD.

void setup()
{
   set_level(0);
   set_name("dancing lights");
   set_category("evocation");
   set_targets(TARGET_ROOM);
   set_cast_time(1);
   set_description("You create up to four lights that resemble floating candles. "
                   "They follow you about and shed light as candles. ");
}

void cast_spell(object ob, object reagent, int success)
{
   object caster = this_body();
   int duration = (spell_skill_rank(caster) + 2) * (1 + success);
   object dl;

   // Check live clones before delayed state registration.
   if (sizeof(filter(children(DANCING_LIGHTS_OBJ), (: clonep($1) && $1->query_owner() == $(caster) :))))
   {
      // Should be safe to use write() as it goes to this_body().
      write("You fail to cast a second dancing lights at a time.");
      return;
   }

   // Create the dancing lights object, it's unique to the caster.
   dl = clone_object (DANCING_LIGHTS_OBJ, duration);
   dl->set_tamed_by(caster);
   dl->set_owner(caster);
   dl->no_chip();
   if (dl->move(environment(caster)) != MOVE_OK)
   {
      dl->remove();
      write("The dancing lights cannot appear here.\n");
      return;
   }
   dl->setup_follow(caster);
   caster->simple_action("$N $vsummon a swarm " + (success ? "of bright " : "") + "of dancing lights.");
}