inherit "/port_tests/nntp/master";
protected int checks;
protected mixed *events = ({});
protected object client;
protected object transport;
protected int throw_reply;
protected int remove_reply;
protected int enqueue_reply;
protected void verify(int condition, string label)
{
   if (!condition) error("NNTP_FAIL: " + label);
   checks++;
   dump("NNTP_OK: " + label + "\n");
}
protected void record(mixed arguments...)
{
   events += ({arguments});
   if (remove_reply) { remove_reply = 0; client->remove(); }
   if (enqueue_reply) { enqueue_reply = 0; client->help(); }
   if (throw_reply) error("intentional callback exception");
}
protected void connect()
{
   events = ({}); throw_reply = remove_reply = enqueue_reply = 0;
   client = clone_object("/port_tests/nntp/client", record, "recording.invalid", 119);
   transport = "/port_tests/nntp/hub"->latest();
}
protected void ready()
{
   connect(); transport->feed("200 recording ready\r\n");
}
protected mixed result(int index) { return events[index][0]; }
protected void total() { dump("NNTP_TOTAL: " + checks + "\n"); }
