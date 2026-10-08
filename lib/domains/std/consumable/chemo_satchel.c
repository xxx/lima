inherit DRINK;

void setup()
{
   set_id("chemo satchel", "chemo", "satchel");
   set_adj("small");
   set_num_drinks(4);
   set_heal_value(0);
   set_weight(1.0);
   set_long("This is a small satchel of drugs marked \"Liquid Prednisone\". It can help your body rid itself of "
            "unwanted polutants.");
   set_drink_action("$N $vsqueeze out a gob from a small drug satchel into $p mouth.");
   set_last_drink_action("$N $vswallow the rest of the drug satchel.");
}

varargs void drink_it(object drinkee)
{
   this_body()->remove_abuse(10 + random(100));
   ::drink_it(drinkee);
}

int query_value()
{
   return to_int(to_float(query_num_drinks()) * 100.0);
}

string get_extra_long()
{
   return ::get_extra_long();
}

int is_material()
{
   return 1;
}