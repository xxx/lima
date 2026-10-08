/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** mailbox.c -- a mailbox to hold message references
**
** 950701, Deathblade: Created.
*/

#include <classes.h>
#include <flags.h>
#include <mudlib.h>
#include <security.h>

inherit M_ACCESS;

inherit CLASS_MAILMSG;

/*
** mailbox: maps message keys to flags
*/
private
mapping mailbox = ([]);

/*
** Flags for the mail
*/
#define F_UNREAD 1
// ### code assumes zero/non-zero; fix if more flags are added

/*
** Who is this mailbox for?
*/
private
nosave string owner;

/*
** What is the current message index.  This value is 0-based and represents
** a message index for messages from the user's perspective.  The value 1
** corresponds to the first key in the (ordered) set of message keys in the
** mailbox.
*/
private
nosave int message_index = 0;

private
nomask string get_fname()
{
   return sprintf("/data/mail/mbox/%c/%s", owner[0], owner);
}

nomask void create(string the_owner)
{
   if (!clonep())
      return;

   if (file_name(previous_object()) != MAILBOX_D)
      error("*Security violation: invalid creation");

   owner = the_owner;
   unguarded(1, (: restore_object(get_fname()) :));
}
private
nomask void save_me()
{
   unguarded(1, (: save_object(get_fname()) :));
}
nomask void remove()
{
   destruct();
}

nomask string query_owner()
{
   return owner;
}

nomask class mail_msg get_one_message(int message_key)
{
   class mail_msg msg;

   // ### need some work on ensuring the call stack is correct
   //     if ( !check_privilege(owner) )
   if (this_user()->query_userid() != owner)
      error("security violation: you are not allowed to use this mailbox\n");

   if (!has_key(mailbox, message_key))
      return nil;

   msg = MAIL_D->get_one_message(message_key);
   if (!msg)
   {
      /* damn. bad message. remove it from the mailbox. */
      m_delete(mailbox, message_key);
      save_me();
   }

   return msg;
}

nomask int query_message_index()
{
   return message_index;
}
nomask void set_message_index(int new_message_index)
{
   message_index = new_message_index;
}

nomask int *query_message_keys()
{
   return sort_array(keys(mailbox), 1);
}
nomask int query_message_count()
{
   return sizeof(mailbox);
}

nomask int query_message_read(int message_key)
{
   return mailbox[message_key];
}
nomask void set_message_read(int message_key)
{
   if (mailbox[message_key])
   {
      mailbox[message_key] = 0;
      save_me();
   }
}
nomask int query_unread_count()
{
   return reduce(values(mailbox), ( : $1 + $2:), 0);
}
nomask int first_unread_message()
{
   int *message_keys = query_message_keys();
   int message_key;
   int message_index;

   foreach (message_key in message_keys)
   {
      if (mailbox[message_key])
         return message_index;

      ++message_index;
   }

   return -1;
}

private nomask void delete_mail_entry(int message_key)
{
   MAIL_D->delete_mail(message_key, owner);
   int deleted_index = member_array(message_key, query_message_keys());
   m_delete(mailbox, message_key);
   if (deleted_index < message_index)
      message_index--;
   if (message_index >= sizeof(mailbox))
      message_index = sizeof(mailbox) - 1;
   if (message_index < 0)
      message_index = 0;

   save_me();
}

nomask void delete_message(int message_key)
{
   // ### need some work on ensuring the call stack is correct
   //     if ( !check_privilege(owner) )
   if (this_user()->query_userid() != owner)
      error("security violation: you are not allowed to read this mail\n");

   if (!has_key(mailbox, message_key))
      error("non-existent message\n");

   delete_mail_entry(message_key);
}

nomask void nuke_mailbox()
{
   if (!check_previous_privilege(1))
      error("security violation: illegal nuke of mailbox\n");

   foreach (int message_key in keys(mailbox))
      delete_mail_entry(message_key);

   mailbox = ([]);
   message_index = 0;
   unguarded(1, (: rm(get_fname() + __SAVE_EXTENSION__) :));
}

nomask void recover_message(int message_key)
{
   if (previous_object() != find_object(MAIL_D))
      error("security violation: illegal attempt to recover mail\n");

   if (has_key(mailbox, message_key))
      return;

   mailbox[message_key] = F_UNREAD;
   save_me();
}

// ### temp functions?  wait and see

nomask void receive_new_message(int message_key)
{
   object user;

   if (previous_object() != find_object(MAIL_D))
      error("security violation: illegal attempt to deliver mail\n");

   mailbox[message_key] = F_UNREAD;
   save_me();

   if ((user = find_user(owner)) && user->query_body()->test_flag(F_BIFF))
      tell(user, ">>You have new mail<<\n");
}
