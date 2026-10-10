/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE: ``loadall <dir>``
//
// Removes and reloads all the files in a specified directory
//
// .. TAGS: RST

/* To help with conversion testing */
#include <mudlib.h>
inherit CMD;

private
void main(mixed *args)
{
   string *files;
   object *obs;

   outf("Attempting to load %s/*.c...  ", args[0]);

   /*
    ** Get all the files in the given directory.  If this command is in
    ** the directory, then remove it... we don't want to dest self.
    */
   files = map(get_dir(args[0] + "/*.c"), ( : $(args[0]) + "/" + $1:));
   files -= ({file_name(this_object()) + ".c"});

   /*
    ** Blow away the old objects
    */
   foreach (string file in files)
   {
      object loaded = find_object(file);
      if (loaded && (loaded == master() || loaded == find_object(SIMUL_OB)))
         error("loadall: resident master/simul efuns require queued recompilation");
   }

   obs = map(files, ( : find_object($1) :)) - ({0});
   obs->remove();
   obs -= ({0});
   map(obs, ( : destruct($1) :));

   /*
    ** Load all the objects
    */
   foreach (string file in files)
      if (!load_object(file))
         error("loadall: failed to load '" + file + "'");

   out("Succeeded.\n");
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
