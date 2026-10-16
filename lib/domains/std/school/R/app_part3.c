#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Area Construction -- part 3");
    set_long("Guidelines for descriptions of ANY kind:\n"
             "\n"
             "1) They must be colorful, wordwise, not colorful ANSI-wise: ANSI colored words\n"
             "don't always show up the same on everybody's screen the same. Remember that\n"
             "when coding ANYTHING with color if color is allowed in what you are coding.\n"
             "\n"
             "2) Descriptions are for painting pictures in the mind: Think of it as telling a\n"
             "short story about a room, item, etc. Remember, while writing it, how your\n"
             "favourite author describes his settings, and apply that to your setting. \n"
             "\n"
             "3) If the room has a ceiling, walls and a floor, you must have descriptions for\n"
             "all of those: Even if it's just something as simple as \"The ceiling is high\n"
             "above your head.\" If it's in the room, no matter how trivial it may seem, it\n"
             "needs a description.\n"
    );
    set_exits( ([
	"area2" : R + "app_part2",
	"area4" : R + "app_part4",
      ]) );
    set_objects( ([
      ]) );
}
