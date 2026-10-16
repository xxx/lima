// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 6");
  set_long("The add and insert commands <a> and <i> are quite\n"
           "similar.  They both put you in to the 'add text' mode.\n"
           "The difference between the 2 commands is where the new\n"
           "text will be placed.  When using the <a> command, the new\n"
           "lines are put AFTER the specified line.  If no line # is\n"
           "given, then the text is put after the CURRENT line.\n"
           "The <i> command works similarly but the text is INSERTED\n"
           "BEFORE the specified line.  The same holds true if no\n"
           "line number is given.\n"
           "   To get out of the 'add text' mode, type a <.> and\n"
           "Ed will put you back in the reqular editing mode.\n"
           "   Now, when a file is opened for the first time and no\n"
           "texts exists in the file, just type <a> and start to\n"
           "add to the file.  Remember to type <w file_name.c> to\n"
           "save it when you are ready.  Then type <q> to get out\n"
           "of ED.  Re-enter ED with <ed file_name.c> and you\n"
           "are ready to edit your file.\n"
  );
  set_exits( ([
    "south" : R + "edtutor05a",
    "north" : R + "edtutor07",
   ]) );
  set_objects( ([
   ]) );
}
