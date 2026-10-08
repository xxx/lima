/* Do not remove the headers from this file! see /USAGE for more info. */

#include <verbs.h>

inherit VERB_OB;

//: COMMAND
// USAGE
//
//  |  ``light <something>``
//  |  ``light <something> with <something>``
//
// Light the fireplace, light the candle with a match.
//
// .. TAGS: RST

void do_light_obj(object ob)
{
   ob->do_light();
}

void do_light_obj_with_obj(object ob, object with)
{
   ob->do_light(with);
}

void create()
{
   clear_flag(NEED_TO_SEE);
   add_rules(({"OBJ", "OBJ with OBJ"}));
}
