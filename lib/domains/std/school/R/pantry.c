#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Wizard's Pantry [home of the foodstuffs]");
  set_long("Among the myriad shelves here in the pantry can be found all types of\n"
           "foodstuffs.  Untold time has been spent in here in preparations of many\n"
           "good times and a few fiascos.  Explore though these rooms and learn.\n"
  );

  add_item("shelves",  
   ([ "look" :("These huge brown shelves are carefully bolted to the wall.\n"),
   ]) );


  add_item("foodstuffs",  
   ([ "look" :("These are the various foods and drinks kept here.\n"),
   ]) );

  set_exits( ([
    "corridor" : R "corridor07",
    "apple" : R + "apple",
    "ale" : R + "ale",
   ]) );
  set_objects( ([
   ]) );
}
