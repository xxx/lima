// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 4");
  set_long("Once you have made a file, you want to save it.  There\n"
           "are many ways to save and or quit in ED.  The easiest way to both\n"
           "save and quit is by typing <x>.  The only thing is that you must\n"
           "be editing a file that already exists.  If you just typed ED\n"
           "when you started, ED will not know the name to save your\n"
           "file under so it will not allow you to quit.\n"
           "   There are also times when you will not want to save when\n"
           "you quit, for instance when you make a mistake and you want\n"
           "to get back to the origional file.  In this case you can\n"
           "type <Q> or <q>.  The capital one will exit you from Ed\n"
           "no matter what and the lower case one will exit you\n"
           "only if the file has not been changed since you opened it\n"
           "or saved last.\n"
  );
  set_exits( ([
    "south" : R + "edtutor03",
    "north" : R + "edtutor05",
   ]) );
  set_objects( ([
   ]) );
}
