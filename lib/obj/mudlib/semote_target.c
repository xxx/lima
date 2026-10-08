inherit "/std/living/grammar";

void create() { set_gender(1); }

string short() { return "Somebody else"; }
string a_short() { return short(); }
string the_short() { return short(); }
string query_named_possessive() { return "Somebody else's"; }
