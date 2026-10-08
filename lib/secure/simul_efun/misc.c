/* Do not remove the headers from this file! see /USAGE for more info. */

#include <driver/type.h>

string evaluate_path(string);
object find_body(string);
object this_body();
private
nosave string *normal_directions =
    ({"up", "down", "north", "northeast", "northwest", "east", "southeast", "southwest", "south", "west"});

// FUNCTION is_normal_direction
// returns 1 if the direction is 'normal'
int is_normal_direction(string dir)
{
   if (member_array(dir, normal_directions) != -1)
      return 1;
   return 0;
}

private mixed *diagnostic_stack(int kind)
{
   switch (kind)
   {
   case 0: return calling_program(-1);
   case 1: return calling_object(-1);
   case 2: return calling_function(-1);
   }
   error("Invalid diagnostic stack selector.\n");
   return ({});
}

//: FUNCTION call_trace
// returns the stack of objects and functions

string call_trace()
{
   string res;
   int i, n;
   object *objects;
   string *programs;
   string *functions;

   res = "";
   programs = diagnostic_stack(0);
   objects = diagnostic_stack(1);
   functions = diagnostic_stack(2);

   n = sizeof(programs);
   for (i = 0; i < n; i++)
   {
      res += sprintf("%25-O %25-s %25-s\n", objects[i], programs[i], functions[i]);
   }
   return res;
}

// There should be an | operator for this.

//: FUNCTION clean_array
// returns a version of the passed array with duplicate
// entries removed.  Eg, clean_array(({1,2,2}))  => ({1,2})
mixed *clean_array(mixed *r)
{
   int i, n;

   r = r & r; // sort.  sort_array() can't hack it.  And no, &= doesn't work.

   n = sizeof(r) - 1;
   while (i < n)
   {
      if (r[i] == r[i + 1])
      {
         int j = i + 1;

         while (j < n && r[i] == r[j + 1])
            j++;

         r[i..j - 1] = ({});
         n -= j - i;
      }
      i++;
   }

   return r;
}

//: FUNCTION rev_explode
// returns a reversable explode, because sometimes this has a desired
// effect.  This sefun requires SANE_EXPLODE_STRING to work properly
// but the mudlib already requires that anyway.
string *rev_explode(string arr_in, string delim)
{
   string *arr_out = ({});
   if (arr_in[0..strlen(delim)-1] == delim)
      arr_out = ({""});
   arr_out += explode(arr_in, delim);
   if (arr_in[ < strlen(delim)..] == delim)
      arr_out += ({""});
   return arr_out;
}

//: FUNCTION cmp
// returns whether its two arguments are equivalent.  This is about
// the same as using the equivalence operator (==), but will return true
// in cases where == does not, such as when comparing 2 arrays.
// Logically 2 arrays should be equivalent, but aren't with ==.
// cmp knows they are.  This is mainly useful when you want to compare
// mappings and arrays.
int cmp(mixed a, mixed b)
{
   int i;
   mixed x, y, z;

   if (arrayp(a) && arrayp(b))
   {

      if (sizeof(a) != (i = sizeof(b)))
         return 0;

      while (i--)
         if (!cmp(a[i], b[i]))
            return 0;
      return 1;
   }

   if (mappingp(a) && mappingp(b))
   {

      if (sizeof(a) != (i = sizeof(b)))
         return 0;

      foreach (x, y in a)
      {
         z = b[x];
         if (!has_key(b, x))
            return 0;
         if (!cmp(y, z))
            return 0;
      }
      return 1;
   }

   return a == b;
}

//: FUNCTION insert
// Inserts the contents of the array of the first argument into
// The array in the second argument before the nth element of the array,
// where n is the 3rd argument passed to insert.
// Rust hacked at this to make it a bit more intuitive...
mixed insert(mixed to_insert, mixed into_array, int where)
{
// ### (db) I don't see any particular need to program defensively...
#if 0
  if( !arrayp( to_insert ) )
    return (void)error("Bad type arg 1 to simul efun insert()");

  if( !arrayp( into_array ) )
    return (void)error("Bad type arg 2 to simul efun insert()");

  if( !intp( where ) )
    return (void)error("Bad type arg 3 to simul efun insert()");
#endif

   return into_array[0..where-1] + to_insert + into_array[where..];
}

