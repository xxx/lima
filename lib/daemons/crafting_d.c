/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** crafting_d.c -- Crafting, materials, salvage and repairs.
** This daemon manages materials, loot, salvage, upgrade and crafting.
** Originally made for PLEXUS mud, but adapted for Nuclear War MUD 2.
**
** Tsath 2019-10-14 - 2020-12-20
**
*/

#include <config/equipment.h>

inherit M_DAEMON_DATA;

#define MATERIALS_CONFIG_FILE "/data/config/crafting-materials"
#define PRIV_NEEDED "Mudlib:daemons"
#define CRAFTING_ITEMS "/domains/std/crafting/"
#define REPAIR_FACTOR 10.0

nosave int *repair_values = ({10, 100, 1000, 10000, 100000});
nosave string *cache_names = ({});
nosave mapping cache_cats = ([]);
private
nosave string currency_type = DOMAIN_D->query_currency();

// Left here mostly as an example for understanding. The materials are defined and read from MATERIALS_CONFIG_FILE at
// start of this daemon.
private
nosave mapping special_mats = (
    ["metal":([128:"steel",
                 64:"darksteel", 32:"silver", 16:"gold", 8:"platinum", 4:"titanium", 2:"adamantine", 1:"orichalcum", ]),
        "wood":([128:"fir", 64:"pine", 32:"oak", 16:"cedar", 8:"larch", 4:"hemlock", 2:"ebony", 1:"bloodwood", ]), ]);

// 128, then 192 (128+64), 224 (192+32), etc. These are tried to the "special_mats" mapping.
// Make sure to update these sums if you change the array. These are used for random selection of power level.
// random(229), and then the number has to be 0..128 or 129..192 etc. Hence only 1:255 chance for level 8 item.
private
nosave int *mat_sums = ({128, 192, 224, 240, 248, 252, 254, 255});
private
nosave mapping crafting_recipes = ([]);
private
nosave mapping recipe_file = ([]);
private
nosave mapping crafting_stations = ([]);

/* Saved variables */
mapping materials = ([]);
mapping upgrade_mat_schemes = ([]);
int longest_material = 0;

/* Functions */

string *query_material_categories();

private mixed *read_recipe(string item)
{
   string contents = read_file(CRAFTING_ITEMS + item);
   string name;
   object obj;
   mixed recipe, station;

   if (!contents)
      error("Unable to read recipe source.");
   foreach (string line in explode(contents, "\n"))
   {
      if (line[0..10] == "//CRAFTING:")
         name = trim(line[11..]);
      else if (line[0..11] == "// CRAFTING:")
         name = trim(line[12..]);
   }
   if (!name || name == "")
      error("Missing //CRAFTING: string.");
   obj = load_object(CRAFTING_ITEMS + item);
   if (!obj)
      error("load_object returned nil.");
   recipe = obj->query_recipe();
   station = obj->query_station() || "unknown";
   if (!mappingp(recipe) || !stringp(station))
      error("Invalid recipe or station result.");
   return ({name, recipe, station});
}

void load_crafting_recipes()
{
   string *files = get_dir(CRAFTING_ITEMS "*.c");
   mapping recipes = ([]), sources = ([]), stations = ([]), recipe_stations = ([]);

   foreach (string item in files)
   {
      mixed *entry;
      string err = catch(entry = read_recipe(item));
      if (err)
      {
         write("Error while loading crafting item " + item + ":\n" + err + "\n");
         continue;
      }
      recipes[entry[0]] = entry[1];
      sources[entry[0]] = item;
      recipe_stations[entry[0]] = entry[2];
   }
   foreach (string name, string station in recipe_stations)
      stations[station] = (stations[station] || ({})) + ({name});

   mapping schemes = checked_upgrades(upgrade_mat_schemes, materials, special_mats, recipes);
   crafting_recipes = recipes;
   recipe_file = sources;
   crafting_stations = stations;
   upgrade_mat_schemes = schemes;
   save_me();
}

mapping query_crafting_stations()
{
   return crafting_stations;
}

varargs mapping query_crafting_recipes(string station)
{
   mapping new_map = ([]);

   if (!crafting_stations[station])
      return ([]);

   foreach (
       string key in filter(keys(crafting_recipes), (
                                                              : member_array($1, crafting_stations[$(station)]) != -1
                                                              :)))
   {
      new_map[key] = crafting_recipes[key];
   }
   return new_map;
}

