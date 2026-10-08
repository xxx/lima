/* Do not remove the headers from this file! see /USAGE for more info. */

// Folded out from DOOR.   --OH. (Around 1996)

//: MODULE
// This module handles an object that has a corrsponding sibling. Typically used for doors, where two door objects in
// two different rooms should act as one object. Could also be used for teleporters, one-way mirrors, and other things
// where the object is on two rooms, but should be considered as one object. Elevator doors use this functionality as
// well.
//
// Note: setup_door() is deprecated. Use this approach instead:
//
// .. code-block:: c
//   set_sibling_ident("oak door");
//   set_door_direction(dir);
//   set_door_destination(room);
//
// .. TAGS: RST

private
string our_ident;
private
mixed sibling_room;
private
object cached_sibling;

//: FUNCTIOn setup_sibling
// A quick way to change the sibling of this object. An elevator door
// changes siblings as it moves through its destinations.
void setup_sibling(string ident, mixed room)
{
   if (ident)
      our_ident = ident;
   if (stringp(room))
      sibling_room = ( : absolute_path($(room), environment()) :);
   else
      sibling_room = room;

   cached_sibling = nil;
}

//: FUNCTION set_sibling_ident
// Set the ID the siblings will be using.
void set_sibling_ident(string ident)
{
   if (ident)
      our_ident = ident;

   cached_sibling = nil;
}

//: FUNCTION set_sibling_room
// Set the room of the sibling. This can either be an object or a string. If a string
// it will then be loaded as an object.
void set_sibling_room(mixed room)
{
   if (stringp(room))
      sibling_room = ( : absolute_path($(room), environment()) :);
   else
      sibling_room = room;

   cached_sibling = nil;
}

//: FUNCTION respond_to_sibling_ident
// For the siblings to be able to find each other, this public function returns
// true if the ident matches.
int respond_to_sibling_ident(string id)
{
   return !!our_ident && our_ident == id;
}

//: FUNCTION get_sibling
// Find the peer, optionally reusing an already resolved movement destination.
varargs object get_sibling(mixed destination)
{
   if (!environment() || !our_ident)
      return nil;

   if (nilp(destination))
      destination = evaluate(sibling_room);
   object room = objectp(destination) ? destination :
      (stringp(destination) ? load_object(destination) : nil);
   if (!room)
   {
      cached_sibling = nil;
      return nil;
   }

   if (cached_sibling && cached_sibling != this_object() &&
       environment(cached_sibling) == room &&
       cached_sibling->respond_to_sibling_ident(our_ident))
      return cached_sibling;

   cached_sibling = nil;
   foreach (object ob in all_inventory(room))
   {
      if (ob != this_object() && ob->respond_to_sibling_ident(our_ident))
      {
         cached_sibling = ob;
         break;
      }
   }
   return cached_sibling;
}

//: FUNCTION update_sibling
// After a door is opened or closed, this is called, and in turn calls
// do_on_open/close() in the door and its sibling.
void update_sibling()
{
   object ob;
   if (ob = get_sibling())
      ob->update_state(this_object());
}

void update_state(object ob)
{
   // Insert stuff to sync
}

private
void initial_move()
{
   object ob;
   cached_sibling = nil;
   if (ob = get_sibling())
      update_state(ob);
}

void internal_setup()
{
   this_object()->add_hook("move", initial_move);
}
