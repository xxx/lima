inherit OBJ;
inherit "/std/adversary/pulse/non_heart_beat";

private object target;
private int attacks;
private object attack_player;
private int fail_attack;
private int stop_during_attack;

void switch_to(object who) { target = who; }
object get_target() { return target; }
void attack()
{
   attacks++;
   attack_player = efun::this_player();
   if (stop_during_attack) stop_attacking();
   if (fail_attack) throw("PULSE_TEST_ATTACK_FAILURE");
}
int query_attacks() { return attacks; }
object query_attack_player() { return attack_player; }
void set_fail_attack(int value) { fail_attack = value; }
void set_stop_during_attack(int value) { stop_during_attack = value; }
void unrelated_callback() { throw("PULSE_TEST_UNUSED: unrelated timer"); }
int unrelated_timer() { return call_out(unrelated_callback, 60); }
