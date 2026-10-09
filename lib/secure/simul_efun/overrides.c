/* Do not remove the headers from this file! see /USAGE for more info. */

#include <commands.h>
#include <playerflags.h>

void debug_message(string message) { efun::tell_object(nil, message + "\n"); }

nomask object this_user() { return efun::this_player(); }

object this_body();
int check_privilege(mixed priv);
int adminp(mixed m);
varargs void tell(object, string);

void write(string str)
{
   if (this_user())
      tell(this_user(), str);
   else
   {
      if (str[ < 1] == '\n')
         str = str[0.. < 2];
      debug_message("]" + str);
   }
}

//: FUNCTION terminal_colour
// Override of efun to avoid some driver issues.
string terminal_colour(string str, mapping m, int wrap, int indent)
{
   str = efun::terminal_colour(str, m, wrap, indent);
   str = replace_string(str, "\e[0;10m", "");
   str = replace_string(str, "\e[49;49m", "");
   return str;
}

//: FUNCTION ed
// The ed() efun is not used by the LIMA mudlib, as we use the new ed
// functionality.  See ed_session.c
nomask varargs void ed(string file, mixed func)
{
   error("ed() not available\n");
}

//: FUNCTION message
// The message() efun is not used by the LIMA mudlib, as we use M_MESSAGES
// for all messages sent to users.  See M_MESSAGES documentation.
nomask varargs void message(mixed msgclass, string message, mixed target, mixed exclude)
{
   error("message() not available\n");
}

//: FUNCTION exec
// The exec efun is never used by the LIMA mudlib, since only bodies are
// reconnected.
nomask int exec(object target, object src)
{
   error("exec() not available\n");
}

//: FUNCTION input_to
// input_to should not be used; the stackable input system should be used
// instead.  See the modal_push() and modal_func() routines.
nomask varargs int input_to()
{
   error("input_to() should not be used.  Use modal_push()/modal_func()\n");
}

//: FUNCTION find_player
// find_body() and find_user() are used to find the body and connection objects,
// respectively.
nomask object find_player(string str)
{
   error("find_player() is obsolete.  Use find_body() instead.\n");
}

//: FUNCTION this_player
// this_body() and this_user() are used to find the body and connection objects,
// respectively.
nomask object this_player(string str)
{
   error("this_player() is obsolete.  Use this_body() or this_user() instead.\n");
}

// Native destruction leaves contents in the dying object unless the mudlib handles them.
private void destruct_with_contents(object ob)
{
   object destination = environment(ob);
   object *contents;
   while (ob && sizeof(contents = all_inventory(ob)))
   {
      foreach (object child in contents)
      {
         if (!child || environment(child) != ob)
            continue;
         child->move(destination);
         if (!ob)
            return;
         if (child && environment(child) == ob)
            destruct_with_contents(child);
      }
   }
   if (ob)
      efun::destruct(ob);
}

//: FUNCTION destruct
// Authorized explicit targets receive remove() before native destruction.
// Omitted targets remove the requester without calling remove() recursively.
// An optional second argument is forwarded to remove(); update supplies 1.
// Cleanup errors propagate and return values are ignored.
nomask void destruct(mixed *args...)
{
   object ob;
   if (sizeof(args) > 2)
      throw("destruct: too many arguments");
   if (!sizeof(args))
      ob = previous_object();
   else
   {
      mixed value = args[0];
      if (value == nil || (intp(value) && value == 0))
         return;
      if (!objectp(value))
         throw("destruct: expected an object");
      ob = value;
   }
   if (!ob)
      return;
   if (!master()->valid_destruct_request(ob))
      throw("destruct: permission denied");
   if (sizeof(args) > 1)
      ob->remove(args[1]);
   else if (sizeof(args))
      ob->remove();
   if (ob)
      destruct_with_contents(ob);
}

//: FUNCTION shutdown
// The shutdown simul exists to prevent anything less than priv 1 from shutting
// down the mud.
nomask void shutdown()
{
   if (check_privilege(1))
      efun::shutdown();
   else
      error("Insufficient privilege to shut down.\n");
}

//: FUNCTION query_snoop
// The query_snoop efun makes no sense in the context of our snoop system.
nomask object query_snoop(object ob)
{
   error("Lima doesn't use regular snoop.\n");
}

//: FUNCTION query_snooping
// The query_snooping efun makes no sense in the context of our snoop system.
nomask object query_snooping(object ob)
{
   error("Lima doesn't use regular snoop.\n");
}

void start_snoop(object snooper, object snoopee)
{
   if (previous_object() == find_object(SNOOP_D))
      efun::snoop(snooper, snoopee);
   else
      error("Only SNOOP_D may call start_snoop.\n");
}

varargs void tell_object()
{
   error("Use tell() instead.\n");
}

varargs void tell_room()
{
   error("Use tell_environment() or tell_from_inside() instead.\n");
}

varargs void shout()
{
   error("Use tell(users() - ({ this_user() }), msg) instead.\n");
}

//: FUNCTION say
// The say() efun is not used by the LIMA mudlib, in favour of the extensive
// messaging system in the body.  Consider using this_body()->other_action()
// instead of say().
void say(string m)
{
   error("say() not available. Consider using this_body()->other_action()\n");
}

// Needs to be mixed, since you can pass in floats, ints, and whatever.
void printf(string format, mixed *rest...)
{
   if (this_user())
      tell(this_user(), sprintf(format, rest...));
   else
      debug_message("]" + sprintf(format, rest...));
}
