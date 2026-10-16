#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Runtime errors and dbxwhere");
    set_long("\n"
             "There will be runtime errors when files have been checked and loaded OK,\n"
             "but when running something goes wrong.\n"
             "\n"
             "The error will be reported, with file name, line number and player/npc name.\n"
             "It will also be logged.\n"
             "\n"
             "A trace of what led up to the error can be obtained with 'dbxwhere <name>'\n"
             "\n"
             "EXAMPLE:\n"
             "Fri Feb 23 07:41:29 2001\n"
             "*Attempted to read '\\0' into string!\n"
             "Object: /trans/cmds/tail at line 33\n"
             "\n"
             "#0: '<function>' at <none> (<function>) at /:0\n"
             "#1: 'dispatch_modal_input' at <none> (/secure/user/inputsys.c) at line 311\n"
             "#2: '<function>' at <none> (<function>) at /:0\n"
             "#3: 'shell_input' at <none> (/obj/secure/shell.c) at line 116\n"
             "#4: 'execute_command' at <none> (/trans/obj/wish.c) at line 240\n"
             "#5: 'call_main' at /trans/cmds/tail (obj/secure/cmd.c) at line 34\n"
             "#6: 'main' at /trans/cmds/tail at line 33\n"
             "\n"
    );
    set_exits( ([
	"backward" : R "problems01",
	"forward" : R "problems03",
      ]) );
    set_objects( ([
      ]) );
}
