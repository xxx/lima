inherit CLASS_FINGER;
private class finger fixture;
private string requested;
void set_fixture(class finger value) { fixture = value; }
string query_requested() { return requested; }
class finger get_finger_data(string userid) {
    requested = userid;
    return userid == "alice" && fixture ? copy_class(fixture) : nil;
}
string show_big_html_finger() { return "<TABLE>Fixture users</TABLE>"; }