mapping query_crafting_recipe(string name)
{
   return crafting_recipes[name];
}

int valid_crafting_recipe(string name)
{
   return !!crafting_recipes[name];
}

mixed *query_all_special_mats()
{
   return ({special_mats, mat_sums});
}

mixed *query_special_mats(string category)
{
   mapping qualities = special_mats[category];
   int *sums = ({});
   int sum = 0;
   if (!qualities)
      return ({nil, mat_sums});
   foreach (int weight in sort_array(keys(qualities), -1))
   {
      sum += weight;
      sums += ({sum});
   }
   return ({qualities, sums});
}

mapping query_upgrade_schemes()
{
   return upgrade_mat_schemes;
}

string *query_upgrade_scheme(string category, string material)
{
   if (upgrade_mat_schemes[category] && upgrade_mat_schemes[category][material])
      return upgrade_mat_schemes[category][material];
   return nil;
}

private
void refresh_cache()
{
   cache_names = ({});
   cache_cats = ([]);
   longest_material = 0;
   foreach (string category in keys(materials))
   {
      foreach (string m in materials[category])
      {
         cache_names += ({m});
         cache_cats[m] = category;
         if (strlen(m) > longest_material)
            longest_material = strlen(m);
      }
   }
}

void clean_up()
{
   destruct();
}

int valid_category(string c)
{
   return member_array(c, keys(materials)) != -1;
}

string material_category(string m)
{
   return cache_cats[m];
}

int valid_material(string m)
{
   return member_array(m, cache_names) != -1;
}

int add_category(string category, string *m)
{
   if (valid_category(category))
      return 0;

   materials[category] = m;
   refresh_cache();
   save_me();
   return 1;
}

string *query_salvage_categories()
{
   return keys(materials);
}

int remove_category(string c)
{
   if (!valid_category(c))
      return 0;

   m_delete(materials, c);
   m_delete(upgrade_mat_schemes, c);
   refresh_cache();
   save_me();
   return 1;
}

string *query_material_categories()
{
   return keys(materials);
}

string *query_materials(string category)
{
   return materials[category] || ({});
}

int query_rarity(string mat)
{
   return 1 + member_array(mat, query_materials(material_category(mat)));
}

object create_material(string m)
{
   object mat;

   if (recipe_file[m])
   {
      mat = clone_object (CRAFTING_ITEMS + recipe_file[m]);
      return mat;
   }
   else if (!valid_material(m))
      return nil;

   if (!mat)
      mat = clone_object (MATERIAL, m);
   return mat;
}

string material_below(string category, string material)
{
   string *mats = query_materials(category);
   int pos;
   if (!sizeof(mats))
      return nil;
   pos = member_array(material, mats);
   pos--;

   if (pos >= 0)
   {
      return mats[pos];
   }
   return nil;
}

string material_above(string category, string material)
{
   string *mats = query_materials(category);
   int pos;
   if (!sizeof(mats))
      return nil;
   pos = member_array(material, mats);
   pos++;

   if (pos < (sizeof(mats)))
      return mats[pos];
   return nil;
}

object create_material_for_me(string m)
{
   create_material(m)->move(this_body());
}

string salvage_description(object *s)
{
   string *parts = ({});

   if (!sizeof(s))
      return "nothing";

   foreach (object ob in s)
   {
      parts += ({ob->short()});
   }
   return tidy_list(parts, "and");
}

int *chance_array(int cnt)
{
   int *pows = ({});
   int total;
   int c = 0;
   int sum = 30;

   for (int i = cnt; i > 0; i--)
   {
      pows += ({pow(2, i + 1)});
      total += pow(2, i + 1);
   }

   foreach (int i in pows)
   {
      int perc = to_int(round((0.0 + i) / total * 70));
      sum += perc;
      pows[c] = sum;
      c++;
   }

   pows = ({30}) + pows;

   return pows;
}

object *salvage_parts(object ob)
{
   mapping salvage = ob->query_salvageable();
   mapping direct_salvage = ob->query_direct_salvage();
   object *salvaged = ({});
   string picked_material;
   int s_roll = random(100) + 1;
   int *chance_ar;
   int total = 0;

