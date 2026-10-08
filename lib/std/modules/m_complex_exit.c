/* Do not remove the headers from this file! see /USAGE for more info. */

/*
 * Written by Tigran@Illusions,Lima Bean. 2/6/99
 * The purpose of this inheritable is to make it easier for writing complex
 * exit type objects.  This is intended to be used by vehicles, and furniture
 * and such things as that.
 *
 * This will serve to allow more full utilization of OBJ and all of the
 * features of the parser.
 */

inherit "/std/classes/move";

/*
 * VARIABLES
 */

private
mixed hidden; /* Is this a hidden exit? */
private
mapping methods = ([]); /* All of the data for methods of this exit */
private
mixed obvious_desc; /* The obvious exit description */
private
mixed base = this_object(); /* The base directory to determine exit
                             * destinations from.  This can be either a
                             * string, object, or function pointer */

/*
 * PROTOTYPES
 */
string query_primary_id();
string *list_methods();
string the_short();
string query_primary_name();

/*
 * FUNCTIONS
 */

//: FUNCTION is_exit
// Return whether or not the object is an exit.
int is_exit()
{
   return 1;
}

private
string *all_methods_matched(string method)
{
   string verb;
   string prep;
   string *matches = ({});
   /* The first word of the method is expected to be the verb, the rest is
    * a preposition, if it exists.  If it is only one word, that is the verb */
   if (sscanf(method, "%s %s", verb, prep) == 2)
   {
      /* Translate the preposition to it's base form */
      prep = PREPOSITION_D->translate_preposition(prep);
      /* Put the method back together */
      method = sprintf("%s %s", verb, prep);
   }
   if (member_array(method, METHOD_D->list_methods()) != -1)
      return ({method});
   /* Must sweep through the methods to find matches.*/
   foreach (string test_method in METHOD_D->list_methods())
   {
      if (member_array(method, METHOD_D->list_method_equivalents(test_method)) != -1)
      {
         matches += ({test_method});
      }
   }
   return matches;
}

/* Return an array of all of the methods which might match the method passed
 * to the function.  It breaks down the preposition to it's lowest form before
 * making its determination, allowing for the fullest range of possible forms.
 */
private
string *methods_matched(string method)
{
   string *matches = all_methods_matched(method);
   /* We only want to return those matches which are also methods on this object
    * so eliminate those that aren't */
   matches = filter(matches, ( : member_array($1, list_methods()) > -1 :));
   /* Return the matches */
   return matches;
}

//: FUNCTION prime_method
// This function merely returns nonzero if the argument is a prime method.  More
// specifically this means that the method is one of the root methods that
// others can be built off of.
int prime_method(string method)
{
   string verb;
   string prep;
   string *matches = ({});
   /* The first word of the method is expected to be the verb, the rest is
    * a preposition, if it exists.  If it is only one word, that is the verb */
   if (sscanf(method, "%s %s", verb, prep) == 2)
   {
      /* Translate the preposition to it's base form */
      prep = PREPOSITION_D->translate_preposition(prep);
      /* Put the method back together */
      method = sprintf("%s %s", verb, prep);
   }
   return member_array(method, METHOD_D->list_methods()) > -1;
}

//: FUNCTION set_base
// Set the base directory from which relative paths are figured
// The argument can be a string (directory), object, or function pointer
// evaluating to one of these.
void set_base(mixed foo)
{
   base = foo;
}

//: FUNCTION query_base
// Return the evaluated directory string or object from which relative paths
// are figured
mixed query_base()
{
   return evaluate(base);
}

/*********   Hidden exits    ********/
//: FUNCTION set_hidden
// Set whether or not the exit is going to show up in the obvious exits list.
// Possible arguments are an int, or a function pointer.
void set_hidden(int i)
{
   hidden = i;
}

//: FUNCTION query_hidden
// Return 1 if the exit is not to show up as an obvious exit, or 0
int query_hidden()
{
   /* Exits are also hidden if there is no "go" method */
   if (member_array("go", keys(methods)) == -1)
      return 0;
   return !!evaluate(hidden);
}

/********   Directions   ********/

