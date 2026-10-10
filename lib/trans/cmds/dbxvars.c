/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE:
//     ``dbxvars <ob> <var>``
//
// Finds global variables in a loaded object by regular expression, e.g.:
//
//  |   ``dbxvars /std/body name``
//  |   Matches:
//  |   corpse_filename     : "/std/corpse"
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
      out("dbxvars <ob> <var>\n");
      return;
   }
   obname = evaluate_path(obname);
   if (!(ob = find_object(obname)))
   {
      out("Can't find object " + obname + "\n");
      return;
   }
   mapping *entries = variable_info(ob);
   string *names = regexp(map(entries, (: $1["name"] :)), pattern);
   if (!sizeof(names))
   {
      out("No matches\n");
      return;
   }
   mapping counts = ([]);
   foreach (string name in names)
   {
      if (counts[name])
         counts[name]++;
      else
         counts[name] = 1;
   }
   out("Matches:\n");
   foreach (mapping entry in entries)
   {
      string name = entry["name"];
      if (!counts[name])
         continue;
      if (counts[name] > 1)
         name += " (" + entry["program"] + ")";
      outf("%-20s: %Q\n", name, entry["value"]);
   }
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
