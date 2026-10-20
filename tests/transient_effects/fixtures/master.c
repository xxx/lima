#define PROBE "/port_tests/transient_effects/probe"
#define HOLDER "/port_tests/transient_effects/holder"
#define DRUNK "/contrib/transient_effect/drunk"

private int checks;
private mapping applications = ([]);
private mapping affected_objects = ([]);
private mapping players = ([]);
private object holder;
private object detached;
private object nonliving;
private object timed;
private object idle;
private string timed_name;
private int polls;
private int timed_id;
private int idle_id;

int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_shutdown(object caller, string program) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }

void error_handler(mapping problem)
{
   dump("EFFECT_ERROR: " + problem["diagnostic"] + "\n");
   efun::shutdown(1);
}

private void verify(int passed, string label)
{
   if (!passed) throw("EFFECT_FAIL: " + label);
   checks++;
   dump("EFFECT_OK: " + label + "\n");
}

void record_effect(object effect, object affected, object player)
{
   string name = file_name(effect);
   applications[name] = (has_key(applications, name) ? applications[name] : 0) + 1;
   affected_objects[name] = affected;
   players[name] = player;
}

private void check_elapsed()
{
   if (objectp(timed))
   {
      if (++polls > 240) throw("EFFECT_FAIL: timer completion deadline");
      call_out(check_elapsed, 0.05);
      return;
   }
   verify(applications[timed_name] == 3, "elapsed duration two gives three applications");
   verify(affected_objects[timed_name] == holder, "elapsed callbacks receive the living environment");
   verify(players[timed_name] == holder, "elapsed callbacks retain configured player context");
   verify(!query_call_out(timed_id), "expiry removes the repeating timer");
   verify(!sizeof(query_call_outs(detached)) && !sizeof(query_call_outs(nonliving)),
          "paused effects remain unscheduled during elapsed dispatch");
   verify(objectp(idle) && idle->query_effect_duration() == 2 && !applications[file_name(idle)],
          "elapsed detached callback preserves duration without applying");
   verify(!query_call_out(idle_id), "elapsed detached callback retires its timer");
   detached->remove();
   nonliving->remove();
   idle->remove();
   holder->remove();
   dump("EFFECT_TOTAL: " + checks + "\n");
   efun::shutdown(0);
}

private void check_expiry(int duration)
{
   object effect = clone_object(PROBE);
   string name = file_name(effect);
   effect->set_effect_name("expiry");
   effect->set_effect_duration(duration);
   effect->move(holder);
   int id = query_call_outs(effect)[0][4];
   effect->heart_beat();
   verify(!objectp(effect) && applications[name] == 1 && !query_call_out(id),
          "duration " + duration + " applies once before expiry");
}

