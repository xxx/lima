/* Do not remove the headers from this file! see /USAGE for more info. */

/* Synonyms for movement methods on complex exits. */
inherit M_DAEMON_DATA;

#define METHOD_CONFIG_FILE "/data/config/methods"

private mapping methods = ([]);

private string normalize_method(string method)
{
   string verb, prep;
   method = trim(method);
   if (!strlen(method))
      error("Empty movement method");
   if (sscanf(method, "%s %s", verb, prep) == 2)
   {
      prep = PREPOSITION_D->translate_preposition(trim(prep));
      if (member_array(prep, PREPOSITION_D->consolidated_preps()) == -1)
         error("Illegal preposition in method");
      return verb + " " + prep;
   }
   return method;
}

private string *normalize_equivalents(string *equivs)
{
   string *result = ({});
   foreach (string method in equivs)
   {
      method = normalize_method(method);
      if (member_array(method, result) == -1)
         result += ({method});
   }
   return result;
}

//: FUNCTION add_method
// Add a method and its equivalents.
void add_method(string method, string *equivs...)
{
   if (!check_privilege("Mudlib:daemons"))
      error("Insufficient privileges to add_method()");
   method = normalize_method(method);
   methods[method] = normalize_equivalents(({method}) + equivs);
   save_me();
}

//: FUNCTION remove_method
// Remove a method and its equivalents.
void remove_method(string method)
{
   if (!check_privilege("Mudlib:daemons"))
      error("Insufficient privileges to remove_method");
   m_delete(methods, normalize_method(method));
   save_me();
}

//: FUNCTION list_methods
// Return the canonical method names.
string *list_methods()
{
   return keys(methods);
}

//: FUNCTION add_method_equivalents
// Add equivalents to an existing method.
void add_method_equivalents(string method, string *equivs...)
{
   if (!check_privilege("Mudlib:daemons"))
      error("Insufficient privileges to add_method_equivalents");
   method = normalize_method(method);
   if (nilp(methods[method]))
      error("Attempted to add equivalents to a nonexistant method");
   methods[method] = normalize_equivalents(equivs + methods[method]);
   save_me();
}

//: FUNCTION remove_method_equivalents
// Remove equivalents from an existing method.
void remove_method_equivalents(string method, string *equivs...)
{
   if (!check_privilege("Mudlib:daemons"))
      error("Insufficient privileges to remove_method_equivalents");
   method = normalize_method(method);
   if (nilp(methods[method]))
      error("Attempted to remove equivalents from a nonexistant method");
   methods[method] -= normalize_equivalents(equivs);
   if (!sizeof(methods[method]))
      m_delete(methods, method);
   save_me();
}

//: FUNCTION list_method_equivalents
// Return the equivalents, or nil for an absent method.
string *list_method_equivalents(string method)
{
   return methods[method];
}

mapping stat_me()
{
   return deep_copy(methods);
}

private mapping read_configuration()
{
   mapping result = ([]);
   string text;
   if (file_size(METHOD_CONFIG_FILE) < 0)
      error("Missing movement method configuration");
   text = read_file(METHOD_CONFIG_FILE);
   if (!text)
      error("Missing movement method configuration");
   foreach (string line in explode(text, "\n"))
   {
      string method, equivs;
      string *aliases = ({});
      line = trim(line);
      if (!strlen(line) || line[0] == '#')
         continue;
      if (sscanf(line, "%s:%s", method, equivs) == 2)
      {
         foreach (string alias in explode(equivs, ","))
         {
            alias = trim(alias);
            if (strlen(alias))
               aliases += ({alias});
         }
      }
      else
         method = line;
      method = normalize_method(method);
      result[method] = normalize_equivalents(aliases + ({method}));
   }
   return result;
}

void load_config_from_file()
{
   if (!check_privilege("Mudlib:daemons"))
      error("Insufficient privileges to load_config_from_file()");
   methods = read_configuration();
   save_me();
}

void create()
{
   ::create();
   if (!sizeof(methods))
   {
      methods = read_configuration();
      save_me();
   }
}
