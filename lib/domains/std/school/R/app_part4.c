#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Area Construction -- part 4");
    set_long("4) Picture yourself walking into a room: Picture yourself walking into the room\n"
             "you're describing. First, describe the room itsself at a quick glance \"A small\n"
             "room, cluttered with various things scattered along the floor. The two windows\n"
             "in the room have dark colored curtains, blocking most of the sunlight from\n"
             "entering. Posters line the walls and a bed sits in the southwest corner of the\n"
             "room.\n"
             "\n"
             "Then, you make the items in the room. First things, thing and floor \"The floor\n"
             "is cluttered with all sorts of electronic equipment on different styles of\n"
             "stands.\" Next, window, windows, curtain, and curtains \"Upon closer inspection,\n"
             "the curtains are mostly dark blue and have an Empire Strikes Back motif.\" Next,\n"
             "posters, poster, walls and wall \"The walls are papered with posters of female\n"
             "super models.\" Next, the bed \"It's just an ordinary twin sized bed covered with\n"
             "a Spider-man quilt. An unusually large lamp sits atop the headboard.\"\n"
    );
    set_exits( ([
	"area3" : R + "app_part3",
	"area5" : R + "app_part5",
      ]) );
    set_objects( ([
      ]) );
}
