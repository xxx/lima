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
   dump("ROOM_ERROR: " + issue["diagnostic"] + "\n");
   efun::shutdown(1);
}

private void verify(int ok, string label)
{
   if (!ok) throw("ROOM_FAIL: " + label);
   checks++;
   dump("ROOM_OK: " + label + "\n");
}

private void finish()
{
   set_this_player(nil);
   dump("ROOM_TOTAL: " + checks + "\n");
   efun::shutdown(0);
}

string *epilog(int unused)
{
   call_out(run_tests, 0);
   return ({});
}
