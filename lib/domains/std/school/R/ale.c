#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("How to make a drink");
  set_long("\n"
           "This example is a liquid - ie it slips through finger unless in a container\n"
           "Start as with any simple object, but inherit FLUID and M_DRINKABLE\n"
           "\n"
           "Optional extras within setup() \n"
           "\n"
           "// Custom message as it evaporates after being spilled \n"
           "    set_evaporate_action( \"The puddle of ale dwindles.\" );\n"
           "\n"
           "// Custom message as you taste it it\n"
           "    set_taste_action( ({\"It's a little bit weak, but it'll do the job.\",\"\"}) );\n"
           "\n"
           "Similar messages can be added for drink and last drink etc\n"
  );
  set_exits( ([
    "pantry" : R + "pantry",
   ]) );
  set_objects( ([
    "/domains/std/ale" : 1,
   ]) );
}
