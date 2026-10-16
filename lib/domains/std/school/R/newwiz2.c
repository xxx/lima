#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("New Wizard Information -- part 2");
    set_long("\n"
             "home  takes you to your workroom\n"
             "home may   would take you to May's workroom\n"
             "\n"
             "goto  takes you to a place or a person\n"
             "goto may   would take you to May wherever she is\n"
             "\n"
             "trans may   transports may to you\n"
             "(ask first, she may not wish to come visiting)\n"
             "\n"
    );
    set_exits( ([
	"south" : R + "newwiz1",
	"north" : R + "newwiz3",
      ]) );
    set_objects( ([
      ]) );
}
