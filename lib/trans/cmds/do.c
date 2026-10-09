/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE: ``do cmd1;cmd2;cmd3``
//
// Does multiple commands
//
// .. TAGS: RST

inherit CMD;

void main(string arg)
{
   string *cmds;

   if (!arg)
   {
      write("Syntax : do <cmd_1>;...;<cmd_n>\n");
      return;
   }

   cmds = explode(arg, ";");

   if (sizeof(cmds))
      map(cmds, ( : this_user()->force_me($1) :));
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
