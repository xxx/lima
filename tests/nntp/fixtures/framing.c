inherit "/port_tests/nntp/check";
void create()
{
   string response = "200 recording ready\r\n220 7 <a@example.invalid> text\r\nSubject: sample\r\n\r\n..dot\r\n...two\r\n . \r\n\u00e9\u4e2d\U0001f409\r\n.\r\n";
   for (int split = 1; split < sizeof(response); split++) {
      connect(); client->article("7");
      verify(transport->wire() == "", "queued before greeting " + split);
      transport->feed(response[0..split - 1]); transport->feed(response[split..]);
      mixed value = sizeof(events) ? result(0) : nil;
      verify(sizeof(events) == 1 && arrayp(value) && sizeof(value) == 7 &&
             value[0] == "220 7 <a@example.invalid> text" && value[1] == "Subject: sample" &&
             value[2] == "" && value[3] == ".dot" && value[4] == "..two" &&
             value[5] == " . " && value[6] == "\u00e9\u4e2d\U0001f409" &&
             transport->wire() == "ARTICLE 7\r\n", "every split retains exact reply " + split);
      client->remove();
   }
   connect(); client->article("7");
   foreach (int character in response) transport->feed(sprintf("%c", character));
   verify(sizeof(events) == 1 && sizeof(result(0)) == 7, "single-character chunks including split CRLF");
   ready(); client->body();
   transport->feed("222 7 <a@example.invalid> body\r\n");
   verify(!sizeof(events), "multiline status alone waits for body");
   transport->feed(".\r");
   verify(!sizeof(events), "split terminator waits for LF");
   transport->feed("\n");
   verify(sizeof(events) == 1 && sizeof(result(0)) == 1, "empty body preserves status header only");
   ready(); client->list();
   transport->feed("215 list follows\r\n.\r\n");
   verify(sizeof(events) == 1 && arrayp(result(0)) && !sizeof(result(0)), "empty LIST returns empty array");
   total();
}
