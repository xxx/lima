/* Do not remove the headers from this file! see /USAGE for more info. */

inherit INDOOR_ROOM;

mapping dests = ([]);
mapping distances = ([]);

int default_distance = 6;
string where;
string travelling_to;
function in_progress;
int progress_delay;
int progress_timer;
int arrival_timer;

int query_distance(string destination)
{
   return distances[where + ":" + destination] || default_distance;
}

int query_in_motion()
{
   return !!travelling_to;
}

string query_where()
{
   return where;
}

string query_location()
{
   return dests[where];
}

protected object resolve_destination(string name)
{
   string path = dests[name];
   object destination;

   if (!path || !strlen(path))
      error(sprintf("Invalid destination '%s' in %O.\n", name, this_object()));
   destination = load_object(evaluate_path(path, nil, 1));
   if (!destination)
      error("Cannot load destination '" + name + "' at " + path + ".\n");
   return destination;
}

void arrive()
{
   object dest;

   if (!travelling_to)
      return;
   dest = resolve_destination(travelling_to);
   remove_call_out(arrival_timer);
   remove_call_out(progress_timer);
   arrival_timer = 0;
   progress_timer = 0;
   where = travelling_to;
   travelling_to = nil;
   dest->arrived(this_object());
}

void do_progress()
{
   int trip = arrival_timer;

   remove_call_out(progress_timer);
   progress_timer = 0;
   if (!travelling_to || !in_progress)
      return;
   evaluate(in_progress);
   if (travelling_to && arrival_timer == trip && in_progress && progress_delay > 0 && !progress_timer)
      progress_timer = call_out(do_progress, progress_delay);
}

void move_to(string dest)
{
   int dist;

   resolve_destination(dest);
   if (dest == travelling_to || dest == where)
      return;
   if (travelling_to)
      error("In motion to '" + travelling_to + " while asked to move_to " + dest + ".\n");

   if (where)
   {
      object origin = resolve_destination(where);
      dist = query_distance(dest);
      origin->departed(this_object());
      travelling_to = dest;
      where = nil;
      arrival_timer = call_out(arrive, dist);
      if (in_progress && progress_delay > 0)
         progress_timer = call_out(do_progress, progress_delay);
   }
   else
   {
      where = dest;
   }
}

//: FUNCTION set_destinations
// Set a mapping between names and locations. The names can be in the format::
//  "church
//  or
//  "2/church" - this format allows the elevator user to use 'push 2' etc.
// Make sure to use the same names when setting distances.
void set_destinations(mapping m)
{
   dests = m;
}

void set_default_distance(int x)
{
   default_distance = x;
}

//: FUNCTION set_distance
// Set distance between destinations.
void set_distance(string d1, string d2, int dist)
{
   distances[d1 + ":" + d2] = dist;
   distances[d2 + ":" + d1] = dist;
}

void set_in_progress(function f, int delay)
{
   in_progress = f;
   progress_delay = delay;
}
