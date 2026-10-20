/* Do not remove the headers from this file! see /USAGE for more info. */

/*
 *
 *  Transient effects inheritable for diseases, intoxication, etc...
 *  Midhir@Mutants and Machinery
 *  July 15 1997
 *
 */

inherit OBJ;
inherit M_GETTABLE;

int effect_duration;
string effect_type;
string effect_name;
private nosave int effect_timer;

private void stop_effect_timer()
{
   remove_call_out(effect_timer);
   effect_timer = 0;
}

private void start_effect_timer()
{
   if (!query_call_out(effect_timer))
      effect_timer = call_out(heart_beat, 2, 2);
}

int remove()
{
   stop_effect_timer();
   return ::remove();
}

int is_transient_effect()
{
   return 1;
}

//: FUNCTION another_effect_exists
// Override to extend a matching resident effect before removing this object.
void another_effect_exists(object ob)
{
   ob->add_effect_duration(effect_duration);
   this_object()->remove();
}

//: FUNCTION set_effect_duration
// Effects run every two seconds, before checking expiry and decrementing.
void set_effect_duration(int d)
{
   effect_duration = d;
}
int query_effect_duration()
{
   return effect_duration;
}
void add_effect_duration(int d)
{
   effect_duration += d;
}

//: FUNCTION set_effect_type
// Set the string type of effect this is, used to make sure only
// one object of each type is in one inventory.
void set_effect_type(string t)
{
   effect_type = t;
}
string query_effect_type()
{
   return effect_type;
}

//: FUNCTION set_effect_name
// Sets the string name of the effect, this isn't currently used for
// anything, but could be used for more descriptive names than set_effect_type.
void set_effect_name(string n)
{
   effect_name = n;
}
string query_effect_name()
{
   return effect_name;
}

//: FUNCTION do_effect
// overload this function to create effects.
// The first argument is the object being afflicted.
void do_effect(object affected)
{
}

void heart_beat()
{
   object affected = environment();
   if (!affected || !affected->is_living())
   {
      stop_effect_timer();
      return;
   }
   do_effect(affected);
   if (!objectp(this_object()))
      return;
   if (effect_duration <= 0)
   {
      this_object()->remove();
      return;
   }
   effect_duration--;
}

void mudlib_setup()
{
   set_attached(1);
   add_save(({"effect_duration"}));
   start_effect_timer();
}

varargs mixed move(object dest, string where)
{
   if (!dest)
      return 0;
   foreach (object ob in all_inventory(dest))
   {
      if (ob == this_object() || !ob->is_transient_effect())
         continue;
      if (ob->query_effect_type() == effect_type && ob->query_effect_name() == effect_name)
      {
         another_effect_exists(ob);
         return;
      }
   }
   start_effect_timer();
   return ::move(dest, where);
}
