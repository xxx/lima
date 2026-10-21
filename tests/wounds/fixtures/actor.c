inherit "/std/race/human";
private int periodic_calls;
private object periodic_player;
int query_periodic_calls() { return periodic_calls; }
object query_periodic_player() { return periodic_player; }
void health_periodic()
{
   periodic_calls++;
   periodic_player = efun::this_player();
   ::health_periodic();
}
protected void die() { throw("WOUNDS_TEST_STOP_DEATH"); }
