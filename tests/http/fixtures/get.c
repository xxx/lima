inherit "/port_tests/http/check";
void create()
{
   boot();
   string packet = "GET /framing.html HTTP/1.1\r\nHost: recording.invalid\r\nX-Trace: alpha:beta\r\n\r\n";
   for (int split = 1; split < sizeof(packet); split++)
   {
      new_peer();
      feed(packet[0..split - 1], "GET prefix callback " + split);
      verify(peer->output() == "" && !peer->removal_count(), "GET waits for full headers " + split);
      feed(packet[split..], "GET suffix callback " + split);
      get_reply("HTTP_FRAGMENT_OK\n", "every GET split serves once " + split);
   }
   new_peer(); feed(packet, "complete GET callback");
   get_reply("HTTP_FRAGMENT_OK\n", "complete GET baseline");
   new_peer();
   for (int i = 0; i < sizeof(packet); i++)
   {
      feed(packet[i..i], "GET single byte callback " + i);
      if (i + 1 < sizeof(packet)) verify(peer->output() == "", "GET single byte wait " + i);
   }
   get_reply("HTTP_FRAGMENT_OK\n", "single-byte GET completion");
   new_peer(); feed("GET /framing.html HTTP/1.0\nHost: recording.invalid\n\n", "LF-only GET callback");
   get_reply("HTTP_FRAGMENT_OK\n", "LF-only compatibility");
   new_peer(); feed("GET /framing.bin HTTP/1.0\r\n\r\n", "binary static GET callback");
   get_reply(to_text(read_bytes("/WWW/framing.bin"), "ISO-8859-1"), "exact binary static response after producer drains");
   new_peer(); feed("GET /scgi/port_http.c?word=one HTTP/1.0\r\n\r\n", "GET CGI query callback");
   get_reply(to_text(read_bytes("/port_tests/http/data/cgi-get.expected"), "ISO-8859-1"), "UTF8 CGI response retains byte content length and CRLF conversion");
   verify(gateway->call_count() == 1 && gateway->last_argument() == "word=one", "GET CGI query argument remains a string");
   new_peer(); feed("GET /scgi/port_http.c HTTP/1.0\r\n\r\n", "GET CGI without query callback");
   get_reply(to_text(read_bytes("/port_tests/http/data/cgi-get.expected"), "ISO-8859-1"), "GET CGI without query response");
   verify(gateway->call_count() == 1 && gateway->last_argument() == nil, "GET CGI omitted argument remains nil");
   total();
}
