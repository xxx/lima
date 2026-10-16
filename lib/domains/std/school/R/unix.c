#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Unix Commands");
    set_long("Basic Unix Commands (Write these down)\n"
             "cd - change directory\n"
             "ls - list the files in the current directory\n"
             "pwd - show (print) working directory\n"
             "more - view the specified file, with pauses\n"
             "cat - view the specified file, without pauses\n"
             "tail - view the last few lines of specified file\n"
             "ed - edit the specified file\n"
             "rm - removes file\n"
             "mv - move or rename\n"
             "mkdir - make directory\n"
             "rmdir - remove directory\n"
             "grep - search file(s) for specified text\n"
             "\n"
             "Use 'help <command>' for further details\n"
);

    set_exits( ([
	"corridor" : R + "corridor02",
      ]) );
}
