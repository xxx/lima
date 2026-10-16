#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
    set_brief("Problems - parse errors");
    set_long("\n"
             "These arise when there is a syntax error in the code,\n"
             "and that prevents the code loading.\n"
             "\n"
             "Usually caused by omitting or mistyping punctuation, eg \" or ;.\n"
             "\n"
             "They are reported, with a line number.\n"
             "eg 'line 4 : parse error before )\n"
             "\n"
             "Also they are logged, in the error-log of the area where you are working.\n"
             "\n"
             "Sometimes the error is actually created on an earlier line,\n"
             "but not discovered until this line.\n"
             "\n"
    );
    set_exits( ([
	"corridor" : R "corridor09",
	"forward" : R "problems02",
     ]) );
    set_objects( ([
      ]) );
}
