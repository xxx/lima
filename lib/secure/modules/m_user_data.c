/* Do not remove the headers from this file! see /USAGE for more info. */

#include <user_data.h>

private mapping user_data_field(string name)
{
   mapping found;
   object daemon = find_object(USER_D);
   if (!daemon || previous_object() != daemon)
      error("user data: caller is not USER_D");
   foreach (mapping field in variable_info(this_object()))
      if (field["name"] == name)
      {
         if (found) error("user data: ambiguous field");
         found = field;
      }
   if (!found) error("user data: unknown field");
   return found;
}

nomask mixed query_user_data(string name)
{
   return deep_copy(user_data_field(name)["value"]);
}

nomask void set_user_data(string name, mixed value)
{
   mapping field = user_data_field(name);
   validate_user_data(name, value);
   store_variable(this_object(), name, deep_copy(value), field["program"]);
}
