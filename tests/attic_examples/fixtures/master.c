private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_shutdown(object caller, string program) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }
void error_handler(mapping issue) { dump("ATTIC_ERROR: " + issue["diagnostic"] + "\n"); efun::shutdown(1); }
private void verify(int ok, string label) {
    if (!ok) throw("ATTIC_FAIL: " + label);
    checks++; dump("ATTIC_OK: " + label + "\n");
}
private void finish() { dump("ATTIC_TOTAL: " + checks + "\n"); efun::shutdown(0); }
string *epilog(int ignored) { call_out(run_tests, 0); return ({}); }
private void run_tests() {
    object table = clone_object("/domains/std/attic/coffee_table");
    verify(table->query_mass() == 64.0, "coffee-table mass retains LARGE as float");
    verify(table->is_attached(), "coffee-table attachment flag remains set");
    verify(table->query_default_relation() == "on" && sizeof(table->get_relations()) == 1,
           "coffee-table constructor establishes its on relation");
    verify(table->query_max_capacity() == 64 && !sizeof(all_inventory(table)),
           "coffee-table starts empty with reference table capacity");
    verify(member_array("table", table->parse_command_id_list()) >= 0
           && member_array("coffee", table->parse_command_adjectiv_id_list()) >= 0,
           "coffee-table identity remains intact");
    object scene = clone_object("/std/indoor_room");
    verify(table->move(scene) == 1, "coffee table enters real room");
    object full = clone_object("/port_tests/attic_examples/item", 64.0);
    object extra = clone_object("/port_tests/attic_examples/item", 1.0);
    full->move(scene); extra->move(scene);
    verify(full->move(table) == 1 && environment(full) == table && table->query_relation(full) == "on",
           "ordinary item move uses table default on relation");
    verify(table->query_capacity() == 64.0, "table accounts for full surface capacity");
    verify(stringp(extra->move(table)) && environment(extra) == scene,
           "over-capacity refusal keeps item in room");
    verify(full->move(scene) == 1 && table->query_capacity() == 0.0,
           "removing item releases table capacity");
    verify(stringp(extra->move(table, "in")) && environment(extra) == scene,
           "coffee table rejects unsupported in relation");
    destruct(full); destruct(extra); destruct(table); destruct(scene);

    string *directions = ({"north", "east", "south", "west"});
    string *briefs = ({"Intersection", "Dead end", "Dead end", "Town road",
        "Dead end", "Town road", "Town road", "Intersection", "Dead end", "Town road",
        "Town road", "Intersection", "Town road", "Intersection", "Intersection", "Intersection"});
    string *longs = ({"The road leads .\n", "The road dead-ends here.  It continues to the north.\n",
        "The road dead-ends here.  It continues to the east.\n", "The road leads north and east.\n",
        "The road dead-ends here.  It continues to the south.\n", "The road leads north and south.\n",
        "The road leads south and east.\n", "The road leads north, south, and east.\n",
        "The road dead-ends here.  It continues to the west.\n", "The road leads north and west.\n",
        "The road leads east and west.\n", "The road leads north, east, and west.\n",
        "The road leads south and west.\n", "The road leads north, south, and west.\n",
        "The road leads south, east, and west.\n", "The road leads north, south, east, and west.\n"});
    for (int mask = 0; mask < 16; mask++) {
        string n = (mask & 1) ? "north" : nil;
        string e = (mask & 2) ? "east" : nil;
        string s = (mask & 4) ? "south" : nil;
        string w = (mask & 8) ? "west" : nil;
        object room = clone_object("/domains/std/attic/townroom", n, e, s, w);
        verify(room->is_outdoors() && room->short() == briefs[mask], sprintf("town %d chooses expected title", mask));
        verify(room->get_base_long() == longs[mask], sprintf("town %d retains direction order and description", mask));
        int count;
        int correct = 1;
        for (int i = 0; i < 4; i++) {
            string destination = room->query_exit_destination(directions[i]);
            if (mask & (1 << i)) {
                count++;
                if (destination != "/domains/std/attic/town/" + directions[i]) correct = 0;
            } else if (!nilp(destination)) correct = 0;
        }
        verify(correct && sizeof(room->query_exit_directions()) == count,
               sprintf("town %d retains exact exits and path prefix", mask));
        destruct(room);
    }
    verify(!sizeof(filter(objects(), (: clonep($1) && (base_name($1) == "/domains/std/attic/townroom"
           || base_name($1) == "/domains/std/attic/coffee_table") :))), "all audited example clones retire");
    finish();
}
