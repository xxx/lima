#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to add simple room chat");
    set_long("\n"
             "Room chat is periodic message to the room.\n"
             "\n"
             "In its simplest version it is just a string for the message,\n"
             "and an integer (number) for the period.\n"
             "\n"
             "  set_room_chat(\"This is a simple room chat\", 5);\n"
    );

  set_room_chat("This is a simple room chat", 5);

    set_exits( ([
	"backward" : R "room",
	"forward" : R "room_chat02",
      ]) );
    set_objects( ([
      ]) );
}
