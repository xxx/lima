// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 5a");
  set_long("Command mode in ED is quite like clicking on an icon in windows because you\n"
           "want to do something.  In ed it has : as a prompt and means you are going to\n"
           "tell it what you want it to do using a command.\n"
           "\n"
           "For example if I typed this:- :4s/hid/hide\n"
           "it would tell ED to substitute on line 4 hide for hid.\n"
           "\n"
           "Insert mode in ED is quite like typing on an old fashioned typewriter where\n"
           "every line has to be returned before starting on the next one.  Normally you\n"
           "enter insert mode by using a 'a, i, or c' in command mode.  This will bring up\n"
           "the numbers if you have them turned on, and will look like this.\n"
           "\n"
           "Say I wanted to add a line after 4, I would type 4a  which would bring up the\n"
           "5. This also effectively pushes the rest of the code down, so don't worry, it\n"
           "won't overwrite your code below it.  Think of a as after, i as insert or \n"
           "before, and c as change.  The next trick is to exit from insert mode back to\n"
           "command mode.  Simple on a new line type a dot . then at the : type I to\n"
           "autoindent your code, and x to exit command mode.\n"
  );
  set_exits( ([
    "south" : R + "edtutor05",
    "north" : R + "edtutor06",
   ]) );
  set_objects( ([
   ]) );
}
