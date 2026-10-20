/* Do not remove the headers from this file! see /USAGE for more info. */

inherit CONTAINER;

void setup()
{
   set_id("table");
   set_adj("coffee", "small", "wooden");
   set_long("It's a simple square table, about 2 feet across, with a plywood surface and simple wooden legs.");
   set_flag(ATTACHED);
   add_relation("on", LARGE);
   set_default_relation("on");
#ifdef USE_SIZE
   set_size(LARGE);
#endif
#ifdef USE_MASS
   set_mass(to_float(LARGE));
#endif
   set_objects(([]));
}