string parse_ref(string Ref)
{
   // If the first thing is a period, we can ignore it.
   string s = Ref[0] == '.' ? Ref[1..] : Ref;
   string result;
   string noun;
   string *pieces = explode(s, ":");

   noun = pieces[0];
   pieces = pieces[1..];
   switch (noun)
   {
   case "me":
      result = "this_body()";
      break;

   case "here":
      result = "environment(this_body())";
      break;

   default:
      if (find_body(noun))
         result = "find_body(\"" + noun + "\")";
      else if (load_object(evaluate_path(noun)))
         result = "find_object(evaluate_path(\"" + noun + "\"))";
      else
         return Ref;
   }

   foreach (noun in pieces)
   {
      switch (noun)
      {
      case "shell":
         result = sprintf("(%s->query_shell_ob() || %s)", result, result);
         break;

      case "link":
         result = sprintf("(%s->query_link())", result);
         break;

      default:
         result = sprintf("present(\"%s\",%s)", noun, result);
         break;
      }
   }
   return result;
}

object parse_ref_into_obj(string Ref)
{
   string s = Ref[0] == '.' ? Ref[1..] : Ref;
   object result;
   string noun;
   string *pieces = explode(s, ":");

   noun = pieces[0];
   pieces = pieces[1..];
   switch (noun)
   {
   case "me":
      result = this_body();
      break;

   case "here":
      result = environment(this_body());
      break;

   default:
      if (find_body(noun))
         result = find_body(noun);
      else if (!(result = load_object(evaluate_path(noun))))
         return nil;
   }

   foreach (noun in pieces)
   {
      switch (noun)
      {
      case "shell":
         result = result->query_shell_ob() || result;
         break;

      case "link":
         result = result->query_link();
         break;

      default:
         result = present(noun, result);
         break;
      }
   }
   return result;
}

/* eval lets you evaluate a string as an expression.
   exec_code allows you to evaluate a string as LPC code. */
varargs mixed exec_code(string arg, string dir, string includefile)
{
   // DO NOT USE UNGUARDED IN HERE
   // If perms are bad, we want to fail.  Use unguarded(1, (: exec_code :))
   // if really necessary -Beek
   string file = dir + "/exec.c";
   object tmp;
   mixed retval;
   mixed info;
   int i;
   string contents;

   if (tmp = find_object(file))
      destruct(tmp);

   rm(file);

   if (!stringp(arg))
      error("Bad type argument 1 to exec_code");
   info = reg_assoc(arg, ({"\\.[a-zA-Z:/\_]+"}), ({0}))[0];
   for (i = 0; i < sizeof(info); i++)
   {
      if (info[i][0] == '.' && strlen(info[i]) > 1)
         info[i] = parse_ref(info[i]);
   }
   arg = implode(info, "");

   if (strsrch(arg, ";") == -1)
      arg = "return " + arg;

   contents = "#include <mudlib.h>                                        \n" +
              "#include <security.h>                                      \n" +
              "  inherit M_ACCESS;                                        \n" +
              "  create()                                                 \n" +
              "   {                                                       \n" +
              "      set_privilege(1);                                    \n" +
              "   }                                                       \n";

   if (includefile)
      contents += sprintf("\n#include \"%s\"\n", includefile);
   contents += sprintf("mixed exec_foo(){ %s", arg);
   contents += ";}\n";
   write_file(file, contents);
   retval = file->exec_foo();
   rm(file);
   return retval;
}

// eval by Rust, so that you can
// convert a string to almost anything.
// bet on it being as slow as dirt, though...

//: FUNCTION eval
// evaluates the string s as an LPC value.  Eg, if you have a string
// someonetyped in: "({1,2,3,4})"  eval returns the actual array
//({1,2,3,4}).

varargs mixed eval(string arg, string includefile)
{
   mixed tmp;
   string file;

   file = "/tmp/eval.c";

   if (tmp = find_object(file))
      destruct(tmp);
   rm(file);
   if (!stringp(arg))
   {
      error("Bad type argument 1 to eval");
      return;
   }
   if (includefile)
      write_file(file, sprintf("#include \"%s\"\n", includefile));

   if (arg == "0")
      return 0;

   if (tmp = to_int(arg))
      if (tmp + "" == arg)
         return tmp;
   if (tmp = to_float(arg))
      if (tmp + "" == arg)
         return tmp;

   if (tmp = find_object(arg) && objectp(tmp))
      return tmp;

   if (strlen(arg) < 4)
      return arg;

   write_file(file, sprintf("mixed foo(){ return %s; }\n", arg));
   if (catch (tmp = file->foo()))
      return arg;

   if (tmp && !stringp(tmp))
      return tmp;

   return arg;
}

