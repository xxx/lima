/* Do not remove the headers from this file! see /USAGE for more info. */

inherit TRANSIENT_SPELL;

//: FUNCTION do_effect
// Executes the shield action effect.
void do_effect(object affected)
{
   object target = environment();
   object spell;
   if (!target || !target->is_living())
   {
      remove();
      return;
   }
   spell = load_object(SPELL_D->query_spell_obname("acid arrow"));

   if (objectp(spell))
      spell->ongoing_damage(query_caster(), query_target(), query_limb());
}

void setup(int strength, int duration, string action, string limb)
{
   transient_spell::setup(strength, duration, action, limb);
   set_effect_type("acid");
   set_effect_name("acid arrow");
   set_effect_duration(2);

   // Several acid arrows on same map are not extended as each
   // caster should have their own arrow.
   cannot_extend();
}
