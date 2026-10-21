inherit "/port_tests/http/check";
void create()
{
   boot();
   string body = "name=one+two&note=first%0D%0Asecond&label=caf%E9";
   for (int keep_alive = 0; keep_alive < 2; keep_alive++)
   {
      string headers = "POST /scgi/port_http.c HTTP/1.1\r\nHost: recording.invalid\r\n" +
         (keep_alive ? "Connection: keep-alive\r\n" : "Connection: close\r\n") +
         "Content-Length: " + sizeof(body) + "\r\nX-Trace: alpha:beta\r\n\r\n";
      string packet = headers + body;
      for (int split = 1; split < sizeof(packet); split++)
      {
         new_peer(); string label = "POST " + (keep_alive ? "keep-alive" : "close") + " split " + split;
         feed(packet[0..split - 1], label + " prefix");
         verify(!gateway->call_count() && peer->output() == "", label + " waits for complete body");
         feed(packet[split..], label + " suffix");
         post_reply(label + " dispatches once");
         mapping form = gateway->last_argument(); mapping info = gateway->last_headers();
         verify(sizeof(form) == 3 && form["name"] == "one two" && form["note"] == "first\r\nsecond" &&
                form["label"] == "café" && info["x-trace"] == "alpha:beta" &&
                info["content-length"] == "" + sizeof(body), label + " exact form and headers");
      }
   }
   new_peer();
   string headers = "POST /scgi/port_http.c HTTP/1.0\r\nContent-Length: " + sizeof(body) + "\r\n\r\n";
   feed(headers, "POST without Connection header waits");
   verify(!gateway->call_count(), "Content-Length applies without Connection header");
   for (int i = 0; i < sizeof(body); i++)
   {
      feed(body[i..i], "POST single body byte " + i);
      if (i + 1 < sizeof(body)) verify(!gateway->call_count(), "POST single body byte wait " + i);
   }
   post_reply("single-byte body completes without resetting remaining length");
   new_peer(); feed(headers + body, "coalesced headers and body callback");
   post_reply("coalesced POST baseline");
   new_peer(); feed("POST /scgi/port_http.c HTTP/1.0\r\nContent-Length: 0\r\n\r\n", "zero-length POST callback");
   post_reply("zero-length body completes at header terminator");
   new_peer(); feed("POST /scgi/port_http.c HTTP/1.0\r\n\r\n", "POST without content length callback");
   post_reply("omitted body length retains empty-body behavior");
   string raw = "note=first\r\nsecond\nthird";
   new_peer(); feed("POST /scgi/port_http.c HTTP/1.0\r\nContent-Length: " + sizeof(raw) + "\r\n\r\n" + raw,
                    "body newline callback");
   post_reply("body newlines are not parsed as header lines");
   verify(gateway->last_argument()["note"] == "first\r\nsecond\nthird", "raw body newlines preserved exactly");
   new_peer(); feed(headers + body + "ignored suffix", "coalesced body suffix callback");
   post_reply("declared body boundary consumes exactly one request");
   verify(sizeof(gateway->last_argument()) == 3 && gateway->last_argument()["label"] == "café",
          "bytes beyond content length do not enter the form");
   feed(headers + body, "late read on closing wrapper");
   verify(gateway->call_count() == 1, "late read cannot dispatch a second request");
   total();
}
