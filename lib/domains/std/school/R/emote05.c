#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Emote Tutorial 5");
  set_long("Additional Notes:\n"
           "additional emote commands:\n"
           "rmemote\n"
           "----------------------------------------------------------------------\n"
           "This command is used to remove emotes from the soul or to remove one\n"
           "rule of an emote from the soul.\n"
           "  Syntax:\n"
           "    rmemote <emote>\n"
           "    ie: \"rmemote smile\"\n"
           "      This removes the entire emote and all of its rules.\n"
           "    rmemote <emote> <rule>\n"
           "    ie: \"rmemote smile LIV\"\n"
           "      This removes only the rule and the message for that rule. It\n"
           "      leaves all other rules alone.\n"
           "\n"
  );
  set_exits( ([
    "north" : R + "emote06",
    "south" : R + "emote04",
   ]) );
  set_objects( ([
   ]) );
}
