// Do not remove the headers from this file! see /USAGE for more info.
#include <hooks.h>

inherit DOOR;

mixed door_closing = 0;
private function departure_hook;
private function arrival_hook;

private void cancel_close()
{
   if (door_closing)
      remove_call_out(door_closing);
   door_closing = 0;
}

private void room_closed(int closed)
{
   object env = environment();
   if (!env)
      return;
   if (closed)
      env->clear_room_state("elevator_door");
   else
      env->set_room_state("elevator_door");
}

private void notify_closed()
{
   object env = environment();
   if (env)
      env->call_hooks("elevator_door_closed", HOOK_IGNORE, 0);
}

void set_closed(int closed)
{
   int was_open = is_open();
   if (closed)
      cancel_close();
   // Update our flag before the peer can notify its controller.
   room_closed(closed);
   ::set_closed(closed);
   if (was_open && query_closed())
      notify_closed();
}

void update_state(object other)
{
   int was_open = is_open();
   ::update_state(other);
   room_closed(query_closed());
   if (query_closed())
      cancel_close();
   if (was_open && query_closed())
      notify_closed();
}

void mudlib_setup(string dir, string dest)
{
   set_id("door");
   set_adj("elevator");
   set_long("This is an elevator door.");
   set_attached(1);

   set_sibling_ident("elevator_door");
   set_door_direction(dir);
   set_door_destination(dest);
}

void call_out_close_door()
{
   cancel_close();
   if (is_open())
   {
      do_on_close();
      set_closed(1);
   }
}

void hook_elevator_will_move()
{
   if (query_closed())
      notify_closed();
   else if (!door_closing)
      door_closing = call_out(call_out_close_door, 15);
}

void hook_elevator_arrived(mixed dest)
{
   cancel_close();
   set_door_destination(dest);
   do_on_open();
   set_closed(0);

   door_closing = call_out(call_out_close_door, 15);
}

mixed direct_open_obj(object ob)
{
   mixed ret = ::direct_open_obj(ob);

   if (!ret || stringp(ret))
      return ret;

   return "Try pressing the elevator button instead.";
}

mixed direct_close_obj(object ob)
{
   mixed ret = ::direct_close_obj(ob);

   if (!ret || stringp(ret))
      return ret;

   return "Try pressing the elevator button instead.";
}

void do_on_open()
{
   ::do_on_open();
   room_closed(0);
}

void do_on_close()
{
   cancel_close();
   ::do_on_close();
   room_closed(1);
}

void on_clone(mixed args...)
{
   ::on_clone(args...);
   room_closed(query_closed());

   object env = environment();
   if (env)
   {
      // Hook replacement requires the same function pointer identity.
      if (!departure_hook)
      {
         departure_hook = hook_elevator_will_move;
         arrival_hook = hook_elevator_arrived;
      }
      env->add_hook("elevator_will_move", departure_hook);
      env->add_hook("elevator_arrived", arrival_hook);
   }
}
