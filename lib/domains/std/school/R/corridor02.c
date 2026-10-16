#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Corridor");
    set_long("This corridor leads new wizards onwards from the school,\n"
             "gaining knowledge as they proceed.\n"
    );
    set_exits( ([
    "unix" : R "unix",
    "backward" : R "corridor01",
    "forward" : R "corridor03",
      ]) );
    set_objects( ([
      ]) );
}
