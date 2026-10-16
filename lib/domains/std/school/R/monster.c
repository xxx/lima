#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make a monster wear and wield");
    set_long("inherit ADVERSARY;\n"
             "void setup() {\n"
             "    ::setup();\n"
             "    set_name(\"Monster\");\n"
             "    set_id(\"monster\");\n"
             "    set_gender(1);\n"
             "    set_proper_name(\"Frank the Monster\");\n"
             "    set_in_room_desc(\"Frank the Monster\");\n"
             "    set_long(\"  This is a very scary monster.\\n\"\n"
             "    );\n"
             "    set_wielding(\"/domains/std/weapon/sword\");\n"
             "    set_wearing(\"/domains/std/armour/chainmail\");\n"
             "}\n"
    );
    set_exits( ([
	"monsters" : R + "zoo",
      ]) );
    set_objects( ([
      ]) );
}
