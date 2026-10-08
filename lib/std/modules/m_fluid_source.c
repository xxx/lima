/* Do not remove the headers from this file! see /USAGE for more info. */

// Renewable sources own the temporary clones used for filling and ID discovery.
string source_of;
mixed fluid_disturb = "The $o1 ripples gently as you disturb it.";
mixed fill_action = "$N $vdip $p $o into $o1 and $vwithdraw some $o2.";

void set_source_of(string what)
{
   object ob = clone_object(what);
   string *ids;
   mixed failure = catch(ids = ob->query_id());
   ob->remove();
   if (failure) error(failure);
   foreach (string id in ids)
      this_object()->add_id(id + "_source");
   this_object()->add_id("fluid_source", "fill_source", "drink_source");
   source_of = what;
}

string is_source_of() { return source_of; }
void set_fluid_disturb_action(mixed x) { fluid_disturb = x; }
mixed get_fluid_disturb_action() { return fluid_disturb; }
void set_fill_action(mixed x) { fill_action = x; }

private int fill_container(object bottle, object fluid)
{
   object actor = this_body();
   string name = fluid->short();
   if (!bottle->fill_with(fluid))
      return 0;
   if (functionp(fill_action))
      evaluate(fill_action, bottle, this_object());
   else if (fill_action && actor)
      actor->simple_action(fill_action, bottle->short(), this_object()->the_short(), name);
   if (functionp(fluid_disturb))
      evaluate(fluid_disturb);
   else if (fluid_disturb && actor)
      actor->my_action(fluid_disturb, name, this_object());
   return 1;
}

int fill_up(object bottle)
{
   object fluid;
   int result;
   mixed failure;
   if (!bottle) return 0;
   fluid = clone_object(this_object()->is_source_of());
   failure = catch(result = fill_container(bottle, fluid));
   if (fluid && !environment(fluid))
      fluid->remove();
   if (failure) error(failure);
   return result;
}

mixed direct_drink_obj() { return "#You cannot drink an entire " + this_object()->query_primary_id(); }
mixed indirect_drink_str_from_obj(string fluid, object ob)
{
   if (member_array(fluid + "_source", this_object()->query_id()) < 0)
      return "You cannot drink " + fluid + " from that.\n";
   return 1;
}
mixed direct_drink_str_from_obj(string fluid, object ob) { return indirect_drink_str_from_obj(fluid, ob); }
int indirect_fill_obj_from_obj() { return 1; }
mixed indirect_drink_obj_from_obj() { return 1; }
