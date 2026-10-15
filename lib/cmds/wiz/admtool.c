/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE
//       ``admtool``
//
// Invokes the menu-driven admtool, for code admin tasks.
//
// .. TAGS: RST

inherit CMD;

private void main(string arg)
{
   clone_object("/obj/admtool/admtool3");
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
