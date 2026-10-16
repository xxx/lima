// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 2");
  set_long("Here you will learn the most basic stuff.  For example, how\n"
           "to get started editing a file.\n"
           "   To start editing a file, just type ed file_name.c\n"
           "This turns the editor on and also puts you in the specified file.\n"
           "If you just want to start from scratch, you can just type ed.\n"
           "Now, when the editor first comes on, line numbers will not\n"
           "appear with your text.  The <n> command toggles the line numbers.\n"
           "   Since this is a LINE editor, you can only work on 1 line at\n"
           "a time.  By typing a period, it will show you your current line.\n"
           "Hitting return will advance you to the next line and show you that one.\n"
           "You can also see a single line by just typing the number of\n"
           "the line you want to see.  This will place you on that line\n"
           "and show it to you also.\n"
           "   If at any time, you want to see a list of the commands in ED, just\n"
           "type <h> while in Ed.\n"
           "   Lastly, there is a function called <auto-indent>.  The capital\n"
           "I is the command to auto-indent your code.  This is a must\n"
           "if you want other wizards to read your code.  It makes it nice\n"
           "and neat and readable.  Use it on all your code.\n"
           "\n"
  );
  set_exits( ([
    "south" : R + "edtutor01",
    "north" : R + "edtutor03",
   ]) );
  set_objects( ([
   ]) );
}
