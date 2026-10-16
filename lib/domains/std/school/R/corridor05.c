#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Corridor");
    set_long("This corridor leads new wizards onwards from the school,\n"
             "gaining knowledge as they proceed.\n"
    );
    set_exits( ([
    "objects" : R "object01",
    "backward" : R "corridor04",
    "forward" : R "corridor06",
      ]) );
    set_objects( ([
      ]) );
}
