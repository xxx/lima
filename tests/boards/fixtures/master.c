inherit M_ACCESS;
inherit CLASS_NEWSMSG;
inherit CLASS_MAILMSG;
private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_shutdown(object caller, string program) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }
void error_handler(mapping issue) { dump("BOARD_ERROR: " + issue["diagnostic"] + "\n"); efun::shutdown(1); }
private void verify(int ok, string label) {
    if (!ok) throw("BOARD_FAIL: " + label);
    checks++; dump("BOARD_OK: " + label + "\n");
}

private int ordinal(string group, int id)
{
   return member_array(id, sort_array(NEWS_D->get_messages(group, 1), 1)) + 1;
}

private void exercise_edits(object user, object board)
{
   verify(board->query_mass() == to_float(SMALL), "portable board has explicit mass");
   int before = NEWS_D->get_group_last_id(BUG_NEWSGROUP);
   board->do_post(nil);
   verify(user->modal_stack_size() == 1, "missing subject starts one subject prompt");
   user->feed("x" * 51);
   verify(user->modal_stack_size() == 1, "long subject retains one retry prompt");
   user->feed("Board article");
   verify(user->modal_stack_size() == 1, "valid subject replaces prompt with actual editor");
   user->feed("posted body λ");
   board->do_switch(TYPO_NEWSGROUP);
   user->feed(".");
   class news_msg post = NEWS_D->get_message(BUG_NEWSGROUP, before + 1);
   verify(post && post.subject == "Board article" && post.body == "posted body λ" && post.userid == "boardaudit",
          "post callback retains starting group subject and current author");
   verify(NEWS_D->get_group_last_id(TYPO_NEWSGROUP) == 0 && user->modal_stack_size() == 0,
          "post completion leaves other group and input stack untouched");
   board->do_switch(BUG_NEWSGROUP);
   board->do_post("Cancelled"); user->feed("~q");
   verify(NEWS_D->get_group_last_id(BUG_NEWSGROUP) == before + 1 && user->modal_stack_size() == 0,
          "explicit editor cancellation creates no post");
   board->do_post(nil); user->feed("");
   verify(user->modal_stack_size() == 0, "empty subject aborts without a stuck prompt");
   board->do_post("Empty body"); user->feed(".");
   verify(NEWS_D->get_message(BUG_NEWSGROUP, before + 2).body == "", "completed empty body remains a visible article");

   int parent = sort_array(NEWS_D->get_messages(BUG_NEWSGROUP, 1), 1)[0];
   before = NEWS_D->get_group_last_id(BUG_NEWSGROUP);
   board->do_followup(ordinal(BUG_NEWSGROUP, parent)); user->feed("followup body");
   board->do_switch(TYPO_NEWSGROUP); user->feed(".");
   class news_msg followup = NEWS_D->get_message(BUG_NEWSGROUP, before + 1);
   verify(followup && followup.body == "followup body" && followup.thread_id == parent,
          "followup callback retains starting group and thread");
   board->do_switch(BUG_NEWSGROUP); user->take_output();
   board->do_followup_with_message(ordinal(BUG_NEWSGROUP, parent));
   string quoted = user->take_output();
   verify(strindex(quoted, "> first body") >= 0 && strindex(quoted, "post #" + parent) >= 0,
          "quoted followup uses original text and actual article id");
   user->feed("quoted response"); user->feed(".");
   followup = NEWS_D->get_message(BUG_NEWSGROUP, before + 2);
   verify(followup.thread_id == parent && strindex(followup.body, "quoted response") >= 0,
          "quoted editor completion posts into original thread");
   board->do_followup(0); board->do_followup_with_message(9999); board->do_reply(0);
   verify(user->modal_stack_size() == 0, "invalid article operations open no editor");
   board->do_remove(ordinal(BUG_NEWSGROUP, before + 1));
   verify(nilp(NEWS_D->get_message(BUG_NEWSGROUP, before + 1).body), "own followup removal reaches selected daemon");

   unguarded(1, (: NEWS_D->add_group("board.retry") :));
   board->do_switch("board.retry"); board->do_post("Retained post"); user->feed("retained post text");
   unguarded(1, (: NEWS_D->remove_group("board.retry") :));
   user->take_output(); user->feed(".");
   string retry_output = user->take_output();
   verify(user->modal_stack_size() == 1 && strindex(retry_output, "Post not completed") >= 0
          && strindex(retry_output, "retained post text") >= 0, "failed post retains original text in replacement editor");
   unguarded(1, (: NEWS_D->add_group("board.retry") :));
   user->feed(".");
   verify(NEWS_D->get_message("board.retry", 1).body == "retained post text" && user->modal_stack_size() == 0,
          "manual retry completes retained post when its group returns");
   board->do_followup(1); user->feed("retained followup text");
   unguarded(1, (: NEWS_D->remove_group("board.retry") :));
   user->take_output(); user->feed("."); retry_output = user->take_output();
   verify(user->modal_stack_size() == 1 && strindex(retry_output, "Followup not completed") >= 0
          && strindex(retry_output, "retained followup text") >= 0, "failed followup retains text without reporting success");
   user->feed("~q"); board->do_switch(BUG_NEWSGROUP);
   verify(user->modal_stack_size() == 0 && board->query_group() == BUG_NEWSGROUP,
          "failed edit can be cancelled and a removed group can be left");

   user->subscribe_group(BUG_NEWSGROUP); user->subscribe_group(TYPO_NEWSGROUP); user->subscribe_group(IDEA_NEWSGROUP);
   user->catch_up_newsgroup(BUG_NEWSGROUP);
   unguarded(1, (: NEWS_D->system_post(IDEA_NEWSGROUP, "Unread group", "idea body", "Boardposter") :));
   board->do_switch("next");
   verify(board->query_group() == IDEA_NEWSGROUP, "next group uses subscribed current user read sets");
   board->read_entry("next"); user->take_output(); board->do_switch("next");
   verify(board->query_group() == IDEA_NEWSGROUP && strindex(user->take_output(), "No new news") >= 0,
          "exhausted group switching preserves selection and reports completion");
   board->do_switch("absent.group");
   verify(board->query_group() == IDEA_NEWSGROUP, "unknown group leaves configured group intact");
   board->set_switchable(0); board->do_switch(BUG_NEWSGROUP);
   verify(board->query_group() == IDEA_NEWSGROUP && !board->direct_switch_obj_to_str(board, BUG_NEWSGROUP),
          "non-switchable board rejects group changes");
   board->set_switchable(1); board->do_switch(BUG_NEWSGROUP);
   verify(board->indirect_post_on_obj_about_str("subject", board) == 1,
          "implicit-board post checker matches the shipped verb interface");

   save_map((["bodies":([])]), "/data/links/b/boardposter");
   save_map((["bodies":([])]), "/data/links/b/boardaudit");
   foreach (string handler in ({"standard", "Mh"}))
   {
      user->unload_mailer(); user->set_default_mailer(MAILER_DIR + "/" + handler);
      int article = unguarded(1, (: NEWS_D->system_post(BUG_NEWSGROUP, "Original subject", "source text", "Boardposter") :));
      int count = MAILBOX_D->get_mailbox("boardposter")->query_message_count();
      board->do_reply(ordinal(BUG_NEWSGROUP, article));
      verify(user->modal_stack_size() == 1, handler + " reply opens actual user mailer editor");
      unguarded(1, (: NEWS_D->change_header(BUG_NEWSGROUP, $(article), "Changed subject") :));
      unguarded(1, (: NEWS_D->remove_post(BUG_NEWSGROUP, $(article)) :));
      user->feed("mail reply λ"); user->feed(".");
      verify(user->modal_stack_size() == 1 && MAILBOX_D->get_mailbox("boardposter")->query_message_count() == count,
             handler + " reply waits for normal Cc confirmation before sending");
      user->feed("");
      object recipient = clone_object("/port_tests/boards/user", "boardposter");
      set_this_player(recipient);
      object box = MAILBOX_D->get_mailbox("boardposter");
      int *keys = box->query_message_keys();
      class mail_msg message = box->get_one_message(keys[<1]);
      verify(sizeof(keys) == count + 1 && message.subject == "Re: Original subject" && message.sender == "boardaudit"
             && member_array("mail reply λ", message.body) >= 0,
             handler + " reply delivers captured subject recipient and text through native mail saves");
      set_this_player(user); destruct(recipient);
      verify(user->modal_stack_size() == 0 && file_size("/tmp/tmail.boardaudit") == -1,
             handler + " successful reply clears modal state and saved draft");
      article = unguarded(1, (: NEWS_D->system_post(BUG_NEWSGROUP, "Cancel reply", "source text", "Boardposter") :));
      write_file("/tmp/tmail.boardaudit", "older draft\n", 1);
      board->do_reply(ordinal(BUG_NEWSGROUP, article)); user->feed("~q");
      verify(user->modal_stack_size() == 0 && read_file("/tmp/tmail.boardaudit") == "older draft\n"
             && MAILBOX_D->get_mailbox("boardposter")->query_message_count() == count + 1,
             handler + " cancelled reply preserves older draft and sends nothing");
      rm("/tmp/tmail.boardaudit");
   }
   int invalid = unguarded(1, (: NEWS_D->system_post(BUG_NEWSGROUP, "No recipient", "text", "Absentboarduser") :));
   board->do_reply(ordinal(BUG_NEWSGROUP, invalid)); user->feed("retained reply"); user->feed("."); user->feed("");
   verify(read_file("/tmp/tmail.boardaudit") == "retained reply\n" && user->modal_stack_size() == 0,
          "invalid local reply destination retains draft through existing mailer flow");
   user->query_mailer()->begin_news_reply(BUG_NEWSGROUP, 99999);
   verify(user->modal_stack_size() == 0, "missing news article opens no reply composer");
}

