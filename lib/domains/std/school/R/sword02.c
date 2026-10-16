#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to add custom combat messages");
    set_long("\n"
             "Continuing with the basic sword, use add_combat_message() within setup.\n"
             "This takes 2 arguments :\n"
             "type (a string, such as dam1 - dam10 for various levels of damage)\n"
             "message, using same tokens as in souls, custom entry messages etc\n"
             "\n"
             "    add_combat_message(\"miss\", \"$N $vtake a clumsy swipe at $t, but only $vsucceed in making a fool of $r.\");\n"
             "\n"
             "See the standard combat messages in MESSAGES_D (/daemons/messages_d)\n"
             "for a variety of examples.\n"
             "\n"
    );
    set_exits( ([
        "backward" : R "sword01",
        "weaponry" : R "weaponry",
      ]) );
    set_objects( ([
      "/domains/std/weapon/sword" : 1,
      ]) );
}
