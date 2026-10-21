#define ACTOR "/port_tests/wounds/actor"
inherit M_ACCESS;
private int checks;
private object room;
private object timed;
private object cancelled;
private int timed_id;
private int cancelled_id;
private int polls;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_shutdown(object caller, string program) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }
int is_turret() { return 0; }
void error_handler(mapping issue)
{
   dump("WOUNDS_ERROR: " + issue["diagnostic"] + "\n");
   efun::shutdown(1);
}
private void verify(int passed, string label)
{
   if (!passed) throw("WOUNDS_FAIL: " + label);
   checks++;
   dump("WOUNDS_OK: " + label + "\n");
}
private mixed *wound_timers(object body)
{
   return filter(query_call_outs(body), (: function_info($1[1])["name"] == "health_periodic" :));
}
private int wound_sum(object body, string limb)
{
   int total = 0;
   foreach (int value in body->query_wounds(limb) || ({})) total += value;
   return total;
}
private void consistent(object body, string limb, string label)
{
   verify(wound_sum(body, limb) == body->query_max_health(limb) - body->query_health(limb), label);
}
private object make_body(string label)
{
   object body = USER_OB->make_race(ACTOR, "wounds" + label);
   body->init_stats();
   body->set_heal_rate(0);
   body->set_max_limb_health("left arm", 100);
   body->set_max_limb_health("right arm", 100);
   body->move(room);
   return body;
}
private void damage_and_healing()
{
   object body = make_body("manual");
   verify(body->query_race() == "human" && body->is_body(), "actual human body constructor");
   verify(!sizeof(body->query_all_wounds()) && nilp(body->query_wounds("left arm")), "unwounded limbs retain absent entries");
   verify(!sizeof(wound_timers(body)), "fresh body has no wound timer");
   verify(body->diagnose_wounds("left arm") == "$P left arm is in good condition.\n", "healthy named-limb diagnosis");
   verify(body->diagnose() == body->diagnose_wounds(), "zero-argument diagnosis wrapper");
   verify(strindex(body->diagnose_msg(50), "badly wounded") >= 0, "inherited generic integer diagnosis");
   verify(body->hurt_us(10, "left arm") == 10, "first damage returns applied amount");
   verify(body->query_health("left arm") == 90 && wound_sum(body, "left arm") == 10, "first damage records wound");
   mixed *rows = wound_timers(body);
   verify(sizeof(rows) == 1 && rows[0][0] == body && rows[0][2] == 15000 && rows[0][3] == 0, "first wound owns one fifteen-second timer");
   int first = rows[0][4];
   verify(body->diagnose_wounds("left arm") == "$P left arm has sustained 1 bruise.\n", "single wound diagnosis");
   body->hurt_us(20, "left arm");
   verify(sizeof(body->query_wounds("left arm")) == 2 && wound_sum(body, "left arm") == 30, "second damage appends wound");
   verify(sizeof(wound_timers(body)) == 1 && wound_timers(body)[0][4] == first, "additional wounds retain one pending timer");
   verify(strindex(body->diagnose_wounds("left arm"), "1 bruise and 1 scratch") >= 0, "multiple wound types use nil separator default");
   body->heal_us(5, "left arm");
   verify(body->query_wounds("left arm")[0] == 5 && body->query_wounds("left arm")[1] == 20, "partial healing reduces oldest wound");
   consistent(body, "left arm", "partial healing preserves health and wound total");
   body->heal_us(7, "left arm");
   verify(sizeof(body->query_wounds("left arm")) == 1 && body->query_wounds("left arm")[0] == 18, "healing crosses wound boundary");
   consistent(body, "left arm", "cross-boundary healing preserves total");
   body->heal_us(50, "left arm");
   verify(body->query_health("left arm") == 100 && nilp(body->query_wounds("left arm")), "complete healing caps health and removes wounds");
   verify(!sizeof(wound_timers(body)) && !query_call_out(first), "last healed wound cancels timer");
   for (int i = 0; i < 12; i++) body->hurt_us(1, "left arm");
   verify(wound_sum(body, "left arm") == 12 && sizeof(body->query_wounds("left arm")) <= 10, "small wounds coalesce without losing damage");
   consistent(body, "left arm", "coalesced wounds preserve total");
   verify(strindex(body->diagnose_wounds("left arm"), "bruises") >= 0, "repeated wound diagnosis uses mudlib pluralization");
   int id = wound_timers(body)[0][4];
   body->heal_all();
   verify(!sizeof(body->query_all_wounds()) && !sizeof(wound_timers(body)) && !query_call_out(id), "heal all clears wounds and pending timer");
   efun::destruct(body);
}
private void reset_disable_and_death()
{
   object body = make_body("reset");
   body->hurt_us(10, "left arm"); body->hurt_us(15, "right arm");
   int id = wound_timers(body)[0][4];
   body->set_max_limb_health("left arm", 80);
   verify(body->query_health("left arm") == 80 && nilp(body->query_wounds("left arm")), "limb maximum reset clears its wounds");
   verify(sizeof(wound_timers(body)) == 1 && wound_timers(body)[0][4] == id, "other wounded limb retains timer after reset");
   body->set_max_limb_health("right arm", 100);
   verify(!sizeof(wound_timers(body)) && !query_call_out(id), "last reset limb cancels timer");
   body->hurt_us(10, "left arm"); body->hurt_us(10, "right arm");
   id = wound_timers(body)[0][4];
   body->disable_limb("left arm");
   verify(body->query_health("left arm") == 0 && nilp(body->query_wounds("left arm")), "disabled limb drops wound record");
   verify(sizeof(wound_timers(body)) == 1 && wound_timers(body)[0][4] == id, "other wounded limb retains timer after disable");
   body->disable_limb("right arm");
   verify(!sizeof(wound_timers(body)) && !query_call_out(id), "last disabled limb cancels timer");
   verify(strindex(body->diagnose_wounds("left arm"), "completely disabled") >= 0, "disabled limb diagnosis");
   body->heal_all();
   body->hurt_us(40, "left arm");
   id = wound_timers(body)[0][4];
   body->health_periodic();
   verify(body->query_periodic_calls() == 1 && !query_call_out(id) && sizeof(wound_timers(body)) == 1, "manual periodic invocation replaces pending timer");
   consistent(body, "left arm", "manual periodic update preserves wound total");
   id = wound_timers(body)[0][4];
   body->reincarnate();
   verify(body->query_periodic_calls() == 2 && !query_call_out(id) && sizeof(wound_timers(body)) == 1, "reincarnation update retains one timer");
   consistent(body, "left arm", "reincarnation update preserves wound total");
   id = wound_timers(body)[0][4];
   mixed problem = catch(body->kill_us());
   verify(stringp(problem) && strindex(problem, "WOUNDS_TEST_STOP_DEATH") >= 0 && body->query_ghost(), "death reaches controlled parent boundary");
   verify(!sizeof(wound_timers(body)) && !query_call_out(id), "death cancels wound timer before parent death handling");
   efun::destruct(body);
   body = make_body("destroy"); body->hurt_us(20, "left arm");
   id = wound_timers(body)[0][4];
   efun::destruct(body);
   verify(!body && !query_call_out(id), "destruction removes owned wound timer");
}
private void check_elapsed()
{
   if (!timed->query_periodic_calls()) {
      if (++polls > 420) throw("WOUNDS_FAIL: timer firing deadline");
      call_out(check_elapsed, 0.05);
      return;
   }
   verify(timed->query_periodic_calls() == 1, "real fifteen-second wound callback fires once");
   verify(!query_call_out(timed_id), "fired one-shot timer retires");
   mixed *rows = wound_timers(timed);
   verify(sizeof(rows) == 1 && rows[0][4] != timed_id && rows[0][2] > 0 && rows[0][2] <= 15000 && rows[0][3] == 0,
          "elapsed callback rearms one fifteen-second timer");
   verify(timed->query_periodic_player() == timed, "elapsed callback retains configured player");
   consistent(timed, "left arm", "elapsed periodic update preserves wound total");
   verify(!cancelled->query_periodic_calls() && !sizeof(wound_timers(cancelled)) && !query_call_out(cancelled_id), "cancelled timer never fires");
   int id = rows[0][4];
   timed->health_periodic();
   verify(timed->query_periodic_calls() == 2 && !query_call_out(id) && sizeof(wound_timers(timed)) == 1, "manual update after firing does not duplicate timer");
   id = wound_timers(timed)[0][4];
   timed->heal_all();
   verify(!sizeof(wound_timers(timed)) && !query_call_out(id) && !sizeof(timed->query_all_wounds()), "healing after elapsed callback cancels rearmed timer");
   efun::destruct(timed); efun::destruct(cancelled); efun::destruct(room);
   verify(!sizeof(object_clones(load_object(ACTOR))), "all wound test clones cleaned up");
   dump("WOUNDS_TOTAL: " + checks + "\n");
   efun::shutdown(0);
}
private void run_tests()
{
   set_privilege(1);
   room = clone_object(INDOOR_ROOM);
   damage_and_healing();
   reset_disable_and_death();
   timed = make_body("timed");
   object previous = set_this_player(timed);
   timed->hurt_us(40, "left arm");
   set_this_player(previous);
   timed_id = wound_timers(timed)[0][4];
   cancelled = make_body("cancelled"); cancelled->hurt_us(20, "left arm");
   cancelled_id = wound_timers(cancelled)[0][4];
   cancelled->heal_all();
   call_out(check_elapsed, 0.05);
}
string *epilog(int ignored) { call_out(run_tests, 0); return ({}); }
