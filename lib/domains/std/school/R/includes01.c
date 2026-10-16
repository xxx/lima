#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Includes -- part one");
  set_long("A .h or header file is nothing more than a group of define that are\n"
           "placed in a single file to be used by .c files.\n"
           "\n"
           "The defines are simply a shorthand way of writing longer paths in\n"
           "a simplified way.  for example:\n"
           "\n"
           "#define MAIN     \"/domains/std/school/\"\n"
           "#define R        MAIN + \"R/\"\n"
           "#define X        MAIN + \"X/\"\n"
           "#define A        MAIN + \"A/\"\n"
           "#define W        MAIN + \"W/\"\n"
           "#define M        MAIN + \"M/\"\n"
           "#define F        MAIN + \"F/\"\n"
           "#define O        MAIN + \"O/\"\n"
           "\n"
           "\n"
  );
  set_exits( ([
    "rooms" : R + "room",
    "forward" : R + "includes02",
   ]) );
  set_objects( ([
   ]) );
}
