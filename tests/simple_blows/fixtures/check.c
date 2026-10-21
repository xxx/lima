inherit M_ACCESS;
inherit "/std/adversary/blows/base";

private int checks;
private int applied_damage;
private string applied_limb;
private int attacker_notifications;
private string armoured_limb;
private object *armours = ({});

int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
varargs int hurt_us(int amount, string limb)
{
   applied_damage = amount;
   applied_limb = limb;
   return amount;
}
varargs void attacked_by(object attacker, int immediate) { attacker_notifications++; }
object *query_targets() { return ({}); }
object *event_get_armours(class event_info evt)
{
   armoured_limb = evt.target_extra;
   return armours;
}
string query_random_limb() { throw("SIMPLE_BLOWS_UNUSED: random limb"); }
void handle_message(mixed message, object target, object weapon, string limb, int percent)
{
   throw("SIMPLE_BLOWS_UNUSED: combat messages");
}
string damage_message(int amount) { throw("SIMPLE_BLOWS_UNUSED: damage messages"); }
void add_experience(int amount) { throw("SIMPLE_BLOWS_UNUSED: experience"); }
object query_weapon() { throw("SIMPLE_BLOWS_UNUSED: weapon selection"); }
varargs void unwield(string limb) { throw("SIMPLE_BLOWS_UNUSED: unwield"); }

private void verify(int passed, string label)
{
   if (!passed) throw("SIMPLE_BLOWS_FAIL: " + label);
   checks++;
   dump("SIMPLE_BLOWS_OK: " + label + "\n");
}

private class event_info make_event(mixed amount, object weapon)
{
   mixed data = amount;
#if BLOW_STYLE == BLOW_TYPES
   if (!stringp(data))
      data = ({ ({"bludgeon"}), data });
#endif
   return new(class event_info, target: nil, target_extra: "torso", weapon: weapon, data: data);
}

private void reset_wear(object armour, object weapon)
{
   armour->reset_durability();
   weapon->reset_durability();
}

private void producer_and_format()
{
   add_event(nil, nil, "torso", 17);
   class event_info evt = queue[0];
   verify(sizeof(queue) == 1 && evt.target_extra == "torso" && nilp(evt.target) &&
          nilp(evt.weapon) && event_damage(evt) == 17, "producer preserves fields and damage");
#if BLOW_STYLE == BLOW_SIMPLE
   verify(intp(evt.data) && evt.data == 17, "simple producer retains scalar damage");
   verify(strindex(event_to_str(evt), " evt.data=17\n") >= 0, "simple numeric formatting");
#else
   verify(arrayp(evt.data) && evt.data[0][0] == "bludgeon" && evt.data[1] == 17,
          "typed producer retains damage types");
   verify(strindex(event_to_str(evt), " evt.data[1]=17\n") >= 0, "typed numeric formatting");
#endif
   verify(strindex(event_to_str(evt), "target_extra=") >= 0 &&
          strindex(event_to_str(evt), "torso") >= 0, "formatter includes limb");
   verify(event_damage(evt) == 17 && evt.target_extra == "torso", "formatting preserves event");
   add_event(nil, nil, nil, 0);
   verify(event_damage(queue[1]) == 0 && intp(event_damage(queue[1])) &&
          nilp(queue[1].target_extra), "producer preserves zero damage and absent limb");
   add_event(nil, nil, nil, "miss");
   verify(queue[2].data == "miss" && strindex(event_to_str(queue[2]), "miss") >= 0,
          "producer and formatter preserve action");
   mixed wrong = this_object();
   mixed failure = catch(new(class event_info, target_extra: wrong));
   verify(stringp(failure) && strindex(failure, "target_extra") >= 0,
          "class constructor rejects object limb");
   failure = catch(evt.target_extra = wrong);
   verify(stringp(failure) && strindex(failure, "target_extra") >= 0 &&
          evt.target_extra == "torso", "class assignment rejects object limb");
}

private void natural_armour()
{
   class event_info evt = make_event(17, nil);
   verify(health_modify_event(evt) == evt && event_damage(evt) == 17,
          "zero natural armour leaves damage unchanged");
   set_natural_armour(1);
   verify(health_modify_event(evt) == evt && event_damage(evt) == 17,
          "unit natural armour has deterministic zero reduction");
   set_natural_armour(2);
   int valid = 1;
   for (int i = 0; i < 16; i++)
   {
      evt = make_event(1, nil);
      health_modify_event(evt);
      valid = valid && intp(event_damage(evt)) && event_damage(evt) >= 0 && event_damage(evt) <= 1;
   }
   verify(valid, "natural armour keeps random reduction in bounds");
   set_natural_armour(100);
   evt = make_event(0, nil);
   verify(health_modify_event(evt) == evt && event_damage(evt) == 0, "natural armour clamps at zero");
   evt = make_event("miss", nil);
   verify(health_modify_event(evt) == evt && evt.data == "miss", "natural armour preserves action");
   set_natural_armour(1);
}

