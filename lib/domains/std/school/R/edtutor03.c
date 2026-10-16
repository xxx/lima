// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 3");
  set_long("The P and L commands are used to show you what code\n"
           "you have written so far.  P shows you the general code and L\n"
           "shows you everything that P shows you, but also includes the\n"
           "control characters that may be hidden in the code.\n"
           "   Since all commands here on the Beyond the Flame are case-sensitive,\n"
           "you have to type lower case p's and l's, otherwise the\n"
           "computer won't recognize the commands.  Now, to see a line\n"
           "specifically by number, you can type #p and that will show\n"
           "you the line # with out any of the hidden characters.  If you\n"
           "want to see a range of lines, you can add in a <,> between\n"
           "two numbers like <1,15p> to show you lines 1 through 15.\n"
           "The same holds true for the l command except you will also see\n"
           "hidden characters.\n"
           "   Now you already know the <.> means current line from the last\n"
           "room, so if you type <1,.l> that will show you all the lines\n"
           "starting at 1 and ending at your current line.  Another important\n"
           "symbol is the <$>.   That means <last line>.  So, if you wanted to\n"
           "see all the lines in a single file, you would type <1,$p> or\n"
           "1 through last line, print.\n"
           "   Now that you can see all the lines of your files, you are ready\n"
           "so see how to save them so you can get out of ed.\n"
  );
  set_exits( ([
    "south" : R + "edtutor02",
    "north" : R + "edtutor04",
   ]) );
  set_objects( ([
   ]) );
}
