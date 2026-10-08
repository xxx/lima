/* Do not remove the headers from this file! see /USAGE for more info. */

#include <move.h>
inherit M_INPUT;

#define CRAFT_COST 5.0
#define MAT_SALVAGE_COST 5.0
#define MAT_CRAFT_COST 1.0
#define MAX_COMPONENT_COUNT (9223372036854775807 / 10)

private string recipe_list;
private string craft_currency_type = DOMAIN_D->query_currency();
private string station_filter;

string get_extra_long()
{
   return recipe_list ? recipe_list : "This station has no extra features.";
}

int is_crafting_station()
{
   return 1;
}

private int can_use_station(object body)
{
   return body && body == this_body() && environment(body) && environment(body) == environment(this_object());
}

private mapping station_recipe(string what)
{
   return CRAFTING_D->query_crafting_recipes(station_filter)[what];
}

private int valid_recipe(mapping recipe)
{
   if (!mappingp(recipe))
      return 0;
   foreach (mixed name, mixed amount in recipe)
      if (!stringp(name) || !CRAFTING_D->valid_material(name) || !intp(amount) || amount < 0)
         return 0;
   return 1;
}

private int has_resources(object body, mapping recipe, float cost)
{
   if (!MONEY_D->sufficient_money(body->query_amt_currency(craft_currency_type), cost))
      return 0;
   foreach (string name, int amount in recipe)
      if (amount && !body->has_material(name, amount))
         return 0;
   return 1;
}

private void consume_resources(object body, mapping recipe, float cost)
{
   foreach (string name, int amount in recipe)
      if (amount && !body->remove_material(name, amount))
         error("Crafting material removal failed after preflight.");
   MONEY_D->handle_subtract_money(body, cost, craft_currency_type);
}

private string material_named(string text)
{
   if (CRAFTING_D->valid_material(text))
      return text;
   string found;
   foreach (string category in CRAFTING_D->query_material_categories())
      foreach (string name in CRAFTING_D->query_materials(category))
         if (M_GRAMMAR->pluralize(name) == text)
         {
            if (found && found != name)
               return nil;
            found = name;
         }
   return found;
}

private mixed *material_request(string what)
{
   if (!stringp(what))
      return nil;
   string *words = filter(explode(trim(what), " "), strlen);
   if (!sizeof(words))
      return nil;
   string material = material_named(implode(words, " "));
   if (material)
      return ({material, 1});
   if (sizeof(words) < 2)
      return nil;
   string count = words[0];
   int number = 0;
   if (member_array(count, ({"a", "an", "one"})) != -1)
      number = 1;
   else
      for (int i = 0; i < strlen(count); i++)
      {
         int digit = count[i] - '0';
         if (digit < 0 || digit > 9 || number > (MAX_COMPONENT_COUNT - digit) / 10)
            return nil;
         number = number * 10 + digit;
      }
   if (number <= 0)
      return nil;
   material = material_named(implode(words[1..], " "));
   return material ? ({material, number}) : nil;
}

private int convert_material(object body, string what, int salvage)
{
   if (!can_use_station(body))
      return 0;
   mixed *request = material_request(what);
   if (!request)
   {
      tell(body, "Use a positive count and a complete material name.\n");
      return 0;
   }
   string material = request[0];
   int number = request[1];
   string category = CRAFTING_D->material_category(material);
   string lower = CRAFTING_D->material_below(category, material);
   if (!lower)
   {
      tell(body, "That material has no lower tier to convert.\n");
      return 0;
   }
   int tier = member_array(material, CRAFTING_D->query_materials(category));
   if (tier > 0 && number > (9223372036854775807 - 20) / tier)
      return 0;
   int difficulty = 20 + number * tier;
   string source = salvage ? material : lower;
   string result = salvage ? lower : material;
   int amount = salvage ? number : number * 10;
   int produced = salvage ? number * 10 : number;
   float cost = salvage ? MAT_SALVAGE_COST : MAT_CRAFT_COST;
   mapping recipe = ([source:amount]);
   if (!has_resources(body, recipe, cost))
   {
      tell(body, "You lack the money or materials for that conversion.\n");
      return 0;
   }
   consume_resources(body, recipe, cost);
   if (!body->add_material(result, produced))
      error("Converted material was rejected.");
   body->my_action("$N $vpay " + MONEY_D->currency_to_string(cost, craft_currency_type) + " to convert materials.\n");
   if (body->test_skill("misc/crafting/" + station_filter, difficulty))
   {
      tell(body, "You get a few materials back because you did well crafting.\n");
      body->add_material(lower, random(number * 5) + number);
   }
   return 1;
}