//: FUNCTION decompose
// Takes any arrays that are elements in arr and merges
// all of its elements as elements of arr.  Eg, decompose(({1,({2,3,}),4}))
// will return: ({1,2,3,4}).  The algorithm is not recursive, so if any of
// the arrays have arrays in them, those arrays remain intact.  Eg,
// decompose( ({1,({({2,3}),4}),5}) )  returns:({1,({2,3}),4,5}).
// See flatten_array for a recursive version.

mixed *decompose(mixed *org)
{
   int i = 0, j;

   if (!arrayp(org))
      error("Bad type arg to decompose");
   org = deep_copy(org);

   while (i < sizeof(org))
   {
      if (arrayp(org[i]))
      {
         j = sizeof(org[i]);
         org[i..i] = org[i];
         i += j; // skip the elements inserted
      }
      else
         i++;
   }
   return org;
}

//: FUNCTION choice
// Returns an array element, mapping value, or integer string scalar.
// Empty arrays and mappings error; an empty string returns integer zero.

mixed choice(mixed value)
{
    if (stringp(value) || arrayp(value))
        return value[random(sizeof(value))];
    if (mappingp(value))
    {
        mixed *indices = keys(value);
        return value[indices[random(sizeof(indices))]];
    }
    throw("choice: expected a string, array or mapping.");
}

mixed sum(mixed *numbers...)
{
   mixed result = 0;
   foreach (mixed number in numbers)
   {
      if (intp(number))
         result += number;
      else if (floatp(number))
         result = to_float(result) + number;
      else
         error("sum: arguments must be ints or floats.");
   }
   return result;
}

/**
 * NAME: array_sum
 * SYNOPSIS: (int|float) array_sum( array int|array float numbers  )
 * DESCRIPTION: Returns the sum of all values in the passed array.
 */

mixed array_sum(mixed *numbers)
{
   return sum(numbers...);
}

//: FUNCTION element_of_weighted
// Returns a property from a mapping weighted by its value.
// The higher the weight (value), the more likely it will be returned.
// Weights must be nonnegative integers whose total fits an int.
// Returns nil when no entry has a positive weight.
// EXAMPLE:
//   element_of_weighted( ([ "element1" : 10, "element2" : 5 ]) )
// Return value will more likely be "element1" due to its higher weight.
mixed element_of_weighted(mapping data)
{
    if (!mappingp(data))
        throw("element_of_weighted: argument must be a mapping.");
    int total = 0;
    foreach (mixed key, mixed weight in data)
    {
        if (!intp(weight) || weight < 0)
            throw("element_of_weighted: weights must be nonnegative integers.");
        int next = total + weight;
        if (next < total)
            throw("element_of_weighted: total weight overflows an integer.");
        total = next;
    }
    if (!total)
        return nil;
    int ticket = random(total);
    foreach (mixed key, int weight in data)
    {
        if (ticket < weight)
            return key;
        ticket -= weight;
    }
    return nil;
}

//: FUNCTION clamp
// Take a value and makes sure it is between the low and high parameters.
int clamp(int x, int low, int high)
{
   return (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)));
}

//: FUNCTION flatten_array
// Takes an array that may contain arrays, and reduces all
// arrays so that the result is a one dimensional array
mixed flatten_array(mixed arr)
{
   int i = 0;

   if (!arrayp(arr))
      error("Bad argument 1 to flatten_array");
   arr = deep_copy(arr);

   while (i < sizeof(arr))
   {
      if (arrayp(arr[i]))
      {
         arr[i..i] = arr[i];
      }
      else
         i++;
   }
   return arr;
}

