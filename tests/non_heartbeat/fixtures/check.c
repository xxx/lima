inherit M_ACCESS;

private int checks;
private int removed_count;
private string removed_name;

int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }

private void verify(int passed, string label)
{
   if (!passed) throw("NON_HEARTBEAT_FAIL: " + label);
   checks++;
   dump("NON_HEARTBEAT_OK: " + label + "\n");
}

void removal_notice(object target)
{
   removed_count++;
   removed_name = file_name(target);
}

private mixed *pulse_rows(object actor, int unrelated)
{
   return filter(query_call_outs(actor), (: $1[4] != $(unrelated) :));
}

private mixed *one_pulse(object actor, int unrelated)
{
   mixed *rows = pulse_rows(actor, unrelated);
   if (sizeof(rows) != 1) throw("NON_HEARTBEAT_FAIL: expected one pulse");
   return rows[0];
}

private mixed consume_and_invoke(mixed *row)
{
   // Native one-shot dispatch removes its row before entering the callback.
   remove_call_out(row[4]);
   function callback = row[1];
   return callback();
}

private void start_and_continue(object actor, object opponent, int unrelated)
{
   verify(!actor->is_attacking() && !sizeof(pulse_rows(actor, unrelated)), "fresh actor has no pulse");
   actor->attacked_by(opponent, 0);
   mixed *row = one_pulse(actor, unrelated);
   verify(actor->is_attacking() && actor->get_target() == opponent && !actor->query_attacks(),
          "automatic start selects target without immediate swing");
   verify(row[0] == actor && row[2] >= 3000 && row[2] <= 7000 && row[3] == 0,
          "default one-shot delay retains jitter");
   actor->attacked_by(this_object(), 1);
   verify(actor->get_target() == this_object() && !actor->query_attacks() &&
          one_pulse(actor, unrelated)[4] == row[4], "active notification retargets without another pulse");
   actor->switch_to(opponent);
   mixed result = actor->continue_fight();
   verify(!result && actor->query_attacks() == 1 && actor->query_attack_player() == opponent &&
          efun::this_player() == opponent, "explicit continuation retains incoming player context");
   verify(sizeof(pulse_rows(actor, unrelated)) == 1 && one_pulse(actor, unrelated)[4] == row[4],
          "continuation reuses pending automatic pulse");
   int before = actor->query_attacks();
   result = consume_and_invoke(row);
   verify(!functionp(result) && actor->query_attacks() == before + 1,
          "registered callback invokes one attack");
   verify(actor->query_attack_player() == actor && efun::this_player() == opponent,
          "callback establishes actor and restores incoming player");
   int successor = one_pulse(actor, unrelated)[4];
   verify(successor != row[4] && !query_call_out(row[4]), "consumed callback registers one successor");
   actor->set_attack_speed(8);
   verify(one_pulse(actor, unrelated)[4] == successor, "nonzero speed change retains pending deadline");
   consume_and_invoke(one_pulse(actor, unrelated));
   row = one_pulse(actor, unrelated);
   verify(row[2] >= 6000 && row[2] <= 10000, "next deadline uses changed interval and jitter");
   actor->stop_attacking();
   verify(!actor->is_attacking() && !query_call_out(row[4]) && !sizeof(pulse_rows(actor, unrelated)),
          "stop immediately cancels owned successor");
   verify(!!query_call_out(unrelated), "stop preserves unrelated timer");
   before = actor->query_attacks();
   function stale = row[1];
   stale();
   verify(actor->query_attacks() == before && !sizeof(pulse_rows(actor, unrelated)),
          "stopped callback is inert");
   actor->set_attack_speed(5);
   actor->attacked_by(opponent, 0);
   row = one_pulse(actor, unrelated);
   verify(row[4] != successor, "restart has a fresh single timer");
   consume_and_invoke(row);
   verify(sizeof(pulse_rows(actor, unrelated)) == 1, "restart continues one timer chain");
   actor->stop_attacking();
}

