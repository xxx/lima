#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Includes -- part two");
  set_long("I have defined MAIN to always mean /domains/school\n"
           "therefore R means /domains/school/R\n"
           "because R includes the define MAIN as well.\n"
           "The major advantage of this system is if I move this from one directory to\n"
           "another I merely have to redefine MAIN and everything will work.\n"
           "\n"
           "There is a second step.  How to put this into a file.\n"
           "#include \"school.h\"  \n"
           "as the top line in the code will do just that.  Then to use it in exits\n"
           "you simply do the following:\n"
           "add_exit( ([\n"
           "     \"direction\" : R + \"filename\",\n"
           "]) );\n"
           "As you can see R is the \"shortcut\" from the .h file.\n"
           "\n"
           "\n"
  );
  set_exits( ([
    "rooms" : R + "room",
    "backward" : R + "includes01",
   ]) );
  set_objects( ([
   ]) );
}
