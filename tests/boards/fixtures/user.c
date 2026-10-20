inherit "/secure/user/newsdata";
inherit "/secure/user/mailbase";

private int saves;
void save_me() { saves++; }
int query_saves() { return saves; }

private mixed *stack = ({});
private string output = "";
private mapping flags = ([]);
private string userid;

void create(string name = "boardaudit") { userid = name; }
object query_body() { return this_object(); }
object query_shell_ob() { return this_object(); }
string query_userid() { return userid; }
string query_name() { return capitalize(userid); }
int query_screen_width() { return 80; }
mixed get_variable(string key) { return key == "MORE" ? 100 : nil; }
int test_flag(int flag) { return flags[flag] || 0; }
void set_flag(int flag) { flags[flag] = 1; }
void clear_flag(int flag) { m_delete(flags, flag); }
varargs void receive_private_msg(string text, int kind) { output += text; }
string take_output() { string old = output; output = ""; return old; }

varargs void modal_push(function callback, mixed prompt, int secure, function back)
{
   stack += ({({callback, prompt, 0})});
}

varargs void modal_simple(function callback, mixed prompt, int secure)
{
   stack += ({({callback, prompt, 1})});
}

void modal_pop() { stack = sizeof(stack) > 1 ? stack[0..<2] : ({}); }
int modal_stack_size() { return sizeof(stack); }

void feed(string input)
{
   if (!sizeof(stack)) throw("BOARD_FAIL: no input handler");
   mixed *frame = stack[<1];
   if (frame[2]) modal_pop();
   evaluate(frame[0], input);
}

varargs void simple_action(string text, mixed *args...) { output += text; }
