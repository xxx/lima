#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Some improvements to a simple shopkeeper");
    set_long("\n"
             "Start with the simple shopkeeper as specified in the previous room\n"
             "\n"
             "Within the setup, there are many options available.\n"
             "// Determine what type of currency we'll use\n"
             "    set_currency_type(\"gold\");\n"
             "\n"
             "// Sets shopkeeper to sell things (0 for nothing)\n"
             "    set_for_sale(1);\n"
             "\n"
             "// Sets shopkeeper to buy things (0 for nothing)\n"
             "    set_will_buy(1);\n"
             "\n"
             "// Set basis for selling price (ie value will be multiplied by this)\n"
             "// Default = 1.5\n"
             "    set_cost_multiplicator(1.5);\n"
             "}\n"
    );
    set_exits( ([
	"finish" : R "zoo",
	"backward" : R "shopkeeper01",
      ]) );
    set_objects( ([
    "/domains/std/shopkeeper" : 1,
      ]) );
}
