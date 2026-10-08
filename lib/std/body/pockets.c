/*
** Rewrite of old money.c and other stuff by Tsath 2020. This file covers anything can the player carries that
** is converted from objects in the MUD into values in parameters into the player.
*/

#define PERCENT_TO_DROP_WHEN_ENCUMBERED 10
#define PERCENT_TO_TAKE_DMG_ENCUMBERED 10

int query_carrying_stat();                     // From stats
varargs float query_capacity(string relation); // From CONTAINER

private
mapping money = ([]);
private
mapping materials = ([]);

mapping query_materials()
{
   if (!mappingp(materials))
      materials = ([]);
   return materials;
}

void empty_materials()
{
   materials = ([]);
}

private
int valid_currency(string currency)
{
   return member_array(currency, MONEY_D->query_currency_types()) != -1;
}

//: FUNCTION add_material
// int add_material(mixed m)
// Adds a material (object) to the pouch and detroys it from inventory.
// If m is a string, it's added directly under that ID.
// The function returns 1 on success or 0 on failure.
varargs int add_material(mixed m, int c)
{
   string m_id;
   if (!m)
      return 0;
   if (c <= 0 && stringp(m))
      c = 1;
   if (objectp(m))
   {
      m_id = m->short();
      c = 1;
   }
   else
      m_id = m;
   if (!CRAFTING_D->valid_material(m_id))
      return 0;

   if (materials[m_id])
      materials[m_id] += c;
   else
      materials[m_id] = c;
   if (objectp(m))
      destruct(m);
   return 1;
}

//: FUNCTION has_material
int has_material(string m, int count)
{
   if (count <= 0)
      return count == 0;
   if ((materials[m] || 0) >= count)
      return 1;
   object *ims = filter(all_inventory(), (: $1->id($(m)) && ($1->is_material() || $1->is_junk()) :));
   return sizeof(ims) >= count;
}

//: FUNCTION remove_material
// Removes from the pouch first, or from inventory if it holds the whole count.
int remove_material(string m, int count)
{
   if (count <= 0)
      return count == 0;
   if (!CRAFTING_D->valid_material(m))
      return 0;
   if ((materials[m] || 0) >= count)
   {
      materials[m] -= count;
      if (!materials[m])
         m_delete(materials, m);
      return 1;
   }
   object *ims = filter(all_inventory(), (: $1->id($(m)) && ($1->is_material() || $1->is_junk()) :));
   if (sizeof(ims) < count)
      return 0;
   ims[0..count - 1]->remove();
   return 1;
}

//: FUNCTION query_amt_money
// This is the functin to call to query the amount of a certain type
// of currency you have.
int query_amt_money(string type)
{
   return to_int(round(query_amt_currency(MONEY_D->query_currency(type)) / MONEY_D->query_factor(type)));
}

//: FUNCTION query_amt_currency
// Balances are stored in base-currency units, including fractional units.
float query_amt_currency(string currency)
{
   currency = MONEY_D->singular_name(currency);
   if (!valid_currency(currency))
      error("Unknown currency '" + currency + "'.");
   return to_float(money[currency] || 0);
}

//: FUNCTION add_money
// Converts a denomination amount into the stored base-currency balance.
void add_money(string type, float amount)
{
   string currency = MONEY_D->query_currency(type);
   float value = amount * MONEY_D->query_factor(type);
   if (amount < 0.0)
      error("Cannot add a negative amount of money.");
   if (value)
      money[currency] = query_amt_currency(currency) + value;
}

//: FUNCTION subtract_money
// Rejects overdrafts before changing the base-currency balance.
void subtract_money(string type, float amount)
{
   string currency = MONEY_D->query_currency(type);
   float value = amount * MONEY_D->query_factor(type);
   float balance = query_amt_currency(currency);
   if (amount < 0.0)
      error("Cannot subtract a negative amount of money.");
   if (!MONEY_D->sufficient_money(balance, value))
      error("Insufficient money.");
   if (MONEY_D->sufficient_money(value, balance))
      m_delete(money, currency);
   else if (value)
      money[currency] = balance - value;
}

//: FUNCTION query_money
// This function will return the complete money mapping
mapping query_money()
{
   if (!mappingp(money))
      money = ([]);
   return money;
}

/* Override max_capacity from container to make it constitution based */
varargs int query_max_capacity(string relation)
{
   return to_int(query_carrying_stat() * 4);
}

/* Override max_capacity from container to make it constitution based */

int query_heavy_capacity()
{
   return query_carrying_stat() * 2;
}

int query_encumbered_capacity()
{
   return query_carrying_stat();
}

varargs int query_no_move_capacity(string relation)
{
   return to_int(query_max_capacity() * 0.9);
}

void before_move()
{
   if (query_capacity() > query_encumbered_capacity())
   {
      int roll = random(100);
      object *obs = ({});
      foreach (object ob in all_inventory(this_body()))
      {
         if (!ob)
            continue;
         if (environment(ob) != this_body())
            continue;
         if (ob->is_armour() && ob->ob_state())
            continue;
         if (ob->is_weapon() && ob->query_wielded_by() == this_body())
            continue;
         obs += ({ob});
      }
      if (sizeof(obs) && roll < PERCENT_TO_DROP_WHEN_ENCUMBERED)
      {
         object drop_item = choice(obs);
         this_body()->simple_action(
             "$N $vare encumbered, so a $o $v1slip out of $p inventory and $v1drop to the floor.", drop_item);
         drop_item->move(environment(this_object()));
      }
   }

   if (query_capacity() > query_heavy_capacity())
   {
      int roll = random(100);
      if (roll < PERCENT_TO_TAKE_DMG_ENCUMBERED)
      {
         string limb = choice(this_body()->query_limbs() - this_body()->query_non_limbs());
         if (this_body()->is_vital_limb(limb) && this_body()->query_health(limb) > 5)
         {
            this_body()->hurt_us(random(3) + 1, limb);
            this_body()->simple_action("$N $vhurt $p $o transporting so much weight.", limb);
         }
         else
         {
            this_body()->hurt_us(random(3) + 1, limb);
            this_body()->simple_action("$N $vhurt $p $o transporting so much weight.", limb);
         }
      }
   }
}