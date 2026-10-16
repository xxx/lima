// Yourwizname   Date   (example:  May  02 Feb 2001)

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Skeleton Room Requirements");
    set_long("#include \"/domains/std/school/school.h\"\n"
             "inherit INDOOR_ROOM;\n"
             "\n"
             "void setup() {\n"
             "    set_area(\"schoolroom\");\n"
             "    set_brief(\"Skeleton Room\");\n"
             "    set_long(\"This is your basic room (skeleton room).\\n\");\n"
             "    add_item(\"desk\", ([\"look\":\"Item descriptions go here.\\n\"]));\n"
             "    set_exits(([\"school\":R \"school\"]));\n"
             "    set_objects(([O \"map\":1]));\n"
             "}\n"
    );
    set_exits( ([
	"rooms" : R "room",
      ]) );
    set_objects( ([
      ]) );
}
