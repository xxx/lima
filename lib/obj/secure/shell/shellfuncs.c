/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** This beast manages function dispatch for shell-specific functions.
** This is a bloody quick hack to get something working.
** It should probably use classes, and definitly should do
** simple checking on number of args.
** Created.  John Viega(rust@Virginia.EDU).     July 2, 1995
*/

nosave mapping dispatch = ([]);
private
nosave mapping personal_bindings = ([]);
private
nosave string *modules = ({});
private
nosave mapping module_objects = ([]);
private
nosave mapping module_func_names = ([]);

protected
void call_user_func(string, mixed);

//: FUNCTION setup_for_save
// Sets up M_SAVE to save some variables
void setup_for_save()
{
   add_save(({"personal_bindings", "modules"}));
}

protected
void shell_bind(string command, function f)
{
   dispatch[command] = f;
}

protected
void shell_bind_if_undefined(string command, function f)
{
   if (!dispatch[command])
      dispatch[command] = f;
}

protected
void shell_unbind(string command)
{
   m_delete(dispatch, command);
}

protected
int bind(string command, string *argv)
{
   string fname;

   if (sizeof(argv) != 1)
      return -1;
   fname = argv[0];
   if (!has_key(personal_bindings, command) && dispatch[command])
      return -2;

   personal_bindings[command] = fname;
   dispatch[command] = &call_user_func(fname);
   save_me();
}

protected
void unbind(string *argv)
{
   string command;

   if (sizeof(argv) != 1)
      return;
   command = argv[0];
   if (!has_key(personal_bindings, command))
      return;

   m_delete(personal_bindings, command);
   m_delete(dispatch, command);
   save_me();
}

protected
void call_user_func(string fname, mixed argv)
{
   string module;

   foreach (module in modules)
   {
      if (member_array(fname, module_func_names[module]) != -1)
      {
         call_other(module_objects[module], fname, argv);
         return;
      }
   }
}

protected
int load_module(mixed argv)
{
   object module_ob;
   if (!(stringp(argv) || (arrayp(argv) && sizeof(argv) == 1 && stringp(argv = argv[0]))))
      return 0;
   if (!(module_ob = load_object(argv)))
      return 0;
   module_objects[argv] = module_ob;
   module_func_names[argv] = functions(module_ob);
   return 1;
}

protected
void set_module_path(string *mpath)
{
   modules = mpath;
   map(modules, load_module);
}

protected void restore_shell_bindings()
{
   foreach (mixed module in modules)
   {
      if (!stringp(module) || module == "")
         error("Invalid saved shell module");
      load_module(module);
   }
   foreach (mixed command, mixed fname in personal_bindings)
   {
      if (!stringp(command) || !stringp(fname) || command == "" || fname == "")
         error("Invalid saved shell binding");
      dispatch[command] = &call_user_func(fname);
   }
}