   if (!direct_salvage || !sizeof(keys(direct_salvage)))
   {

      foreach (string key in keys(salvage))
      {
         if (picked_material)
            continue;
         total += salvage[key];
         if (s_roll <= total)
         {
            picked_material = key;
         }
      }

      chance_ar = chance_array(sizeof(materials[picked_material]));

      for (int i = 0; i < ob->query_salvage_level(); i++)
      {
         int index = -1;
         int chance = chance_ar[0];
         int cnt = 0;

         s_roll = random(100) + 1 + ob->query_salvage_level();
         if (s_roll > 100)
            s_roll = 100;

         while (s_roll > chance)
         {
            cnt++;
            chance = chance_ar[cnt];
            index++;
         }

         if (index >= 0)
            salvaged += ({create_material(materials[picked_material][index])});
      }

      // TBUG("Salvaged: "+sprintf("%O",salvaged));
   }
   else
   {
      foreach (string mat, int num in direct_salvage)
      {
         for (int i = 0; i < num; i++)
         {
            salvaged += ({create_material(mat)});
         }
      }
   }
   ob->remove();
   return salvaged;
}

void salvage_obj(object ob)
{
   object *salvaged = ({});
   string short = ob->short();   // Keep the short since the ob is gone after we salvage it.
   salvaged = salvage_parts(ob); // Ob no longer exists.
   if (sizeof(salvaged))
   {
      printf("You salvage %s from the %s.\n", salvage_description(salvaged), short);

      foreach (object mat in salvaged)
      {
         this_body()->add_material(mat);
      }
   }
   else
      printf("You salvage nothing from the %s.\n", short);
}

float money_to_repair(object ob)
{
   if (!ob)
      return 0.0;
   return ob->missing_durability() / REPAIR_FACTOR;
}

string repair_cost_string(mixed obs)
{
   if (!obs || objectp(obs))
      return MONEY_D->currency_to_string(money_to_repair(obs), currency_type);
   mapping plan = repair_plan(nil, repair_objects(obs), 0);
   return MONEY_D->currency_to_string(plan["money"], currency_type);
}

private object *repair_objects(mixed obs)
{
   object *result = ({});
   if (objectp(obs))
      obs = ({obs});
   if (!arrayp(obs))
      error("Expected an object or array of repair objects.");
   foreach (mixed ob in obs)
   {
      if (!objectp(ob))
         error("Invalid repair object.");
      if (member_array(ob, result) == -1 && ob->missing_durability() > 0)
         result += ({ob});
   }
   return result;
}

private mapping repair_plan(object player, object *obs, int use_materials)
{
   mapping pouch = use_materials ? deep_copy(player->query_materials()) : ([]);
   mapping picked = (["money":0.0]);
   int cash_damage = 0;
   foreach (object ob in obs)
   {
      int missing = ob->missing_durability();
      int remainder = missing;
      mapping salvage = use_materials ? ob->query_salvageable() : nil;
      if (salvage && sizeof(salvage))
      {
         int total = 0;
         foreach (string category, mixed percent in salvage)
         {
            if (!materials[category] || !intp(percent) || percent < 0 || percent > 100)
               error("Invalid repair salvage category or percentage.");
            total += percent;
         }
         if (total != 100)
            error("Repair salvage percentages must total 100.");
         foreach (string category, int percent in salvage)
         {
            int needed = percent * missing / 100;
            remainder -= needed;
            if (!picked[category])
               picked[category] = ({0, 0, 0, 0, 0});
            int tiers = min(sizeof(repair_values), sizeof(materials[category]));
            for (int i = tiers - 1; i >= 0 && needed; i--)
            {
               string name = materials[category][i];
               mixed available = pouch[name] || 0;
               if (!intp(available) || available < 0)
                  error("Invalid repair material count.");
               int count = min(needed / repair_values[i], available);
               if (i == 0 && needed % repair_values[i] && count < available)
                  count++;
               if (!count)
                  continue;
               pouch[name] = available - count;
               picked[category][i] += count;
               needed = max(0, needed - count * repair_values[i]);
            }
            remainder += needed;
         }
      }
      cash_damage += remainder;
   }
   picked["money"] = cash_damage / REPAIR_FACTOR;
   return picked;
}

