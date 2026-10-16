/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OBJ;
inherit M_GETTABLE;
inherit M_READABLE;

void setup() {
    set_id("map");
    set_untouched_desc("A map of the academy is pinned to the wall.");
    set_in_room_desc( "A map of the academy has been discarded here.");
    set_long("It has a crude map of the academy drawn on it.");
    set_text("                |-----------|----------|\n"
             "                |           |          |\n"
             "                | Corridor9 | Problems |\n"
             "                |           |          |\n"
             "    |-----------|-----------|----------|\n"
             "    |   Zoo     |           |\n"
             "    |  Monsters | Corridor8 |\n"
             "    |of the mud |           |\n"
             "    |-----------|-----------|----------|\n"
             "                |           |  Pantry  |\n"
             "                | Corridor7 | Food and |\n"
             "                |           |  drinks  |\n"
             "    |-----------|-----------|----------|\n"
             "    | Weaponry  |           |          |\n"
             "    |-----------| Corridor6 | Emoting  |\n"
             "    |  armoury   |           | Tutorial |\n"
             "    |-----------|-----------|----------|\n"
             "    |           |           |\n"
             "    |  Objects  | Corridor5 |\n"
             "    |           |           |\n"
             "    |-----------|-----------|-------------------|\n"
             "                |           |      Rooms        |\n"
             "                | Corridor4 |chats, basic room  | \n"
             "                |           |add_item, add_exits|\n"
             "                |           |set_long, includes |\n"
             "    |-----------|-----------|-------------------|\n"
             "    |    Areas  |           |\n"
             "    |Setting up | Corridor3 |\n"
             "    | Standards |           |\n"
             "    |-----------|-----------|----------|\n"
             "    |           |           |\n"
             "    |  Unix     | Corridor2 |\n"
             "    |           |           | \n"
             "    |-----------|-----------|----------|\n"
             "                |           |    ED    |\n"
             "                | Corridor1 | tutorial |\n"
             "                |           |          |\n"
             "    |-----------|-----------|----------|\n"
             "    |           ||---------||\n"
             "    |   Newwiz  || Academy ||\n"
             "    |           ||---------||\n"
             "    |-----------|-----------|\n");
}
