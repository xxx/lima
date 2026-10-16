// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 9");
  set_long("The search command lets you look for the specified string\n"
           "and it looks like </TEXT>.  This command will find the next\n"
           "occurance of TEXT in your code.  Remember this command is case\n"
           "sensitive like all the other commands in ED.  If you want\n"
           "to see all the occurances of TEXT in your code, you can\n"
           "find them by typing <g/TEXT/p> meaning globally find TEXT\n"
           "and print it out on the screen.\n"
  );
  set_exits( ([
    "south" : R + "edtutor08",
    "north" : R + "edtutor10",
   ]) );
  set_objects( ([
   ]) );
}
