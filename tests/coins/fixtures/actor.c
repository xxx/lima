inherit OBJ;
inherit "/std/body/pockets";
private string output = "";
int query_carrying_stat() { return 10; }
varargs float query_capacity(string relation) { return 0.0; }
object query_body() { return this_object(); }
string query_userid() { return "moneyaudit"; }
varargs void simple_action(string text, mixed *args...) { output += text; }
varargs void receive_private_msg(string text, int flags, int kind) { output += text; }
string take_output() { string old = output; output = ""; return old; }
void setup() { set_id("actor"); set_proper_name("actor"); set_mass(1.0); }
