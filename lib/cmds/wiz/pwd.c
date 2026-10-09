/* Do not remove the headers from this file! see /USAGE for more info. */

// Rust/Belboz

//: COMMAND
// USAGE:  ``pwd``
//
// Shows you what directory you are currently "in".
//
// ``pwd``
//
//  */wiz/zifnab/obj*
//
// .. TAGS: RST

inherit CMD;

private
void main()
{
   outf("Dir:  %s\nFile: %s\n", get_user_variable("pwd") || "NONE", get_user_variable("cwf") || "NONE");
}


protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main();
}
