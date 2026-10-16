#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Interactive npc");
    set_long("\n"
             "Adapt from the example used in the basic or simple npc.\n"
             "\n"
             "Start by adding periodic actions, in set_actions.\n"
             "First argument - pause between actions (seconds)\n"
             "Second argument - string for action, or string array for alternatives.\n"
             "\n"
             "  set_actions( 10, ({\n"
             "    \"say What are you waiting for?\",\n"
             "\t  \"say Hello there!\",\n"
             "\t  \"say I don't like winter.\",\n"
             "\t  \"say I don't like snow.\",\n"
             "  }) );\n"
             "\n" );

    set_area("wiz_area");

    set_exits( ([
	"backward" : R "zoo",
	"forward" : R "interactive02",
      ]) );

   set_objects( ([
     "/domains/std/monster/harry" : 1,
     ]) );
}
