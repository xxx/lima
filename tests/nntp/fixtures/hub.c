private object last;
void created(object socket) { last = socket; }
object latest() { return last; }
