// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 12");
  set_long("ed testroom.c\n"
           "n\n"
           "1z\n"
           "z\n"
           "x\n"
           "\n"
           "ed testroom.c\n"
           "n\n"
           "1,$p\n"
           "x\n"
           "\n"
           "\n"
           "ed testroom.c\n"
           "n\n"
           "1,$p\n"
           "w\n"
           "q\n"
           "\n"
           "Emergency way out \n"
           "ed testroom.c\n"
           "n\n"
           "Q\n"
  );
  set_exits( ([
    "south" : R + "edtutor11",
    "corridor" : R + "corridor01",
   ]) );
  set_objects( ([
   ]) );
}
