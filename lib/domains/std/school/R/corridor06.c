#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Corridor");
  set_long("This corridor leads new wizards onwards from the school,\n"
           "gaining knowledge as they proceed.\n"
  );
  set_exits( ([
    "weaponry" : R "weaponry",
    "armoury" : R "armoury",
    "backward" : R "corridor05",
    "forward" : R "corridor07",
    "emoting" : R "emote01",
   ]) );
  set_objects( ([
   ]) );
}
