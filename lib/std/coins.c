/* Do not remove the headers from this file! see /USAGE for more info. */

/* cleaned up and adjusted to denominations 10-Feb-98 by MonicaS
   Tsath: Rewritten to fit with floats.
   */

inherit OBJ;
inherit M_GETTABLE;
inherit M_MESSAGES;

// Pile balances are stored in base-currency units.
private
mapping coins = ([]);

string long_descr()
{
   string *descr = ({});
   foreach (string currency in keys(coins))
   {
      mapping denom = MONEY_D->calculate_denominations(coins[currency], currency);
      foreach (string denom_name, int value in denom)
      {
         descr += ({MONEY_D->denomination_to_string(value, denom_name)});
      }
   }

   return "A pile of coins consisting of " + format_list(descr, nil) + ".";
}

private
float query_amt_coins(string type)
{
   return coins[type] || 0.0;
}

string *query_coins()
{
   return keys(coins);
}

void split_coins(float amount, string type)
{
   float balance = query_amt_coins(type);
   if (amount <= 0.0)
   {
      write("Please specify a positive amount.\n");
      return;
   }
   if (!MONEY_D->sufficient_money(balance, amount))
   {
      write("There aren't that many coins of that type here.\n");
      return;
   }
   this_body()->add_money(type, amount);
   if (MONEY_D->sufficient_money(amount, balance))
      m_delete(coins, type);
   else
      coins[type] = balance - amount;
   this_body()->simple_action("$N $vtake some coins.");
   if (!sizeof(coins))
      remove();
}

mixed get(mixed args...)
{
   string amount_str = sizeof(args) > 0 ? args[0] : nil;
   string type = sizeof(args) > 1 ? args[1] : nil;

   if (!amount_str)
   {
      foreach (string currency, float value in coins)
         this_body()->add_money(currency, value);
      this_body()->simple_action("$N $vtake the pile of coins.");
      remove();
      return "";
   }

   string currency;
   float factor = 1.0;
   if (type == "coin" || type == "coins")
   {
      string *types = query_coins();
      if (sizeof(types) != 1)
      {
         write("Which type of coins do you want to get?\n");
         return;
      }
      currency = types[0];
   }
   else
   {
      type = MONEY_D->singular_name(type);
      if (!MONEY_D->is_denomination(type))
      {
         write("There are no coins of that type here.\n");
         return;
      }
      currency = MONEY_D->query_currency(type);
      factor = MONEY_D->query_factor(type);
   }
   float balance = query_amt_coins(currency);
   if (balance <= 0.0)
   {
      write("There are no coins of that type here.\n");
      return;
   }
   float amount = amount_str == "all" ? balance : to_float(to_int(amount_str)) * factor;
   split_coins(amount, currency);
}

void merge_coins(float amount, string type)
{
   this_object()->add_id(type);
   coins[type] = query_amt_coins(type) + amount;
}

void setup(float amount, string type)
{
   coins[type] = amount;
   set_id("coins", type, "money", "coin", "pile", "pile of coins");
   add_adj(type);
   set_in_room_desc("A pile of coins");
   set_proper_name("coins");
   set_long(long_descr);
   set_gettable(1);
   set_plural(1);
}
