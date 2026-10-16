#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make a basic sword");
    set_long("\n"
             "Inherit WEAPON for a basic weapon.\n"
             "Set its id, adjectives, weight and value in setup().\n"
             "Set weapon_class - approx max hps of damage from a hit.\n"
             "\n"
             "inherit WEAPON;\n"
             "\n"
             "\n"
             "void setup() {\n"
             "    set_id(\"sword\");\n"
             "    set_adj(\"dull\");\n"
             "    set_weapon_class(15);\n"
             "    set_weight(1.0);\n"
             "    set_value(1000.0);\n"
             "}\n"
             "\n"
    );
    set_exits( ([
        "weaponry" : R + "weaponry",
        "forward" : R "sword02",
      ]) );
    set_objects( ([
      "/domains/std/weapon/sword" : 1,
      ]) );
}
