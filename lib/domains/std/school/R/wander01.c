#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Wandering npc");
    set_long("\n"
             "Adapt from the example used in the basic or simple npc.\n"
             "\n"
             "Start by inheriting WANDERING_MONSTER instead of ADVERSARY\n"
             "inherit WANDERING_MONSTER;\n"
             "\n"
             "Within setup, handle the actual wandering:\n"
             "Set the area where it will wander (see also next room)\n"
             "    set_wander_area(\"wiz_area\");\n"
             "\n"
             "Set time between movements\n"
             "    set_wander_time(5);\n"
             "\n"
             "Set number of moves before it stops wandering.\n"
             "If a player is met, the count restarts.\n"
             "    set_max_moves(10);\n"
             "\n"
             "Start wandering\n"
             "    call_out(start_wandering, 0);\n"
             "\n" );

    set_area("wiz_area");

    set_exits( ([
	"backward" : R "zoo",
	"forward" : R "wander02",
      ]) );

         set_objects( ([
      ]) );
}
