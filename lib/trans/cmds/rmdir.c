/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE:  ``rmdir < directory >``
//
// This command will delete a directory if it is empty.
//
// ``rmdir /wiz/zifnab/obj``
//
// will try and delete a directory named obj from /wiz/zifnab
//
// ``rmdir obj``
//
// will try and delete a directory named obj from my current directory
//
// .. TAGS: RST

inherit CMD;

private
void main(mixed *arg)
{
   string dir;
   foreach (dir in arg[0])
   {
      int removed;
      if (!catch(removed = rmdir(dir)) && removed)
         outf("Removed directory: %s.\n", dir);
      else
         outf("Failed to remove directory: %s.\n", dir);
   }
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
