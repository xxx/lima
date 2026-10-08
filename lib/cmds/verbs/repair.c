/* Do not remove the headers from this file! see /USAGE for more info. */

inherit VERB_OB;
inherit M_INPUT;

//: COMMAND
// USAGE
//
//  |  ``repair <something>``
//  |  ``repair <something> <preposition> <something>``
//
//  Repair the broken window, repair your sword with metal shards.
//
// .. TAGS: RST

void do_repair()
{
   write("Repair what with what?\n");
}

private int accessible(object body, object item)
{
   if (!body || !item || !environment(body))
      return 0;
   object place = environment(item);
   if (place == environment(body))
      return 1;
   while (place && place != environment(body))
   {
      if (place == body)
         return 1;
      place = environment(place);
   }
   return 0;
}

private nomask void repair_with_what(object body, mixed what, string reply)
{
   string choice = lower_case(trim(reply));
   if (choice != "m" && choice != "r")
   {
      write("Aborting.\n");
      return;
   }
   if (!body || body != this_body())
   {
      write("The repair is no longer available.\n");
      return;
   }
   object *items = objectp(what) ? ({what}) : what;
   if (!arrayp(items) || !sizeof(items))
      return;
   foreach (object item in items)
      if (!accessible(body, item) || (arrayp(what) && environment(item) != body))
      {
         write("A repair item is no longer available.\n");
         return;
      }
   if (choice == "m")
   {
      write("Repairing using money.\n");
      write(CRAFTING_D->pay_for_repair_with_money(body, what) ? "All repaired.\n" : "You didn't have the money?\n");
   }
   else
   {
      write("Using materials for repairs.\n");
      write(CRAFTING_D->pay_for_repair(body, what) ? "All repaired.\n" : "You were missing parts?\n");
   }
}

void do_repair_obj_with_obj(object ob, object with)
{
}

void do_repair_obj_str_obj(object ob, string s, object with)
{
   if (s != "using" && s != "with")
   {
      write("Repair what how, sorry?");
      return;
   }
   if (ob->is_living())
   {
      write(capitalize(ob->query_name()) + " seems beyond repair, sorry.\n");
      return;
   }

   if (ob)
      ob->do_repair(with);
}

void do_repair_obj(object ob)
{
   object *tools = ({});
   object tool;

   object body = this_body();
   if (!accessible(body, ob))
      return;

   if (ob->missing_durability())
   {
      mixed *estimate = CRAFTING_D->estimate_with_mats(body, ob);
      write(estimate[1]);
      if (estimate[0])
      {
         write("\t%^MENU_CHOICE%^r%^RESET%^: Use materials to Repair " + ob->short() + "?\n" +
               "\t%^MENU_CHOICE%^m%^RESET%^: Use only money, " + CRAFTING_D->repair_cost_string(ob) + ", instead?\n" +
               "\t%^MENU_CHOICE%^Q%^RESET%^: Quit (default)");
         modal_simple(&repair_with_what(body, ob));
      }
      else
      {
         printf("You do not have the funds to repair %s.\n", ob->the_short());
      }
      return;
   }

   if (all_inventory(body))
      tools = filter(all_inventory(body), ( : $1->is_tool() :));

   if (sizeof(tools))
   {
      tool = choice(tools);
      write("(Using " + tool->the_short() + ")\n");
      do_repair_obj_str_obj(ob, "with", tool);
   }
   else
   {
      if (ob)
         ob->do_repair();
   }
}

void do_repair_obs(object *obs)
{
   object *filtered_obs = ({});
   mixed *estimate;
   object body = this_body();

   foreach (object ob in obs)
   {
      if (ob && environment(ob) == body && ob->missing_durability() && member_array(ob, filtered_obs) == -1)
      {
         filtered_obs += ({ob});
      }
   }
   if (!sizeof(filtered_obs))
   {
      write("You find nothing in need of repair.\n");
      return;
   }
   estimate = CRAFTING_D->estimate_with_mats(body, filtered_obs);
   write(estimate[1]);
   if (estimate[0])
   {
      write("\t%^MENU_CHOICE%^r%^RESET%^: Use materials to Repair?\n"
            "\t%^MENU_CHOICE%^m%^RESET%^: Use only Money, " +
            CRAFTING_D->repair_cost_string(filtered_obs) +
            ", instead?\n"
            "\t%^MENU_CHOICE%^Q%^RESET%^: Quit (default)");
      modal_simple(&repair_with_what(body, filtered_obs));
   }
   else
   {
      printf("You do not have the funds to repair the items.\n");
   }
}

void create()
{
   add_rules(({"", "OBJ", "OBS", "OBJ STR OBJ"}), ({"fix", "patch"}));
}
