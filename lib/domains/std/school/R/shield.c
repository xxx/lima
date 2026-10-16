#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make a basic shield");
    set_long("\n"
             "inherit ARMOUR;\n"
             "\n"
             "void setup() {\n"
             "    set_adj(\"Black\",\"black\",\"right\",\"arm\");\n"
             "    add_adj(\"right\");\n"
             "    set_armour_class (10);\n"
             "    set_id(\"shield\",\"Shield\",\"armour\",\"armour\");\n"
             "    set_long(\"This is a black shield for demonstration.\\n\"\n"
             "    );\n"
             "    set_weight(5.0);\n"
             "    set_slot(\"right hand\");\n"
             "}\n"
    );
    set_exits( ([
	"armoury" : R "armoury",
      ]) );
    set_objects( ([
      ]) );
}
