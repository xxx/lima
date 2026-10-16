#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Emote Tutorial 4");
  set_long("Additional Notes:\n"
           "  * addemote can also be used on existing emotes to modify them or to\n"
           "    add more rules.\n"
           "  * Each rule must be created individually, there are a shortcuts, but\n"
           "    we'll deal with them in one of the future lessons.\n"
           "  * To modify an existing emote rule, just pretend you are making a new\n"
           "    one.  Type \"addemote <verb>\" then enter the rule you want to change,\n"
           "    then enter the new message.  Nothing to it.\n"
           "\n"
           "Very Important Additional Note:\n"
           "  * To see all the rules and messages for an existing emote use the\n"
           "    command \"showemote\".  The syntax is \"showemote <verb>\"  This is a\n"
           "    very useful tool for learning how to code emotes b/c you can easily\n"
           "    see all the rules in action.\n"
  );
  set_exits( ([
    "north" : R + "emote05",
    "south" : R + "emote03",
   ]) );
  set_objects( ([
   ]) );
}
