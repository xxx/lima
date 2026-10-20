#include <move.h>
inherit INDOOR_ROOM;
varargs mixed receive_object(object target, string relation)
{
   if (base_name(target) == COINS)
      return MOVE_NOT_ALLOWED;
   return ::receive_object(target, relation);
}
