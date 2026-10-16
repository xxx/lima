#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to do a set_long");
    set_long("Here you type in the text that you want to say about the room.\n"
             "\n"
             "This is a quick glance of what you would see when you enter the room.\n"
             "The add_item will give a fuller description of the room.\n"
             "\n"
             "Please do not tell me what is in the next room.\n"
             "Tell me about textures, colours, materials.\n"
             "\n"
             "Avoid YOU or YOUR in any description.\n"
             "\n"
             "Requirement is a minimum of 3 full lines of normal screen width (80 chars).\n"
    );
    set_exits( ([
	"backward" : R "room",
	"forward" : R "set_long02",
      ]) );
    set_objects( ([
      ]) );
}
