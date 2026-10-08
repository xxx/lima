/* Do not remove the headers from this file! see /USAGE for more info. */

/*
 * Handling of multiple bodies per user.
 *
 * Created 2022 by Tsath
 */

#define BODY_FNAME 0
#define BODY_LEVEL 1
#define BODY_RACE 2
#define BODY_GENDER 3

nomask void save_me();
mixed unguarded(mixed priv, function fp);

private
mapping bodies = ([]);
private
string selected;

string query_userid();
object query_body();

private void authorize_body_change()
{
   object caller = previous_object();
   if (this_user() != this_object() || !interactive(this_object()))
      error("user bodies: not the owning user");
   if (caller && caller != this_object() && base_name(caller) != USER_MENU)
      error("user bodies: caller is not the user menu");
}

void register_body(string name)
{
   authorize_body_change();
   USER_D->register_body(query_userid(), name);
}

void set_body(string name, string fname, string race, int gender)
{
   mapping before;
   mixed problem;
   authorize_body_change();
   if (!mappingp(bodies)) bodies = ([]);
   before = bodies;
   bodies = bodies + ([name:({fname, 1, race, gender})]);
   problem = catch(USER_D->register_body(query_userid(), name));
   if (problem)
   {
      bodies = before;
      error(problem);
   }
   if (strlen(name) > 5 && name[0..4] == "guest") return;
   save_me();
}

void remove_body(string name)
{
   object current = query_body();
   authorize_body_change();
   if (!mappingp(bodies) || !has_key(bodies, name)) error("user bodies: no such character");
   if (USER_D->find_real_user(name) != query_userid()) error("user bodies: ownership mismatch");
   if (current && lower_case(current->query_name()) == name) error("user bodies: character is active");
   unguarded(1, &call_other(LAST_LOGIN_D, "remove_user", name, 0));
   if (unguarded(1, &is_file(USER_PATH(name) + __SAVE_EXTENSION__)))
      unguarded(1, &rm(USER_PATH(name) + __SAVE_EXTENSION__));
   USER_D->remove_body(query_userid(), name);
   m_delete(bodies, name);
   if (selected == name) selected = nil;
   save_me();
}

string query_selected_body()
{
   return selected;
}

int query_num_bodies()
{
   if (!mappingp(bodies))
      bodies = ([]);

   return sizeof(keys(bodies));
}

int set_selected_body(string name)
{
   authorize_body_change();
   name = lower_case(name);
   if (member_array(name, keys(bodies)) != -1)
   {
      selected = name;
      save_me();
      return 1;
   }
   return 0;
}

mapping query_bodies()
{
   if (!mappingp(bodies))
      bodies = ([]);

   return deep_copy(bodies);
}

int update_body(object body)
{
   string name;
   if (previous_object() != body || body != query_body()) error("user bodies: not the current body");
   name = lower_case(body->query_name());
   if (arrayp(bodies[name]))
   {
      bodies[name][BODY_LEVEL] = body->query_level();
      bodies[name][BODY_RACE] = body->query_race();
      bodies[name][BODY_GENDER] = body->query_gender();
      if (strlen(name) > 5 && name[0..4] == "guest")
         return 1;
      save_me();
      // TBUG("Body "+name+" updated in user.");
      return 1;
   }
   return 0;
}

protected
nomask void set_body_fname(string name, string new_body_fname)
{
   bodies[name][BODY_FNAME] = new_body_fname;
   if (strlen(name) > 5 && name[0..4] == "guest")
      return;
   save_me();
}

varargs string query_fname(string name)
{
   if (!name)
      name = query_selected_body();
   if (!mappingp(bodies))
      set_body(name, "/std/race/human.c", "human", 0);
   if (bodies[name])
      return bodies[name][BODY_FNAME];
   return "/std/race/human.c";
}

int query_gender(string name)
{
   if (!mappingp(bodies))
      set_body(name, "/std/race/human.c", "human", 0);
   if (bodies[name])
      return bodies[name][BODY_GENDER];
   return 0;
}