//: FUNCTION call_out_chain
// Does a call_out to a list of functions, one following
// another, with each returning the delay till the next one is called.
protected
void handle_chain(object ob, mixed *funcs, mixed *args)
{
   int delay;
   if (!sizeof(funcs))
      return;
   if (stringp(funcs[0]))
   {
      delay = call_other(ob, funcs[0], args...);
   }
   else
   {
      delay = evaluate(funcs[0], args...);
   }
   call_out(( : handle_chain:), delay, ob, funcs[1..], args);
}

void call_out_chain(mixed *funcs, int delay, mixed *args...)
{
   call_out(( : handle_chain:), delay, previous_object(), funcs, args);
}

mixed *my_call_outs()
{
   object p = previous_object();
   return filter(call_out_info(), ( : $1[0] == $(p) :));
}

// Beek fixed not to loop forever if exp < 0, etc
// also optimized extensively
int pow(int num, int exp)
{
   int res;

   switch (exp)
   {
   case 0:
      if (!num)
         error("pow(0, 0) is undefined.\n");
      return 1;
   case 1:
      return num;
   case 2:
      return num * num;
   case 3:
      return num * num * num;

   case 4:        // 2 multiplications, not 3
      num *= num; // 1, 2, 4
      return num * num;

   case 5:             // 3 multiplications, not 4
      exp = num * num; // 1, 2, 4, 5
      exp *= exp;
      return exp * num;

   case 6:        // 3 multiplications, not 5
      num *= num; // 1, 2, 4, 6
      return num * num * num;

   case 7:             // 4 multiplications, not 6
      exp = num * num; // 1, 2, 4, 6, 7
      exp *= exp * exp;
      return exp * num;

   case 8:        // 3 multiplications, not 7
      num *= num; // 1, 2, 4, 8
      num *= num;
      return num * num;

   case 9: // 4, not 8
      exp = num * num;
      exp *= exp;
      exp *= exp;
      return num * exp;

   case 10: // 4, not 9
      num *= num;
      exp = num * num;
      exp *= exp;
      return exp * num;

   case 11: // 5, not 10
      exp = num * num;
      res = exp * exp;
      res *= res;
      return res * num * exp;

   case 12: // 4, not 11 ...
      num *= num;
      num *= num;
      exp = num * num;
      return exp * num;

   default:
      if (exp < 0)
         return 0;
      // exp > 12 here ...
      switch (num)
      {
      case 0:
         return 0;
      case 1:
         return 1;
      case 2:
         return 1 << exp;
      default:
         // sub optimal, but pretty good
         // O(log n) multiplications, not O(n)
         res = 1;

         while (exp)
         {
            if (exp & 1)
               res *= num;
            num *= num;
            exp >>= 1;
         }
         return res;
      }
   }
}

int fuzzy_divide(int top, int bottom)
{
   if (random(bottom) < (top % bottom))
      return top / bottom + 1;
   else
      return top / bottom;
}

string implode_by_arr(string *arr1, string *arr2)
{
   string res = "";
   int i;

   if (sizeof(arr2) != (sizeof(arr1) + 1))
      error("second arg needs to have 1 more arg than first.\n");

   res += arr2[0];
   for (i = 0; i < sizeof(arr1); i++)
   {
      res += arr1[i];
      res += arr2[i + 1];
   }
   return res;
}

/* This one by cowl. */
/* Cleaned up, optimized, and extended by Beek */
/*
 * type 0 = 3 hours 5 minutes 32 seconds
 * type 1 = 3h5m32s
 * type 2 = 3h
 * type 3 = 3h 5m 32s
 */