private void armour_sink(object armour, object weapon)
{
   armour->set_armour_class(2);
   weapon->set_max_durability(1000);
   reset_wear(armour, weapon);
   verify(armour->max_durability() == 1000 && weapon->query_durability() == 1000,
          "real durability initialization");
   armour->set_armour_class(0);
   class event_info evt = make_event(17, weapon);
   verify(armour->sink_modify_event(evt) == evt && event_damage(evt) == 17 &&
          armour->query_durability() == 1000 && weapon->query_durability() == 1000,
          "zero armour leaves damage and durability unchanged");
   armour->set_armour_class(2);
   evt = make_event(17, nil);
#if BLOW_STYLE == BLOW_SIMPLE
   int unresisted = 16;
#else
   int unresisted = 17;
#endif
   verify(armour->sink_modify_event(evt) == evt && event_damage(evt) == unresisted &&
          armour->query_durability() == 1000, "unresisted damage without weapon");
   mapping resistances = armour->query_resistances();
   resistances["bludgeon"] = 3;
   evt = make_event(17, weapon);
#if BLOW_STYLE == BLOW_SIMPLE
   int resisted = 16;
#else
   int resisted = 12;
#endif
   verify(armour->sink_modify_event(evt) == evt && event_damage(evt) == resisted,
          "selected armour reduction preserves payload representation");
   verify(armour->query_durability() == 1000 - (17 - resisted) &&
          weapon->query_durability() == 1000 - (17 - resisted),
          "actual armour and weapon wear use absorbed damage");
   m_delete(resistances, "bludgeon");
   reset_wear(armour, weapon);
   foreach (string action in ({"", "x", "miss", "disarm", "fatal"}))
   {
      evt = make_event(action, weapon);
      verify(armour->sink_modify_event(evt) == evt && evt.data == action &&
             armour->query_durability() == 1000 && weapon->query_durability() == 1000,
             "sink preserves action " + sprintf("%O", action));
   }
   evt = make_event(0, weapon);
   verify(armour->sink_modify_event(evt) == evt && event_damage(evt) == 0 &&
          armour->query_durability() == 1000 && weapon->query_durability() == 1000,
          "zero damage causes no wear");
   armour->set_armour_class(40);
   resistances["bludgeon"] = 3;
   reset_wear(armour, weapon);
   evt = make_event(17, weapon);
   verify(armour->sink_modify_event(evt) == evt && event_damage(evt) == 0,
          "armour clamps fully absorbed damage");
   verify(armour->query_durability() == 20000 - 17 && weapon->query_durability() == 1000 - 17,
          "fully absorbed damage caps durability wear");
   m_delete(resistances, "bludgeon");
   armour->set_armour_class(2);
   mapping weaknesses = armour->query_weaknesses();
   weaknesses["bludgeon"] = 4;
   reset_wear(armour, weapon);
   evt = make_event(17, weapon);
#if BLOW_STYLE == BLOW_SIMPLE
   int weakened = 16;
#else
   int weakened = 19;
#endif
   verify(armour->sink_modify_event(evt) == evt && event_damage(evt) == weakened,
          "typed weakness and untyped fallback retain existing rules");
   verify(armour->query_durability() == 1000 - abs(17 - weakened) &&
          weapon->query_durability() == 1000 - abs(17 - weakened),
          "existing durability handles signed reduction");
   m_delete(weaknesses, "bludgeon");
#if BLOW_STYLE == BLOW_TYPES
   evt = make_event(17, nil);
   evt.data = ({"untyped", 17});
   verify(armour->sink_modify_event(evt) == evt && event_damage(evt) == 16,
          "typed non-array type field retains untyped fallback");
#endif
}

private void combined_path(object armour, object weapon)
{
   armours = ({armour});
   mapping resistances = armour->query_resistances();
   resistances["bludgeon"] = 3;
   reset_wear(armour, weapon);
   class event_info evt = make_event(17, weapon);
#if BLOW_STYLE == BLOW_SIMPLE
   int damage = 16;
#else
   int damage = 12;
#endif
   verify(modify_event(evt) == evt && event_damage(evt) == damage && evt.target_extra == "torso" &&
          armoured_limb == "torso",
          "combined modifiers retain event identity and limb");
   verify(armour->query_durability() == 1000 - (17 - damage) &&
          weapon->query_durability() == 1000 - (17 - damage),
          "combined modifiers apply durability once");
   verify(do_damage_event(evt) == damage && applied_damage == damage && applied_limb == "torso" &&
          attacker_notifications == 1,
          "modified damage dispatch reaches recording health receiver");
}

void create()
{
   set_privilege(1);
   producer_and_format();
   natural_armour();
   object armour = load_object("/port_tests/simple_blows/armour");
   object weapon = load_object("/port_tests/simple_blows/weapon");
   armour_sink(armour, weapon);
   combined_path(armour, weapon);
   dump(sprintf("SIMPLE_BLOWS_TOTAL: %d\n", checks));
}
