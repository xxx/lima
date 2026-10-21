inherit "/port_tests/http/master";
protected string gateway = "/secure/cgi/port_http";
protected int checks;
protected object server;
protected object listener;
protected object peer;
protected void verify(int condition, string label)
{
   if (!condition) error("HTTP_FAIL: " + label);
   checks++; dump("HTTP_OK: " + label + "\n");
}
protected void boot()
{
   server = clone_object("/port_tests/http/daemon");
   listener = "/port_tests/http/hub"->latest();
   verify(listener->selected_style() == 11, "binary listener constructor shape");
}
protected void new_peer()
{
   peer = clone_object("/port_tests/http/socket");
   gateway->reset_calls();
}
protected void feed(string text, string label)
{
   verify(!listener->deliver(peer, text), label);
}
protected void get_reply(string expected, string label)
{
   peer->drain();
   string output = peer->output();
   int end = strindex(output, "\r\n\r\n");
   verify(end >= 0 && strindex(output, "HTTP/1.1 200 OK\r\n") == 0 &&
          strindex(output[0..end + 1], "Content-Length: " + sizeof(expected) + "\r\n") >= 0 &&
          output[end + 4..] == expected && peer->removal_count() == 1, label);
}
protected void post_reply(string label)
{
   verify(gateway->call_count() == 1 &&
          peer->output() == to_text(to_bytes("CGI é\n\n", "UTF-8"), "ISO-8859-1") &&
          peer->removal_count() == 1, label);
}
protected void total()
{
   server->remove();
   dump("HTTP_TOTAL: " + checks + "\n");
}
