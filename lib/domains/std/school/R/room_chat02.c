#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to add choice of room chat");
    set_long("\n"
             "A choice of room chats can be give, by putting them in a string array:\n"
             "\n"
             "  set_room_chat( ({\n"
             "    \"This is one of the options\",\n"
             "    \"This is a different choice\",\n"
             "    \"Yet another option\",\n"
             "    }),\n"
             "    5);\n"
             "\n"
    );

  set_room_chat( ({
    "This is one of the options",
    "This is a different choice",
    "Yet another option",
    }),
    5);

    set_exits( ([
	"backward" : R "room_chat01",
	"forward" : R "room_chat03",
      ]) );
    set_objects( ([
      ]) );
}
