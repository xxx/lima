#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Emote Tutorial 7");
  set_long("targetemote\n"
           "----------------------------------------------------------------------\n"
           "This is the other autoemote command.  It creates an emote with four\n"
           "basic rules.\n"
           "  Syntax:\n"
           "    targetemote <emote>\n"
           "    ie: \"targetemote smile\"\n"
           "This would create a smile emote with the following rules:\n"
           "  \"LIV\" -> $N $vsmile at $t.  // Living arg.\n"
           "  \"OBJ\" -> $N $vsmile at $o.  // Object arg.\n"
           "  \"STR\" -> $N $vsmile $o.     // String arg.\n"
           "  \"\"    -> $N $vsmile.        // Default\n"
  );
  set_exits( ([
    "corridor" : R + "corridor06",
    "south" : R + "emote06",
   ]) );
  set_objects( ([
   ]) );
}
