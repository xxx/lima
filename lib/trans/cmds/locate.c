/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE:
//
//     |  ``locate <target>``
//     |  ``locate -u``
//
// Find any file containing the specified target within its name.
//
// The system relies on a database of filenames for speed.
// To rebuild that database, use a -u flag.
//
// .. TAGS: RST

inherit CMD;

// ************ Wizard info *************
// If you are receiving warnings or errors, reduce this value to a smaller value.
// This will, however, increase the time taken to rebuild the database.
#define ITERS_PER_CALL 50

// Do not touch these.
#define DATA_FILE "/data/locate.codes"
#define TMP_DATA_FILE "/data/locate.tmp"

mixed *stack;
string path;
int building_database = 0;
int started_at;

private
void end_building()
{
   mixed failure = catch(rename(TMP_DATA_FILE, DATA_FILE));
   building_database = 0;
   if (failure)
      printf("Locate DB build failed: %s\n", failure);
   else
      printf("Locate DB build done in %s.\n", time_to_string(time() - started_at, 0));
}

private
void rebuild_locate_db()
{
   mixed failure = catch {
      int i = ITERS_PER_CALL;
      while (i-- && sizeof(stack))
      {
         string new_path;
         string *this_dir;
         if (intp(stack[0]))
         {
            stack = stack[1..];
            path = split_path(path)[0];
            i++;
            continue;
         }
         new_path = path + stack[0] + "/";
         stack = stack[1..];
         this_dir = get_dir(new_path + "*");
         foreach (string name in this_dir)
            write_file(TMP_DATA_FILE, join_path(new_path, name) + "\n");
         this_dir = filter(this_dir, (: is_directory(join_path($(new_path), $1)) :));
         if (sizeof(this_dir))
         {
            stack = this_dir + ({0}) + stack;
            path = new_path;
         }
      }
   };
   if (failure)
   {
      stack = ({});
      building_database = 0;
      printf("Locate DB build failed: %s\n", failure);
      return;
   }
   if (sizeof(stack))
      call_out(rebuild_locate_db, 1);
   else
      // Rename requires the temporary file to exist on disk before publication.
      call_out(end_building, 0);
}

private
void begin_database_build()
{
   mixed failure = catch(write_file(TMP_DATA_FILE, "", 1));
   if (failure)
   {
      outf("Locate DB build failed: %s\n", failure);
      return;
   }
   stack = ({""});
   path = "";
   building_database = 1;
   rebuild_locate_db();
}

private
void main(mixed *arg, mapping flags)
{
   if (!arg[0] && !flags["u"])
   {
      out("Syntax: locate <string>\n" + "        locate -u (to update the database)");
      return;
   }
   if (flags["u"])
   {
      if (building_database)
      {
         out("Already building database.\n");
         return;
      }
      out("Building database... This can take a while for large MUDs.\n");
      started_at = time();
      begin_database_build();
   }
   else
   {
      string *matches;
      mixed failure = catch {
         string text = read_file(DATA_FILE);
         string *names = sizeof(text) ? explode(text, "\n") : ({});
         if (sizeof(text) && text[<1] == '\n')
            names = names[0..<2];
         matches = regexp(names, arg[0]);
         if (!sizeof(matches))
            out("No matching files found.\n");
         else
            out(implode(matches, "\n") + "\n");
      };
      if (failure)
         outf("Cannot search locate database: %s\n", failure);
   }
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg, flags);
}
