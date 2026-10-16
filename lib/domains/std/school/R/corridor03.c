#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Corridor");
    set_long("This corridor leads new wizards onwards from the school,\n"
             "gaining knowledge as they proceed.\n"
    );
    set_exits( ([
    "areas" : R "app_part1",
    "backward" : R "corridor02",
    "forward" : R "corridor04",
      ]) );
    set_objects( ([
      ]) );
}