//: FUNCTION set_obvious_description
// Set the obvious exit description.  This is the string that will appear in
// the obvious exits line of a room description.  It can be either a function
// pointer or a string.
void set_obvious_description(mixed desc)
{
   obvious_desc = desc;
}

//: FUNCTION query_obvious_description
// Return the unevaluated obvious description.
mixed query_obvious_description()
{
   /* If there is no go method, there is no description to be obvious */
   if (member_array("go", keys(methods)) == -1)
      return 0;
   return obvious_desc ? obvious_desc : query_primary_name();
}

//: FUNCTION query_direction
// Return the diretion of the exit, that is to show up for obvious exits.
string query_direction()
{
   string tmp;
   if (tmp = evaluate(query_obvious_description()))
      return tmp;
   return query_primary_id();
}

/********   Destination   ********/

//: FUNCTION set_method
// Setup a go method.  The first argument is the verb to be used, the second
// argument is the destination to move the body to when invoked successfully,
// the third argument is the check to be performed before invoking the method
// the fourth argument is an array of possible exit messages, and the fifth
// argument is an array of possible enter messages.  Only the first two
// arguments are required, the rest are optional.
varargs void set_method(string method, mixed destination, mixed checks, mixed *exit_messages, mixed *enter_messages)
{
   class move_data new_method = new (class move_data);
   string verb, prep;
   string *matches = all_methods_matched(method);
   if (sizeof(matches) != 1)
   {
      error(sprintf("Cannot set method '%s'.  Method is either too ambiguous "
                    "or should be added to METHOD_D",
                    method));
   }
   if (destination)
      new_method.destination = destination;
   if (checks)
      new_method.checks = checks;
   else
      new_method.checks = 1;

   // Exit messages
   if (arrayp(exit_messages))
      new_method.exit_messages = exit_messages;
   else if (stringp(exit_messages))
      new_method.exit_messages = ({exit_messages});
   else
      new_method.exit_messages = ({});

   // Enter messages
   if (arrayp(enter_messages))
      new_method.enter_messages = enter_messages;
   else if (stringp(enter_messages))
      new_method.enter_messages = ({enter_messages});
   else
      new_method.enter_messages = ({});
   if (sscanf(method, "%s %s", verb, prep) == 2)
      new_method.relation = PREPOSITION_D->translate_preposition(prep);
   new_method.exit_dir = the_short;
   new_method.source = this_object();
   methods[matches[0]] = new_method;
}

//: FUNCTION add_method
// Add a go method.  The first argument is the verb to be used, the second
// argument is the destination to move the body to when invoked successfully,
// the third argument is the check to be performed before invoking the method
// the fourth argument is an array of possible exit messages, and the fifth
// argument is an array of possible enter messages.  Only the first two
// arguments are required, the rest are optional.
varargs void add_method(string method, mixed destination, mixed checks, mixed *exit_messages, mixed *enter_messages)
{
   set_method(method, destination, checks, exit_messages, enter_messages);
}

//: FUNCTION remove_method
// Remove a go method from the exit
void remove_method(string method)
{
   string *matches = methods_matched(method);
   if (sizeof(matches) > 1)
      error("Attempted to remove_method on an ambiguous method");
   if (sizeof(matches))
      m_delete(methods, matches[0]);
}

//: FUNCTION list_methods
// Return a list of arrays of all of the current go methods of the exit
string *list_methods()
{
   return keys(methods);
}

//: FUNCTION has_method
// Return true if the method exists
int has_method(string method)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      return 0;
   return 1;
}

//: FUNCTION set_method_checks
// Set the checks for a method.  The checks can be either an int, function
// pointer, or string.
void set_method_checks(string method, mixed checks)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to set_method_checks on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to set_method_checks on an ambiguous method");
   methods[applicable_methods[0]].checks = checks;
}

//: FUNCTION query_method_checks
// Return the evaluated checks on the given method.
// If 1 is returned, the checks is successful, 0 - the checks is a failure and
// the parser will generate an error (maybe), else, a string should be returned
// which is the error message received by the body.
mixed query_method_checks(string method)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to query_method_checks on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to query_method_checks on an ambiguous method");
   return evaluate(methods[applicable_methods[0]].checks, query_obvious_description(), this_body());
}

