// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 5");
  set_long("   Lastly there is the <w> command.  This saves the file you\n"
           "are working on but does not exit you from ED.  Again\n"
           "if you are working on a file with no name, you must type\n"
           "<w file_name.c> and then <q>.  Then re-enter the file with\n"
           "<ed file_name.c>.  At this time you can just type <w> or\n"
           "<x> and the computer will know what name to save your file\n"
           "under.\n"
  );
  set_exits( ([
    "south" : R + "edtutor04",
    "north" : R + "edtutor05a",
   ]) );
  set_objects( ([
   ]) );
}
