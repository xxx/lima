#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Room set_long - alternate style");
  set_long("\
Similarly to old style, the section starts and finishes with tokens.
In this case quotes at both ends, and backslash at the beginning.
No quotes are needed at the ends of lines.

As in the new style, \
lines can be broken at the end of clauses and sentences, \
provided that they are followed by a backslash, \
which allows the player's client to handle line wrapping.
"
  +  "\nset_long(\"\\\n" + "Similarly to old style, the section starts and finishes with tokens.\n"
                           "In this case quotes at both ends, and backslash at the beginning.\n"
                           "No quotes are needed at the ends of lines.\n"
                           "\n"
                           "As in the new style, \\\n"
                           "lines can be broken at the end of clauses and sentences, \\\n"
                           "provided that they are followed by a backslash, \\\n"
                           "which allows the player's client to handle line wrapping.\n"
  + "\");\n"
 );
    set_exits( ([
	"backward" : R "set_long03",
	"finish" : R "room",
      ]) );
    set_objects( ([
      ]) );
}
