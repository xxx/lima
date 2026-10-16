#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make a simple object");
    set_long("First inherit the underlying object:\n"
             "\n"
             "inherit OBJ;\n"
             "\n"
             "Then, in the setup, set the 3 basic descriptions needed:\n"
             "id is the name that you use to interact (eg look at it by 'look map')\n"
             "in_room_desc is what you see at the bottom of the room desc\n"
             "long is what you see when you look at the map\n"
             "\n"
             "void setup() {\n"
             "  set_id(\"map\");\n"
             "  set_in_room_desc( \"A map of Lima Bean has been discarded here.\");\n"
             "  set_long(\"It has a crude map of Lima Bean written on it.\");\n"
             "}\n"
    );
    set_exits( ([
	"corridor" : R + "corridor05",
	"east" : R + "object02",
      ]) );
    set_objects( ([
  "/domains/std/object/map" : 1,
      ]) );
}
