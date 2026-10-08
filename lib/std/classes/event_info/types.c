/* Do not remove the headers from this file! see /USAGE for more info. */

#include <combat_modules.h>

class event_info
{
   object target;
   string target_extra;
   object weapon;
   mixed data;
}

int
event_damage(class event_info evt)
{
   return evt.data[sizeof(evt.data) - 1];
}

//: FUNCTION event_to_str
// string event_to_str(class event_info evt)
// Shows an event on the combat event queue as a string.
// Only used for debug purposes, but oh so very useful.
string event_to_str(class event_info evt)
{
   string out = sprintf("Event:\n target=%O\n target_extra=%O\n weapon=%O\n",
                        evt.target, evt.target_extra, evt.weapon);

   if (arrayp(evt.data))
   {
      int count = 0;
      foreach (mixed ele in evt.data)
      {
         out += sprintf(" evt.data[%d]=%O\n", count, ele);
         count++;
      }
   }
   else
      out += sprintf(" evt.data=%O\n", evt.data);

   return out;
}
