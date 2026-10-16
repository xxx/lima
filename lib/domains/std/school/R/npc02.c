#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("More on the simple npc");
    set_long("\n"
             "Continuing with the previous example, add name and proper_name.\n"
             "Specify gender.\n"
             "\n"
             "Name normally used\n"
             "    set_name(\"Tiamat\");\n"
             "\n"
             "Formal name if needed\n"
             "    set_proper_name(\"Tiamat\");\n"
             "\n"
             "Use 1 for male, 2 for female, 0 for neuter.\n"
             "    set_gender(1);\n"
             "\n"
             "Set the hit points\n"
             "    set_max_health(300);\n"
             "\n" );
    set_exits( ([
	"finish" : R "zoo",
	"backward" : R "npc01",
      ]) );
    set_objects( ([
      ]) );
}
