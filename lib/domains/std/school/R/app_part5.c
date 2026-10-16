#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Area Construction -- part 5");
    set_long("Then, you go into the item descriptions for the items described in the other\n"
             "item descriptions. It's sort of like layering the descriptions based on how you\n"
             "would really look at them when you enter a room. At first glance, you see the\n"
             "basics. Next, you look at groups of objects such as posters and windows. Then,\n"
             "you look at specifics such as TV and lamp. A good idea is to make a room that\n"
             "is EXACTLY like your real life bedroom(like I just did), and use it for guide\n"
             "for future rooms. Don't forget, just because I didn't say there was a ceiling,\n"
             "doesn't mean it isn't there. And, it may have something unusual on it, such as\n"
             "a confederate flag or christmas lights that you wouldn't necessarily see just\n"
             "walking into the room unless you looked specifically at the ceiling. \n"
             "\n"
             "5) Avoid the use of \"you\" and \"your\" in room descriptions: Not only does this\n"
             "make for a bad description, especially in rooms, it makes the MUD god angry.\n"
             "Start a description with something like \"A long, dark hallway stretches east\n"
             "and west.\" Item descriptions are ok to use you and your.\n"
    );
    set_exits( ([
	"area4" : R + "app_part4",
	"area6" : R + "app_part6",
      ]) );
    set_objects( ([
      ]) );
}
