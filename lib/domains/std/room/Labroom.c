/* Do not remove the headers from this file! see /USAGE for more info. */

inherit INDOOR_ROOM;

void setup(int x, int y, int north, int east, int south, int west)
{
   string *dirs = ({});

   string path = file_name(this_object());

   if (!clonep(this_object()) && path != __DIR__ "Labroom")
   {
      int idx = strsrch(path, "/", -1);
      string server = path[0..idx - 1];
      int *configuration = server->query_room_configuration(path[idx + 1..]);

      if (!configuration)
         error("Invalid labyrinth room coordinates.\n");
      x = configuration[0];
      y = configuration[1];
      north = configuration[2];
      east = configuration[3];
      south = configuration[4];
      west = configuration[5];
   }

   if (north == ' ')
   {
      add_exit("north", __DIR__ + "labyrinth/" + x + "," + (y - 1));
      dirs += ({"north"});
   }
   if (east == ' ')
   {
      add_exit("east", __DIR__ + "labyrinth/" + (x + 1) + "," + y);
      dirs += ({"east"});
   }
   if (south == ' ')
   {
      add_exit("south", __DIR__ + "labyrinth/" + x + "," + (y + 1));
      dirs += ({"south"});
   }
   if (west == ' ')
   {
      add_exit("west", __DIR__ + "labyrinth/" + (x - 1) + "," + y);
      dirs += ({"west"});
   }
   set_brief("Labyrinth");
   set_long("You are in a small corridor in a large labyrinth.  " +
            (sizeof(dirs) > 1 ? "Passages lead " : "A passage leads ") + format_list(dirs, nil) + ".\n");
   set_default_error("Cold stone bars your way.\n");
}
