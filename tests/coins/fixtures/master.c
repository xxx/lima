inherit M_ACCESS;
private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_shutdown(object caller, string program) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }
void error_handler(mapping issue) { dump("COINS_ERROR: " + issue["diagnostic"] + "\n"); efun::shutdown(1); }
private void verify(int ok, string label)
{
   if (!ok) throw("COINS_FAIL: " + label);
   checks++; dump("COINS_OK: " + label + "\n");
}
private void balance(object actor, string currency, float expected, string label)
{
   verify(abs(actor->query_amt_currency(currency) - expected) < 0.000000000001, label);
}
private object make_actor(object room, float crowns = 2.0, float credits = 0.0)
{
   object actor = clone_object("/port_tests/coins/actor");
   set_this_player(actor);
   actor->add_money("auditcrown", crowns);
   actor->add_money("auditcredit", credits);
   actor->move(room);
   return actor;
}
private object room() { return clone_object(INDOOR_ROOM); }
private void drop(string amount, string type) { "/cmds/verbs/drop"->do_drop_wrd_str(amount, type); }
private void take(string amount, string type) { "/cmds/verbs/get"->do_get_wrd_str(amount, type); }
private object pile(object where) { return present("money", where); }

private void creation_and_pickup()
{
   object where = room(); object actor = make_actor(where);
   drop("1", "auditquarters"); object coins = pile(where);
   verify(coins && base_name(coins) == COINS && sizeof(coins->query_coins()) == 1 &&
          coins->query_coins()[0] == "auditcrown", "denomination drop creates canonical base-currency pile");
   balance(actor, "auditcrown", 1.75, "denomination drop debits its converted value once");
   verify(strindex(coins->get_base_long(), "1 auditquarter") >= 0, "fractional dropped pile renders through real money daemon");
   "/cmds/verbs/get"->do_get_obj(coins);
   verify(!coins && !pile(where), "ordinary whole-object pickup consumes dropped pile");
   balance(actor, "auditcrown", 2.0, "drop and whole-object pickup conserve initial balance");

   where = room(); actor = make_actor(where);
   coins = clone_object(COINS, 0.5, "auditcrown"); coins->move(where);
   drop("2", "auditquarter");
   verify(pile(where) == coins && sizeof(coins->query_coins()) == 1, "same-currency drop merges into existing pile");
   balance(actor, "auditcrown", 1.5, "same-currency merge debits denomination value");
   verify(strindex(coins->get_base_long(), "1 auditcrown") >= 0, "merged value renders as one base unit");
   take("1", "auditquarters");
   balance(actor, "auditcrown", 1.75, "plural denomination pickup credits one fractional unit");
   verify(strindex(coins->get_base_long(), "3 auditquarters") >= 0, "partial pickup leaves correct denomination value");
   take("all", "auditquarter");
   verify(!coins && !pile(where), "all-denomination pickup consumes remaining currency balance");
   balance(actor, "auditcrown", 2.5, "existing-pile round trip conserves wallet plus seeded pile");
}

private void multiple_currencies()
{
   object where = room(); object actor = make_actor(where, 3.0, 2.0);
   drop("1", "auditquarter"); object coins = pile(where);
   drop("3", "audittenth");
   verify(pile(where) == coins && sizeof(coins->query_coins()) == 2 &&
          member_array("auditcrown", coins->query_coins()) >= 0 && member_array("auditcredit", coins->query_coins()) >= 0,
          "new currency merges with explicit missing-balance initialization");
   balance(actor, "auditcrown", 2.75, "mixed pile debits crown denomination correctly");
   balance(actor, "auditcredit", 1.7, "mixed pile debits credit denomination correctly");
   string description = coins->get_base_long();
   verify(strindex(description, "auditquarter") >= 0 && strindex(description, "audittenths") >= 0,
          "multiple-currency pile renders both fractional denominations");
   actor->take_output(); take("all", "coins");
   verify(pile(where) == coins && strindex(actor->take_output(), "Which type") >= 0,
          "generic all pickup requires a currency for a mixed pile");
   balance(actor, "auditcrown", 2.75, "ambiguous pickup preserves crown wallet");
   balance(actor, "auditcredit", 1.7, "ambiguous pickup preserves credit wallet");
   take("2", "audittenths");
   balance(actor, "auditcredit", 1.9, "partial pickup translates second currency denomination");
   balance(actor, "auditcrown", 2.75, "second-currency pickup leaves first currency unchanged");
   take("all", "auditcrown");
   verify(coins && sizeof(coins->query_coins()) == 1 && coins->query_coins()[0] == "auditcredit",
          "all pickup removes only selected currency from mixed pile");
   balance(actor, "auditcrown", 3.0, "first currency round trip conserves value");
   take("all", "coins");
   verify(!coins && !pile(where), "generic all pickup consumes a single remaining currency");
   balance(actor, "auditcredit", 2.0, "second currency round trip conserves value");
}

