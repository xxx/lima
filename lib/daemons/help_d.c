/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** File: help_d.c
**
** Created 25-OCT-94 by Deathblade for the new help system.
**
** This daemon keeps track of topic.pathname mappings.
** It will also manage a topic name appearing in multiple
** locations.
*/

#include <dirs.h>
#include <mudlib.h>

inherit M_DAEMON_DATA;

// count total functions found in rst files for completion check.
private
nosave int fun_total;

// mapping topic_refs, to record directories and topic names
//       which should be used as additional topic references for all files
//       within those dirs - eg all files within /help/player/bin as to be
//       available via "help player commands"

private
nosave mapping topic_refs =
    (["/help/player/bin/":"player_commands",
        "/help/player/command/":"player_commands", "/help/player/quests/":"quests", "/help/player/":"topics",
            "/help/wizard/bin/":"wizard_commands", "/help/admin/command/":"admin_commands", ]);

// mapping tags holds tags for each of the documentation files.
private
mapping tags;

/*
** This mapping contains all the topics in the system.  It
** maps topic names to an array of pathnames.  The level-
** restriction on the topic is then computed by examining
** the lead part of the pathname.
*/
private
mapping topics;

/*
** This mapping contains the level-restrictions on the help
** sub-directories.  It is read from the _restrict file in
** the /help directory.
**
** It maps subdir names to levels.
*/
private
mapping restrict;

private
nosave int pending_count;
private
nosave mixed rebuild_error;
private
nosave object initiator;

private
nosave mixed *ignore = ({DIR_HELP "/autodoc/FIXME/"});

nomask void process_dir(string path);

private
int f_restrict(string s)
{
   string *r;

   if (s[0] == '#')
      return 0;
   if (member_array(':', s) == -1)
      return 0;
   r = explode(s, ":");
   restrict[trim(r[0])] = to_int(r[1]);
}

private
parse_rst(string path, string file)
{
   string *contents = filter(explode(read_file(path + file), "\n"),
                                   (
                                       : strsrch($1, ".. c:function::") != -1 || strsrch($1, "TAGS: ") != -1
                                       :));

   if (!sizeof(contents))
      return;

   fun_total += sizeof(contents);

   foreach (string f in contents)
   {
      string fun, tmp;
      fun = explode(explode(f, "(")[0], " ")[ < 1];
      if (fun[0] == '*')
         fun = fun[1..];

      if (strlen(f) > 7 && f[0..7] == ".. TAGS:")
      {
         string *taglist = explode(f[8..], " ");
         foreach (string t in taglist)
         {
            if (tags[t])
               tags[t] += ({path + file});
            else
               tags[t] = ({path + file});
         }
         continue;
      }

      if (topics[fun])
         topics[fun] += ({path + file});
      else
         topics[fun] = ({path + file});
   }
}

private
void process_file(string path, string file)
{
   string pathname;
   string *player_dirs = keys(topic_refs);
   string rfile = "";

   if (file == "." || file == "..")
      return;

   if (file[0] == '_' || file == ".gitignore")
      return;

   pathname = path + file;

   if (file_size(pathname) == 0)
   {
      write("Warning: '" + pathname + "' contains nothing.\n");
      return;
   }
   if (file_size(pathname) == -2)
   {
      pathname += "/";
      /* Ack. Avoid execution cost errors. */
      call_out(&process_dir(pathname), (1 * TOO_GREEDY_DAEMONS));
      ++pending_count;
   }

   if (strlen(file) > 4 && file[ < 4..] == ".rst")
   {
      parse_rst(path, file);
      file = file[0.. < 5];
   }

   if (strlen(file) > 4 && file[ < 3..] == ".md")
   {
      file = file[0.. < 4];
   }

   if (strsrch(file, "-") != -1)
      file = implode(explode(file, "-")[1..], "");

   file = lower_case(file);

   if (topics[file])
      topics[file] += ({pathname});
   else
      topics[file] = ({pathname});

   if (member_array(path, player_dirs) > -1)
   {
      rfile = topic_refs[path];
      if (topics[rfile])
         topics[rfile] += ({pathname});
      else
         topics[rfile] = ({pathname});
   }
}

private void finish_rebuild()
{
   string message;
   object recipient;

   if (pending_count)
      return;

   if (!rebuild_error)
      rebuild_error = catch(save_me());

   if (rebuild_error)
      message = "HELP_D rebuild failed; the previous saved index was retained.\n" +
                sprintf("%O\n", rebuild_error);
   else
      message = "HELP_D has finished the rebuild.\n" +
                sprintf("  %5.5s", "" + fun_total) + " functions added to index.\n" +
                sprintf("  %5.5s", "" + sizeof(clean_array(flatten_array(values(topics))))) + " help files known.\n" +
                sprintf("  %5.5s", "" + sizeof(keys(topics))) + " topics found in index.\n";

   recipient = initiator;
   initiator = nil;
   if (recipient)
   {
      if (catch(tell(recipient, message)))
         debug_message(message);
   }
   else if (rebuild_error)
      debug_message(message);
}

nomask void process_dir(string path)
{
   mixed failure;

   if (member_array(path, ignore) == -1)
      failure = catch(map(get_dir(path + "*"), &process_file(path)));
   if (failure && !rebuild_error)
      rebuild_error = failure;

   --pending_count;
   finish_rebuild();
}

private void scan_help_root()
{
   string *lines = explode(read_file(DIR_HELP "/_restrict"), "\n");
   map(lines, f_restrict);

   string *dirs = filter(get_dir(DIR_HELP "/*"), ( : $(restrict)[$1] != 99 :));
   map(dirs, &process_file(DIR_HELP "/"));
}

nomask void rebuild_data()
{
   mixed failure;

   if (pending_count)
   {
      write("HELP_D is STILL rebuilding!\n");
      return;
   }

   pending_count = 1;
   initiator = this_user();
   rebuild_error = nil;
   write("HELP_D is beginning the rebuild...\n");

   topics = ([]);
   tags = ([]);
   restrict = ([]);
   fun_total = 0;

   failure = catch(scan_help_root());
   if (failure)
      rebuild_error = failure;
   --pending_count;
   finish_rebuild();
}

nomask void create()
{
   ::create();

   if (!topics)
      rebuild_data();
}

/*
** find_topic()
**
** Will return nil if the topic does not exist (within the current
** player's level).  Otherwise, an array of pathnames will be
** returned.
*/
nomask string *find_topic(string name)
{
   mixed *result;
   int lvl;

   if (!this_body())
      return nil;
   result = topics[lower_case(name)];
   if (!result)
      return nil;

   // ### simulate the old levels
   lvl = adminp(this_user()) ? 5 : wizardp(this_user()) ? 1 : 0;

   return filter(
       result,
       (: [string file, int lvl] {
          mixed *parts = explode(file, "/");
          if (sizeof(parts) < 3)
             return 1;
          return (lvl >= restrict[parts[1]] || lvl >= restrict[parts[1] + "/" + parts[2]]);
       } :),
       lvl);
}

/*
** conflict_report()
**
** Print out a report of the topics that have conflicts.
*/
nomask void conflict_report()
{
   mixed *values;

   values = values(topics);
   values = filter(values, ( : sizeof($1) > 1 :));
   more(sprintf("%O", values));
}

/*
** Return the topic and restrict list.  Note that these are
** the originals -- the topic mapping is awfully large to
** copy, so we'll just assume we have nice clients.
*/
mapping query_topics()
{
   return topics;
}

mapping query_tags()
{
   return tags;
}

mapping query_restrict()
{
   return restrict;
}
