#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Wizard's Weaponry");
    set_long("  The blacksmith hall where weapons are created. There is a large anvil \n"
             "and a flat screen computer floating in the air suspended by a \n"
             "mechanical armature. Along the shiny white walls are several racks.\n"
    );

    add_item("anvil",  
      ([ "look" :("Its a white hot anvil made the hardest substance you have ever seen. This \n"
                  "preheated source for smithing eliminates the need for a hearth to forge \n"
                  "weapons. A smith only has to place his item on it and state it to be \n"
                  "melted to the percentage of the object's melting point.\n"),
      ]) );


    add_item("computer",  
      ([ "look" :("Yep its a neat computer. Its marvelously thin at only 1 inch thick \n"
                  "without a cluncky box to go with it because its all imbeded inside the \n"
                  "back of the monitor like a PADD.\n"),
      ]) );

    add_item("walls",  
      ([ "look" :("The walls are bright and clean. They they seem to be made of a substance \n"
                  "that is heat sinking to keep the shop cool. There are two racks of \n"
                  "current weapon types. One consisting of lightsabers and another \n"
                  "consisting of swords. Under the racks is a sign that reads \"Under \n"
                  "Construction. More weapon types to come soon to a blacksmith near you.\"\n"),
      ]) );

    set_exits( ([
	"corridor" : R "corridor06",
	"basic" : R "sword01",
      ]) );
    set_objects( ([
      ]) );
}
