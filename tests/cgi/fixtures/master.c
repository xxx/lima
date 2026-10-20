#include <ports.h>
inherit CLASS_FINGER;
private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }
private void verify_cgi(int passed, string label) {
    if (!passed) throw("CGI_FAIL: " + label);
    checks++;
    dump("CGI_OK: " + label + "\n");
}
private int has(string text, string needle) {
    return stringp(text) && strindex(text, needle) >= 0;
}
private class finger finger_info() {
    return new(class finger, visname: "Alice", level: "wizard", title: "%^RED%^Captain%^RESET%^",
        last_login: 1700000000, idle: 0, plan: "Line one\nLine two\n");
}
private mixed *mud_data(string library, int status, string email) {
    return ({status, "mud.invalid", 7777, 0, 0, "Lima 1", library,
        "lpc-rs", "LP", "Open", email});
}
void create() {
    object docs = load_object("/WWW/cgi/autodoc");
    string index = docs->main();
    verify_cgi(has(index, "<title>Automatic Documentation Interface</title>"), "autodoc omitted query");
    verify_cgi(docs->main(nil) == index, "autodoc nil query");
    verify_cgi(docs->main("") == index, "autodoc empty query");
    verify_cgi(docs->main("=") == index, "autodoc empty query prefix");
    verify_cgi(has(index, "autodoc.c?=command") && has(index, "autodoc.c?=overview"), "autodoc index links");
    string out = docs->main("=command");
    verify_cgi(has(out, "<title>Automatic Command Documentation</title>")
        && has(out, "autodoc.c?=command-look"), "autodoc command list");
    out = docs->main("=command-look");
    verify_cgi(has(out, "Look command<br>See the room.<br>"), "autodoc command body");
    out = docs->main("=functions");
    verify_cgi(has(out, "autodoc.c?=functions-widget"), "autodoc function module list");
    out = docs->main("=functions-widget");
    verify_cgi(has(out, "autodoc.c?=functions-widget-query_note"), "autodoc captured module link");
    out = docs->main("=functions-widget-query_note");
    verify_cgi(has(out, "<H3>string query_note()\n</H3>")
        && has(out, "Returns note.<br>"), "autodoc function body");
    out = docs->main("=hook");
    verify_cgi(has(out, "autodoc.c?=hook-after_move"), "autodoc hook list");
    out = docs->main("=hook-after_move");
    verify_cgi(has(out, "void after_move()") && has(out, "After movement.<br>"), "autodoc hook body");
    out = docs->main("=modules");
    verify_cgi(has(out, "autodoc.c?=modules-m_demo"), "autodoc module list");
    out = docs->main("=modules-m_demo");
    verify_cgi(has(out, "Demo module<br>Provides a demo.<br>"), "autodoc module body");
    verify_cgi(docs->module_summary_line("m_demo.c")
        == "<LI> <A HREF=\"autodoc.c?=functions-m_demo\"> M_DEMO </A>", "autodoc linked uppercase module label");
    verify_cgi(docs->module_summary_line("m_absent.c") == "<LI> M_ABSENT", "autodoc plain uppercase module label");
    verify_cgi(docs->object_summary_line("cgi_probe.c")
        == "<LI> <A HREF=\"autodoc.c?=tree-cgi_probe\"> CGI_PROBE </A>", "autodoc uppercase object label");
    verify_cgi(docs->last_part("/std/cgi_probe.c") == "cgi_probe"
        && docs->last_part("/std/cgi_probe") == "cgi_probe", "autodoc path names");
    out = docs->main("=tree-cgi_probe");
    verify_cgi(has(out, "autodoc.c?=functions-cgi_probe")
        && has(out, "/std/cgi_parent.c") && has(out, "</UL>"), "autodoc recursive inheritance tree");
    out = docs->main("=overview");
    verify_cgi(has(out, "<h3>Base Objects</h3>") && has(out, "<h3>Modules</h3>")
        && has(out, "CGI_PROBE") && has(out, "M_MESSAGES"), "autodoc actual directory overview");
    verify_cgi(nilp(docs->main("unknown")), "autodoc unknown route result");

    object router = load_object(IMUD_D);
    object mud = load_object("/WWW/cgi/mudinfo");
    router->set_fixture(([]));
    verify_cgi(mud->main() == "<h3>No muds out of 0 match your pattern.<h3>", "mudinfo empty router list");
    router->set_fixture((["Beta Realm": mud_data("Lima", -1, "unknown"),
        "AlphaMud": mud_data("Nightmare", 0, "admin@example.invalid"),
        "Café": mud_data("Other", -1, "unknown")]));
    out = mud->main();
    verify_cgi(has(out, "Which of the following muds") && has(out, "Beta+Realm") && has(out, "Café"), "mudinfo omitted query lists choices");
    verify_cgi(strindex(out, "AlphaMud") < strindex(out, "Beta+Realm"), "mudinfo sorted choices");
    verify_cgi(mud->main("") == out && mud->main("=") == out, "mudinfo empty query forms");
    verify_cgi(mud->main("not-a-mud") == "<h3>No muds out of 3 match your pattern.<h3>", "mudinfo no match count");
    out = mud->main("=bETA+realm");
    verify_cgi(has(out, "telnet://mud.invalid:7777>Beta Realm</a>") && has(out, "<TD>Connected</TD>"), "mudinfo case and query-space normalization");
    verify_cgi(has(out, "limamudlib.dev:7881>Lima</a>") && !has(out, "mailto:"), "mudinfo Lima and unknown-email fields");
    out = mud->main("alpha*");
    verify_cgi(has(out, "Nightmare") && has(out, "<TD>Not connected</TD>")
        && has(out, "mailto:admin@example.invalid"), "mudinfo glob and offline fields");
    out = mud->main("CAFÉ");
    verify_cgi(has(out, "Café") && has(out, "Other"), "mudinfo Unicode case matching");
    foreach (string library in ({"Discworld", "Tmi-2", "DGD"})) {
        router->set_fixture((["One": mud_data(library, -1, "unknown")]));
        out = mud->main("one");
        verify_cgi(has(out, library), "mudinfo library branch " + library);
    }
    verify_cgi(sizeof(router->query_mudlist()) == 1, "mudinfo preserves provider mapping");

    object finger = load_object(FINGER_D);
    object who = load_object("/WWW/cgi/who");
    verify_cgi(who->main() == "<TABLE>Fixture users</TABLE>"
        && who->main(nil) == "<TABLE>Fixture users</TABLE>", "who omitted and nil query");
    verify_cgi(who->main("") == "<TABLE>Fixture users</TABLE>"
        && who->main("=") == "<TABLE>Fixture users</TABLE>", "who empty query forms");
    verify_cgi(who->main(([])) == "<TABLE>Fixture users</TABLE>", "who omitted form name");
    out = who->main("nobody");
    verify_cgi(has(out, "nobody</em>: No such player."), "who missing user");
    finger->set_fixture(finger_info());
    out = who->main("= Alice ");
    verify_cgi(finger->query_requested() == "alice" && has(out, "Finger info for Alice@"), "who query normalization");
    verify_cgi(has(out, ">Captain</font>") && !has(out, "%^") && strindex(out, 27) == -1, "who strips title colours");
    verify_cgi(has(out, "(private or none given)") && has(out, "(none)")
        && has(out, "Alice has no mail."), "who absent optional fields");
    verify_cgi(has(out, "Line one<br>\nLine two<br>\n"), "who plan line breaks");
    verify_cgi(who->main((["name":"=ALICE"])) == out, "who form query normalization");
    verify_cgi(who->main((["name":"=ALICE"]), (["host":"localhost"])) == out, "who HTTP form and headers call");
    class finger info = finger_info();
    info.email = "alice@example.invalid";
    info.home_page = "https://example.invalid/alice";
    info.nickname = "Ace";
    info.position = "Builder";
    info.real_name = "Alice Example";
    info.mail_count = 2;
    info.mail_unread = 1;
    info.idle = 3900;
    finger->set_fixture(info);
    out = who->main("alice");
    verify_cgi(has(out, "mailto:alice@example.invalid") && has(out, "Homepage: <a href=\"https://example.invalid/alice\""), "who email and homepage links");
    verify_cgi(has(out, "Ace") && has(out, "Position: Builder") && has(out, "Alice Example"), "who populated optional fields");
    verify_cgi(has(out, "2 messages, 1 of which is unread."), "who unread mail slice assignment");
    verify_cgi(has(out, " (idle 1h)"), "who idle hour formatting");
    info.idle = 120;
    info.mail_count = 1;
    info.mail_unread = 0;
    info.title = nil;
    info.plan = nil;
    info.home_page = nil;
    finger->set_fixture(info);
    out = who->main("alice");
    verify_cgi(has(out, " (idle 2m)") && has(out, "Alice has 1 message."), "who idle minute and singular mail");
    verify_cgi(has(out, ">Alice</font>") && has(out, "No plan.<br>"), "who absent title and plan");
    verify_cgi(has(out, "http://" + __HOST__ + ":" + PORT_HTTP + "/~alice"), "who existing wizard homepage");
    info.idle = -1;
    info.last_login = 0;
    info.mail_count = 2;
    info.mail_unread = 2;
    finger->set_fixture(info);
    out = who->main("alice");
    verify_cgi(has(out, "Left at <unknown>") && !has(out, " (idle"), "who logged-out status");
    verify_cgi(has(out, "2 of which are unread."), "who plural unread mail");
    info.idle = -2;
    info.last_login = 1700000000;
    finger->set_fixture(info);
    out = who->main("alice");
    verify_cgi(has(out, "On since ") && has(out, " (linkdead)"), "who linkdead status");
    verify_cgi(who->stripped("%^ROOM_SHORT%^Text%^RESET%^") == "Text", "who real ANSI translations");
    dump("CGI_TOTAL: " + checks + "\n");
}
