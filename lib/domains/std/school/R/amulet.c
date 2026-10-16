#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make a basic amulet");
    set_long("inherit ARMOUR;\n"
             "\n"
             "void setup() {\n"
             "    set_adj(\"Dusty\",\"dusty\",\"neck\");\n"
             "    set_armour_class (4);\n"
             "    set_id(\"Desert Amulet\",\"Amulet\",\"amulet\",\"desert amulet\",\"armour\",\"armour\");\n"
             "    set_long(\"This is an amulet of the Mythryl Desert Valley. It seems to have some sort of\\n\"\n"
             "             \"magical aura because it is glowing a little and is making somewhat of a faint\\n\"\n"
             "             \"noise.\\n\"\n"
             "    );\n"
             "    set_weight(0.2);\n"
             "    set_slot(\"neck\");\n"
             "}\n"
    );
    set_exits( ([
	"armoury" : R "armoury",
      ]) );
    set_objects( ([
      ]) );
}
