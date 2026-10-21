inherit "/port_tests/http/check";
private void feed_file(string name, int width)
{
   string packet = read_file("/port_tests/http/data/" + name);
   for (int start = 0; start < sizeof(packet); start += width)
   {
      feed(packet[start..start + width - 1], name + " chunk " + start);
      if (start + width < sizeof(packet))
         verify(!gateway->call_count() && peer->output() == "", name + " waits " + start);
   }
}
void create()
{
   boot();
   new_peer(); feed_file("large-header.request", 257);
   post_reply("large header block retained across ordinary chunk boundaries");
   verify(gateway->last_argument()["name"] == "value", "body after large header block remains exact");
   mapping info = gateway->last_headers(); int headers_correct = sizeof(info) == 101;
   for (int i = 0; i < 100; i++)
      if (info["x-block-" + i] != "x" * 200) headers_correct = 0;
   verify(headers_correct, "all large-block header names and values preserved");

   new_peer(); feed_file("large-body.request", 257);
   post_reply("large form body assembled without repeated string concatenation");
   mapping form = gateway->last_argument();
   verify(sizeof(form) == 1000, "all large-body fields delivered");
   int correct = 1;
   for (int i = 0; i < 1000; i++)
      if (form["field" + i] != sprintf("v%04d", i)) correct = 0;
   verify(correct, "large-body values preserved at every field");
   verify(gateway->last_headers()["x-long"] == "streamed",
          "POST header survives body fragments");

   new_peer();
   object first = peer;
   feed("GET /framing.html HTTP/1.0\r\nHost: fir", "first peer header prefix");
   new_peer(); object second = peer;
   feed("POST /scgi/port_http.c HTTP/1.0\r\nContent-Length: 10\r\n\r\nname=", "second peer body prefix");
   verify(!gateway->call_count() && first->output() == "" && second->output() == "",
          "interleaved peers retain separate pending state");
   peer = first; feed("st\r\n\r\n", "first peer header completion");
   get_reply("HTTP_FRAGMENT_OK\n", "first peer completes independently");
   verify(!gateway->call_count() && second->output() == "", "first completion does not consume second body");
   peer = second; feed("value", "second peer body completion");
   post_reply("second peer completes independently");
   verify(gateway->last_argument()["name"] == "value", "interleaved POST body remains exact");

   new_peer(); feed("GET /framing.html HTTP/1.0\r\nX-Unfinished:", "partial header before EOF");
   verify(peer->output() == "", "incomplete header has no response before EOF");
   listener->eof(peer);
   verify(peer->removal_count() == 1 && peer->output() == "", "header EOF produces no request dispatch");
   new_peer(); feed("POST /scgi/port_http.c HTTP/1.0\r\nContent-Length: 10\r\n\r\nname=", "partial body before EOF");
   verify(!gateway->call_count(), "incomplete body has not invoked CGI");
   listener->eof(peer);
   verify(peer->removal_count() == 1 && !gateway->call_count(), "body EOF does not deliver a truncated form");
   new_peer(); feed("POST /scgi/port_http.c HTTP/1.0\r\nContent-Length: 10\r\n\r\nname=value", "request after earlier EOFs");
   post_reply("other requests remain usable after EOF cleanup");
   total();
}
