#ifndef LIMA_USER_DATA_H
#define LIMA_USER_DATA_H

private void validate_user_data(string name, mixed value)
{
   if (name == "failures")
   {
      if (nilp(value)) return;
      if (!arrayp(value)) error("user data: failures must be an array");
      foreach (mixed row in value)
         if (!arrayp(row) || sizeof(row) != 2 || !intp(row[0]) || !stringp(row[1]))
            error("user data: invalid failure record");
   }
   else if (!stringp(value) && !nilp(value))
      error("user data: " + name + " must be a string");
}

#endif
