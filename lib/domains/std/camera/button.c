/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OBJ;
inherit M_GETTABLE;

void setup()
{
   set_adj("camera", "polaroid");
   set_id("button");
   set_long("It's a small black button.");
   set_getmsg("#That would surely break the camera.");
#ifdef USE_SIZE
   set_size(SMALL);
#endif
#ifdef USE_MASS
   set_mass(to_float(SMALL));
#endif
}

int do_not_restore()
{
   return 1;
}

mixed direct_press_obj(object ob)
{
   return default_object_checks();
}

void do_press(string s)
{
   environment(this_object())->take_picture();
}