mapping estimate_objects(object player, mixed obs)
{
   return repair_plan(player, repair_objects(obs), 1);
}

private int can_pay_repair(object player, mapping plan)
{
   if (plan["money"] && !MONEY_D->sufficient_money(player->query_amt_currency(currency_type), plan["money"]))
      return 0;
   foreach (string category, mixed counts in plan)
   {
      if (category == "money")
         continue;
      for (int i = 0; i < sizeof(counts); i++)
         if (counts[i] && !player->has_material(materials[category][i], counts[i]))
            return 0;
   }
   return 1;
}

private int execute_repair(object player, object *obs, mapping plan)
{
   if (!can_pay_repair(player, plan))
      return 0;
   foreach (string category, mixed counts in plan)
   {
      if (category == "money")
         continue;
      for (int i = 0; i < sizeof(counts); i++)
         if (counts[i] && !player->remove_material(materials[category][i], counts[i]))
            error("Repair material removal failed after preflight.");
   }
   if (plan["money"])
      MONEY_D->handle_subtract_money(player, plan["money"], currency_type);
   // A caller that catches a failing item hook retains earlier mutations.
   obs->durability_after_repair();
   obs->remove_adj(DAMAGED_EQ_NAME);
   if (sizeof(obs))
      player->my_action("$N $vrepair for " + MONEY_D->currency_to_string(plan["money"], currency_type) + ".\n");
   return 1;
}

int pay_for_repair_with_money(object player, mixed obs)
{
   object *items = repair_objects(obs);
   return execute_repair(player, items, repair_plan(player, items, 0));
}

int pay_for_repair(object player, mixed obs)
{
   object *items = repair_objects(obs);
   return execute_repair(player, items, repair_plan(player, items, 1));
}

mixed estimate_with_mats(object player, mixed obs)
{
   object *items = repair_objects(obs);
   mapping plan = repair_plan(player, items, 1);
   mapping pouch = player->query_materials();
   string *names = ({});
   float full_cost = 0.0;
   int possible = can_pay_repair(player, plan);
   foreach (object ob in items)
   {
      names += ({capitalize(ob->short())});
      full_cost += money_to_repair(ob);
   }
   string result = "Repair estimate for: " + implode(names, ", ") + "\n\n";
   foreach (string category, mixed counts in plan)
   {
      if (category == "money")
         continue;
      string *parts = ({});
      for (int i = 0; i < sizeof(counts); i++)
      {
         if (!counts[i])
            continue;
         string name = materials[category][i];
         parts += ({sprintf("[%d/%d] %s", counts[i], pouch[name], name)});
      }
      if (sizeof(parts))
         result += capitalize(category) + ": " + implode(parts, ", ") + "\n";
   }
   result += "Money: " + MONEY_D->currency_to_string(plan["money"], currency_type) + ".";
   if (!possible)
      result += " %%^RED%%^(You lack repair resources!)%%^RESET%%^";
   result += "\n";
   if (full_cost > plan["money"])
      result += "(You save " + MONEY_D->currency_to_string(full_cost - plan["money"], currency_type) + " by using materials)\n";
   return ({possible, result});
}

private int valid_upgrade(string category, string material, string *names,
                          mapping categories, mapping qualities, mapping recipes)
{
   if (!categories[category])
   {
      write("Invalid salvage category '" + category + "'.\n");
      return 0;
   }
   if (!qualities[category] || member_array(material, values(qualities[category])) == -1)
   {
      write("Invalid special material for " + category + ": '" + material + "'.\n");
      return 0;
   }
   if (!names || !sizeof(names))
   {
      write("Invalid empty recipe list for " + category + ".\n");
      return 0;
   }
   foreach (string name in names)
   {
      if (!recipes[name])
      {
         write("Unknown recipe '" + name + "' for category '" + category + "' material '" + material + "'.\n");
         return 0;
      }
   }
   return 1;
}

private mapping checked_upgrades(mapping schemes, mapping categories, mapping qualities, mapping recipes)
{
   mapping result = ([]);
   foreach (string category, mapping entries in schemes)
   {
      foreach (string material, string *names in entries)
      {
         if (!valid_upgrade(category, material, names, categories, qualities, recipes))
            continue;
         if (!result[category])
            result[category] = ([]);
         result[category][material] = names;
      }
   }
   return result;
}

