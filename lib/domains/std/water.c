/* Do not remove the headers from this file! see /USAGE for more info. */

inherit FLUID;
inherit M_DRINKABLE;

void setup()
{
   set_adj("fresh");
   set_id("water");
#ifdef USE_SIZE
   set_size(1);
#endif
#ifdef USE_MASS
   set_mass(1.0);
#endif
   set_in_room_desc("There is a puddle of water here.");
   set_gettable("#It slips through your fingers.");
   set_evaporate_action();
   set_quantity(1);
   //    set_quantity_unit( "gloop" );
}
