// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 1");
  set_long("Welcome to the world of ED.  In this land you will learn about the\n"
           "editor that the Wizards use here at Beyond the Flame.  It is called ED.\n"
           "On your journey through these tutorial rooms, pay careful attention\n"
           "to the things that Ed has to say.  He is an expert when it comes to\n"
           "this editor.  He should be, it was named after him.\n"
           "   Now once you are in ED, you have a new bunch of commands that\n"
           "you must learn.  Remember, all the other commands that you\n"
           "normally use as a wizard like <goto> and <tell> will not work\n"
           "WITHIN ED.  So, what you must do if you want to use a 'regular'\n"
           "character command is put a <!> before the statement.  This\n"
           "tells ED that the command is not an ED command but is a regular\n"
           "command and allows you to execute that command.  This will\n"
           "be very helpful when you have questions to ask others when\n"
           "you are editing.\n"
           "   So to begin on your journey into the land of ED, just type\n"
           "<north>.  The same command holds true throughout these\n"
           "rooms.  To go south to the previous room, type <south>.  That is all\n"
           "you need to know to get you started using the program ED.\n"
  );
  set_exits( ([
    "north" : R + "edtutor02",
    "corridor" : R + "corridor01",
   ]) );
  set_objects( ([
   ]) );
}
