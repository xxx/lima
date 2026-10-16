#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Corridor");
    set_long("This corridor leads new wizards onwards from the school,\n"
             "gaining knowledge as they proceed.\n"
    );
    set_exits( ([
    "monsters" : R "zoo",
    "backward" : R "corridor07",
    "forward" : R "corridor09",
      ]) );
    set_objects( ([
      ]) );
}
