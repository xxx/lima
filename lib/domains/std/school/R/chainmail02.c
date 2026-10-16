#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Extras for basic armour");
    set_long("\n"
             "Starting with chainmail, as in previous room:\n"
             "\n"
             "Within setup(), add custom wear messages\n"
             "Use set_resist() for strength adjustment against different damage types.\n"
             "\n"
             "\n"
             "    set_resist(\"slashing\", 2); // a little better against blades\n"
             "    set_wearmsg( \"$N $vput on a $o.\" );\n"
             "    set_slot(\"torso\");\n"
             "}\n"
             "\n"
    );
    set_exits( ([
	"finish" : R "armoury",
	"backward" : "chainmail02",
      ]) );
    set_objects( ([
    "/domains/std/armour/chainmail" : 1,
      ]) );
}
