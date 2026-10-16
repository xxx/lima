#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("How to do a set_exit");
  set_long("To do a set_exit think of it this way:\n"
           "   \"direction going to\" : \"path of the room going to\",\n"
           "\n"
           "so...\n"
           "   \"may\" : WIZ_DIR+\"/may/workroom\",\n"
           "will take you in the direction \"may\"\n"
           "and the workroom is the location doing to\n"
           "please note do not add the .c to the path.\n"
           "\n"
           "// exits need to be the full path if they are in a different directory\n"
           "// exits that use an include as areas are required to do use this:\n"
           "// \"direction\" : R \"filename\",\n"
           "// where R is defined in the .h file as the path needed to find this\n"
           "// filename\n"
           "    set_exits( ([\n"
           "        \"may\" : WIZ_DIR+\"/may/workroom\",\n"
           "        \"east\" : \"shop\",\n"
           "        \"south\" : \"fmagehall\",\n"
           "        \"west\" : \"donation\"\n"
           "      ]) );\n"
           "// omit the next line if you do not want hidden exits\n"
           "    set_hidden_exits(\"west\", \"east\");\n"
  );
  set_exits( ([
    "rooms" : R + "room",
   ]) );
}
