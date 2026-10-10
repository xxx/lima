/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE:
//     ``dbxfuncs <ob> <funcs>``
//
// Finds functions in an objects, e.g.:
//
//  |   ``dbxfuncs /secure/simul_efun arr``
//  |   Matches:
//  |   mixed * clean_array(x)        (defined in /secure/simul_efun/misc)
//  |   mixed array_sum(x)            (defined in /secure/simul_efun/misc)
//  |   mixed flatten_array(x)        (defined in /secure/simul_efun/misc)
//
// .. TAGS: RST

#include <mudlib.h>

inherit CMD;

private void main(string str)
{
   string obname, pattern;
   object ob;

   if (!str || sscanf(str, "%s %s", obname, pattern) != 2)
   {
      outf("dbxfuncs <ob> <funcs>\n");
      return;
   }

   obname = evaluate_path(obname);
   if (!(ob = find_object(obname)))
   {
      outf("Can't find object %s\n", obname);
      return;
   }

   string *names = regexp(functions(ob), pattern);
   if (!sizeof(names))
   {
      outf("No matches\n");
      return;
   }

   outf("Matches:\n");
   foreach (string name in names)
   {
      mapping signature = function_signature(name, ob);
      string line = signature["return_type"] + " " + name + "(";
      line += implode(allocate(signature["num_args"], "x"), ", ") + ")";
      string where = signature["program"];
      if (where[<2..] == ".c")
         where = where[..<3];
      outf("%-30s(defined in %s)\n", line, where);
   }
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
