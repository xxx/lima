/* Do not remove the headers from this file! see /USAGE for more info. */

inherit DOOR;

private void room_closed(int closed)
{
   object env = environment();
   if (!env)
      return;
   if (closed)
      env->clear_room_state("gate_open");
   else
      env->set_room_state("gate_open");
}

void set_closed(int closed)
{
   ::set_closed(closed);
   room_closed(query_closed());
}

void update_state(object other)
{
   ::update_state(other);
   room_closed(query_closed());
}

void do_on_open()
{
   ::do_on_open();
   room_closed(0);
}

void do_on_close()
{
   ::do_on_close();
   room_closed(1);
}

void on_clone(mixed args...)
{
   ::on_clone(args...);
   room_closed(query_closed());
}

void setup(string dir, string dest)
{
   set_id("gate");
   set_adj("rusty", "rusty gate");
   set_long("The gate is very rusty and doesnt look like it could stop anything anymore");
   set_sibling_ident("rusty gate");
   set_door_direction(dir);
   set_door_destination(dest);
   set_flag(ATTACHED);
}
