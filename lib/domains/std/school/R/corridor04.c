#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Corridor");
    set_long("This corridor leads new wizards onwards from the school,\n"
             "gaining knowledge as they proceed.\n"
    );
    set_exits( ([
    "rooms" : R "room",
    "backward" : R "corridor03",
    "forward" : R "corridor05",
      ]) );
    set_objects( ([
      ]) );
}
