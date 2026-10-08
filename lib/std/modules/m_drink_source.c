/* Do not remove the headers from this file! see /USAGE for more info. */

// Source drinks use a disposable sample for their taste action.
mixed drink_action = "$N $vdrink some $o from $o1.";

void set_drink_action(mixed x)
{
   if (!x)
      this_object()->remove_id("drink_source");
   else
      this_object()->add_id("drink_source");
   drink_action = x;
}

private int sample_drink(object fluid_ob)
{
   mixed action;
   object actor = this_body();
   if (functionp(drink_action))
   {
      if (!evaluate(drink_action)) return 0;
   }
   else if (actor)
      actor->simple_action(drink_action, fluid_ob->query_primary_name(), this_object()->the_short());
   action = this_object()->get_fluid_disturb_action();
   if (functionp(action))
   {
      if (!evaluate(action)) return 0;
   }
   else if (action && actor)
      actor->my_action(action, fluid_ob, this_object());
   action = fluid_ob->get_taste_action();
   if (functionp(action)) return evaluate(action);
   if (action && actor) actor->my_action(action, fluid_ob);
   return 1;
}

varargs int drink_from_it(mixed fluid)
{
   object sample;
   int result;
   mixed failure;
   if (!drink_action) return 0;
   sample = clone_object(this_object()->is_source_of());
   failure = catch(result = sample_drink(sample));
   if (sample) sample->remove();
   if (failure) error(failure);
   return result;
}

int is_drink_source() { return !!drink_action; }
mixed direct_drink_obj() { return "#You cannot drink an entire " + this_object()->query_primary_id(); }
mixed direct_drink_from_obj() { return drink_action ? 1 : "Try putting the fluid into something.\n"; }
mixed direct_drink_str_from_obj(string fluid)
{
   if (!drink_action || member_array(fluid + "_source", this_object()->query_id()) < 0)
      return "You cannot drink " + fluid + " from that.\n";
   return 1;
}
mixed indirect_drink_obj_from_obj() { return direct_drink_from_obj(); }
