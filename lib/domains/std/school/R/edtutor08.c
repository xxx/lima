// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 8");
  set_long("The <c> command is for CHANGE.  This command combines\n"
           "the <d> command and the <a> command into one command.\n"
           "What this command does is delete the specified lines and\n"
           "then put you in to 'add mode'.  For instance, if you wanted\n"
           "to change the text in a line and it was too much to 'substitute'\n"
           "for, and it was on line 10, you could write <10c>.  That would\n"
           "get rid of line 10 and put you in 'add text' mode at line 10.\n"
           "   You can also delete multiple lines by using the comma.  For\n"
           "instance, you could get rid of lines 1-5 and begin to add\n"
           "text by typing <1,5c>.\n"
  );
  set_exits( ([
    "south" : R + "edtutor07",
    "north" : R + "edtutor09",
   ]) );
  set_objects( ([
   ]) );
}
