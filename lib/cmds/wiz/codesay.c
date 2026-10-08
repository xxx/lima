/* Do not remove the headers from this file! see /USAGE for more info. */

inherit CMD;
inherit M_ED;

//: COMMAND
// USAGE ``codesay <string>``
//
// Displays LPC code with its existing lines indented, preserving literal text.
// Inline snippets stay inline.
//
// > ``codesay void create() { ::create(); }``
//
//   |  You say:
//   |  void create() { ::create(); }
//
// .. TAGS: RST

void create()
{
   ::create();
   no_redirection();
}

private void main(string source)
{
   string own;
   string room;
   mixed problem;
   if (!source || source == "")
   {
      write("Say what?\n");
      return;
   }
   problem = catch {
      string formatted = indent_code(source);
      string heading = this_body()->query_name() + " says:\n";
      // Leave room for the receiver's terminating newline within native string bounds.
      if (sizeof(to_bytes(formatted, "UTF-8")) + sizeof(to_bytes(heading, "UTF-8")) + 16 > 8192)
         error("formatted code exceeds message size limit");
      own = "You say:\n" + formatted;
      room = heading + formatted;
   };
   if (problem)
   {
      write("Unable to format code: " + problem + "\n");
      return;
   }
   tell(this_user(), own, TREAT_AS_BLOB);
   tell_environment(this_body(), room, TREAT_AS_BLOB, ({ this_body() }));
}

protected varargs void dispatch_main(mixed raw, mixed parsed, mixed flags, mixed implode_info)
{
   main(raw);
}
