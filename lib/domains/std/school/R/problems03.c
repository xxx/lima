#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Further information from dbxframe");
    set_long("Each of the steps which led up to an error can be shown with dbxframe.\n"
             "This will display a few lines of code either side, and variable values.\n"
             "Using the previous example, and abbreviating the display:\n"
             "'dbxframe 3'\n"
             "------/obj/secure/shell.c:116 - shell_input(\"tail execute\")\n"
             "locals:\n"
             "---------------------------------------------------------------\n"
             "        input = input[1..];\n"
             "    else\n"
             "        input = expand_alias(input);\n"
             "    if ( input != \"\" )\n"
             "=>      execute_command(input);\n"
             "}\n"
             "\n"
    );
    set_exits( ([
	"backward" : R "problems02",
	"corridor" : R "corridor09",
      ]) );
    set_objects( ([
      ]) );
}
