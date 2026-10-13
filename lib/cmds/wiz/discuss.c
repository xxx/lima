/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE ``discuss <thing>``
//
// Invokes the annotation menu, allowing annotations to be added to objects,
// and subsequently viewed and deleted.
//
// .. TAGS: RST

inherit CMD;

private
void main(mixed *arg)
{
   if (arg[0]->is_user() || member_array(arg[0], bodies()) != -1)
   {
      out("You can't discuss a user, just objects.\n");
      return;
   }
   clone_object (ANNO_MENU, arg[0])->start_menu();
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
