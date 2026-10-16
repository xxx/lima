#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Emote Tutorial 6");
  set_long("stupidemote\n"
           "----------------------------------------------------------------------\n"
           "This is one of the two commands which will automatically create basic\n"
           "emotes.  It creates an emote and adds two default rules.\n"
           "  Syntax:\n"
           "    stupidemote <emote>\n"
           "    ie: \"stupidemote smile\"\n"
           "The above example would create a smile emote with the following rules:\n"
           "  \"\"    -> $N $vsmile.    // Default\n"
           "  \"STR\" -> $N $vsmile $o. // String argument\n"
  );
  set_exits( ([
    "north" : R + "emote07",
    "south" : R + "emote05",
   ]) );
  set_objects( ([
   ]) );
}
