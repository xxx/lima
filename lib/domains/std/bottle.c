/* Do not remove the headers from this file! see /USAGE for more info. */

inherit CONTAINER;
inherit M_GETTABLE;
inherit M_FLUID_CONTAINER;

void setup()
{
   set_id("bottle", "flask");
   add_adj("clear");
   set_long("Appears to be a normal looking bottle.");
   set_relations("in", "within");
   set_default_relation("in");
   set_max_capacity(5);
}