private void fractional_all_and_repeated_units()
{
   object where = room(); object actor = make_actor(where, 1.25);
   drop("all", "auditcrown"); object coins = pile(where);
   balance(actor, "auditcrown", 0.0, "all drop consumes full fractional wallet balance");
   verify(strindex(coins->get_base_long(), "auditquarter") >= 0, "all drop retains fractional remainder in pile");
   take("1", "auditquarter");
   balance(actor, "auditcrown", 0.25, "fractional pickup from all-drop pile has correct units");
   drop("all", "auditquarters");
   balance(actor, "auditcrown", 0.0, "all-denomination drop consumes fractional wallet balance");
   take("all", "auditquarters");
   verify(!coins, "all-denomination round trip consumes complete pile");
   balance(actor, "auditcrown", 1.25, "all drop and pickup preserve original fractional total");

   where = room(); actor = make_actor(where, 0.0, 1.0);
   for (int i = 0; i < 10; i++) drop("1", "audittenth");
   coins = pile(where);
   balance(actor, "auditcredit", 0.0, "repeated decimal drops exhaust wallet without overdraw");
   verify(coins && sizeof(coins->query_coins()) == 1, "repeated decimal drops use one pile and currency");
   for (int i = 0; i < 9; i++) take("1", "audittenth");
   balance(actor, "auditcredit", 0.9, "repeated decimal pickups retain expected wallet value");
   take("1", "audittenth");
   verify(!coins && !pile(where), "final decimal pickup removes rounding-only pile residue");
   balance(actor, "auditcredit", 1.0, "repeated decimal round trip conserves total value");
}

private void refusals_and_base_units()
{
   object where = room(); object actor = make_actor(where, 1.0);
   drop("5", "auditquarter"); drop("0", "auditcrown"); drop("-1", "auditcrown");
   verify(!pile(where), "insufficient zero and negative drops create no pile");
   balance(actor, "auditcrown", 1.0, "refused drops preserve wallet balance");
   drop("all", "auditcredit");
   verify(!pile(where), "all drop from empty currency creates no pile");
   object coins = clone_object(COINS, 0.5, "auditcrown"); coins->move(where);
   take("3", "auditquarter"); take("0", "auditquarter"); take("-1", "auditquarter");
   balance(actor, "auditcrown", 1.0, "insufficient zero and negative pickups preserve wallet");
   verify(coins && strindex(coins->get_base_long(), "2 auditquarters") >= 0,
          "refused pickups preserve complete pile value");
   take("1", "auditcredit");
   verify(coins && sizeof(coins->query_coins()) == 1, "absent currency pickup preserves existing pile");
   take("all", "coin");
   balance(actor, "auditcrown", 1.5, "refused operations leave seeded total recoverable");

   where = room(); actor = make_actor(where, 1.0);
   drop("1", "auditcrown"); coins = pile(where);
   balance(actor, "auditcrown", 0.0, "ordinary named base-currency drop remains correct");
   take("1", "auditcrown");
   verify(!coins, "ordinary named base-currency pickup consumes pile");
   balance(actor, "auditcrown", 1.0, "ordinary base-currency round trip conserves value");
   coins = clone_object(COINS, 1.5, "auditcrown"); coins->move(where);
   take("1", "coins");
   balance(actor, "auditcrown", 2.0, "generic numeric coin pickup retains base-unit meaning");
   take("all", "coins");
   balance(actor, "auditcrown", 2.5, "generic all pickup includes fractional base remainder");
   coins = clone_object(COINS, 1.125, "auditcrown"); coins->move(where);
   "/cmds/verbs/get"->do_get_obj(coins);
   verify(!coins, "float-valued constructor pile supports ordinary whole pickup");
   balance(actor, "auditcrown", 3.625, "whole pickup preserves float constructor values without truncation");
}

private void failed_destination_and_merge()
{
   object where = clone_object("/port_tests/coins/refusing_room");
   object actor = make_actor(where, 1.0);
   int clones = sizeof(object_clones(load_object(COINS)));
   drop("1", "auditquarter");
   balance(actor, "auditcrown", 1.0, "refused placement leaves wallet unchanged");
   verify(!pile(where) && sizeof(object_clones(load_object(COINS))) == clones,
          "refused placement removes the unplaced new pile");

   where = room(); actor = make_actor(where, 1.0);
   object coins = clone_object("/port_tests/coins/refusing_pile", 0.5, "auditcrown"); coins->move(where);
   mixed problem = catch(drop("1", "auditquarter"));
   verify(stringp(problem) && strindex(problem, "Fixture merge refusal") >= 0,
          "merge exception reaches ordinary caller without false success");
   balance(actor, "auditcrown", 1.0, "caught merge refusal occurs before wallet debit");
   "/cmds/verbs/get"->do_get_obj(coins);
   balance(actor, "auditcrown", 1.5, "merge refusal preserves wallet plus existing pile total");
}

private void run_tests()
{
   set_privilege(1);
   unguarded(1, (: MONEY_D->add_currency("auditcrown", "auditcrowns") :));
   unguarded(1, (: MONEY_D->add_denomination("auditcrown", "auditquarter", "auditquarters", 0.25) :));
   unguarded(1, (: MONEY_D->add_currency("auditcredit", "auditcredits") :));
   unguarded(1, (: MONEY_D->add_denomination("auditcredit", "audittenth", "audittenths", 0.1) :));
   creation_and_pickup();
   multiple_currencies();
   fractional_all_and_repeated_units();
   refusals_and_base_units();
   failed_destination_and_merge();
   set_this_player(nil);
   dump("COINS_TOTAL: " + checks + "\n");
   efun::shutdown(0);
}
string *epilog(int ignored) { call_out(run_tests, 0); return ({}); }
