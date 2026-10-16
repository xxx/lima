// May's Ed Tutorial for Wizards

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Ed Tutorial 10");
  set_long("   The SUBSTITUTE command may be THE most important command\n"
           "that you will learn in ED.  It is the command that you use\n"
           "to change text on a single line.  This is important because\n"
           "this command saves you from having to type over an entire line.\n"
           "Lets say that you have a line that starts off:  set_logn(...\n"
           "and you want to change it to set_long(...  What you would type\n"
           "is <s/logn/long>.  This SUBSTITUTES in 'long' for 'logn'.  You\n"
           "need not use front slashes between the 2 strings being changed.\n"
           "You can use any character as long as you only use one single\n"
           "character.  For example, we can write out the first example\n"
           "as <s!logn!long>.  This will accomplish exactly the same thing.\n"
           "This is important when you are working with text of directories,\n"
           "text that has a frontslash in it.  You would not want to use the\n"
           "frontslash so you could use <s!/name/dir!/name/dir2>.  In this\n"
           "case, the '!' is the character that divided the strings and the\n"
           "slash is used in the string.  You may also want to use the 's'\n"
           "command over a set number of lines.  In that case you would\n"
           "write <1,25s/str1/str2>.  This will change the first occurance\n"
           "of str1 on each line to str2.  If you want to change ALL\n"
           "occurances of str1 to str2 you would have to add an extra\n"
           "'g' command to the original text so it would look like\n"
           "this: <1,25s/str1/str2/g>.\n"
  );
  set_exits( ([
    "south" : R + "edtutor09",
    "north" : R + "edtutor11",
   ]) );
  set_objects( ([
   ]) );
}
