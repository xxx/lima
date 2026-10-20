/* Do not remove the headers from this file! see /USAGE for more info. */

// Onyx @ Red Dragon Nov. 1995

#include <mudlib.h>

inherit CMD;

#define MAKER __DIR__ "buildmenu"

private
void main()
{
   clone_object (MAKER)->start_menu();
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main();
}
