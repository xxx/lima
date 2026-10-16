#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Corridor");
    set_long("This corridor leads new wizards onwards from the school,\n"
             "gaining knowledge as they proceed.\n"
    );
    set_exits( ([
    "edtutor" : R "edtutor01",
    "backward" : R "school",
    "forward" : R "corridor02",
      ]) );
    set_objects( ([
      ]) );
}
