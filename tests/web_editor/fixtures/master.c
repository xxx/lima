inherit M_ACCESS;

private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_shutdown(object caller, string program) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }

void error_handler(mapping issue)
{
   dump("WEBED_ERROR: " + issue["diagnostic"] + "\n");
   efun::shutdown(1);
}

private void verify(int ok, string label)
{
   if (!ok) throw("WEBED_FAIL: " + label);
   checks++;
   dump("WEBED_OK: " + label + "\n");
}

private mapping form(string action, string filename)
{
   return (["user":"webaudit", "password":"editor-test-pass", action:1,
            "loadfile":filename, "saveas":filename, "contents":""]);
}

private string page(mapping request)
{
   return "/secure/cgi/webed"->main(request, nil);
}

private void run_tests()
{
   set_privilege(1);
   if (!wizardp("webaudit")) unguarded(1, (: SECURE_D->create_wizard("webaudit") :));
   verify(wizardp("webaudit"), "synthetic wizard uses current role helper");
   save_map((["password":crypt("editor-test-pass", 0), "bodies":([])]), "/data/links/w/webaudit");
   write_file("/wiz/webaudit/full.txt", "line <one> & \"quote\"\nsecond\n", 1);
   write_file("/wiz/webaudit/empty.txt", "", 1);
   verify(strindex(page(nil), "TEXTAREA") < 0, "initial CGI preserves blank-login presentation");
   mapping request = form("load", "/wiz/webaudit/full.txt");
   string loaded = page(request);
   verify(strindex(loaded, "TEXTAREA") >= 0 && strindex(loaded, "line &lt;one&gt; &amp; &quot;quote&quot;") >= 0,
          "existing text loads through CGI and current account lookup");
   verify(request["saveas"] == "/wiz/webaudit/full.txt", "load preserves canonical save target");
   loaded = page(form("load", "/wiz/webaudit/empty.txt"));
   verify(strindex(loaded, "TEXTAREA") >= 0 && strindex(loaded, "Permission to read denied") < 0,
          "empty existing file retains editable presentation");
   request = form("load", "/wiz/webaudit/new.txt");
   loaded = page(request);
   verify(strindex(loaded, "New file") >= 0 && strindex(loaded, "TEXTAREA") >= 0,
          "new file receives empty editor");
   verify(request["saveas"] == "/wiz/webaudit/new.txt" && request["contents"] == ""
          && file_size("/wiz/webaudit/new.txt") == -1, "new-file load stages no file creation");
   verify(strindex(page(form("load", "/wiz/webaudit")), "Is a DIRECTORY") >= 0,
          "directory load preserves existing message");
   write_file("/wiz/webaudit/full.txt", "freshly reloaded\n", 1);
   request = form("reload", "/wiz/webaudit/full.txt");
   request["contents"] = "stale form text";
   verify(strindex(page(request), "freshly reloaded") >= 0, "reload reads current file contents");
   request = form("loadinh", "/wiz/webaudit/full.txt"); request["inh"] = "/wiz/webaudit/full.txt";
   verify(strindex(page(request), "freshly reloaded") >= 0, "retained inherited-file action delegates to load");
   request = form("save", "/wiz/webaudit/saved.txt"); request["contents"] = "new\r\ntext\r\n";
   verify(strindex(page(request), "TEXTAREA") >= 0, "normal save returns editor");
   verify(read_file("/wiz/webaudit/saved.txt") == "new\ntext\n", "save preserves CRLF normalization");
   request["contents"] = "replacement\n"; page(request);
   verify(read_file("/wiz/webaudit/saved.txt") == "replacement\n", "repeated save overwrites contents");
   request = form("save", "/wiz/webaudit/empty-saved.txt");
   verify(strindex(page(request), "TEXTAREA") >= 0 && file_size("/wiz/webaudit/empty-saved.txt") == 0,
          "empty save creates an editable empty file");
   request = form("save", "/wiz/webaudit/absent/file.txt"); request["contents"] = "unsaved draft";
   loaded = page(request);
   verify(strindex(loaded, "COULD NOT SAVE FILE") >= 0 && strindex(loaded, "unsaved draft") >= 0,
          "native write error returns existing message and draft");
   verify(file_size("/wiz/webaudit/absent/file.txt") == -1
          && read_file("/wiz/webaudit/saved.txt") == "replacement\n", "failed save leaves earlier file intact");
   verify(strindex(page(form("reload", "/wiz/webaudit/saved.txt")), "replacement") >= 0,
          "editor remains usable after failed save");
   request = form("save", "/wiz/webaudit/refused.txt"); request["contents"] = "draft"; request["password"] = nil;
   verify(strindex(page(request), "Login info is incorrect") >= 0 && file_size("/wiz/webaudit/refused.txt") == -1,
          "missing login fields retain normal refusal");
   request["password"] = "wrong-fixture-password";
   verify(strindex(page(request), "Login info is incorrect") >= 0 && file_size("/wiz/webaudit/refused.txt") == -1,
          "incorrect fixture password retains normal refusal");
   write_file("/wiz/webaudit/unicode.txt", "café <window> & λ\n", 1);
   loaded = page(form("load", "/wiz/webaudit/unicode.txt"));
   verify(strindex(loaded, "café &lt;window&gt; &amp; λ") >= 0, "UTF-8 editor text preserves existing entity encoding");
   dump("WEBED_TOTAL: " + checks + "\n");
   efun::shutdown(0);
}

string *epilog(int ignored)
{
   call_out(run_tests, 0);
   return ({});
}