private void manual_mode(object actor, object opponent, int unrelated)
{
   actor->attacked_by(opponent, 0);
   mixed *row = one_pulse(actor, unrelated);
   actor->set_attack_speed(0);
   verify(!query_call_out(row[4]) && !sizeof(pulse_rows(actor, unrelated)),
          "entering manual mode cancels pending pulse");
   int before = actor->query_attacks();
   function stale = row[1];
   stale();
   verify(actor->query_attacks() == before && !sizeof(pulse_rows(actor, unrelated)),
          "manual state rejects a stale callback");
   actor->set_attack_speed(5);
   verify(sizeof(pulse_rows(actor, unrelated)) == 1, "active manual-to-automatic transition rearms once");
   actor->stop_attacking();
   actor->set_attack_speed(0);
   actor->switch_to(nil);
   actor->attacked_by(opponent, 0);
   verify(nilp(actor->get_target()) && !actor->is_attacking() &&
          actor->query_attacks() == before && !sizeof(pulse_rows(actor, unrelated)),
          "manual deferred notification retains no-op policy");
   actor->attacked_by(opponent, 1);
   verify(actor->get_target() == opponent && actor->query_attacks() == before + 1 &&
          !actor->is_attacking() && !sizeof(pulse_rows(actor, unrelated)),
          "manual immediate notification attacks without automatic state");
   verify(actor->query_attack_player() == opponent && efun::this_player() == opponent,
          "manual immediate attack retains incoming player");
   mixed result = actor->continue_fight();
   verify(!result && actor->query_attacks() == before + 2 && !sizeof(pulse_rows(actor, unrelated)),
          "manual continuation attacks without scheduling");
   actor->switch_to(nil);
   verify(actor->continue_fight() == "You aren't attacking anyone.\n",
          "no-target continuation preserves response");
   actor->set_attack_speed(5);
}

private void stop_and_errors(object actor, object opponent, int unrelated)
{
   actor->set_stop_during_attack(1);
   int before = actor->query_attacks();
   actor->attacked_by(opponent, 1);
   verify(actor->query_attacks() == before + 1 && !actor->is_attacking() &&
          !sizeof(pulse_rows(actor, unrelated)), "stop within immediate attack cancels preregistered successor");
   actor->set_stop_during_attack(0);
   actor->attacked_by(opponent, 0);
   mixed *row = one_pulse(actor, unrelated);
   actor->set_stop_during_attack(1);
   consume_and_invoke(row);
   verify(!actor->is_attacking() && !sizeof(pulse_rows(actor, unrelated)) && efun::this_player() == opponent,
          "stop within callback cancels successor and restores player");
   actor->set_stop_during_attack(0);
   actor->set_fail_attack(1);
   before = actor->query_attacks();
   mixed problem = catch(actor->attacked_by(opponent, 1));
   verify(stringp(problem) && strindex(problem, "PULSE_TEST_ATTACK_FAILURE") >= 0 &&
          actor->query_attacks() == before + 1 && actor->query_attack_player() == opponent &&
          efun::this_player() == opponent, "caught immediate failure retains incoming context");
   verify(sizeof(pulse_rows(actor, unrelated)) == 1, "caught immediate failure retains earlier registration");
   row = one_pulse(actor, unrelated);
   problem = catch(consume_and_invoke(row));
   verify(stringp(problem) && strindex(problem, "PULSE_TEST_ATTACK_FAILURE") >= 0 &&
          actor->query_attack_player() == actor && efun::this_player() == opponent,
          "callback failure restores incoming player before propagating");
   verify(sizeof(pulse_rows(actor, unrelated)) == 1, "caught callback failure leaves its one queued successor");
   actor->set_fail_attack(0);
   actor->stop_attacking();
   verify(!sizeof(pulse_rows(actor, unrelated)) && query_call_out(unrelated),
          "failure cleanup cancels only combat timer");
   actor->set_attack_speed(-2);
   actor->attacked_by(opponent, 0);
   row = one_pulse(actor, unrelated);
   verify(row[2] >= -4000 && row[2] <= 0 && row[3] == 0,
          "nonpositive delay retains native pending metadata");
   actor->stop_attacking();
   actor->set_attack_speed(5);
   verify(actor->query_penalty() >= 0 && actor->query_penalty() <= 6, "penalty remains bounded by existing rule");
}

void create()
{
   set_privilege(1);
   object actor = clone_object("/port_tests/non_heartbeat/actor");
   object opponent = clone_object("/port_tests/non_heartbeat/actor");
   int unrelated = actor->unrelated_timer();
   object previous = set_this_player(opponent);
   start_and_continue(actor, opponent, unrelated);
   manual_mode(actor, opponent, unrelated);
   stop_and_errors(actor, opponent, unrelated);
   actor->add_hook("remove", &removal_notice(actor));
   string name = file_name(actor);
   actor->set_light(4);
   actor->attacked_by(opponent, 0);
   mixed *row = one_pulse(actor, unrelated);
   actor->remove();
   verify(removed_count == 1 && removed_name == name, "inherited object removal runs normal remove hook");
   verify(!objectp(actor) && !find_object(name), "inherited object removal destroys actor");
   verify(!query_call_out(row[4]) && !query_call_out(unrelated),
          "native destruction cancels owned combat and unrelated timers");
   set_this_player(previous);
   opponent->remove();
   verify(!objectp(opponent) && !sizeof(query_call_outs(-1)), "all fixture objects and timers cleaned before commit");
   dump(sprintf("NON_HEARTBEAT_TOTAL: %d\n", checks));
}
