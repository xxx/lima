/* Do not remove the headers from this file! see /USAGE for more info. */

// You must inherit M_INPUT for this stuff to work.
void modal_simple(function, mixed);
void do_digging(object);

private
void answer_question(string input)
{
   object o;

   if (!stringp(input))
      return;
   if (o = present(input, environment(this_body())))
      do_digging(o);
   else
      this_user()->modal_pass(input);
}

void do_digging(object arg)
{
   object *o;

   if (!arg)
   {
      o = filter(all_inventory(environment(this_body())), ( : $1->is_diggable() :));
      switch (sizeof(o))
      {
      case 0:
         write("There is nothing in which to dig here.\n");
         return;
      case 1:
         printf("[in %s]\n", o[0]->the_short());
         arg = o[0];
         break;
      default:
         modal_simple(answer_question, "What do you want to dig in? ");
         return;
      }
   }
   arg->dig();
}

int can_dig()
{
   return 1;
}

int can_dig_with_obj()
{
   return 1;
}

int direct_dig_with_obj()
{
   return 1;
}

int indirect_dig_obj_with_obj()
{
   return 1;
}

int indirect_dig_in_obj_with_obj()
{
   return 1;
}

int indirect_dig_up_obj_with_obj()
{
   return 1;
}

int indirect_dig_out_obj_with_obj()
{
   return 1;
}

int direct_dig_with_obj_in_obj()
{
   return 1;
}
