/* Do not remove the headers from this file! see /USAGE for more info. */

//      /bin/system/_update.c
//      from the Nightmare Mudlib
//      destructs an old version of a file and loads a new one
//      created by Sulam 911219
//      security changes by Descartes of Borg 930810
//      mixed with the TMI update command by Beek 9301119
//      ported to ZorkMud, Beek 5/13/94
//      also added some features from the previous ZorkMUD update
//      ok, so I ended up rewriting alot of it.
//      Made it able to deal with 3.2's deep-only inherit_list

//: COMMAND
//$$ see : renew
// USAGE
//
//    |  ``update <filename>``
//    |  ``update <object>``
//
// Destructs the old version of a file and loads a new one
// Flags :
//  |  -r - Ingnore inherited files
//  |  -R - Update all inherited files
//  |  -z - Update only inherited files "older" than the target file.
//
// .. TAGS: RST

#include <daemons.h>

inherit CMD;

int do_update(string file, int deep);

private
void main(mixed *arg, mapping flags)
{
   object *obs;
   object ob;
   int deep_up;
   string file;
   object shell_ob = this_user()->query_shell_ob();

   // This is a quick hack to make update accept cfile*
   if (arg[0] && !stringp(arg[0]))
   {
      foreach (file in arg[0])
         catch (main(({file}), flags));
      return;
   }

   if (flags["r"])
      deep_up = 1;
   if (flags["R"])
      deep_up = 2;
   if (flags["z"])
      deep_up = 3;

   file = arg[0];
   if (!file && shell_ob)
      file = shell_ob->get_variable("cwf");
   if (!file || file == "here")
      file = "/" + file_name(environment(this_body()));

   file = evaluate_path(file);

   if (shell_ob)
      shell_ob->set_cwf(file);

   sscanf(file, "%s.c", file);
   // DO NOT check if the file exists.  It's really bad for virtual objects.
   if (file_size(file) == -2 && file_size(file + ".c") < 0)
   {
      outf("update: file is a directory.\n");
      return;
   }

   if (file_size(file + ".c") && shell_ob)
      shell_ob->set_cwf(file + ".c");

   check_reload_target(file);
   obs = ({});
   if ((ob = find_object(file)) && ob != find_object(VOID_ROOM))
   {
      obs = filter(
          all_inventory(ob), (: [object ob] {
             object link = ob->query_link();
             if (link && interactive(link))
             {
                ob->move(VOID_ROOM);
                return 1;
             }
             destruct(ob);
             return 0;
          } :));
   }
   if (file[ < 2.. < 1] != ".c" && file[ < 4.. < 1] != ".scr")
      file += ".c";
   if (do_update(file, deep_up) < time())
      out(file + ": No update necessary.\n");

   obs->move(file);
}

int do_update(string file, int deep)
{
   object ob;
   int newest_inh = 0;

   check_reload_target(file);
   if (deep == 3)
   {
      mixed *info = update_file_info(file);
      ob = load_object(file);
      if (!ob)
         error("update: could not load " + file);

      newest_inh = update_inherits(ob, deep);
      if (sizeof(info))
      {
         if (info[1] > newest_inh)
            newest_inh = info[1];

         // return if object isn't out of date.
         if (info[2] >= newest_inh)
            return info[2];
      }
      destruct(ob, 1); // pass 1: we're coming back soon
   }
   else
   {
      ob = find_object(file);
      if (ob)
      {
         if (deep)
            update_inherits(ob, deep);
         destruct(ob, 1); // pass 1: we're coming back soon
      }
   }
   if (!load_object(file))
      error("update: could not load " + file);
   if (file[0] != '/')
      file = "/" + file; // bug in inherit_list()
   out(file + ": Updated and loaded.\n");
   return time();
}

private int update_inherits(object ob, int deep)
{
   string *files = deep == 1 ? inherit_list(ob) : deep_inherit_list(ob);
   int newest = 0;
   int updated;

   if (deep == 3)
   {
      // Loaded direct parents already account for their own ancestor timestamps.
      foreach (string file in inherit_list(ob))
      {
         object parent = find_object(file);
         if (!parent)
            continue;
         files -= ({file}) + deep_inherit_list(parent);
         updated = do_update(file, deep);
         if (updated > newest)
            newest = updated;
      }
   }

   foreach (string file in files)
   {
      if (find_object(file))
         updated = do_update(file, deep == 3 ? deep : 0);
      else if (deep == 3)
      {
         updated = file_time(file);
         if (updated < 0)
            error("update: could not read inherited source " + file);
      }
      else
         continue;
      if (updated > newest)
         newest = updated;
   }
   return newest;
}

private int *update_file_info(string file)
{
   int size = file_size(file);
   int modified = file_time(file);
   object loaded = find_object(file);
   if (size < 0 || modified < 0)
      return ({});
   return ({size, modified, loaded ? object_time(loaded) : 0});
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg, flags);
}

private void check_reload_target(string file)
{
   object ob = find_object(file);
   if (ob && (ob == master() || ob == find_object(SIMUL_OB)))
      error("update: resident master/simul efuns require queued recompilation; use master for the master object");
}
