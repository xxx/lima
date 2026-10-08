/* Do not remove the headers from this file! see /USAGE for more info. */

#include <config/user_menu.h>
#include <mudlib.h>
#include <security.h>

inherit M_DAEMON_DATA;

#include <user_data.h>

private mapping user_names = ([]);
private nosave mapping body_owners = ([]);
private nosave string *user_queries = ({"failures", "email", "real_name", "password", "url", "bodies"});
private nosave string *user_sets = ({"failures", "password"});
private nosave string *body_queries = ({"nickname", "plan", "wiz_position", "title"});
private nosave string *body_sets = ({"plan", "wiz_position"});

private string valid_name(string name)
{
   if (!name || !sizeof(regexp(({name}), "^[a-z][a-z0-9]*$")))
      error("USER_D: invalid canonical name");
   return name;
}

private void rebuild_owners()
{
   mapping owners = ([]);
   foreach (mixed user, mixed names in user_names)
   {
      if (!stringp(user)) error("USER_D: invalid registry account");
      valid_name(user);
      if (!arrayp(names)) error("USER_D: invalid registry body list");
      foreach (mixed name in names)
      {
         if (!stringp(name)) error("USER_D: invalid registry character");
         valid_name(name);
         if (has_key(owners, name))
            error("USER_D: duplicate character ownership: " + name);
         owners[name] = user;
      }
   }
   body_owners = owners;
}

void create()
{
   ::create();
   rebuild_owners();
}

private object account_object(string user)
{
   object *found = ({});
   foreach (string path in ({USER_OB, USER_OB_BAK}))
   {
      object prototype = find_object(path);
      if (prototype)
         foreach (object candidate in object_clones(prototype))
            if (candidate->query_userid() == user)
               found += ({candidate});
   }
   if (sizeof(found) > 1) error("USER_D: multiple live account objects: " + user);
   return sizeof(found) ? found[0] : nil;
}

private int account_exists(string user)
{
   return !!account_object(user) || unguarded(1, &is_file(LINK_PATH(user) + __SAVE_EXTENSION__));
}

private mapping account_bodies(string user, object online)
{
   mapping data;
   if (!online) online = account_object(user);
   if (online) data = online->query_bodies();
   else data = unguarded(1, &restore_map(LINK_PATH(user)))["bodies"];
   if (!mappingp(data)) error("USER_D: account has no body records");
   return data;
}

private object authorize_registry(string user)
{
   object caller = previous_object();
   if (caller && (base_name(caller) == USER_OB || base_name(caller) == USER_OB_BAK)
       && caller == this_user() && interactive(caller) && caller->query_userid() == user)
      return caller;
   if (!check_privilege(1)) error("USER_D: registry permission denied");
   return nil;
}

private void require_body_record(string user, string body, object account)
{
   mixed row = account_bodies(user, account)[body];
   if (!arrayp(row) || sizeof(row) != 4 || !stringp(row[0]) || !intp(row[1])
       || !stringp(row[2]) || !intp(row[3]))
      error("USER_D: character is missing from account body records");
}

nomask void register_body(string user, string body)
{
   valid_name(user);
   if (!body) body = user;
   valid_name(body);
   object account = authorize_registry(user);
   require_body_record(user, body, account);
   if (body != user && account_exists(body)) error("USER_D: character name conflicts with another account");
   if (has_key(body_owners, body))
   {
      if (body_owners[body] != user) error("USER_D: character already belongs to another account");
      return;
   }
   user_names[user] = (user_names[user] || ({})) + ({body});
   body_owners[body] = user;
   save_me();
}

nomask void remove_body(string user, string body)
{
   valid_name(user);
   valid_name(body);
   object account = authorize_registry(user);
   if (!has_key(body_owners, body)) return;
   if (body_owners[body] != user) error("USER_D: character belongs to another account");
   require_body_record(user, body, account);
   user_names[user] -= ({body});
   if (!sizeof(user_names[user])) m_delete(user_names, user);
   m_delete(body_owners, body);
   save_me();
}

string find_real_user(string body)
{
   valid_name(body);
   return body_owners[body];
}

private string resolve_account(string name)
{
   string owner = body_owners[name];
   int account = account_exists(name);
   if (account && owner && owner != name)
      error("USER_D: ambiguous account and character name: " + name);
   return account ? name : owner;
}

private object live_body(string account, string name)
{
   object user = account_object(account);
   object body = user && user->query_body();
   if (body && lower_case(body->query_name()) == name && body->query_link() == user)
      return body;
   return nil;
}

private mixed query_field(string identity, string account, string field, int user_field)
{
   object ob;
   string path;
   if (user_field)
   {
      ob = account_object(account);
      path = LINK_PATH(account);
   }
   else
   {
      if (!has_key(body_owners, identity)) error("USER_D: body fields require a character name");
      ob = live_body(account, identity);
      path = USER_PATH(identity);
   }
   if (ob) return ob->query_user_data(field);
   return restore_map(path)[field];
}

nomask mixed *query_variable(string identity, string *fields)
{
   mixed *result = ({});
   string account;
   if (!check_privilege(1)) error("insufficient privilege to query variables\n");
   valid_name(identity);
   foreach (string field in fields)
      if (member_array(field, user_queries) == -1 && member_array(field, body_queries) == -1)
         error("illegal variable request\n");
   account = resolve_account(identity);
   if (!account) return nil;
   foreach (string field in fields)
      result += ({query_field(identity, account, field, member_array(field, user_queries) != -1)});
   return result;
}

nomask void set_variable(string identity, string field, mixed value)
{
   int user_field = member_array(field, user_sets) != -1;
   string account;
   string path;
   object ob;
   mapping data;
   if (!check_privilege(1)) error("insufficient privilege to set variables\n");
   valid_name(identity);
   if (!user_field && member_array(field, body_sets) == -1) error("illegal variable assignment\n");
   validate_user_data(field, value);
   account = resolve_account(identity);
   if (!account) error("no such user\n");
   if (user_field)
   {
      ob = account_object(account);
      path = LINK_PATH(account);
   }
   else
   {
      if (!has_key(body_owners, identity)) error("USER_D: body fields require a character name");
      ob = live_body(account, identity);
      path = USER_PATH(identity);
   }
   if (ob)
   {
      ob->set_user_data(field, value);
      return;
   }
   if (!is_file(path + __SAVE_EXTENSION__)) error("no such user\n");
   data = restore_map(path);
   data[field] = deep_copy(value);
   save_map(data, path);
}

nomask int user_exists(string user)
{
   valid_name(user);
   return unguarded(1, &is_file(LINK_PATH(user) + __SAVE_EXTENSION__));
}
