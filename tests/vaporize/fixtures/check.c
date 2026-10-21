#include <flags.h>
inherit M_ACCESS;

private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }

private void verify(int passed, string label)
{
   if (!passed) throw("VAPORIZE_FAIL: " + label);
   checks++;
   dump("VAPORIZE_OK: " + label + "\n");
}

private object make_actor(object room)
{
   object actor = clone_object("/port_tests/vaporize/actor");
   foreach (mixed *row in query_call_outs(actor)) remove_call_out(row[4]);
   if (actor->move(room) != 1) throw("VAPORIZE_FAIL: actor placement");
   return actor;
}

private mixed *death_timer(object actor)
{
   mixed *rows = filter(query_call_outs(actor), (: $1[2] == 1000 && $1[3] == 0 :));
   if (sizeof(rows) != 1) throw("VAPORIZE_FAIL: expected one death timer");
   return rows[0];
}

private mixed consume_and_invoke(mixed *row)
{
   // Native one-shot dispatch consumes its row before callback execution.
   remove_call_out(row[4]);
   function callback = row[1];
   return callback();
}

private int corpses()
{
   return sizeof(filter(objects(), (: clonep($1) && base_name($1) == CORPSE :)));
}

private void held_inventory(object room)
{
   object actor = make_actor(room);
   verify(actor->is_adversary() && !actor->is_body(), "actual NPC constructor and placement");
   object worn = clone_object("/port_tests/vaporize/armour");
   object unworn = clone_object("/port_tests/vaporize/armour");
   worn->set_slot("torso");
   unworn->set_slot("head");
   object item = clone_object("/port_tests/vaporize/item");
   verify(worn->move(actor) == 1 && unworn->move(actor) == 1 && item->move(actor) == 1,
          "mixed inventory enters actual NPC");
   worn->do_wear();
   verify(worn->test_flag(F_WORN) && member_array(worn, actor->query_armours("torso")) >= 0,
          "actual armour registers in limb slot");
   verify(!unworn->test_flag(F_WORN) && !worn->query_unwear_calls() && !unworn->query_unwear_calls(),
          "unworn armour starts without removal calls");
   verify(stringp(worn->move(room)) && environment(worn) == actor,
          "worn drop hook blocks transfer before unwearing");
   int count = corpses();
   actor->drop_corpse();
   verify(sizeof(all_inventory(actor)) == 3 && corpses() == count,
          "NPC drop_corpse hook leaves inventory without creating corpse");
   actor->attacked_by(this_object(), 0);
   mixed *combat = filter(query_call_outs(actor), (: $1[3] == 2000 :));
   verify(sizeof(combat) == 1 && actor->is_attacking(), "real heartbeat registers before death hook");
   actor->trigger_death_hook();
   mixed *row = death_timer(actor);
   verify(actor->is_attacking() && query_call_out(combat[0][4]) &&
          row[0] == actor && row[2] == 1000 && row[3] == 0,
          "death retains one-second deferral and existing combat state");
   verify(objectp(actor) && sizeof(all_inventory(actor)) == 3,
          "inventory stays with NPC until callback invocation");
   string name = file_name(actor);
   mixed result = consume_and_invoke(row);
   verify(!functionp(result) && !objectp(actor) && !find_object(name),
          "callback invokes vaporization and removes NPC");
   verify(objectp(worn) && objectp(unworn) && objectp(item) &&
          environment(worn) == room && environment(unworn) == room && environment(item) == room,
          "all three inventory objects survive in room");
   verify(!worn->test_flag(F_WORN) && worn->query_unwear_calls() == 1,
          "worn armour is removed exactly once");
   verify(!unworn->test_flag(F_WORN) && !unworn->query_unwear_calls(),
          "unworn armour receives no unwear call");
   verify(item->query_mass() == 2.0 && sizeof(all_inventory(room)) == 3,
          "ordinary item and room inventory remain intact");
   verify(!query_call_out(combat[0][4]) && !query_call_out(row[4]),
          "NPC destruction cancels its pending timers");
   verify(corpses() == count, "held-inventory vaporization creates no corpse");
   worn->remove();
   unworn->remove();
   item->remove();
   verify(!sizeof(all_inventory(room)), "held-inventory fixtures clean up normally");
}

private void empty_inventory(object room)
{
   object actor = make_actor(room);
   int count = corpses();
   verify(!sizeof(all_inventory(actor)) && !actor->is_attacking(), "empty NPC starts without combat");
   actor->trigger_death_hook();
   mixed *row = death_timer(actor);
   verify(row[2] == 1000 && row[3] == 0, "empty NPC retains one-shot death delay");
   mixed result = consume_and_invoke(row);
   verify(!functionp(result) && !objectp(actor) && !query_call_out(row[4]),
          "empty NPC callback removes owner and timer");
   verify(!sizeof(all_inventory(room)) && corpses() == count, "empty NPC leaves neither items nor corpse");
}

void create()
{
   set_privilege(1);
   object room = clone_object("/port_tests/vaporize/room");
   held_inventory(room);
   empty_inventory(room);
   room->remove();
   foreach (mixed *row in query_call_outs(-1)) remove_call_out(row[4]);
   verify(!objectp(room) && !sizeof(query_call_outs(-1)), "owned fixture objects and timers cleaned before commit");
   dump(sprintf("VAPORIZE_TOTAL: %d\n", checks));
}