private void run_tests() {
    set_privilege(1);
    object user = clone_object("/port_tests/boards/user");
    set_this_player(user);
    object board = clone_object("/domains/std/object/portable_board");
    verify(board->query_group() == BUG_NEWSGROUP && board->query_switchable(),
           "portable board setup uses configured report group and switching flag");
    verify(board->has_entries() && board->is_bulletin_board(), "board retains readable and verb marker interfaces");
    verify(function_exists("direct_get_obj", board), "portable board retains gettable module");
    string desc = board->a_short();
    verify(stringp(desc) && strindex(desc, "0 unread, 0 total") >= 0, "empty portable short is text with correct counts");
    verify(board->read_entry("next") == "No more messages.\n", "empty next read is bounded");
    verify(strindex(board->get_base_long(), "bereft of posts") >= 0, "dynamic long invokes real header renderer");
    int first = unguarded(1, (: NEWS_D->system_post(BUG_NEWSGROUP, "First article", "first body", "Boardaudit") :));
    int removed = unguarded(1, (: NEWS_D->system_post(BUG_NEWSGROUP, "Removed article", "removed body", "Boardaudit") :));
    int last = unguarded(1, (: NEWS_D->system_post(BUG_NEWSGROUP, "Last article", "last body", "Boardaudit") :));
    verify(first > 0 && removed > first && last > removed, "selected news daemon creates real ordered articles");
    unguarded(1, (: NEWS_D->remove_post(BUG_NEWSGROUP, removed) :));
    verify(strindex(board->a_short(), "2 unread, 2 total") >= 0, "description excludes removed article");
    string headers = board->get_base_long();
    verify(strindex(headers, "First article") >= 0 && strindex(headers, "Last article") >= 0 &&
           strindex(headers, "Removed article") == -1, "real header renderer filters removed article");
    string reject = "You can only read posts that are on the bulletin board!\n";
    verify(board->read_entry("0") == reject && board->read_entry("-1") == reject &&
           board->read_entry("3") == reject, "numeric reads reject zero negative and out of range");
    verify(user->get_news_id_read(BUG_NEWSGROUP) == "", "invalid reads preserve current user read set");
    string text = board->read_entry("2");
    verify(strindex(text, "Last article") >= 0 && strindex(text, "last body") >= 0,
           "visible ordinal resolves across removed daemon id");
    verify(user->set_member(user->get_news_id_read(BUG_NEWSGROUP), last) &&
           !user->set_member(user->get_news_id_read(BUG_NEWSGROUP), first), "numeric read records exact article without a highwater counter");
    verify(strindex(board->a_short(), "1 unread, 2 total") >= 0, "portable description updates after reading");
    text = board->read_entry("next");
    verify(strindex(text, "first body") >= 0, "next read returns earlier unread article after out of order read");
    verify(board->read_entry("next") == "No more messages.\n", "next stops after all active articles are read");
    verify(user->query_saves() == 2, "current user news module saves each completed read");
    object second = clone_object("/domains/std/object/portable_board");
    verify(strindex(second->a_short(), "0 unread, 2 total") >= 0, "another board shares the current user read set");
    set_this_player(nil);
    verify(stringp(board->a_short()), "short description also supports absent player context");
    set_this_player(user);
    exercise_edits(user, board);
    dump("BOARD_TOTAL: " + checks + "\n");
    efun::shutdown(0);
}
string *epilog(int ignored) { call_out(run_tests, 0); return ({}); }
