#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Room set_long - explicit line breaks");
    set_long("Use explicit newline escapes when the positions of line breaks matter.\n"
             "Adjacent quoted strings form one description:\n"
             "\n"
             "set_long(\"The first line of a sign.\\n\"\n"
             "         \"The second line of the sign.\\n\");\n"
             "\n"
             "Each \\n in the source inserts a newline into the displayed text.\n"
             "For paragraphs that clients should wrap, the next lesson shows how to join\n"
             "source lines with spaces instead.\n"
    );
    set_exits( ([
	"backward" : R "set_long01",
	"forward" : R "set_long03",
      ]) );
    set_objects( ([
      ]) );
}
