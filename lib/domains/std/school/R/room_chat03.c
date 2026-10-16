#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to add function as room chat");
    set_long("\n"
             "A function can be used in a room chats, by putting it in (: :).\n"
             "This example uses the function time() to return the current time,\n"
             "then ctime() to convert it to a 'sensible' form.\n"
             "\n"
             "  set_room_chat( (: ctime(time()):), 5);\n"
             "\n"
    );

  set_room_chat( (: ctime(time()):), 5);

    set_exits( ([
	"backward" : R "room_chat02",
	"finish" : R "room",
      ]) );
    set_objects( ([
      ]) );
}
