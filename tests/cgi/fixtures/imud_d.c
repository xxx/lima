private mapping fixture = ([]);
void set_fixture(mapping value) { fixture = value; }
mapping query_mudlist() { return deep_copy(fixture); }
