#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Area Construction -- part 2");
    set_long("There are two preferred styles of subdirectory structure I will accept:\n"
             "A F R W M O    or armour   food    rooms    weapon   npc   obj\n"
             "Please name your area appropriately.  area/  is not acceptable but\n"
             "caves/  is IF you area is caves.\n"
             "\n"
             "Reminder:  no YOU or YOUR in any form is acceptable in your descriptions.\n"
             "You need 3 lines of 75 columns in your descriptions as well.\n"
             "\n"
             "Domain Lords:  Rules apply to you as well.\n"
             "\n"
             "All rooms will contain items and will be dense.  By dense I mean every\n"
             "room will have something to do in them, not necessarily just hack and slay.\n"
             "Of course, there is more to an area than just rooms.  Monsters, armour,\n"
             "Weapons, Objects also need to be done for an area to be complete.\n"
             "\n"
    );
    set_exits( ([
	"area3" : R + "app_part3",
	"area1" : R + "app_part1",
      ]) );
    set_objects( ([
      ]) );
}
