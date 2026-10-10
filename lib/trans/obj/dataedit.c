/* Do not remove the headers from this file! see /USAGE for more info. */

#include <security.h>

inherit MENUS;
inherit M_ACCESS;
inherit M_COLOURS;

/****** Begin private data and functions */
private class menu toplevel;
private class section main, other;
private class menu_item quit1, quit2, blank;

private string fname;
private mapping data;

private string menu_title()
{
   return "Save file editor: \"" + fname + "\"";
}

private void update()
{
   set_menu_title(toplevel, menu_title());
}

private mapping load_data_file(string fn)
{
   string text = read_file(fn);
   mapping result;
   string name, value;

   if (!text)
      return nil;
   result = ([]);
   string *lines = explode(text, "\n");
   if (lines[0] == "")
      lines = lines[1..];
   if (sizeof(lines) && lines[<1] == "")
      lines = lines[0..<2];
   foreach (string line in lines)
   {
      if (line[0] == '#')
         continue;
      if (sscanf(line, "%s %s", name, value) != 2)
         error("Syntax error in data file.\n");
      result[name] = restore_value(value);
   }
   return result;
}

private int save_data_file(string fname)
{
   string result = "";

   foreach (string var, mixed value in data)
      result += var + " " + save_value(value) + "\n";

   return write_file(fname, result, 1);
}

private mixed expression(string str)
{
   mixed *parts = reg_assoc(str, ({"\\$\\$", "\\$[a-zA-Z0-9_]+"}), ({1, 2}));
   string expr = "mixed expression(mapping internal_data) {\nreturn ";
   mixed value;
   object ob;

   for (int i = 0; i < sizeof(parts[0]); i++)
   {
      switch (parts[1][i])
      {
      case 0:
         expr += parts[0][i];
         break;
      case 1:
         expr += "$";
         break;
      case 2:
         expr += "(internal_data[\"" + parts[0][i][1..] + "\"])";
         break;
      }
   }
   expr += ";\n}\n";

   write_file("/tmp/dataedit.c", expr, 1);
   if (ob = find_object("/tmp/dataedit"))
      destruct(ob);
   ob = load_object("/tmp/dataedit");
   value = ob->expression(data);
   destruct(ob);
   rm("/tmp/dataedit.c");

   return value;
}

private void print_variables()
{
   string *nicedata = sort_array(keys(data), 1);
   int w = this_user()->query_screen_width() - 20;
   int max_length = max(map(nicedata, ( : strlen($1) :)))+4;

   write("Variables:\n" + colour_table(nicedata, w, w / max_length));
}

private void quit_and_discard()
{
   quit_menu_application();
}

private void quit_and_save()
{
   if (save_data_file(fname))
      quit_menu_application();
   else
      write("Failed.\n");
}

private void remove_variable(string var)
{
   if (!has_key(data, var))
      write("No such variable.\n");
   else
   {
      m_delete(data, var);
      write("Done.\n");
   }
   update();
}

private void remove_var()
{
   get_input_then_call(remove_variable, "Variable: ");
}

private void rename_variable(string oldv, string newv)
{
   if (has_key(data, newv))
      write("A variable by that name already exists.\n");
   else
   {
      data[newv] = data[oldv];
      m_delete(data, oldv);
      write("Done.\n");
   }
   update();
}

private void get_new_name(string old)
{
   if (!has_key(data, old))
      write("No such variable.\n");
   else
   {
      get_input_then_call(&rename_variable(old), "New name: ");
   }
}

private void rename_var()
{
   get_input_then_call(get_new_name, "Old name: ");
}

private void set_variable(string vname, string valstr)
{
   mixed value = expression(valstr);
   data[vname] = value;
   write("Done.\n");
}

private void get_value(string vname)
{
   get_input_then_call(&set_variable(vname), "Value: ");
}

private void set_var()
{
   get_input_then_call(get_value, "Variable: ");
}

private void print_variable(string expr)
{
   printf("Value is: %O\n", expression(expr));
}

private void print()
{
   get_input_then_call(print_variable, "Expression: ");
}

private void merge_file(string fname)
{
   mapping data2;

   if (!(data2 = load_data_file(fname)))
   {
      write("No such file.\n");
      return;
   }

   foreach (mixed key, mixed value in data2)
   {
      if (!data[key])
         data[key] = value;
      else
      {
         if (typeof(data[key]) != typeof(value))
            write("Type mismatch in merging variable '" + key + "': " + typeof(data[key]) + " vs " + typeof(value) +
                  "\n");
         switch (typeof(value))
         {
         case "mapping":
         case "array":
            data[key] += value;
            break;
         default:
            if (value == data[key])
               continue;
            write("While merging variable '" + key + "': Cannot merge objects of type " + typeof(value) + "\n");
         }
      }
   }
   update();
}

private void merge()
{
   get_input_then_call(merge_file, "File to merge with: ");
}

private void save_to_file(string file)
{
   if (save_data_file(file))
   {
      fname = file;
      update();
   }
   else
      write("Failed.\n");
}

private void save_to()
{
   get_input_then_call(save_to_file, "New filename: ");
}

/****** End of private functions */
void start_menu(string file)
{
   if (!(data = load_data_file(fname = file)))
   {
      write(file + ": No such file.\n");
      destruct(this_object());
      return;
   }

   frame_init_user();
   toplevel = new_menu(menu_title());
   main = new_section("Edit data", "accent");
   other = new_section("Other", "warning");
   add_section_item(toplevel, main);
   add_section_item(toplevel, other);

   //Main section
   add_menu_item(main, new_menu_item("Delete Variable", remove_var, "d"));
   add_menu_item(main, new_menu_item("Rename Variable", rename_var, "r"));
   add_menu_item(main, new_menu_item("Set Variable", set_var, "="));
   add_menu_item(main, new_menu_item("List all variables", print_variables, "l"));
   add_menu_item(main, new_menu_item("Print Expression", print, "p"));
   add_menu_item(main, new_menu_item("Merge Datafile", merge, "m"));
   
   //Other section
   add_menu_item(other, new_menu_item("Save as ...", save_to, "s"));
   add_menu_item(other, new_menu_item("Quit and Save", quit_and_save, "q"));
   add_menu_item(other, new_menu_item("Quit w/o Saving", quit_and_discard, "Q"));

   set_menu_prompt(toplevel, "[dr=pmsqQ] ");
   init_menu_application(toplevel);
}

void create()
{
   set_privilege(1);
}
