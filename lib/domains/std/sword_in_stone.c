/* Do not remove the headers from this file! see /USAGE for more info. */

inherit WEAPON;

void setup()
{
   set_id("sword");
   set_weapon_class(25);
#ifdef USE_SIZE
   set_size(MEDIUM);
#endif
#ifdef USE_MASS
   set_mass(to_float(MEDIUM));
#endif
   set_long("Well, at least you weren't chosen by a lady in a lake.\n");
}
