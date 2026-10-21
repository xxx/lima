inherit "/port_tests/nntp/check";
void create()
{
   ready();
   client->post("/port_tests/nntp/data/large.txt"); client->ihave("<b@example.invalid>", "/port_tests/nntp/data/small.txt"); client->help();
   verify(transport->wire() == "POST\r\n" && !sizeof(events), "POST waits for continuation before body");
   transport->feed("340 send article\r\n");
   verify(sizeof(events) == 1 && result(0) == "340 send article" && sizeof(transport->frames()) == 2,
          "POST continuation produces one bounded initial body frame");
   int chunks = transport->drain();
   string post_wire = read_file("/port_tests/nntp/data/post.expected");
   verify(chunks > 64 && transport->wire() == post_wire && !transport->removed(),
          "large UTF8 article streams over sixty-four frames with exact dot stuffing");
   verify(sizeof(events) == 1 && strindex(transport->wire(), "IHAVE ") < 0,
          "queued IHAVE stays behind POST final reply");
   transport->feed("240 accepted\r\n");
   verify(sizeof(events) == 2 && result(1) == "240 accepted" &&
          transport->wire() == read_file("/port_tests/nntp/data/post-ihave.expected"), "POST completion releases next command");
   transport->feed("335 transfer article\r\n"); transport->drain();
   string have_wire = read_file("/port_tests/nntp/data/have.expected");
   verify(sizeof(events) == 3 && result(2) == "335 transfer article" && transport->wire() == have_wire,
          "IHAVE uses its own body and preserves continuation callback");
   transport->feed("235 transferred\r\n");
   verify(sizeof(events) == 4 && result(3) == "235 transferred" &&
          transport->wire() == read_file("/port_tests/nntp/data/have-help.expected"), "IHAVE confirmation releases queued HELP");
   transport->feed("100 help\r\n.\r\n");
   verify(sizeof(events) == 5 && arrayp(result(4)), "queued command reply remains aligned after two body transfers");

   for (int i = 0; i < 7; i++) {
      ready(); client->post("/port_tests/nntp/data/ending" + i + ".txt");
      transport->feed("340 continue\r\n"); transport->drain();
      verify(transport->wire() == "POST\r\n" + read_file("/port_tests/nntp/data/ending" + i + ".expected"),
             "file newline normalization and dot handling " + i);
      transport->feed("240 accepted\r\n");
      verify(sizeof(events) == 2 && result(1) == "240 accepted", "file final confirmation " + i);
   }
   ready(); client->post("/port_tests/nntp/data/small.txt"); client->help();
   transport->feed("440 posting denied\r\n100 help\r\n.\r\n");
   verify(sizeof(events) == 2 && result(0) == "440 posting denied" && transport->wire() == "POST\r\nHELP\r\n",
          "POST rejection sends no body and advances queue");
   ready(); client->ihave("<c@example.invalid>", "/port_tests/nntp/data/small.txt"); client->slave();
   transport->feed("435 not wanted\r\n202 slave noted\r\n");
   verify(sizeof(events) == 2 && result(0) == "435 not wanted" &&
          transport->wire() == "IHAVE <c@example.invalid>\r\nSLAVE\r\n", "IHAVE rejection sends no body and advances queue");
   ready(); client->post("/port_tests/nntp/data/small.txt"); client->help();
   transport->feed("340 continue\r\n"); transport->drain(); transport->feed("441 failed\r\n100 help\r\n.\r\n");
   verify(sizeof(events) == 3 && result(1) == "441 failed" && arrayp(result(2)), "POST final failure still completes its request");
   ready(); client->ihave("<c@example.invalid>", "/port_tests/nntp/data/small.txt"); client->slave();
   transport->feed("335 continue\r\n"); transport->drain(); transport->feed("437 rejected\r\n202 slave noted\r\n");
   verify(sizeof(events) == 3 && result(1) == "437 rejected" && result(2) == "202 slave noted", "IHAVE final failure still completes its request");
   connect(); client->post("/port_tests/nntp/data/small.txt"); client->ihave("<c@example.invalid>", "/port_tests/nntp/data/small.txt"); client->help();
   transport->feed("201 posting unavailable\r\n100 help\r\n.\r\n");
   verify(sizeof(events) == 3 && result(0) == "nntp error: not confirmed for posting." &&
          result(1) == result(0) && arrayp(result(2)) && transport->wire() == "HELP\r\n",
          "201 preserves existing gate for both upload entry points after greeting");
   ready(); client->post("/port_tests/nntp/data/absent"); client->ihave("<c@example.invalid>", "/port_tests/nntp/data/absent");
   verify(sizeof(events) == 2 && result(0) == "nntp error: couldn't find local file." &&
          result(1) == result(0) && transport->wire() == "", "missing local files send no commands");
   ready(); client->post("/port_tests/nntp/data/small.txt"); client->help();
   transport->feed("340 continue\r\n240 accepted\r\n100 help\r\n.\r\n");
   verify(sizeof(events) == 3 && result(1) == "240 accepted" && arrayp(result(2)) &&
          !transport->removed() && transport->drain() == 0,
          "final response can precede drain notification after complete body is queued");
   total();
}
