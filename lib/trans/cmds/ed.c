/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE:  ``ed [ filename ]``
//
// The mud file editor.
// With no file name supplied, the last file editted will be edited again.
// If you supply a file/path that does not exist, ed will respond with [new]
// then your ed prompt.
//
// Help for ed commands is available from within ed itself.
//
// .. TAGS: RST

inherit CMD;

private nosave mapping locks = ([]);

private nomask void unlock(string fname, object owner)
{
   if (locks[fname] == owner)
      m_delete(locks, fname);
}

nomask private void main(mixed *argv)
{
   mixed fname, problem;
   object owner = this_user();
   object session;
   function release;

   if (sizeof(argv))
      fname = argv[0];
   if (objectp(fname))
      fname = base_name(fname);

   if (!fname)
   {
      fname = get_user_variable("cwf");
      if (!fname)
      {
         out("You have no cwf. Please specify a file.\n");
         return;
      }
   }
   else
      fname = evaluate_path(fname);

   if (objectp(locks[fname]))
   {
      printf("Sorry, that file is already being edited by %s.\n", locks[fname]->query_userid());
      return;
   }

   if (file_size(fname) > MAX_FILE_SIZE)
   {
      printf("Sorry, that file is too large to edit.\n");
      return;
   }

   locks[fname] = owner;
   release = &unlock(fname, owner);
   problem = catch {
      session = clone_object(ED_SESSION);
      session->begin_editing(fname, 0, release, release);
      owner->query_shell_ob()->set_cwf(fname);
   };
   if (problem)
   {
      if (session)
         catch(session->remove());
      unlock(fname, owner);
      printf("Unable to start editor: %s\n", problem);
   }
}

protected varargs void dispatch_main(mixed raw, mixed argv, mixed flags, mixed implode_info)
{
   foreach (string word in this_user()->query_shell_ob()->argument_explode(raw)[0])
      if (word == "|" || word == "<" || word == ">" || word == ">>")
      {
         out("The interactive editor does not support pipes or redirection.\n");
         return;
      }
   main(argv);
}

protected void create()
{
   ::create();
   no_redirection();
   no_pipeline();
}
