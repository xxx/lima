#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make basic gloves");
    set_long("inherit ARMOUR;\n"
             "\n"
             "void setup() {\n"
             "    set_adj(\"Black\",\"black\",\"hands\");\n"
             "    set_armour_class (3);\n"
             "    set_id(\"gloves\",\"Gloves\",\"mitts\",\"armour\",\"armour\");\n"
             "    set_long(\"These are black gloves for demonstration\\n\"\n"
             "    );\n"
             "    set_weight(0.2);\n"
             "    set_slot(\"hands\");\n"
             "}\n"
    );
    set_exits( ([
	"armoury" : R "armoury",
      ]) );
}
