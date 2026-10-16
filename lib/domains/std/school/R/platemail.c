#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make platemail");
    set_long("inherit ARMOUR;\n"
             "\n"
             "void setup() {\n"
             "    set_adj(\"Black\",\"black\",\"torso\");\n"
             "    set_armour_class (8);\n"
             "    set_id(\"platemail\",\"Platemail\",\"mail\",\"plate\",\"armour\",\"armour\");\n"
             "    set_long(\"A black platemail that is made out of some strange substance. It has\\n\"\n"
             "             \"no refelctive surfaces but shapes in the form of a black storm trooper's \\n\"\n"
             "             \"torso shape.\\n\"\n"
             "    );\n"
             "    set_weight(25.0);\n"
             "    set_slot(\"torso\");\n"
             "}\n"
    );
    set_exits( ([
	"armoury" : R "armoury",
      ]) );
    set_objects( ([
      ]) );
}
