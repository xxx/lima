/* Do not remove the headers from this file! see /USAGE for more info. */

#include <combat_modules.h>

class event_info
{
   object target;
#ifdef HEALTH_USES_LIMBS
   string target_extra;
#endif
   object weapon;
   mixed data;
}

int event_damage(class event_info evt)
{
   return evt.data;
}

string event_to_str(class event_info evt)
{
   string out = sprintf("Event:\n target=%O\n", evt.target);
#ifdef HEALTH_USES_LIMBS
   out += sprintf(" target_extra=%O\n", evt.target_extra);
#endif
   return out + sprintf(" weapon=%O\n evt.data=%O\n", evt.weapon, evt.data);
}
