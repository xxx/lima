private object owner;
private function reader;
private function closer;
private function writer;
private string *writes = ({});
private string address;
private int style;
private int removals;
private int fail_write;
varargs void create(int selected, string target, function read, function close)
{
   if (!target) return;
   owner = previous_object(); style = selected; address = target;
   reader = read; closer = close;
   "/port_tests/nntp/hub"->created(this_object());
}
void set_write_callback(function callback)
{
   if (previous_object() != owner) error("recording transport: wrong owner");
   writer = callback;
}
void send(string text)
{
   if (previous_object() != owner) error("recording transport: wrong owner");
   if (fail_write) error("recording transport: requested write failure");
   if (removals) error("recording transport: removed");
   if (sizeof(to_bytes(text, "UTF-8")) > 8192) error("recording transport: oversized frame");
   writes += ({text});
}
void remove() { removals++; }
string *frames() { return writes[0..]; }
string wire() { return implode(writes, ""); }
int removed() { return removals; }
string endpoint() { return address; }
int selected_style() { return style; }
void reject_writes() { fail_write = 1; }
void feed(string text) { evaluate(reader, this_object(), text); }
void stale_read(object other, string text) { evaluate(reader, other, text); }
void close() { evaluate(closer, this_object()); }
void stale_close(object other) { evaluate(closer, other); }
int drain()
{
   int count;
   for (int i = 0; i < 512 && !removals; i++) {
      mixed chunk = evaluate(writer, this_object());
      if (chunk == nil || !sizeof(chunk)) return count;
      if (!stringp(chunk) || sizeof(to_bytes(chunk, "UTF-8")) > 8192)
         error("recording transport: invalid producer frame");
      writes += ({chunk}); count++;
   }
   if (!removals) error("recording transport: producer did not finish");
   return count;
}