int add_upgrade_scheme(string category, string special_material, string *recipes)
{
   if (!valid_upgrade(category, special_material, recipes, materials, special_mats, crafting_recipes))
      return 0;
   if (!upgrade_mat_schemes[category])
      upgrade_mat_schemes[category] = ([]);
   upgrade_mat_schemes[category][special_material] = recipes;
   save_me();
   return 1;
}

private
int strtype(string category)
{
   int i = -1;
   string tmp;

   if (category[0..6] == "quality")
      i = 1;
   else if (strsrch(category, "/") != -1)
      i = 2;
   else if (sscanf(category, "[%s]", tmp) == 1)
      i = 0;

   return i;
}

int query_longest_mat()
{
   return longest_material;
}

private mapping read_material_config()
{
   mapping categories = ([]), qualities = ([]), schemes = ([]);
   if (file_size(MATERIALS_CONFIG_FILE) < 0)
   {
      write("Error: Missing config file '" + MATERIALS_CONFIG_FILE + "'.");
      return nil;
   }
   foreach (string line in explode(read_file(MATERIALS_CONFIG_FILE), "\n"))
   {
      string category, parts, material;
      string *names;
      line = trim(line);
      if (line == "" || line[0] == '#' || sscanf(line, "%s:%s", category, parts) != 2)
         continue;
      category = trim(category);
      names = map(explode(parts, ","), trim) - ({""});
      switch (strtype(category))
      {
      case 0:
         categories[category[1..<2]] = names;
         break;
      case 1:
         if (category[0..7] != "quality-" || strlen(category) <= 8 || !sizeof(names) || sizeof(names) > 8)
            error("Invalid quality definition for '" + category + "'.");
         category = category[8..];
         qualities[category] = ([]);
         for (int i = 0; i < sizeof(names); i++)
            qualities[category][1 << (sizeof(names) - i - 1)] = names[i];
         break;
      case 2:
         sscanf(category, "%s/%s", category, material);
         category = trim(category);
         material = trim(material);
         if (!schemes[category])
            schemes[category] = ([]);
         schemes[category][material] = names;
         break;
      default:
         error("Unknown type for '" + category + "'.");
      }
   }
   return (["materials":categories, "qualities":qualities, "upgrades":schemes]);
}

void load_config_from_file()
{
   mapping config = read_material_config();
   mapping schemes;
   if (!config)
      return;
   schemes = checked_upgrades(config["upgrades"], config["materials"], config["qualities"], crafting_recipes);
   materials = config["materials"];
   special_mats = config["qualities"];
   upgrade_mat_schemes = schemes;
   refresh_cache();
   save_me();
   write(__FILE__ + ": " + MATERIALS_CONFIG_FILE + " loaded and saved.");
}

int count_materials()
{
   return array_sum(values(map(materials, ( : sizeof($2) :))));
}

void stat_me()
{
   foreach (string category in keys(materials))
   {
      write("%^BOLD%^Category: %^RESET%^%^YELLOW%^" + category + "%^RESET%^\n");
      write(" " + format_list(materials[category], "and"));
      write("\n");
   }
   write("\n\n%^BLUE%^Crafting recipes:%^RESET%^ \n");
   write("  " + implode(sort_array(keys(crafting_recipes), 1), "\n  "));
   write("\n");

   write("\n%^BLUE%^Upgrade schemes: %^RESET%^\n");
   foreach (string name, mapping m in upgrade_mat_schemes)
   {
      write("Category: " + name + "\n");
      foreach (string name2, string * sar in m)
      {
         write(" [" + name2 + "]\n\t" + implode(sar, ",") + "\n");
      }
      write("\n");
   }
}

void create()
{
   mapping config;
   int fresh;
   ::create();
   config = read_material_config();
   fresh = !sizeof(materials);
   if (fresh && config)
      materials = config["materials"];
   if (config)
      special_mats = config["qualities"];
   refresh_cache();
   load_crafting_recipes();
   if (fresh && config)
      upgrade_mat_schemes = checked_upgrades(config["upgrades"], materials, special_mats, crafting_recipes);
   save_me();
}

//@CRAFTING_D->remove_category("plastic")
//@CRAFTING_D->add_category("plastic",({"soft pvc", "molded plastic", "fiberglass spool","ethylene granulate"}))