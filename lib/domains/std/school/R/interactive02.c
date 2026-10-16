#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Interactive npc - responses");
    set_long("\n"
             "This is only the simple part of an extensive subject.\n"
             "\n"
             "Adapt from the simple npc and inherit M_TRIGGERS.\n"
             "\n"
             "Within setup(), add the response patterns.\n"
             "Each consists of 2 strings - trigger to match, and what to do in response.\n"
             "\n"
             "  add_pattern(\"%s laughs\", \"say Did I miss a joke?\");\n"
             "  add_pattern(\"%s chuckles\", \"smirk\");\n"
             "  add_pattern(\"%s enters\", (: \"say Hi \" + $1 + \", nice to see you !\" :) );\n"
             "Note the last case picks up who enters, in the %s,\n"
             "then uses it in $1 in the response.\n"
             "\n" );

    set_exits( ([
	"finish" : R "zoo",
	"backward" : R "interactive01",
      ]) );

   set_objects( ([
     "/domains/std/monster/harry" : 1,
     ]) );
}
