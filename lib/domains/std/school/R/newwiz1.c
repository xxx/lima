#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("New Wizard Information -- part 1");
    set_long("\n"
             "New wizards need to know a few basic commands and how to get started.\n"
             "\n"
             "First type wiz /on    -- this turns on the wiz line.  type wiz hello\n"
             "\n"
             "Next type set PROMPT %p >\n"
             "This after you type cd  will show /wiz/yourname >\n"
             "\n"
    );
    set_exits( ([
	"down" : R + "school",
	"north" : R + "newwiz2",
      ]) );
    set_objects( ([
      ]) );
}
