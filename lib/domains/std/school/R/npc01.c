#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make a simple npc");
    set_long("Start with a file similar to simple object - but inheriting ADVERSARY.\n"
             "Add various names and descriptions - id, in_room_desc and long.\n"
             "\n"
             "inherit ADVERSARY;\n"
             "void setup() {\n"
             "    set_id(\"dragon\", \"tiamat\");\n"
             "    set_in_room_desc(\"Tiamat the Dragon\");\n"
             "    set_long(\"Getting close enough to see what a dragon looks like\\n\"\n"
             "             \"is not a good idea.\\n\");\n"
             "\n"
             "}\n"
    );
    set_exits( ([
	"monsters" : R "zoo",
	"forward" : R "npc02",
      ]) );
    set_objects( ([
      ]) );
}
