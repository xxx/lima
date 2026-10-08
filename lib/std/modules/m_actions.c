/* Do not remove the headers from this file! see /USAGE for more info. */

#include <commands.h>
#include <daemons.h>
#include <mudlib.h>

//: MODULE
// This module adds functions in monsters so they can do actions or respond to actions.
//
// .. TAGS: RST

void add_hook(string, function);
void remove_hook(string, function);
object query_target();
object query_link();

void action_arrival(object);
void action_departure(object);

private
nosave function arrival_fn = action_arrival;
private
nosave function departure_fn = action_departure;
private
nosave function movement_fn = action_movement;

private
function my_hook;
private
nosave string *response_queue = ({});
private nosave int *response_timers = ({});
private nosave int action_timer;
private
int delay_time = 5;
private
string *my_actions;

private
object env;

mixed need_hooks()
{
   return 1;
}

object query_body()
{
   return this_object();
}

object query_shell_ob()
{
   if (this_object()->query_link())
      return this_object()->query_link()->query_shell_ob();
   return this_object();
}

string *query_actions()
{
   return my_actions;
}

//: FUNCTION set_delay_time
// Sets the amount of time between possible actions. The minimum value
// is 5, which is default and is fine for most situations. For a large
// number of action objects, increasing this number (and increasing the
// chance that an action can be executed at the same time) could be
// beneficial.
void set_delay_time(int x)
{
   delay_time = x >= 5 ? x : 5;
}

int query_delay_time()
{
   return delay_time;
}

//: FUNCTION do_game_command
// Emulates handling of emotes and player commands for NPCs that inherit this
// module.  E.g. do_game_command("wield sword").  do_game_command("smile hap*").
void do_game_command(string str)
{
   object save_tu;
   mixed *winner;
   string verb, argument;

   save_tu = this_user();
   set_this_user(this_object());

   mixed problem = catch {
      verb = str;
      sscanf(verb, "%s %s", verb, argument);

      winner = CMD_D->find_cmd_in_path(verb, ({CMD_DIR_PLAYER "/"}));
      if (arrayp(winner))
         winner[0]->call_main("", "", "", "", "", "", argument);
      else if (environment())
      {
         mixed result = efun::parse_sentence(str, 0, nil, nil, this_object(), this_user());

         if (stringp(result))
            write(result);
      }
   };

   set_this_user(save_tu);
   if (problem) throw(problem);
}

private
void do_respond()
{
   response_timers = filter(response_timers, (: query_call_out($1) :));
   mixed cmd = response_queue[0];
   if (stringp(cmd))
      do_game_command(cmd);
   else
      evaluate(cmd);
   response_queue = response_queue[1..];
}

private
void cancel_pending_actions()
{
   stop_actions();
   foreach (int timer in response_timers)
      remove_call_out(timer);
   response_timers = ({});
   response_queue = ({});
}

private
void ensure_action_cleanup()
{
   if (!my_hook)
   {
      my_hook = cancel_pending_actions;
      add_hook("remove", my_hook);
   }
}

private
void run_actions()
{
   action_timer = 0;
   actions();
}

private
void schedule_actions()
{
   if (!query_call_out(action_timer))
      action_timer = call_out(run_actions, delay_time);
   ensure_action_cleanup();
}

//: FUNCTION respond
// Does a command [using do_game_command] after a small delay.  This is the
// best way to have NPCs react to events, as a slight delay makes it a bit
// more believable, and also if you do things immediately, the action may
// PRECEDE the thing you are responding to.  Example: Troll which attacks
// anyone who says 'foo'.  I say 'foo', you and the troll are in the room.
// If the message is delivered to the troll before you, the troll will attack
// me BEFORE the message gets to you.
void respond(string str)
{
   response_queue += ({str});
   response_timers = filter(response_timers, (: query_call_out($1) :));
   response_timers += ({call_out(do_respond, random(3))});
   ensure_action_cleanup();
}

// Return the number of listeners (player in room)
int query_listeners()
{
   object *inv;
   if (!env)
      return 0;
   if (inv = all_inventory(env))
      return sizeof(filter(inv, ( : $1->query_link() :)));
   return 0;
}

// The periodic action call_out
void actions()
{
   if (!this_object()->query_target() && sizeof(my_actions))
      do_game_command(choice(my_actions));
   if (my_actions && query_listeners())
   {
      schedule_actions();
   }
}

// If last player leaves, remove call_out
void action_departure(object who)
{
   if (who->query_link())
   {
      if (!query_listeners())
         stop_actions();
   }
}

// If first player arrives, add call_out
void action_arrival(object who)
{
   if (who->query_link())
   {
      if (query_listeners() == 1)
      {
         schedule_actions();
      }
   }
}

// It moves the "object_arrived/left" hooks (which are associated with a room)
// when the action monster moves
void action_movement()
{
   if (env)
   {
      env->remove_hook("object_arrived", arrival_fn);
      env->remove_hook("object_left", departure_fn);
   }

   env = environment();
   env->add_hook("object_arrived", arrival_fn);
   env->add_hook("object_left", departure_fn);
   if (query_listeners())
      schedule_actions();
}

void start_actions()
{
   // If we have no actions, stop here.
   if (!arrayp(my_actions))
      return;

   add_hook("move", movement_fn);
   if (env = environment(this_object()))
   {
      env->add_hook("object_arrived", arrival_fn);
      env->add_hook("object_left", departure_fn);
   }

   if (query_listeners())
   {
      schedule_actions();
   }
}

void stop_actions()
{
   remove_call_out(action_timer);
   action_timer = 0;
}

//: FUNCTION set_actions
// This function sets a list of actions and a delay,
// and chooses one to be executed time each time that delay elapses.
// This function should only be called from within setup().
void set_actions(int delay, string *actions)
{
   delay_time = delay;
   my_actions = actions;
   start_actions();
}
