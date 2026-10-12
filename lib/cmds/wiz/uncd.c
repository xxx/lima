/* Do not remove the headers from this file! see /USAGE for more info. */

/* Beek: Nov 2 96 */

//: COMMAND
// USAGE: ``uncd``
//
// Switch to previous working directory.
//
// .. TAGS: RST

inherit CMD;

private
void main()
{
   object ob = this_user()->query_shell_ob();

   ob->swap_pwd();

   outf("new cwd: %s\n", ob->get_variable("pwd") || "NONE");
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main();
}