int simple_craft(object body, string what)
{
   if (CRAFTING_D->valid_crafting_recipe(what))
      return craft_item(body, what);
   return convert_material(body, what, 0);
}

int simple_salvage(object body, string what)
{
   return convert_material(body, what, 1);
}

private void discard_craft(object craft)
{
   if (!craft)
      return;
   craft->remove();
   if (craft)
      error("The crafted item refused cleanup.");
}

void craft_item_confirmed(object body, string what, string reply)
{
   if (lower_case(trim(reply)) != "y")
   {
      write("Craft aborted.\n");
      return;
   }
   if (!can_use_station(body))
   {
      write("You can no longer use that station.\n");
      return;
   }
   mapping recipe = deep_copy(station_recipe(what));
   if (!valid_recipe(recipe) || !has_resources(body, recipe, CRAFT_COST))
   {
      write("The recipe, money or materials are no longer available.\n");
      return;
   }
   object craft = CRAFTING_D->create_material(what);
   if (!craft)
   {
      write("The item could not be created.\n");
      return;
   }
   mixed moved, xp = 0;
   mixed problem;
   if (function_exists("crafting_xp", craft))
      problem = catch(xp = craft->crafting_xp());
   if (problem || !intp(xp) || xp < 0 || xp > MAX_COMPONENT_COUNT)
   {
      discard_craft(craft);
      if (problem)
         throw(problem);
      error("Invalid crafting experience value.");
   }
   problem = catch(moved = craft->move(body));
   if (problem || moved != MOVE_OK || !craft || environment(craft) != body)
   {
      if (craft)
         discard_craft(craft);
      if (problem)
         throw(problem);
      write(stringp(moved) ? moved : "The crafted item could not be delivered.\n");
      return;
   }
   if (!can_use_station(body) || !mappingp(station_recipe(what)) || !has_resources(body, recipe, CRAFT_COST))
   {
      discard_craft(craft);
      write("The recipe, money or materials are no longer available.\n");
      return;
   }
   // Caught payment failures can retain earlier resource mutations.
   problem = catch(consume_resources(body, recipe, CRAFT_COST));
   if (problem)
   {
      discard_craft(craft);
      throw(problem);
   }
   body->test_skill("misc/crafting/" + station_filter, xp * 10);
   if (xp)
      body->add_experience(xp);
   body->my_action("$N $vpay " + MONEY_D->currency_to_string(CRAFT_COST, craft_currency_type) + " to craft an item.\n");
   body->simple_action("$N $vcraft $o at the $o1.", add_article(what), this_object());
}

int craft_item(object body, string what)
{
   if (!can_use_station(body))
      return 0;
   mapping recipe = station_recipe(what);
   if (!valid_recipe(recipe))
   {
      tell(body, "That recipe is not available at this station.\n");
      return 0;
   }
   int missing = 0;
   string materials = "";
   foreach (string name, int amount in recipe)
   {
      if (!amount)
         continue;
      int available = body->has_material(name, amount);
      materials += (available ? " %^GREEN%^+%^RESET%^ " : " %^RED%^-%^RESET%^ ") + amount + " x " + name +
                   (available ? " (got it)\n" : " (missing)\n");
      if (!available)
         missing++;
   }
   if (missing)
   {
      write("To craft " + what + ", you are missing " + missing + " types of materials:\n" + materials);
      return 0;
   }
   if (!MONEY_D->sufficient_money(body->query_amt_currency(craft_currency_type), CRAFT_COST))
   {
      write("You need " + MONEY_D->currency_to_string(CRAFT_COST, craft_currency_type) + " to craft that item.\n");
      return 0;
   }
   write("You have all materials needed! This craft will consume:\n" + materials);
   write("Craft item - costs " + MONEY_D->currency_to_string(CRAFT_COST, craft_currency_type) + "? (y/N) ");
   modal_simple(&craft_item_confirmed(body, what));
   return 1;
}

void setup_recipes(string filter)
{
   string text = "%^YELLOW%^" + capitalize(filter) + " recipes:%^RESET%^\n";
   station_filter = filter;
   foreach (string name in keys(CRAFTING_D->query_crafting_recipes(filter)))
      text += "\tcraft " + name + "\n";
   recipe_list = text;
}
