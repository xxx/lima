/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
//$$see: dbx
// USAGE: ``dbxframe [<who>] <frame number>``
//
// Check the last saved error frame for yourself or ``who``.
// Do 'help dbx' for more info.
//
// .. TAGS: RST

#include <security.h>

inherit CMD;

private nosave int report_truncated;

// Command stdio uses one 8 KiB string, so cap characters before appending UTF-8.
private void report_out(string text)
{
   int remaining = 1600 - strlen(get_output());

   if (report_truncated)
      return;
   if (strlen(text) > remaining)
   {
      if (remaining)
         out(text[0..remaining - 1]);
      out("\nReport truncated.\n");
      report_truncated = 1;
   }
   else
      out(text);
}

private void print_vars(mixed *vars)
{
   int i;

   for (i = 0; i < sizeof(vars); i++)
   {
      mapping variable = vars[i];
      if (i)
         report_out(", ");
      report_out(sprintf("%s %s = %s", variable["type"], variable["name"], variable["value"]));
      if (variable["truncated"])
         report_out(" [truncated]");
   }
}

private mapping get_the_error(string str)
{
   return unguarded(1, (: master()->query_error($(str)) :));
}

private void print_source(string file, int line)
{
   int before = (line > 5 ? 5 : line - 1);
   string current = read_file(file, line, 1);
   string prior = "";
   string after;

   if (!current || current == "")
   {
      report_out("Source unavailable.\n");
      return;
   }
   if (before)
      prior = read_file(file, line - before, before);
   after = read_file(file, line + 1, 5);
   if (prior)
      report_out(prior);
   report_out("=>");
   report_out(current);
   if (current[<1] != '\n')
      report_out("\n");
   if (after)
      report_out(after);
}

private void main(string str)
{
   mapping record;
   mapping frame;
   mixed *frames;
   string who;
   int num;

   // We do not have any ANSI here, but also don't want emojis.
   set_output_flags(NO_ANSI);
   report_truncated = 0;

   if (!str || (sscanf(str, "%d", num) == 0 && sscanf(str, "%s %d", who, num) == 0))
   {
      out("dbxframe [<who>] <frame>\n");
      return;
   }
   if (!who)
      record = get_the_error(this_user()->query_userid());
   else
   {
      record = get_the_error(who);
      if (!record && !find_body(who) && who != "last")
      {
         out("No such player.\n");
         return;
      }
   }
   if (!record)
   {
      out("No error.\n");
      return;
   }
   frames = record["frames"];
   if (!arrayp(frames))
   {
      out(record["caught"] ? "Frame data unavailable for caught errors.\n"
                           : "Frame data unavailable for this error.\n");
      return;
   }
   if (num < 0 || num >= sizeof(frames))
   {
      out("No such frame.\n");
      return;
   }
   frame = frames[num];

   if (record["frames_omitted"])
      report_out(sprintf("Omitted outer frames: %d\n", record["frames_omitted"]));
   if (frame["variables_omitted"])
      report_out(sprintf("Omitted variables: %d\n", frame["variables_omitted"]));
   if (frame["truncated"])
      report_out("Saved frame data is truncated.\n");
   report_out(sprintf("------%s:%i - %s(", frame["file"] != "" ? frame["file"] : frame["program"],
                      frame["line"], frame["function"]));
   print_vars(frame["arguments"]);
   report_out(")\n");
   report_out(sprintf("program: %s; object: %s\n", frame["program"], frame["object"]));
   report_out("locals: ");
   print_vars(frame["locals"]);
   report_out("\n----------------------------------------------------------------\n");

   if (frame["file"] == "" || frame["line"] < 1 ||
       catch(print_source(frame["file"], frame["line"])))
      report_out("Source unavailable.\n");
}

protected varargs void dispatch_main(mixed first, mixed second, mixed third, mixed fourth)
{
   main(first);
}
