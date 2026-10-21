inherit "/port_tests/nntp/check";
void create()
{
   connect();
   verify(transport->endpoint() == "recording.invalid 119" && transport->selected_style() == 2,
          "selected text constructor shape and port");
   client->group("example.group"); client->stat("7"); client->next(); client->last();
   transport->feed("200 greeting\r\n211 003 0007 0009 example.group description\r\n223 7 <first@example.invalid>\r\n223 8 <next@example.invalid>\r\n223 7 <last@example.invalid>\r\n");
   verify(sizeof(events) == 4 && sizeof(events[0]) == 5 &&
          events[0][0] == 211 && intp(events[0][1]) && events[0][1] == 3 &&
          intp(events[0][2]) && events[0][2] == 7 && intp(events[0][3]) &&
          events[0][3] == 9 && events[0][4] == "example.group", "GROUP numeric fields and canonical name");
   verify(sizeof(events[1]) == 3 && events[1][0] == 223 && intp(events[1][1]) &&
          events[1][1] == 7 && events[1][2] == "<first@example.invalid>" &&
          events[2][1] == 8 && events[3][1] == 7, "STAT NEXT LAST numeric article and message id");
   verify(transport->wire() == "GROUP example.group\r\nSTAT 7\r\nNEXT\r\nLAST\r\n", "queued commands serialize in call order");
   ready(); client->list(); client->head("7"); client->body("7"); client->article(); client->help();
   transport->feed("215 groups\r\nexample.group 9 7 y\r\n.\r\n221 7 <a@example.invalid> head\r\nSubject: example\r\n.\r\n222 7 <a@example.invalid> body\r\ntext\r\n.\r\n220 7 <a@example.invalid> article\r\nSubject: example\r\n\r\ntext\r\n.\r\n100 help\r\nHELP\r\n.\r\n");
   verify(sizeof(events) == 5 && arrayp(result(0)) && sizeof(result(0)) == 1 &&
          result(0)[0][0] == "example.group" && result(0)[0][1] == "9" &&
          result(0)[0][3] == "y", "LIST retains word-array shape");
   verify(sizeof(events[1]) == 1 && sizeof(result(1)) == 2 &&
          result(1)[0] == "221 7 <a@example.invalid> head", "HEAD raw array includes status");
   verify(sizeof(events[2]) == 1 && sizeof(result(2)) == 2 &&
          result(2)[1] == "text", "BODY raw array includes text");
   verify(sizeof(events[3]) == 1 && sizeof(result(3)) == 4 &&
          result(3)[2] == "", "ARTICLE raw array preserves header separator");
   verify(arrayp(result(4)) && result(4)[1] == "HELP", "HELP raw multiline reply");
   verify(transport->wire() == "LIST\r\nHEAD 7\r\nBODY 7\r\nARTICLE\r\nHELP\r\n", "article command formatting and optional ids");
   ready(); client->newgroups(0); client->newnews("example.*", -1); client->slave();
   transport->feed("231 groups\r\nexample.group 9 7 y\r\n.\r\n230 articles\r\n<a@example.invalid>\r\n.\r\n202 slave noted\r\n");
   verify(transport->wire() == "NEWGROUPS 700101 000000 GMT\r\nNEWNEWS example.* 691231 235959 GMT\r\nSLAVE\r\n", "date commands send legacy GMT wire shape");
   verify(sizeof(events) == 3 && arrayp(result(0)) && result(0)[1] == "example.group 9 7 y" &&
          result(1)[1] == "<a@example.invalid>" && result(2) == "202 slave noted", "NEWGROUPS NEWNEWS SLAVE raw replies");
   ready(); client->group("missing"); client->stat("7"); client->list(); client->article("7");
   transport->feed("411 missing group\r\n423 missing article\r\n503 list disabled\r\n430 absent\r\n");
   verify(sizeof(events) == 4 && result(0) == "nntp error: unexpected reply: 411" &&
          result(1) == "nntp error: unexpected reply: 423" &&
          result(2) == "nntp error: unexpected reply: 503" && result(3) == "430 absent" &&
          !transport->removed(), "ordinary negative replies complete requests without desynchronizing");
   ready(); client->help(); client->xhdr("subject", "1-9"); client->slave();
   transport->feed("100 help\r\n.\r\n202 slave noted\r\n");
   verify(sizeof(events) == 3 && result(1) == "nntp error: not implemented yet." &&
          transport->wire() == "HELP\r\nSLAVE\r\n", "XHDR reports limitation in queue order without issuing unsupported command");
   ready(); enqueue_reply = 1; client->group("example.group");
   transport->feed("211 3 7 9 example.group\r\n100 help\r\n.\r\n");
   verify(sizeof(events) == 2 && transport->wire() == "GROUP example.group\r\nHELP\r\n", "callback can queue another request after completed state");
   total();
}
