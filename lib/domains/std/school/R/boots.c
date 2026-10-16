#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make basic boots");
    set_long("inherit ARMOUR;\n"
             "inherit M_READABLE;\n"
             "\n"
             "void setup() {\n"
             "    set_adj(\"Black\",\"black\",\"feet\");\n"
             "    set_armour_class (3);\n"
             "    set_id(\"boots\",\"Boots\",\"armour\",\"armour\");\n"
             "    add_id(\"inscription\");\n"
             "    set_long(\"These are black boots of the darkest black leather you have ever seen. \\n\"\n"
             "             \"They are sturdy looking boots. On the boots is a inscription.\\n\"\n"
             "    );\n"
             "    set_text(\"The inscription reads 'I am boots of swift, wear me, love me for I protect\\n\"\n"
             "             \"your feet everywhere you go.'\\n\"\n"
             "    );\n"
             "    set_weight(1.0);\n"
             "    set_slot(\"feet\");\n"
             "}\n"
    );
    set_exits( ([
	"armoury" : R "armoury",
      ]) );
}
