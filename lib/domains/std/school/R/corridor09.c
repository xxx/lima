#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Corridor");
    set_long("This corridor leads new wizards onwards from the school,\n"
             "gaining knowledge as they proceed.\n"
    );
    set_exits( ([
    "problems" : R "problems01",
    "backward" : R "corridor08",
    "finish" : R "school",
      ]) );
    set_objects( ([
      ]) );
}
