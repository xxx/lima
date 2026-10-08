/* Do not remove the headers from this file! see /USAGE for more info. */

/* Inheritable persistence by Beek for Nightmare, ported by Rust. */

#include <mudlib.h>

private nosave int save_recurse;
private nosave mixed *saved = ({});
private nosave string *old_fnames = ({"/obj/shells/wish", "/obj/pshell"});

private int omitted(mixed value)
{
   return nilp(value) || (intp(value) && value == 0);
}

private void check_name(mixed name)
{
   if (!stringp(name) || name == "" || name[0] == '#')
      error("M_SAVE: invalid variable name\n");
}

protected void add_save(mixed *vars)
{
   mixed *pending = ({});
   foreach (mixed entry in vars)
   {
      if (omitted(entry))
         continue;
      if (!functionp(entry))
         check_name(entry);
      if (member_array(entry, saved) == -1 && member_array(entry, pending) == -1)
         pending += ({entry});
   }
   saved += pending;
}

protected mixed *get_saved()
{
   return saved + ({});
}

protected void set_save_recurse(int flag)
{
   save_recurse = flag;
}

private mapping *save_variables(string state)
{
   string *names = ({});
   mapping declarations = ([]);
   mapping *result = ({});
   foreach (mixed entry in saved)
   {
      mixed value = functionp(entry) ? evaluate(entry, state) : entry;
      mixed *expanded = arrayp(value) ? value : ({value});
      foreach (mixed name in expanded)
      {
         if (omitted(name))
            continue;
         check_name(name);
         if (member_array(name, names) == -1)
            names += ({name});
      }
   }
   foreach (mapping info in variable_info(this_object()))
      declarations[info["name"]] = has_key(declarations, info["name"]) ? 0 : info;
   foreach (string name in names)
   {
      if (!mappingp(declarations[name]))
         error("M_SAVE: missing or ambiguous variable " + name + "\n");
      result += ({declarations[name]});
   }
   return result;
}

private string save_state(int recursep, int things_only)
{
   mapping data = ([]);
   if (recursep)
      save_recurse = 1;
   foreach (mapping info in save_variables("saving"))
      data[info["name"]] = info["value"];
   data["#base_name#"] = base_name(this_object());
   if (save_recurse)
   {
      string *children = ({});
      foreach (object child in all_inventory())
      {
         mixed value;
         if (child->do_not_restore() || (things_only && child->is_living()))
            continue;
         value = child->save_to_string(1);
         if (omitted(value))
            continue;
         if (!stringp(value))
            error("M_SAVE: invalid child save result\n");
         children += ({value});
      }
      data["#inventory#"] = children;
   }
   return save_value(data);
}

varargs string save_to_string(int recursep)
{
   return save_state(recursep, 0);
}

varargs string save_things_to_string(int recursep)
{
   return save_state(recursep, 1);
}

// Validate the complete tree's structure before writes or child creation.
private mapping decode_state(mixed value, int depth)
{
   mixed base;
   mixed inventory = ({});
   mapping data;
   if (depth > 128)
      error("M_SAVE: inventory nesting too deep\n");
   if (stringp(value))
      value = restore_value(value);
   if (arrayp(value))
   {
      if ((sizeof(value) != 2 && sizeof(value) != 3) || !arrayp(value[1]))
         error("M_SAVE: invalid positional save\n");
      base = value[0];
      data = (["values":value[1], "legacy":1]);
      if (sizeof(value) == 3)
         inventory = value[2];
   }
   else if (mappingp(value))
   {
      mapping fields = ([]);
      base = value["#base_name#"];
      foreach (mixed name, mixed field in value)
      {
         if (name == "#base_name#" || name == "#inventory#")
            continue;
         check_name(name);
         fields[name] = field;
      }
      data = (["values":fields, "legacy":0]);
      if (has_key(value, "#inventory#"))
         inventory = value["#inventory#"];
   }
   else
      error("M_SAVE: expected a mapping or positional save\n");
   if (!stringp(base) || base == "" || base[0] != '/')
      error("M_SAVE: invalid base name\n");
   if (omitted(inventory))
      inventory = ({});
   if (!arrayp(inventory))
      error("M_SAVE: invalid inventory\n");
   data["base"] = base;
   data["children"] = ({});
   foreach (mixed child in inventory)
      data["children"] += ({decode_state(child, depth + 1)});
   return data;
}

private mixed encoded_state(mapping state)
{
   mixed *children = map(state["children"], encoded_state);
   if (state["legacy"])
      return ({state["base"], state["values"], children});
   return state["values"] + (["#base_name#":state["base"], "#inventory#":children]);
}

private void restore_state(mapping state, int recurse)
{
   mapping *variables;
   mapping fields = ([]);
   mapping allowed = ([]);
   if (state["base"] != base_name(this_object()) && member_array(state["base"], old_fnames) == -1)
      error("Invalid save string (ob)\n");
   variables = save_variables("loading");
   foreach (mapping info in variables)
      allowed[info["name"]] = info;
   if (state["legacy"])
   {
      mixed *values = state["values"];
      if (sizeof(values) != sizeof(variables))
         error("Invalid save string (size)\n");
      for (int i = 0; i < sizeof(variables); i++)
         fields[variables[i]["name"]] = values[i];
   }
   else
      fields = state["values"];
   foreach (string name in keys(fields))
      if (!has_key(allowed, name))
         error("M_SAVE: unregistered variable " + name + "\n");
   // Declaration order also preserves positional partial-restore behavior.
   foreach (mapping info in variables)
      if (has_key(fields, info["name"]))
         store_variable(this_object(), info["name"], fields[info["name"]], info["program"]);
   foreach (mapping child in state["children"])
   {
      object ob = clone_object(child["base"]);
      ob->load_from_string(encoded_state(child), state["legacy"] ? (child["legacy"] ? 1 : 2) : recurse + 1);
   }
   if (state["legacy"] ? recurse != 0 : recurse > 1)
      move_object(previous_object());
}

void load_from_string(mixed value, int recurse)
{
   restore_state(decode_state(value, 0), recurse);
}

void old_load_from_string(mixed value, int recurse)
{
   mapping state = decode_state(value, 0);
   if (!state["legacy"])
      error("M_SAVE: expected a positional save\n");
   restore_state(state, recurse);
}
