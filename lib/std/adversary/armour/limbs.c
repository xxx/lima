/* Do not remove the headers from this file! see /USAGE for more info. */

inherit CLASS_EVENT_INFO;
inherit CLASS_WEAR_INFO;

int is_limb(string);
string *query_limbs();
string query_random_limb();
string *query_non_limbs();
varargs int query_max_health(string);

private
mapping armours = ([]);

mapping query_armour_map()
{
   return armours;
}

class wear_info find_wi(string s)
{
   class wear_info wi;

   wi = armours[s];

   if (!wi)
   {
      if (!is_limb(s))
         return nil;
      wi = armours[s] = new (class wear_info);
   }
   return wi;
}

//: FUNCTION query_armours
// object *query_armours(string s);
// Returns the armours that are covering limb 's'.
object *query_armours(string s)
{
   class wear_info wi;
   object *armours = ({});

   if (query_max_health(s) == -1)
      return nil;

   wi = find_wi(s);
   if (!wi)
      return nil;

   armours += ({wi.primary}) + (arrayp(wi.others) ? wi.others : ({nil})) + ({wi.secondary});
   armours -= ({nil});
   return sizeof(armours) ? armours : nil;
}

private void prune_armour_slot(string limb, class wear_info wi)
{
   if (!sizeof(wi.others))
      wi.others = nil;
   if (!wi.primary && !wi.secondary && !wi.others)
      m_delete(armours, limb);
}

//: FUNCTION update_cover
// Refresh the additional limbs covered by a worn item.
void update_cover(object what)
{
   string *also;
   class wear_info wi;
   int worn;

   if (!what)
      return;

   foreach (string limb, class wear_info entry in armours)
      if (entry.primary == what || entry.secondary == what)
         worn = 1;

   if (!worn)
      return;

   also = what->also_covers();
   if (!also)
      also = ({});

   foreach (string limb in keys(armours))
   {
      class wear_info entry = armours[limb];
      if (entry.others && member_array(what, entry.others) != -1 &&
          (entry.primary == what || entry.secondary == what || member_array(limb, also) == -1))
      {
         entry.others -= ({what, nil});
         prune_armour_slot(limb, entry);
      }
   }

   foreach (string limb in also)
      if (wi = find_wi(limb))
      {
         if (wi.primary == what || wi.secondary == what)
            continue;
         if (!wi.others)
            wi.others = ({});
         if (member_array(what, wi.others) == -1)
            wi.others += ({what});
         wi.others -= ({nil});
      }
}

//: FUNCTION wear_item
// nomask int wear_item(object what, string where);
// Forces the adversary to wear 'what' on its 'where' limb.
nomask int wear_item(object what, string where)
{
   class wear_info wi;
   string orig_w = where;

   // Find the limb we want to use.
   if (is_limb(where))
      wi = find_wi(where);
   else
   {
      if (is_limb("right " + where))
      {
         where = "right " + where;
         wi = find_wi(where);
         // If this is a primary slot item and we're already
         // wearing something there, try left limb instead.
         if (wi && wi.primary && !what->query_worn_under())
         {
            where = "left " + orig_w;
            wi = find_wi(where);
         }
      }
   }

   // Check if this is worn under other things.
   if (what->query_worn_under())
   {
      if (!wi || wi.secondary)
         return 0;

      wi.secondary = what;
      update_cover(what);
      return 1;
   }

   if (!wi || wi.primary)
      return 0;

   wi.primary = what;

   update_cover(what);
   return 1;
}

//: FUNCTION remove_item
// nomask int remove_item(object what, string where);
// Removes armour 'what' from the 'where' limb.
nomask int remove_item(object what, string where)
{
   class wear_info wi;
   string orig_w = where;

   if (!(wi = armours[where]) || (wi.primary != what && wi.secondary != what))
   {
      where = "left " + where;
      if (!(wi = armours[where]) || (wi.primary != what && wi.secondary != what))
      {
         where = "right " + orig_w;
         if (!(wi = armours[where]) || (wi.primary != what && wi.secondary != what))
         {
            return 0;
         }
      }
   }

   if (wi.primary == what)
      wi.primary = nil;
   if (wi.secondary == what)
      wi.secondary = nil;
   prune_armour_slot(where, wi);

   foreach (string limb in keys(armours))
   {
      class wear_info entry = armours[limb];
      if (entry.others && member_array(what, entry.others) != -1)
      {
         entry.others -= ({what, nil});
         prune_armour_slot(limb, entry);
      }
   }
   return 1;
}

//: FUNCTION has_body_slot
// int has_body_slot(string slot);
// Returns 1 if the body slot is a valid one.
int has_body_slot(string slot)
{
   return is_limb(slot);
}

//: FUNCTION query_armour_slots
// string *query_armour_slots()
// Returns all valid armour slots on an adversary.
string *query_armour_slots()
{
   return query_limbs() - query_non_limbs();
}

string query_random_armour_slot()
{
   return query_random_limb();
}

object *event_get_armours(class event_info evt)
{
   // Following ifdef added so this would update nicely always
   return query_armours(evt.target_extra);
}
