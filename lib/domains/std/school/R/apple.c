#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("How to make food");
  set_long("\n"
           "Start as with any simple object, but inherit FOOD instead of OBJ\n"
           "\n"
           "Add these extras within setup()\n"
           "\n"
           "// Set the number of bites to finish the food\n"
           "    set_num_eats(5);\n"
           "\n"
           "// Set the message for eating a normal bite\n"
           "    set_eat_action(\"$N $vtake a bite from an apple.\");\n"
           "\n"
           "// Set the message for eating the last bite\n"
           "    set_last_eat_action(\"$N $veat the last of $p apple.\");\n"
           "}\n"
           "\n"
  );
  set_exits( ([
    "pantry" : R + "pantry",
   ]) );
  set_objects( ([
    "/domains/std/consumable/apple" : 1,
   ]) );
}
