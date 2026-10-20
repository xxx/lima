/* Do not remove the headers from this file! see /USAGE for more info. */

/* replacement for COINS becuase not all money is in coins */

inherit OBJ;
inherit M_GETTABLE;
inherit M_MESSAGES;

// Balances are integer counts keyed by denomination.
private
mapping money = ([]);

string long_descr()
{
   string descr;
   string denomination;
   int amount;

   foreach (denomination, amount in money)
   {
      if (descr)
         descr += ", ";
      else
         descr = "A pile of money consisting of ";
      descr += MONEY_D->denomination_to_string(amount, denomination);
   }
   return descr;
}

private
int query_amt_money(string type)
{
   return money[type] || 0;
}

string *query_money()
{
   return keys(money);
}

void split_money(int amount, string type)
{
   type = MONEY_D->singular_name(type);
   int available = query_amt_money(type);
   if (amount <= 0)
   {
      write("Please specify a positive amount.\n");
      return;
   }
   if (amount > available)
   {
      write("There aren't that many money of that type here.\n");
      return;
   }
   this_body()->add_money(type, to_float(amount));
   money[type] = available - amount;
   this_body()->simple_action("$N $vtake " + MONEY_D->denomination_to_string(amount, type) + ".");
   if (!money[type])
   {
      m_delete(money, type);
      if (!sizeof(money))
         remove();
   }
}

mixed get(mixed args...)
{
   string amount_str = sizeof(args) > 0 ? args[0] : nil;
   string type = sizeof(args) > 1 ? args[1] : nil;
   if (!amount_str)
   {
      foreach (string denomination, int amount in money)
         this_body()->add_money(denomination, to_float(amount));
      this_body()->simple_action("$N $vtake the pile of money.");
      remove();
      return "";
   }
   if (type == "coin" || type == "coins")
   {
      string *types = query_money();
      if (sizeof(types) != 1)
      {
         write("Which type of money do you want to get?\n");
         return;
      }
      type = types[0];
   }
   else
      type = MONEY_D->singular_name(type);

   int available = query_amt_money(type);
   if (available <= 0)
   {
      write("There are no money of that type here.\n");
      return;
   }
   int amount = amount_str == "all" ? available : to_int(amount_str);
   split_money(amount, type);
}

void merge_money(int amount, string type)
{
   type = MONEY_D->singular_name(type);
   this_object()->add_id(type);
   money[type] = query_amt_money(type) + amount;
}

void setup(int amount, string type)
{
   amount = to_int(amount);
   type = MONEY_D->singular_name(type);
   money[type] = amount;
   set_id("money", type, "coins", "pile", "pile of money");
   add_adj(type);
   set_in_room_desc("A pile of money");
   set_proper_name("money");
   set_long(long_descr);
   set_gettable(1);
   set_plural(1);
}
