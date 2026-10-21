inherit M_ACCESS;
inherit CLASS_LIMB;
private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }
private void verify(int passed, string label)
{
   if (!passed) throw("RIFTS_ADVANCEMENT_FAIL: " + label);
   checks++;
   dump("RIFTS_ADVANCEMENT_OK: " + label + "\n");
}
private void cancel_setup(object who)
{
   foreach (mixed *row in query_call_outs(who)) remove_call_out(row[4]);
}
void create()
{
   set_privilege(1);
   object npc = clone_object("/port_tests/rifts_advancement/actor");
   object body = USER_OB->make_race("/std/race/human", "riftsadvancementaudit");
   cancel_setup(npc);
   cancel_setup(body);
   verify(npc->is_adversary() && !npc->is_body() && body->is_body() && body->query_race() == "human",
          "actual NPC and caller-guarded human body constructors");
   verify(!sizeof(body->query_hp_rolls()) && !sizeof(body->query_stats()),
          "constructor leaves stats and HP rolls uninitialized");
   body->init_stats();
#if BODY_STATS == BODY_STATS_RIFTS
   body->set_stat("pe", 18);
   body->refresh_stats();
   verify(body->query_stat("pe") == 18 && body->query_health_stat() == 18,
          "paired Rifts provider supplies real PE stat");
   int endurance = 18;
#else
   verify(body->query_stat("pe") == 0 && member_array("pe", values(body->stat_abrev())) < 0,
          "Lima stat provider has no PE and supplies zero");
   int endurance = 0;
#endif
   verify(body->query_level() == 1 && body->query_experience() == 0 &&
          body->query_level_for_xp(3000) == 4 && body->query_xp_for_level(4) == 3000,
          "existing integer level and XP conversion");
   body->set_karma(100);
   verify(body->modify_karma(-1) == 96 && body->query_karma() == 96,
          "karma adaptation converts complete sum toward zero");
   body->set_karma(10000);
   verify(body->query_karma() == 1000 && body->modify_karma(100) == 1000,
          "existing karma setter getter and modifier clamp behavior");
   body->reset_hp();
   int first = body->hp_for_level(1);
   int *rolls = body->query_hp_rolls();
   verify(sizeof(rolls) == 1 && first == rolls[0] && first >= endurance + 1 && first <= endurance + 6,
          "first HP roll uses selected PE plus one d6");
   verify(body->hp_for_level(1) == first && sizeof(body->query_hp_rolls()) == 1,
          "same-level query reuses first HP roll");
   body->set_level(4);
   rolls = body->query_hp_rolls();
   int valid = sizeof(rolls) == 4 && rolls[0] == first;
   for (int i = 1; i < sizeof(rolls); i++) valid = valid && rolls[i] >= 1 && rolls[i] <= 6;
   verify(valid && body->query_level() == 4 && body->query_experience() == 3000,
          "set_level grows cache with bounded retained HP rolls");
   int total = array_sum(rolls);
   verify(body->hp_for_level(4) == total && body->hp_adjustment(100, 4) == 3 * total,
          "highest-level HP sum and multiplier");
   mapping template = BODY_D->get_body(body->query_body_style());
   class limb torso = template["torso"];
   int expected = to_int(torso.max_health / 100.0 * 3 * total);
   verify(body->query_max_health("torso") == expected,
          "actual limb health receives highest-level HP adjustment");
   int lower = body->hp_for_level(1);
   verify(lower == rolls[0] + rolls[1] && lower > first,
          "cached lower-level query includes one extra roll");
   body->set_level(1);
   expected = to_int(torso.max_health / 100.0 * 3 * lower);
   verify(body->query_level() == 1 && body->query_experience() == 0 &&
          sizeof(body->query_hp_rolls()) == 4 && body->query_max_health("torso") == expected,
          "lowering real body level retains cache and applies extra roll");
   body->reset_hp();
   verify(!sizeof(body->query_hp_rolls()), "explicit HP reset clears roll cache");
   body->update_body_style(body->query_body_style());
   rolls = body->query_hp_rolls();
   expected = to_int(torso.max_health / 100.0 * 3 * rolls[0]);
   verify(sizeof(rolls) == 1 && rolls[0] >= endurance + 1 && rolls[0] <= endurance + 6 &&
          body->query_max_health("torso") == expected,
          "reset and actual body refresh return to one PE-based roll");
   int shield = body->shield_adjustment(100, 1);
   verify(shield >= 42 && shield <= 72 && shield % 3 == 0 &&
          body->shield_adjustment(100, 4) == shield,
          "existing SDC multiplier is bounded and independent of level");
   efun::destruct(body);
   efun::destruct(npc);
   foreach (mixed *row in query_call_outs(-1)) remove_call_out(row[4]);
   verify(!objectp(body) && !objectp(npc) && !sizeof(query_call_outs(-1)),
          "owned fixtures destructed and timers cancelled");
   dump(sprintf("RIFTS_ADVANCEMENT_TOTAL: %d\n", checks));
}
