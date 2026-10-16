// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 11");
  set_long("The MOVE and COPY commands are not used quite as\n"
           "often as the 's' command, but they still are quite useful.\n"
           "They either MOVE or COPY entire lines to some other place\n"
           "within your code. The commands are 't' and 'm' for COPY\n"
           "and MOVE respectively.  The line #'s you want to move or copy\n"
           "come first followed by the 't' or 'm' and then the line #\n"
           "you want the text to come AFTER comes last.  For instance,\n"
           "if you wanted to MOVE lines 10 to 25 to after line 100\n"
           "you would type <10,25 m 100>.  COPY works the same way\n"
           "but you don't take the original lines out of the code\n"
           "like you do when you use MOVE.\n"
           "   You can also import in entire copies of files from outside your\n"
           "code. This is done with the 'r' command.  It imports the file into\n"
           "your current file where ever you designate.  If you don't\n"
           "designate a line #, the imported file goes at the end of the\n"
           "current file being edited.  The import command looks like:\n"
           "<# r /path/name.c>.  You must specify the entire file path\n"
           "and name of the file being imported.\n"
  );
  set_exits( ([
    "south" : R + "edtutor10",
    "north" : R + "edtutor12",
   ]) );
  set_objects( ([
   ]) );
}