//: FUNCTION set_method_destination
// Set the destination for a given method
// The first argument is the method to have the destination assigned, and
// the second argument is either a string or function pointer which will return
// a string.
void set_method_destination(string method, mixed destination)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to set_method_destination on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to set_method_destination on an ambiguous method");
   methods[applicable_methods[0]].destination = destination;
}

//: FUNCTION query_method_destination
// Return the evaluated destination for the given method.
// The argument is the method being checked
mixed query_method_destination(string method)
{
   string *applicable_methods = methods_matched(method);
   mixed ret;
   if (!sizeof(applicable_methods))
      error("Attempted to query_method_destination on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to query_method_destination on an ambiguous method");
   ret = evaluate(methods[applicable_methods[0]].destination);
   if (stringp(ret))
      return absolute_path(ret, query_base());
   return ret;
}

//: FUNCTION set_method_enter_messages
// Set the enter messages to be used by the given method.
// Acceptable arguments are strings, or function pointers, or an array of
// either (mixed is acceptable)
// The method is to be seen by the bodies in the room that the body is entering
varargs void set_method_enter_messages(string method, mixed *messages...)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to set_method_enter_messages on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to set_method_enter_messages on an ambiguous method");
   methods[applicable_methods[0]].enter_messages = flatten_array(messages);
}

//: FUNCTION add_method_enter_messages
// Add the enter messages to be used by the given method.
// Acceptable arguments are strings, or function pointers, or an array of
// either (mixed is acceptable)
// The method is to be seen by the bodies in the room that the body is entering
varargs void add_method_enter_messages(string method, mixed messages...)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to add_method_enter_messages on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to add_method_enter_messages on an ambiguous method");
   methods[applicable_methods[0]].enter_messages += flatten_array(messages);
}

//: FUNCTION remove_method_enter_messages
// Remove the enter messages to be used by the given method.
// Acceptable arguments are strings, or function pointers, or an array of
// either (mixed is acceptable)
// The method is to be seen by the bodies in the room that the body is entering
varargs void remove_method_enter_messages(string method, mixed messages...)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to remove_method_enter_messages on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to remove_method_enter_messages on an ambiguous method");
   methods[applicable_methods[0]].enter_messages -= flatten_array(messages);
}

//: FUNCTION query_method_enter_message
// Return a random method enter message
// The method is to be seen by the bodies in the room that the body is entering
string query_method_enter_message(string method)
{
   string *applicable_methods = methods_matched(method);
   mixed ret;

   if (!sizeof(applicable_methods))
      error("Attempted to query_method_enter_message on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to query_method_enter_message on an ambiguous method");
   if (!sizeof(methods[applicable_methods[0]].enter_messages))
      return nil;

   ret = methods[applicable_methods[0]].enter_messages;
   /* We know that ret has to be an *at this point, select one element
    * randomly */
   ret = ret[random(sizeof(ret))];
   /* Evaluate it on the chance it's a function pointer */
   ret = evaluate(ret);
   return ret;
}

//: FUNCTION list_method_enter_messages
// Return an array of the method's enter messages
mixed *list_method_enter_messages(string method)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to list_method_enter_messages on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to list_method_enter_messages on an ambiguous method");
   return methods[applicable_methods[0]].enter_messages;
}

//: FUNCTION set_method_exit_messages
// Set the exit messages to be used by the given method.
// Acceptable arguments are strings, or function pointers, or an array of
// either (mixed is acceptable)
// The method is to be seen by the bodies in the room that the body is exiting
varargs void set_method_exit_messages(string method, mixed messages...)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to set_method_exit_messages on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to set_method_exit_messages on an ambiguous method");
   methods[applicable_methods[0]].exit_messages = flatten_array(messages);
}

//: FUNCTION add_method_exit_messages
// Add the exit messages to be used by the given method.
// Acceptable arguments are strings, or function pointers, or an array of
// either (mixed is acceptable)
// The method is to be seen by the bodies in the room that the body is exiting
varargs void add_method_exit_messages(string method, mixed messages...)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to add_method_exit_messages on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to add_method_exit_messages on an ambiguous method");
   methods[applicable_methods[0]].exit_messages += flatten_array(messages);
}

