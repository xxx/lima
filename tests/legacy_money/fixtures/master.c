#ifdef USE_MONEY
#error The default profile must keep USE_MONEY unselected
#endif
inherit M_ACCESS;
private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_shutdown(object caller, string program) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }
void error_handler(mapping issue) { dump("LEGACY_MONEY_ERROR: " + issue["diagnostic"] + "\n"); efun::shutdown(1); }
private void verify(int ok, string label)
{
   if (!ok) throw("LEGACY_MONEY_FAIL: " + label);
   checks++; dump("LEGACY_MONEY_OK: " + label + "\n");
}
private void balance(object actor, string currency, float expected, string label)
{
   verify(abs(actor->query_amt_currency(currency) - expected) < 0.000000000001, label);
}
private object make_actor(object where)
{
   object actor = clone_object("/port_tests/legacy_money/actor");
   set_this_player(actor);
   actor->move(where);
   return actor;
}
private object make_pile(object where, int amount, string type)
{
   object result = clone_object(MONEY, amount, type);
   result->move(where);
   return result;
}
private void take(string amount, string type)
{
   "/cmds/verbs/get"->do_get_wrd_str(amount, type);
}
private void single_denomination()
{
   object where = clone_object(INDOOR_ROOM); object actor = make_actor(where);
   object money = make_pile(where, 4, "auditquarters");
   verify(base_name(money) == MONEY && sizeof(money->query_money()) == 1 && money->query_money()[0] == "auditquarter",
          "constructor normalizes denomination alias and retains denomination key");
   verify(money->is_gettable() && money->get() == "" && !money,
          "zero-argument get satisfies gettable contract and consumes pile");
   balance(actor, "auditcrown", 1.0, "whole legacy pickup converts four quarters to one currency unit");

   money = make_pile(where, 4, "auditquarter");
   verify(strindex(money->get_base_long(), "4 auditquarters") >= 0, "bare description callback returns text");
   take("1", "coin");
   balance(actor, "auditcrown", 1.25, "generic numeric coin pickup counts the sole denomination");
   verify(strindex(money->get_base_long(), "3 auditquarters") >= 0, "partial legacy pickup retains integer count");
   take("all", "coins");
   verify(!money && !present("money", where), "generic all pickup consumes sole denomination");
   balance(actor, "auditcrown", 2.0, "generic numeric and all pickup recover seeded total");

   money = make_pile(where, 2, "auditquarter");
   money->merge_money(2, "auditquarters");
   verify(sizeof(money->query_money()) == 1 && strindex(money->get_base_long(), "4 auditquarters") >= 0,
          "merge canonicalizes alias and adds integer counts");
   take("1", "auditquarters");
   balance(actor, "auditcrown", 2.25, "plural denomination pickup credits fractional value once");
   "/cmds/verbs/get"->do_get_obj(money);
   verify(!money, "actual get verb whole-object path handles legacy return contract");
   balance(actor, "auditcrown", 3.0, "same-denomination merge and pickup conserve seeded value");
}
private void shared_currency_denominations()
{
   object where = clone_object(INDOOR_ROOM); object actor = make_actor(where);
   object money = make_pile(where, 2, "auditquarter");
   money->merge_money(3, "audithalves");
   verify(sizeof(money->query_money()) == 2 && member_array("audithalf", money->query_money()) >= 0,
          "new denomination merge initializes a missing integer balance");
   string description = money->get_base_long();
   verify(strindex(description, "2 auditquarters") >= 0 && strindex(description, "3 audithalves") >= 0,
          "description retains separate counts within one currency");
   actor->take_output(); take("all", "coins");
   verify(money && sizeof(money->query_money()) == 2 && strindex(actor->take_output(), "Which type") >= 0,
          "generic pickup remains ambiguous for multiple denominations of one currency");
   balance(actor, "auditcrown", 0.0, "ambiguous denomination pickup preserves wallet");
   take("all", "auditquarters");
   verify(money && sizeof(money->query_money()) == 1 && money->query_money()[0] == "audithalf",
          "named all pickup consumes only that denomination");
   balance(actor, "auditcrown", 0.5, "named all quarters credits their own value");
   take("all", "audithalves");
   verify(!money, "last denomination pickup removes empty legacy pile");
   balance(actor, "auditcrown", 2.0, "separate denomination pickups conserve shared-currency total");
}
private void separate_currencies()
{
   object where = clone_object(INDOOR_ROOM); object actor = make_actor(where);
   object money = make_pile(where, 3, "auditquarter");
   money->merge_money(2, "audittenths");
   verify(sizeof(money->query_money()) == 2 && member_array("audittenth", money->query_money()) >= 0,
          "legacy merge supports denomination from another currency");
   take("1", "audittenth");
   balance(actor, "auditcredit", 0.1, "second-currency partial pickup converts its denomination");
   balance(actor, "auditcrown", 0.0, "second-currency pickup leaves first currency unchanged");
   take("all", "auditquarter");
   balance(actor, "auditcrown", 0.75, "first-currency all pickup credits exact denomination count");
   verify(money && sizeof(money->query_money()) == 1 && money->query_money()[0] == "audittenth",
          "first-currency pickup retains other denomination");
   "/cmds/verbs/get"->do_get_obj(money);
   balance(actor, "auditcredit", 0.2, "whole pickup completes second-currency total");
   verify(!money, "mixed-currency pile disappears after all values are recovered");
}
private void refusals_and_repetition()
{
   object where = clone_object(INDOOR_ROOM); object actor = make_actor(where);
   object money = make_pile(where, 3, "auditquarter");
   take("4", "auditquarter"); take("0", "auditquarter"); take("-1", "auditquarter");
   money->split_money(0, "auditquarter"); money->split_money(-1, "auditquarter");
   balance(actor, "auditcrown", 0.0, "insufficient zero and negative pickups preserve wallet");
   verify(strindex(money->get_base_long(), "3 auditquarters") >= 0,
          "refused pickups preserve complete denomination count");
   take("all", "audittenth");
   verify(sizeof(money->query_money()) == 1 && money->query_money()[0] == "auditquarter",
          "absent denomination pickup preserves existing pile");
   take("all", "auditquarter");
   verify(!money, "refused requests leave original pile recoverable");
   balance(actor, "auditcrown", 0.75, "refused requests do not change recovered total");
   money = make_pile(where, 10, "audittenth");
   for (int i = 0; i < 9; i++) take("1", "audittenth");
   verify(money && strindex(money->get_base_long(), "1 audittenth") >= 0,
          "repeated pickup retains exact final integer count");
   balance(actor, "auditcredit", 0.9, "repeated decimal denomination credits retain expected wallet value");
   take("1", "audittenth");
   verify(!money && !present("money", where), "final integer-count pickup removes pile without residue");
   balance(actor, "auditcredit", 1.0, "repeated integer-count pickup conserves currency value");
   money = make_pile(where, 2, "auditquarter"); money->merge_money(2, "audithalf");
   "/cmds/verbs/get"->do_get_obj(money);
   verify(!money, "whole-object pickup consumes a multi-denomination pile");
   balance(actor, "auditcrown", 2.25, "whole multi-denomination pickup sums their converted values");
}
private void run_tests()
{
   set_privilege(1);
   verify(COINS == "/std/coins" && MONEY == "/std/money", "default and optional provider macros remain distinct");
   unguarded(1, (: MONEY_D->add_currency("auditcrown", "auditcrowns") :));
   unguarded(1, (: MONEY_D->add_denomination("auditcrown", "auditquarter", "auditquarters", 0.25) :));
   unguarded(1, (: MONEY_D->add_denomination("auditcrown", "audithalf", "audithalves", 0.5) :));
   unguarded(1, (: MONEY_D->add_currency("auditcredit", "auditcredits") :));
   unguarded(1, (: MONEY_D->add_denomination("auditcredit", "audittenth", "audittenths", 0.1) :));
   single_denomination(); shared_currency_denominations(); separate_currencies(); refusals_and_repetition();
   set_this_player(nil);
   dump("LEGACY_MONEY_TOTAL: " + checks + "\n");
   efun::shutdown(0);
}
string *epilog(int ignored) { call_out(run_tests, 0); return ({}); }
