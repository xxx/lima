#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Emote Tutorial 1");
  set_long("command   : addemote\n"
           "help file : /help/wizard/bin/addemote\n"
           "\n"
           "Be sure to read the real help files along with this little tutorial to\n"
           "be sure that I don't forget anything.  This command comes in three (3)\n"
           "steps.  The first is to enter the name of the emote.  This is done as\n"
           "follows: \"addemote <verb>\" (ie: \"addemote smile\").\n"
           "\n"
           "The next step is to enter the rule.  Let me give some examples to\n"
           "explain.  If I was to type \"smile lancelot\", it would call the emote\n"
           "\"smile\" and since lancelot is a living, it would apply the rule \"LIV\".\n"
           "If I was to type \"smile broadly lancelot\", it would call the rule\n"
           "\"STR LIV\" b/c broadly is a string and lancelot is alive.  The main\n"
           "rules are: LIV = living in room (Players/Monsters), OBJ = Object in\n"
           "room, and STR = anything else.  You can also mix text in with the rule.\n"
           "Like if I typed: \"smile broadly at lancelot\" it would apply the rule:\n"
           "\"STR at LIV\", get it?\n"
           "\n"
  );
  set_exits( ([
    "north" : R + "emote02",
    "corridor" : R + "corridor06",
   ]) );
  set_objects( ([
   ]) );
}