private void run_tests()
{
   object blueprint = load_object("/contrib/transient_effect/transient_effect");
   verify(!sizeof(query_call_outs(blueprint)), "blueprint has no timer");
   holder = clone_object(HOLDER);
   detached = clone_object(PROBE);
   detached->set_effect_duration(2);
   verify(detached->is_attached(), "effect remains attached");
   mixed *rows = query_call_outs(detached);
   verify(sizeof(rows) == 1 && rows[0][0] == detached && rows[0][3] == 2000,
          "clone owns one two-second repeating timer");
   detached->heart_beat();
   verify(!applications[file_name(detached)] && detached->query_effect_duration() == 2
          && !sizeof(query_call_outs(detached)), "detached effect pauses without duration loss");
   verify(detached->move(nil) == 0 && !sizeof(query_call_outs(detached)), "nil destination is refused without restarting");
   verify(detached->move(holder) == 1 && sizeof(query_call_outs(detached)) == 1,
          "living move restarts one timer");
   int resident_id = query_call_outs(detached)[0][4];
   detached->move(holder);
   verify(objectp(detached) && query_call_outs(detached)[0][4] == resident_id,
          "self move preserves resident and timer");
   object incoming = clone_object(PROBE);
   incoming->set_effect_duration(7);
   int incoming_id = query_call_outs(incoming)[0][4];
   verify(nilp(incoming->move(holder)) && !objectp(incoming) && detached->query_effect_duration() == 9,
          "matching identity merges duration and removes incoming");
   verify(!query_call_out(incoming_id) && sizeof(query_call_outs(detached)) == 1,
          "merge cancels incoming timer and retains one resident timer");
   object different = clone_object(PROBE);
   different->set_effect_name("other");
   verify(different->move(holder) == 1 && environment(different) == holder,
          "different name stays independent");
   different->remove();
   different = clone_object(PROBE);
   different->set_effect_type("other");
   verify(different->move(holder) == 1 && environment(different) == holder,
          "different type stays independent");
   different->remove();
   detached->heart_beat();
   verify(applications[file_name(detached)] == 1 && detached->query_effect_duration() == 8,
          "living update applies before decrementing");
   holder->set_living(0);
   detached->heart_beat();
   verify(applications[file_name(detached)] == 1 && detached->query_effect_duration() == 8
          && !sizeof(query_call_outs(detached)), "nonliving host pauses without applying");
   nonliving = clone_object(PROBE);
   nonliving->set_effect_name("other");
   nonliving->move(holder);
   nonliving->heart_beat();
   verify(!sizeof(query_call_outs(nonliving)), "nonliving destination stops the timer");
   holder->set_living(1);
   check_expiry(0);
   check_expiry(-1);
   object self_removing = clone_object(PROBE);
   string self_name = file_name(self_removing);
   self_removing->set_effect_name("self removing");
   self_removing->set_effect_duration(99);
   self_removing->set_remove_during_effect();
   self_removing->move(holder);
   int self_id = query_call_outs(self_removing)[0][4];
   self_removing->heart_beat();
   verify(!objectp(self_removing) && applications[self_name] == 1 && !query_call_out(self_id),
          "effect may remove itself during callback");
   object drunk = clone_object(DRUNK);
   object extra = clone_object(DRUNK);
   verify(drunk->query_effect_type() == "drunk" && drunk->query_effect_name() == "alcohol"
          && drunk->query_effect_duration() == 10, "drunk setup retains identity and duration");
   drunk->set_alcohol_strength(1);
   drunk->add_alcohol_strength(-2);
   verify(drunk->query_alcohol_strength() == 0, "negative strength addition clamps at zero");
   drunk->set_alcohol_strength(-3);
   verify(drunk->query_alcohol_strength() == 0, "negative strength setter clamps at zero");
   holder->set_bound(0);
   drunk->do_effect(holder);
   verify(holder->query_stuns() == 1 && holder->query_heals() == 1, "zero strength dispatches bounded zero amounts");
   drunk->set_alcohol_strength(1);
   drunk->add_alcohol_strength(2);
   verify(drunk->query_alcohol_strength() == 3, "positive strength addition is retained");
   extra->set_alcohol_strength(4);
   drunk->move(holder);
   extra->move(holder);
   verify(!objectp(extra) && drunk->query_alcohol_strength() == 7 && drunk->query_effect_duration() == 20,
          "drunk merge composes strength and inherited duration");
   holder->set_bound(7);
   for (int i = 0; i < 50; i++) drunk->do_effect(holder);
   verify(holder->query_stuns() == 51 && holder->query_heals() == 51,
          "drunk callback dispatches bounded stun and healing on each update");
   dump("EFFECT_COMMANDS: " + holder->query_commands() + "\n");
   dump("EFFECT_ACTIONS: " + holder->query_actions() + "\n");
   mapping drunk_save = restore_value(drunk->save_to_string());
   verify(drunk_save["effect_duration"] == 20 && !has_key(drunk_save, "alcohol_strength"),
          "drunk retains existing duration-only save registration");
   int drunk_id = query_call_outs(drunk)[0][4];
   drunk->remove();
   verify(!query_call_out(drunk_id), "explicit removal cancels the timer");
   mapping saved = restore_value(detached->save_to_string());
   verify(saved["effect_duration"] == 8 && !has_key(saved, "effect_timer"),
          "duration saves while timer stays nosave");
   object body = load_object("/std/body");
   verify(!function_exists("stun_us", body) && function_exists("heal_us", body),
          "selected body retains its distinct stun contract");
   verify(function_signature("force_game_command", body)["num_args"] == 1,
          "selected command API takes one argument");
   object previous = set_this_player(holder);
   timed = clone_object(PROBE);
   set_this_player(previous);
   timed_name = file_name(timed);
   timed->set_effect_type("timed");
   timed->set_effect_duration(2);
   timed->move(holder);
   timed_id = query_call_outs(timed)[0][4];
   idle = clone_object(PROBE);
   idle->set_effect_duration(2);
   idle_id = query_call_outs(idle)[0][4];
   call_out(check_elapsed, 0.05);
}

string *epilog(int load_empty)
{
   call_out(run_tests, 0);
   return ({});
}
