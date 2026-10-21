inherit "/port_tests/nntp/check";
void create()
{
   ready(); client->help(); client->quit();
   verify(transport->wire() == "HELP\r\n", "QUIT waits behind active command");
   verify(catch(client->list()) != nil, "new requests reject after queued QUIT");
   transport->feed("100 help\r\n.\r\n205 goodbye\r\n");
   verify(sizeof(events) == 2 && result(1) == "205 goodbye" && transport->removed() == 1 &&
          transport->wire() == "HELP\r\nQUIT\r\n", "QUIT completes callback and removes transport once");
   transport->close(); transport->feed("205 repeated\r\n"); client->remove();
   verify(sizeof(events) == 2 && transport->removed() == 1, "QUIT ignores late close and reads");
   verify(catch(client->help()) != nil, "logical closure rejects writes while removed transport object persists");
   ready(); client->article("7"); client->help();
   transport->feed("220 7 <a@example.invalid> body\r\npartial\r"); transport->close();
   verify(sizeof(events) == 1 && stringp(result(0)) && transport->removed() == 1 &&
          transport->wire() == "ARTICLE 7\r\n", "EOF discards incomplete reply and pending queue with one terminal report");
   transport->close(); transport->feed("\n.\r\n");
   verify(sizeof(events) == 1 && transport->removed() == 1, "repeated EOF and residual data cannot complete abandoned reply");
   connect(); client->list(); transport->close();
   verify(sizeof(events) == 1 && transport->wire() == "" && transport->removed() == 1, "EOF before greeting clears unsent commands");
   connect(); client->help(); transport->feed("502 unavailable\r\n");
   verify(sizeof(events) == 1 && transport->removed() == 1 && transport->wire() == "", "failed greeting never sends queued command");
   ready(); transport->feed("202 unsolicited\r\n");
   verify(sizeof(events) == 1 && transport->removed() == 1, "empty pending queue handles unsolicited status explicitly");
   ready(); client->help(); client->list(); transport->feed("nonnumeric reply\r\n");
   verify(sizeof(events) == 1 && transport->removed() == 1 && transport->wire() == "HELP\r\n", "invalid status terminates without advancing pending commands");
   ready(); client->group("example.group"); transport->feed("211 many 7 9 example.group\r\n");
   verify(sizeof(events) == 1 && stringp(result(0)) && transport->removed() == 1, "invalid numeric GROUP field is not converted to zero");
   ready(); client->stat("7"); transport->feed("223 unknown <a@example.invalid>\r\n");
   verify(sizeof(events) == 1 && transport->removed() == 1, "invalid numeric STAT field has explicit terminal result");
   ready(); client->help(); transport->feed("215 wrong response family\r\n");
   verify(sizeof(events) == 1 && transport->removed() == 1, "unexpected success family cannot consume a different multiline reply");
   ready(); client->help(); client->list(); transport->feed("400 closing service\r\n");
   verify(sizeof(events) == 1 && transport->removed() == 1 && transport->wire() == "HELP\r\n", "server shutdown clears pending commands");
   ready(); client->post("/port_tests/nntp/data/large.txt"); client->help(); transport->feed("340 continue\r\n");
   int frames = sizeof(transport->frames()); transport->close(); transport->drain();
   verify(sizeof(events) == 2 && transport->removed() == 1 && sizeof(transport->frames()) == frames,
          "EOF during body stops producer and reports once after continuation");
   ready(); client->post("/port_tests/nntp/data/large.txt"); transport->feed("340 continue\r\n240 premature\r\n");
   verify(sizeof(events) == 2 && transport->removed() == 1, "final reply while body pieces remain cannot release following command");
   ready(); client->help(); client->list(); throw_reply = 1;
   transport->feed("100 help\r\n.\r\n"); transport->close();
   verify(sizeof(events) == 1 && transport->removed() == 1 && transport->wire() == "HELP\r\n",
          "throwing response callback clears queue without recursive notification");
   ready(); client->help(); client->list(); remove_reply = 1;
   transport->feed("100 help\r\n.\r\n");
   verify(sizeof(events) == 1 && transport->removed() == 1 && transport->wire() == "HELP\r\n", "callback removal prevents dispatch of next request");
   ready(); transport->reject_writes(); client->help();
   verify(sizeof(events) == 1 && transport->removed() == 1, "command send failure terminates and reports");
   ready(); client->post("/port_tests/nntp/data/small.txt"); transport->reject_writes(); transport->feed("340 continue\r\n");
   verify(sizeof(events) == 1 && transport->removed() == 1, "first body send failure reports error instead of continuation");
   ready(); client->help(); transport->stale_read(this_object(), "100 stale\r\n.\r\n"); transport->stale_close(this_object());
   verify(!sizeof(events) && !transport->removed(), "callbacks from another socket do not change active request");
   transport->feed("100 real\r\n.\r\n");
   verify(sizeof(events) == 1 && result(0)[0] == "100 real", "active socket still completes after stale callbacks");
   ready(); client->help(); client->remove(); client->remove(); transport->feed("100 ignored\r\n.\r\n"); transport->close();
   verify(!sizeof(events) && transport->removed() == 1, "explicit removal clears state once without synthetic callback");
   ready(); client->article("7"); transport->feed("220 7 <a@example.invalid> body\r\n");
   string long_line = read_file("/port_tests/nntp/data/long-line.txt");
   for (int start = 0; start < sizeof(long_line); start += 4096)
      transport->feed(long_line[start..start + 4095]);
   verify(sizeof(events) == 1 && result(0) == "nntp error: could not process response." &&
          transport->removed() == 1, "native string limit closes and reports instead of leaving a stalled reply");
   total();
}
