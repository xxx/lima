#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Extending a simple object");
    set_long("First inherit additional modules:\n"
             "To make it 'gettable' add M_GETTABLE, for reading add M_READABLE\n"
             "\n"
             "inherit M_GETTABLE;\n"
             "inherit M_READABLE;\n"
             "\n"
             "More can now be set in the setup function -\n"
             "untouched_desc for how it shows in the room before being touched\n"
             "text for what you see when you read it\n"
             "\n"
             "    set_untouched_desc(\"A map of Lima Bean is pinned to the wall.\");\n"
             "    set_text(\"             MORTAL AREA   ATTIC\\n\"\n"
             "             \"                  |       /\\n\"\n"
             "             \"                  |      /  QUIET ROOM\\n\"\n"
             "             \"    LAVA CAVE     |     /  /\\n\"\n"
             "             \"             \\\\    |    /  /       PLAINS\\n\"\n"
             "             \"              \\\\   |  U/  /           |\\n\"\n"
             "             \"  ELEVATOR -- WIZARD HALL ---- EXAMPLE ROOM -- CAR WASH\\n\"\n"
             "             \"             /D   |                  |\\n\"\n"
             "             \"            /     |                  |\\n\"\n"
             "             \"GENERAL STORE     |          RELATIONS & TOYS  -- ENVIRONMENTAL ROOM\\n\"\n"
             "             \"               MONSTER ROOM          |\\n\"\n"
             "             \"                                     |\\n\"\n"
             "             \"                               CULT OF LIMA\\n\");\n"
    );
    set_exits( ([
	"west" : R + "object01",
	"east" : R + "object03",
      ]) );
    set_objects( ([
  "/domains/std/object/map" : 1,
      ]) );
}