string convert_time(int sec, int type)
{
   int weeks, days, hours, minutes, seconds;
   string ret;

   minutes = sec / 60;
   seconds = sec - (minutes * 60);
   hours = minutes / 60;
   minutes = minutes - (hours * 60);
   days = hours / 24;
   hours = hours - (days * 24);
   weeks = days / 7;
   days = days - (weeks * 7);

   ret = "";
   switch (type)
   {
   case 0:
      if (weeks)
         ret += M_GRAMMAR->number_of(weeks, "week") + " ";
      if (days)
         ret += M_GRAMMAR->number_of(days, "day") + " ";
      if (hours)
         ret += M_GRAMMAR->number_of(hours, "hour") + " ";
      if (minutes)
         ret += M_GRAMMAR->number_of(minutes, "minute") + " ";
      if (seconds && ret != "")
         ret += "and ";
      if (seconds)
         ret += M_GRAMMAR->number_of(seconds, "second") + " ";
      ret = ret[0.. < 2];
      break;
   case 1:
      if (weeks)
         ret += weeks + "w";
      if (days)
         ret += days + "d";
      if (hours)
         ret += hours + "h";
      if (minutes)
         ret += minutes + "m";
      if (seconds)
         ret += seconds + "s";
      break;
   case 2:
      if (weeks)
      {
         ret += weeks + "w";
         break;
      }
      if (days)
      {
         ret += days + "d";
         break;
      }
      if (hours)
      {
         ret += hours + "h";
         break;
      }
      if (minutes)
      {
         ret += minutes + "m";
         break;
      }
      if (seconds)
      {
         ret += seconds + "s";
         break;
      }
      break;
   case 3:
      if (weeks)
         ret += weeks + "w ";
      if (days)
         ret += days + "d ";
      if (hours)
         ret += hours + "h ";
      if (minutes)
         ret += minutes + "m ";
      if (seconds)
         ret += seconds + "s ";
      ret = ret[0.. < 2];
      break;
   }
   return ret;
}

//: FUNCTION sort_by_value
// mixed sort_by_value(mixed arr, function) returns the *arr sorted in such
// a way that the elements are in increasing order, as defined by the
// value of the function f
mixed *sort_by_value(mixed arr, function value_func)
{
   return sort_array(arr, ( : evaluate($(value_func), $1) - evaluate($(value_func), $2) :));
}

string lima_version()
{
   return "Lima 1.1a4";
}

varargs string identify(mixed a)
{
   int i, s;
   string ret;
   mapping real_map;

   if (intp(a))
      return "" + a;
   if (nilp(a))
      return "nil";
   if (floatp(a))
      return "" + a;
   if (objectp(a))
   {
      /*
      if (ret = a->GetKeyName())
         ret += " ";
      else
         ret = "";
      */
      return "OBJ(" + ret + file_name(a) + ")";
   }
   if (stringp(a))
   {
      a = replace_string(a, "\"", "\\\"");
      a = "\"" + a + "\"";
      a = replace_string(a, "\\", "\\\\");
      a = replace_string(a, "\\\"", "\"");
      a = replace_string(a, "\n", "\\n");
      a = replace_string(a, "\t", "\\t");
      return a;
   }
   if (arrayp(a))
   {
      ret = "({ ";
      s = sizeof(a);
      for (i = 0; i < s; i++)
      {
         if (i)
            ret += ", ";
         ret += identify(a[i]);
      }
      return ret + (s ? " " : "") + "})";
   }
   if (mappingp(a))
   {
      ret = "([ ";
      real_map = (mapping)(a);
      a = keys(real_map);
      s = sizeof(a);
      for (i = 0; i < s; i++)
      {
         if (i)
            ret += ", ";
         ret += identify(a[i]) + " : " + identify(real_map[a[i]]);
      }
      return ret + (s ? " " : "") + "])";
   }
   if (functionp(a))
      return sprintf("%O", a);
   return "UNKNOWN";
}

varargs string get_stack(int x)
{
   int i, s;
   string list = "";
   string *stack0 = diagnostic_stack(0);
   object *stack1 = diagnostic_stack(1);
   string *stack2 = diagnostic_stack(2);
   for (i = 0, s = sizeof(stack1); i < s; i++)
   {
      list += "\n" + i + ":" + identify(stack0[i]) + "." + identify(stack1[i]) + "." + identify(stack2[i]);
   }

   if (x)
   {
      list += "\n" + identify(previous_object(-1));
   }

   return list;
}

//: FUNCTION clone_class
// Create a copy of an existing class.
mixed clone_class(mixed x)
{
   return assemble_class(disassemble_class(x));
}

//: FUNCTION pround
// Preceision round a float into a string with a specific number of decimals.
// Example:
//
//     pround(3.14152526,4) -> "3.1415"
string pround(float f, int p)
{
   return sprintf("%:" + p + "f", f);
}

//: FUNCTION nnew
// Clone a number of the same files with the same arguments and return them.
// Example:
//
//     nnew(3,"/domains/std/consumable/beer")->move(this_body());
object *nnew(int count, string file, string *args...)
{
   return ({clone_object (file, args...)}) + (--count > 0 ? nnew(count, file, args...) : ({}));
}
