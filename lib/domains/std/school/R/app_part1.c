#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Area Construction -- part 1");
    set_long("Subject: requirements for area construction\n"
             "\n"
             "Lords of the domains are responsible for maintaining standards.\n"
             "\n"
             "The following rooms outline generally accepted standards.\n"
             "\n"
    );
    set_exits( ([
	"corridor" : R + "corridor03",
	"area2" : R + "app_part2",
      ]) );
    set_objects( ([
      ]) );
}
