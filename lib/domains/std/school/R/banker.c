#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("How to make a banker");
    set_long("\n"
             "Start with a file similar to simple npc.\n"
             "Inherit ACCOUNTANT for the banker base.\n"
             "\n"
             "Additional items needed in setup() :\n"
             "\n"
             "//Short name (or id) for the bank\n"
             "  set_bank_id(\"lima\");\n"
             "\n"
             "//Formal name for the bank\n"
             "  set_bank_name(\"The Imperial Bank of the Bean\");\n"
             "\n"
             "//Type of currency it keeps accounts in\n"
             "  set_currency_type(\"dollar\");\n"
             "\n"
             "//Fee charged it your cash is in the 'wrong' currency (%)\n"
             "  set_exchange_fee(5.0);\n"
             "\n"
             "}\n"
    );
    set_exits( ([
	"monsters" : R "zoo",
      ]) );
    set_objects( ([
    "/domains/std/bank_accountant" : 1,
      ]) );
}
