#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("New Wizard Information -- part 2");
    set_long("\n"
             "ed will allow you to edit a file\n"
             "\n"
             "update will attempt to compile and load the file\n"
             "\n"
             "clone will produce a copy of the object in the file\n"
             "\n"
             "dest will destruct an item\n"
             "\n"
);
    set_exits( ([
	"south" : R "newwiz2",
	"academy" : R "school",
      ]) );
    set_objects( ([
      ]) );
}
