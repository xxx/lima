#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Emote Tutorial 2");
  set_long("Now for the third part.  This is where it gets fun.  The message for\n"
           "the rule.  Always enter the message in first person.  Like the\n"
           "message for smile for the basic rule, \"LIV\", is \"$N $vsmile at $t.\"\n"
           "Now let me explain some of the tags.  The dollar sign ($) is what\n"
           "signals a tag.  $n is the name of the typer.  If you capitalize it,\n"
           "\"$N\" then it will cap the name.  $v is to signal that the following\n"
           "word is a verb.  This is so an \"s\" will be added or \"is\" will be changed\n"
           "to \"are\" for other people in the room to view.  ALWAYS put this in front\n"
           "of every verb.  $t is the target.  The target is either the LIV or the\n"
           "OBJ in the rule.  If the target is a proper noun, such as a name, it\n"
           "will be automatically capitalized; therefore, there is no need to use\n"
           "a $T because a $t will suffice.  The only time you really need to use a\n"
           "$<cap letter> is at the start of a sentence.\n"
           "\n"
           "Now for multiple targets.  Let us go back to our smile example.  One of\n"
           "the smile rules is \"LIV LIV\" so if I was to type: \"smile may lancelot\",\n"
           "it is this rule that would be used.  The message for this rule is:\n"
           "\"$N $vsmile at $t1 and $t2.\" See how you can call target 1 and target 2?\n"
  );
  set_exits( ([
    "north" : R + "emote03",
    "south" : R + "emote01",
   ]) );
  set_objects( ([
   ]) );
}
