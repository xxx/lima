#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make basic armour");
    set_long("\n"
             "Start as with simple object, but inherit the file ARMOUR instead of OBJ.\n"
             "\n"
             "Within the setup, two additional items to add:\n"
             "set_armour_class(<num>) - damage absorbed will be random 1 to <num>\n"
             "set_slot(<place>) - sets where it is worn.\n"
             "Note slots are predetermined, and have size limits.\n"
             "\n"
             "inherit ARMOUR;\n"
             "\n"
             "void setup() {\n"
             "    set_adj(\"chainmail\");\n"
             "    set_id(\"shirt\", \"chainmail\");\n"
             "    set_armour_class(4);\n"
             "    set_slot(\"torso\");\n"
             "}\n"
             "\n"
    );
    set_exits( ([
	"armoury" : R "armoury",
	"forward" : "chainmail02",
      ]) );
    set_objects( ([
    "/domains/std/armour/chainmail" : 1,
      ]) );
}
