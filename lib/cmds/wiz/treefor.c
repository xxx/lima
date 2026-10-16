/* Do not remove the headers from this file! see /USAGE for more info. */

// treefor
// By Beek - derived from codefor, which was derived from eval

//: COMMAND
//$$ see: codefor
// USAGE: ``treefor <lpc statements>``
//
// Displays the driver's checked frontend tree without executing the statements.
//
// >``treefor int x; int y; return x+y;``
//
// .. TAGS: RST

#include <msgtypes.h>

inherit CMD;

void create()
{
   ::create();
   no_redirection();
}

#define SYNTAX "USAGE:  treefor <lpc statements>\n"

private void main(string str)
{
   if (!str || str == "")
   {
      write(SYNTAX);
      return;
   }

   mixed failure = catch {
      string source = "#include <mudlib.h>\n"
                      "#include <daemons.h>\n"
                      "#include <config.h>\n"
                      "mixed eval_function(){\n" + str + ";\n}\n";
      foreach (string line in syntax_tree("/tmp/TREEFOR", source))
      {
         tell(this_user(), line, TREAT_AS_BLOB | MSG_PROMPT);
         tell(this_user(), "\n", TREAT_AS_BLOB | MSG_PROMPT);
      }
   };
   if (failure)
      write("treefor: " + failure + "\n");
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
