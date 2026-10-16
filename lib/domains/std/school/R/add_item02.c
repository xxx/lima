#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Add_items for multiple names");
    set_long("\n"
             "You can use one description to relate to multiple names.\n"
             "Just include all the alternatives as an array at the beginning.\n"
             "\n"
             "  add_item( \"desk\", \"table\", \"furniture\" ,\n"
             "      ([ \"look\" :\n"
             "         \"It defies belief, it's actually a desk made of dark matter.  \"\n"
             "         \"Surely, it takes a great mind to use it, yet alone work on it.  \"\n"
             "         \"To even place anything on the surface would destroy it.  \"\n"
             "         \"There is only one drawer made of the same material.\"\n"
             "         \"\\n\",\n"
             "      ]) );\n"
    );

  add_item( "desk", "table", "furniture" ,
      ([ "look" :
         "It defies belief, it's actually a desk made of dark matter.  "
         "Surely, it takes a great mind to use it, yet alone work on it.  "
         "To even place anything on the surface would destroy it.  "
         "There is only one drawer made of the same material."
         "\n",
      ]) );

    set_exits( ([
	"backward" : R + "add_item01",
	"forward" : R + "add_item03",
      ]) );
    set_objects( ([
      ]) );
}
