/* Do not remove the headers from this file! see /USAGE for more info. */

inherit ADMTOOL_BASE;

nomask string module_name()
{
   return "memory";
}

nomask string module_key()
{
   return "M";
}

private
void obj_and_prog()
{
   mapping info = ([]);
   mixed *k;
   string *ret;

   foreach (object o in objects())
      info[file_name(o)] = object_memory_info(o)["total_bytes"];

   k = keys(info);
   k = sort_array(k, ( : $(info)[$2] - $(info)[$1] :));

   ret = allocate(sizeof(k) + 2);
   ret[0] = "Estimated bytes including shared program/data storage; rows are not additive.";
   ret[1] = "";
   for (int i = 0; i < sizeof(k); i++)
      ret[i + 2] = sprintf("%-70.70s:%i", k[i], info[k[i]]);

   more(ret);
}

nomask class command_info *module_commands()
{
   return ({new (class command_info, key : "o", desc : "objects and programs", action : obj_and_prog)});
}
