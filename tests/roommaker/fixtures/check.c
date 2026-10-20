#include "/port_tests/roommaker/master.h"

private void run_tests()
{
   object default_room = load_object("/generated/default");
   object indoor = load_object("/generated/indoor");
   object outdoor = load_object("/generated/outdoor");
   object reset = load_object("/generated/reset");
   object composed = load_object("/generated/composed");
   object other = load_object("/generated/other");
   verify(default_room->is_indoors() && default_room->short() == "RoomMaker Room", "default room constructs with default description");
   verify(!sizeof(all_inventory(default_room)) && !sizeof(default_room->query_exit_directions()), "default room has empty items objects and exits");
   verify(indoor->is_indoors() && !indoor->is_outdoors(), "indoor room constructs with intended provider");
   verify(indoor->query_light() == 3, "generated light value applies");
   verify(outdoor->is_outdoors() && outdoor->query_weather() == 2, "outdoor room constructs with enabled weather");
   verify(reset->is_indoors() && !reset->is_outdoors(), "weather reset constructs indoors");
   verify(indoor->short() == "A \"quoted\" room with \\ paths", "short description retains quotes and backslashes");
   verify(indoor->get_base_long() == "A \"quoted\" line with \\ paths\nsecond line\n", "long description survives edit abort and source generation");
   verify(composed->query_light() == 4, "overwritten source applies its latest value");
   verify(composed->query_exit_destination("north") == "/domains/std/room/Example_Room", "exit destination survives editor completion and abort");
   verify(sizeof(composed->query_exit_directions()) == 1, "deleted exit stays absent");
   object *items = filter(all_inventory(composed), (: base_name($1) == SIMPLE_OB :));
   verify(sizeof(items) == 1, "item generation creates one real simple object");
   verify(items[0]->get_base_long() == "Letters \"and\" \\ slashes\n", "item text survives editor completion abort and source quoting");
   verify(member_array("a \"quoted\" sign", items[0]->parse_command_id_list()) >= 0, "item identifier retains quoted key");
   verify(sizeof(filter(all_inventory(composed), (: base_name($1) == "/std/object" :))) == 2,
          "edited object quantity generates two objects");
   verify(sizeof(all_inventory(composed)) == 3, "deleted object stays absent");
   verify(other->short() == "RoomMaker Room" && !sizeof(all_inventory(other)), "second menu generates independent default room");
   foreach (object room in ({default_room, indoor, outdoor, reset, composed, other})) destruct(room);
   finish();
}
