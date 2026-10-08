/* Do not remove the headers from this file! see /USAGE for more info. */

#include <light.h>

inherit TORCH;

void setup()
{
   set_adj("magic");
   set_id("torch");
   set_long("It looks like an ordinary torch, but you guess that it's probably magic because it's humming.");
   set_light_msgs("As $n $vlight the $o, it starts burning with a blue flame.",
                  "As $n $vlight the $o, it starts burning with a blue flame.");
   set_source(( : $1->can_light_the_magic_torch() ? (mixed)1 : "It doesn't seem to be the right type of flame.\n" :));
   set_value(to_float(15000));
   set_fuel(INF_FUEL);
   set_untouched_desc("A torch lies in the dust.");
}

int sound()
{
   write("It's humming with a low regular buzz.\n");
   return 1;
}
