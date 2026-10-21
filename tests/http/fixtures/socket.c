private function reader;
private function closer;
private function writer;
private bytes *writes = ({});
private int removed;
private int style;
varargs void create(int selected, mixed port, function read, function close)
{
   if (!selected) return;
   style = selected; reader = read; closer = close;
   "/port_tests/http/hub"->created(this_object());
}
void send(mixed text)
{
   if (removed) error("recording socket: removed");
   if (stringp(text)) text = to_bytes(text, "UTF-8");
   if (!bytesp(text)) error("recording socket: expected bytes or text");
   writes += ({text});
}
void set_write_callback(function callback) { writer = callback; }
void remove() { removed++; }
int removal_count() { return removed; }
int selected_style() { return style; }
string output()
{
   string *parts = ({});
   foreach (bytes part in writes) parts += ({to_text(part, "ISO-8859-1")});
   return implode(parts, "");
}
mixed deliver(object peer, string text)
{
   if (peer->removal_count()) return 0;
   return catch(evaluate(reader, peer, to_bytes(text, "ISO-8859-1")));
}
void eof(object peer)
{
   peer->remove();
   evaluate(closer, peer);
}
void drain()
{
   if (!writer) return;
   for (int i = 0; i < 512 && !removed; i++) evaluate(writer, this_object());
   if (!removed) error("recording socket: writer did not finish");
}
