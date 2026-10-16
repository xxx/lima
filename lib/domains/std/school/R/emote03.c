#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Emote Tutorial 3");
  set_long("complete tag list:\n"
           "\n"
           "$n  = A name: defaults to the name of the doer.\n"
           "$t  = Name of the target\n"
           "$v  = A verb: signals that the next word is a verb.\n"
           "$p# = Returns the possesive of a name.  The number indicates which name\n"
           "      to return the possesive of.  $p0 is the doer, $p1 is the first\n"
           "      target, $p2 is the second target, and so on.\n"
           "$r# = Returns the reflexive.  (ie: yourself, himself, herself) It works\n"
           "      like $p#.\n"
           "$o  = This represents the object used in the emote (other than the target).\n"
           "      eg: \"smile broadly\" would call rule \"STR\" with 'broadly' as $o\n"
           "      and the message is \"$N $vsmile $o.\"\n"
  );
  set_exits( ([
    "north" : R + "emote04",
    "south" : R + "emote02",
   ]) );
  set_objects( ([
   ]) );
}
