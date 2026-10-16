/* Do not remove the headers from this file! see /USAGE for more info. */

inherit VERB_OB;

//: COMMAND
// USAGE
//
//  |  ``fire <something>``
//  |  ``fire <something> at <someone>``
//
// Fire something, or fire it at someone.
//
// .. TAGS: RST

void do_fire_obj(object ob)
{
   ob->do_fire();
}

void do_fire_obs(object *obs)
{
   handle_obs(obs, do_fire_obj);
}

mixed indirect_fire_obj_at_liv(object ob, object liv)
{
   return liv->direct_kill_liv(liv);
}

void do_fire_obj_at_liv(object ob, object liv)
{
   ob->do_fire_at(liv);
}

void do_fire_obs_at_liv(object *obs, object liv)
{
   handle_obs(obs, do_fire_obj_at_liv, liv);
}

void do_fire_obj_obj(object ob1, object ob2)
{
   ob1->do_fire_exit(ob2);
}

void create()
{
   add_rules(({"OBS", "OBS at LIV", "OBJ OBJ"}), ({}));
}
