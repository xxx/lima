/* Do not remove the headers from this file! see /USAGE for more info. */

// Fluid vessels delegate drinking and pouring to their contents.
#include <move.h>

mixed leak_action = "$O leaks out of $o1.";
mixed full_action = "The $o1 is too full for $o.";
mixed fluid_only = 1;

private object contained_fluid()
{
   foreach (object ob in all_inventory(this_object()))
      if (ob->id("fluid"))
         return ob;
   return nil;
}

varargs int can_hold_fluid(mixed ob) { return 1; }
void set_fluid_only(mixed x) { fluid_only = x; }
mixed query_fluid_only() { return fluid_only; }
void set_leak_action(mixed x) { leak_action = x; }
void set_full_action(mixed x) { full_action = x; }

int leak(object ob)
{
   object actor = this_body();
   if (functionp(leak_action))
      evaluate(leak_action, ob);
   else if (!actor || !environment(actor))
      ob->remove();
   else if (ob->move(environment(actor)) == MOVE_OK)
   {
      if (leak_action)
         actor->simple_action(leak_action, ob, this_object());
      ob->start_evaporation();
   }
   return 0;
}

private float remaining_capacity()
{
   return to_float(this_object()->query_max_capacity()) - this_object()->query_capacity();
}

int will_fit(object fluid)
{
   return fluid && fluid != this_object() && fluid->query_mass() <= remaining_capacity();
}

private void show_full_action(object ob)
{
   if (functionp(full_action))
      evaluate(full_action, ob);
   else if (full_action && this_body())
      this_body()->my_action(full_action, ob->short(), this_object());
}

varargs object fluid_already(object fluid, string other_id)
{
   if (!fluid)
      return nil;
   if (!other_id)
      other_id = fluid->query_primary_id();
   foreach (object ob in all_inventory(this_object()))
      if (ob != fluid && ob->id("fluid") && ob->id(other_id))
         return ob;
   return nil;
}

private int transfer_fluid(object fluid, int refill)
{
   object resident;
   float previous_mass, room_left;
   int previous_quantity;
   if (!fluid || environment(fluid) == this_object())
      return 0;
   if (!this_object()->can_hold_fluid(fluid))
      return leak(fluid);
   room_left = remaining_capacity();
   if (room_left <= 0.0 || (!refill && !will_fit(fluid)))
   {
      show_full_action(fluid);
      return 0;
   }
   if (!fluid->fill_action(this_object()))
      return 0;
   resident = fluid_already(fluid);
   if (!resident)
      resident = fluid_already(fluid, "fluid");
   previous_mass = fluid->query_mass();
   previous_quantity = fluid->query_quantity();
   if (refill)
      fluid->set_fluid_level(room_left);
   if (fluid->move(this_object()) != MOVE_OK)
   {
      if (refill)
      {
         fluid->set_fluid_level(previous_mass);
         fluid->set_quantity(previous_quantity);
      }
      return 0;
   }
   if (resident)
   {
      if (resident->query_primary_id() == fluid->query_primary_id())
         return resident->add_fluid(fluid);
      return resident->do_mix(fluid);
   }
   return 1;
}

int move_fluid(object ob) { return transfer_fluid(ob, 1); }
int part_move_fluid(object ob) { return transfer_fluid(ob, 0); }
int fill_with(object fluid) { return transfer_fluid(fluid, 1); }
int part_fill_with(object fluid) { return transfer_fluid(fluid, 0); }

varargs void drink_from_it(object fluid)
{
   object ob = contained_fluid();
   if (fluid && objectp(fluid))
      ob = environment(fluid) == this_object() && fluid->id("fluid") ? fluid : nil;
   if (!ob)
   {
      write("Pity there's nothing in it.\n");
      return;
   }
   ob->drink_it();
}

void drink_it() { drink_from_it(nil); }

varargs mixed pour(string how, object where, object from)
{
   object fluid = contained_fluid();
   if (!fluid)
      return this_object()->the_short() + " is empty.";
   fluid->pour(how, where, this_object());
   return 1;
}

string get_extra_long()
{
   object ob = contained_fluid();
   int capacity = this_object()->query_max_capacity();
   int percent;
   if (!ob || capacity <= 0)
      return "It is empty.\n";
   percent = to_int(100.0 * ob->query_mass() / to_float(capacity));
   if (percent <= 0) return "It is empty.\n";
   if (percent <= 20) return "It is almost empty.\n";
   if (percent <= 40) return "It is less than half full.\n";
   if (percent <= 60) return "It is about half full.\n";
   if (percent <= 80) return "It is more than half full.\n";
   if (percent < 100) return "It is almost full.\n";
   return "It is full to the brim.\n";
}

mixed have_flask()
{
   if (environment() != this_body())
      return "You do not have " + this_object()->the_short() + ".";
   return 1;
}

mixed indirect_pour_obj_wrd_obj() { return 1; }
mixed direct_pour_obj_wrd_obj() { return have_flask(); }
mixed direct_pour_obj() { return have_flask(); }
mixed direct_fill_obj() { return have_flask(); }
mixed direct_fill_obj_with_obj() { return have_flask(); }
mixed direct_fill_obj_with_str() { return have_flask(); }
mixed direct_fill_obj_from_obj() { return have_flask(); }
mixed direct_drink_from_obj() { return have_flask(); }
mixed indirect_drink_obj_from_obj() { return have_flask(); }
mixed direct_drink_obj()
{
   mixed held = have_flask();
   if (held != 1) return held;
   return contained_fluid() ? 1 : this_object()->the_short() + " is empty.";
}
