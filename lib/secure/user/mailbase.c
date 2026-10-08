/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** mailbase.c
**
** Common mail handling functions for various mail handlers to use.
** Written by Rust (?)
** Revised by Deathblade 7-Sep-94 for additional functionality
** 950706, Deathblade: removed functionality; assumed by mailbox.c
*/

private
string default_mailer;

private
nosave string *valid_mailers = ({MAILER_DIR "/standard", MAILER_DIR "/Mh"});

/*
** Current mail handler object (clone of default_mailer)
*/
private
nosave object mailer;

nomask object query_mailer()
{
   if (!mailer)
   {
      if (default_mailer)
         mailer = clone_object (default_mailer);
      else
         mailer = clone_object (valid_mailers[0]);
   }
   return mailer;
}

nomask string query_default_mailer()
{
   return default_mailer;
}
nomask int dispatch_mail_command(string input)
{
   if (this_user() != this_object() || previous_object() != this_object()->query_shell_ob())
      error("illegal mail command caller\n");

   if (!mailer && default_mailer != valid_mailers[1])
      return 0;

   object handler = query_mailer();
   if (base_name(handler) != valid_mailers[1])
      return 0;

   return handler->parse_Mh_cmd(input);
}
nomask void set_default_mailer(string new_default_mailer)
{
   default_mailer = new_default_mailer;
}

nomask void unload_mailer()
{
   if (mailer)
      mailer->remove();
}
