/* Do not remove the headers from this file! see /USAGE for more info. */

string virtual_blueprint(string arg)
{
   return (__DIR__ "Labyrinth")->virtual_blueprint(arg);
}

int *query_room_configuration(string arg)
{
   return (__DIR__ "Labyrinth")->query_room_configuration(arg);
}
