/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// codefor
// By Beek - derived from eval

//: COMMAND
//$$ see: treefor
// USAGE: ``codefor <lpc expression>``
//
// Displays a disassembly of the LPC expression without invoking it.
//
// .. TAGS: RST

#include <msgtypes.h>

inherit CMD;

void create()
{
   ::create();
   no_redirection();
}

#define SYNTAX "USAGE:  codefor <lpc expression>\n"

private void main(string str)
{
   object scratch;
   mixed failure;

   if (!str || str == "")
   {
      write(SYNTAX);
      return;
   }

   failure = catch {
      string source = "#include <mudlib.h>\n"
                      "#include <daemons.h>\n"
                      "#include <config.h>\n"
                      "mixed eval_function(){\n" + str + ";\n}\n";
      scratch = compile_string(wiz_dir(this_user()) + "/CODEFOR", source);
      // Keep a full-size listing line separate from its newline.
      foreach (string line in disassemble(scratch))
      {
         tell(this_user(), line, TREAT_AS_BLOB | MSG_PROMPT);
         tell(this_user(), "\n", TREAT_AS_BLOB | MSG_PROMPT);
      }
   };
   if (scratch)
      destruct(scratch);
   if (failure)
      write("codefor: " + failure + "\n");
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
