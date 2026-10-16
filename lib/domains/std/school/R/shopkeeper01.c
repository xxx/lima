#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make a simple shopkeeper");
    set_long("\n"
             "Start with a file similar to simple npc - but inheriting M_VENDOR as well.\n"
             "\n"
             "inherit ADVERSARY;\n"
             "inherit M_VENDOR;\n"
             "\n"
             "Within the setup, specify what he sells by set_sell( ([ ]) ).\n"
             "First element of each line= filename of object to sell\n"
             "Second element = number in stock (-1 for limitless supply)\n"
             "\n"
             "    set_sell(([\n"
             "     __DIR__ \"apple\" : -1,\n"
             "      \"^std/weapon/sword\" : 3,\n"
             "     ]));\n"
             "\n"
             "}\n"
    );
    set_exits( ([
	"zoo" : R "zoo",
	"forward" : R "shopkeeper02",
      ]) );
    set_objects( ([
      ]) );
}
