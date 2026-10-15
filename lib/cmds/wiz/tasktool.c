/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE ``tasktool``
//
// Invokes the menu-driven tasktool system,
// which is intended as a "todo" handler.
//
// .. TAGS: RST

inherit CMD;

private
void main(string arg)
{
   clone_object (TASKTOOL);
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
