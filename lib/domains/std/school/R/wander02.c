#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Wandering npc - part 2");
    set_long("\n"
             "The wandering monster's wander zone can be specified.\n"
             "If no wander zone is set, it will wander everywhere.\n"
             "If one is set, it will only wander to rooms with corresponding zone.\n"
             "\n"
             "In this example, this room and the previous room have been set thus:\n"
             "\n"
             "  set_area(\"wiz_area\");\n"
             "\n" );

    set_area("wiz_area");

    set_exits( ([
	"finish" : R "zoo",
	"backward" : R "wander01",
      ]) );
    set_objects( ([
    "/domains/std/monster/dragon" : 1,
      ]) );
}
