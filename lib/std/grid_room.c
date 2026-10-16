/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OUTDOOR_ROOM;

void setup()
{
   string path = file_name(this_object());
   int last = strsrch(path, "/", -1);
   int first = strsrch(path[0..last - 1], "/", -1);
   string server;

   if (first <= 0)
      return;
   server = path[0..first - 1];
   server->configure_virtual_room(this_object(), path[first + 1..]);
}
