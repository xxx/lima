private object listener;
void created(object socket) { listener = socket; }
object latest() { return listener; }