//: FUNCTION remove_method_exit_messages
// Remove the exit messages to be used by the given method.
// Acceptable arguments are strings, or function pointers, or an array of
// either (mixed is acceptable)
// The method is to be seen by the bodies in the room that the body is exiting
varargs void remove_method_exit_messages(string method, mixed messages...)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to remove_method_exit_messages on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to remove_method_exit_messages on an ambiguous method");
   methods[applicable_methods[0]].exit_messages -= flatten_array(messages);
}

//: FUNCTION query_method_exit_messages
// Return a random method exit message
// The method is to be seen by the bodies in the room that the body is exiting
string query_method_exit_message(string method)
{
   string *applicable_methods = methods_matched(method);
   mixed ret;

   if (!sizeof(applicable_methods))
      error("Attempted to query_method_exit_message on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to query_method_exit_message on an ambiguous method");
   if (!sizeof(methods[applicable_methods[0]].exit_messages))
      return nil;

   ret = methods[applicable_methods[0]].exit_messages;
   /* We know that ret has to be an array at this point, select one element
    * randomly */
   ret = ret[random(sizeof(ret))];
   /* Evaluate it on the chance it's a function pointer */
   ret = evaluate(ret);
   return ret;
}

//: FUNCTION list_method_exit_messages
// Return an array of the method's exit messages
mixed *list_method_exit_messages(string method)
{
   string *applicable_methods = methods_matched(method);
   if (!sizeof(applicable_methods))
      error("Attempted to list_method_exit_messages on a nonexistant method");
   if (sizeof(applicable_methods) > 1)
      error("Attempted to list_method_exit_messages on an ambiguous method");
   return methods[applicable_methods[0]].exit_messages;
}

/*
 * Parser Rules
 */
private string requested_method(string verb, string rule, mixed *args)
{
   if (rule == "WRD OBJ" || rule == "WRD")
   {
      if (!sizeof(args) || !stringp(args[0]))
         return nil;
      return verb + " " + PREPOSITION_D->translate_preposition(args[0]);
   }
   if (rule == "OBJ" || rule == "")
      return verb;
   return nil;
}

mixed complex_exit_direct_verb_rule(string verb, string rule, mixed args...)
{
   string method;
   string *matches;
   if (rule != "WRD OBJ" && rule != "OBJ")
      return 0;
   method = requested_method(verb, rule, args);
   if (!method)
      return 0;
   matches = methods_matched(method);
   if (!sizeof(matches))
      return 0;
   if (sizeof(matches) > 1)
      return sprintf("Unable to determine whether you want to %s the %s", format_list(matches, "or"), the_short());
   return query_method_checks(matches[0]);
}

int complex_exit_do_verb_rule(string verb, string rule, mixed args...)
{
   string method = requested_method(verb, rule, args);
   string *matches;
   string match;
   class move_data data;
   object sibling;
   mixed checks;
   if (!method)
      return 0;
   matches = methods_matched(method);
   if (sizeof(matches) != 1)
      return 0;
   match = matches[0];
   checks = query_method_checks(match);
   if (stringp(checks))
   {
      if (strlen(checks) && checks[0] == '#')
         checks = checks[1..];
      write(checks);
      return 0;
   }
   if (!intp(checks) || checks != 1)
      return 0;

   data = deep_copy(methods[match]);
   data.destination = query_method_destination(match);
   if (objectp(data.destination))
      data.destination = file_name(data.destination);
   data.exit_messages = query_method_exit_message(match);
   if (sibling = this_object()->get_sibling())
      data.enter_messages = sibling->query_method_enter_message(match);
   else
      data.enter_messages = query_method_enter_message(match);
   data.through = sibling;
   data.who = this_body();
   return this_body()->move_to(data);
}

string stat_me()
{
   string ret = "Methods: \n";
   foreach (string met in list_methods())
   {
      ret = sprintf("%s\t%O: Destination - %O\n", ret, met, query_method_destination(met));
   }
   return ret;
}
