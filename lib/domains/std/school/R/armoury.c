#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Wizard's armoury");
    set_long("The armoury.\n"
    );
    set_exits( ([
	"corridor" : R "corridor06",
	"amulet" : R "amulet",
	"gloves" : R "gloves",
	"shield" : R "shield",
	"boots" : R "boots",
	"platemail" : R "platemail",
	"basic" : R "chainmail01",
      ]) );
    set_objects( ([
      ]) );
}
