#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Optional extras for a simple object");
    set_long("Adding some weight:\n"
             "within setup(), use a floating-point mass in kilograms:\n"
             "\n"
             "    set_weight(1.0);\n"
             "\n"
             "Objects already provide value support. Set a floating-point value in setup():\n"
             "\n"
             "    set_value(1.0);\n"
    );
    set_exits( ([
	"west" : R + "object02",
	"corridor" : R + "corridor05",
      ]) );
    set_objects( ([
      ]) );
}
