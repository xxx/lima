inherit M_ACCESS;
void create() { set_privilege(1); }
object make_race(string path, string name) { return clone_object(path, name); }
