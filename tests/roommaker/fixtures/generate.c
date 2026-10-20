#include "/port_tests/roommaker/master.h"
#include <edit.h>

private object user;

private string send_line(string input)
{
   user->take_output();
   user->feed(input);
   return user->take_output();
}

private string save_room(string name)
{
   send_line("f");
   send_line("n");
   send_line(name);
   string result = send_line("f");
   send_line("m");
   return result;
}

private void run_tests()
{
   user = clone_object("/port_tests/roommaker/user");
   set_this_player(user);
   user->run_command();
   object menu = user->menu_owner();
   verify(objectp(menu) && base_name(menu) == "/contrib/roommaker/buildmenu", "command opens its bundled menu");
   verify(user->modal_stack_size() == 1 && strindex(user->take_output(), "Build a Room") >= 0,
          "top-level menu renders with one modal frame");
   verify(strindex(send_line("r"), "Edit descriptions") >= 0, "room submenu renders");
   send_line("f");
   send_line("p");
   verify(user->modal_stack_size() == 2, "file-path action opens a prompt");
   send_line("/generated");
   verify(user->modal_stack_size() == 1, "file-path prompt returns to menu");
   send_line("m");
   verify(strindex(save_room("default.c"), "File saved to /generated/default.c") >= 0,
          "file-menu action saves default room");
   verify(strindex(read_file("/generated/default.c"), "inherit INDOOR_ROOM;") >= 0,
          "default template selects indoor provider");
   send_line("d");
   send_line("s");
   verify(user->modal_stack_size() == 2, "short-description action opens prompt");
   send_line("A \"quoted\" room with \\ paths");
   verify(user->modal_stack_size() == 1, "short-description prompt returns to menu");
   send_line("h");
   verify(strindex(send_line("3"), "New light level is 3") >= 0, "light prompt updates light");
   send_line("a"); send_line("mudlib.h");
   send_line("a"); send_line("mudlib.h");
   verify(strindex(send_line("v"), "<mudlib.h>") >= 0, "include prompt and description summary work");
   send_line("l");
   verify(user->modal_stack_size() == 2, "long-description action opens actual text editor");
   send_line("A \"quoted\" line with \\ paths"); send_line("second line"); send_line(".");
   verify(user->modal_stack_size() == 1, "long editor completion returns to menu");
   send_line("l"); send_line("~q");
   verify(user->modal_stack_size() == 1, "long editor abort returns to menu");
   send_line("m");
   verify(strindex(save_room("indoor.c"), "File saved") >= 0, "edited indoor room saves");
   send_line("d"); send_line("w"); send_line("2"); send_line("m");
   verify(strindex(save_room("outdoor.c"), "File saved") >= 0, "weather prompt saves outdoor room");
   verify(strindex(read_file("/generated/outdoor.c"), "set_weather(2)") >= 0,
          "outdoor source enables selected weather flag");
   send_line("d"); send_line("w"); send_line("0"); send_line("m");
   verify(strindex(save_room("reset.c"), "File saved") >= 0, "weather reset saves indoor room");
   send_line("i"); send_line("a"); send_line("a \"quoted\" sign");
   verify(user->modal_stack_size() == 2, "new-item key opens actual editor");
   send_line("Letters \"and\" \\ slashes"); send_line(".");
   verify(user->modal_stack_size() == 1, "bound item callback returns to item menu");
   verify(strindex(send_line("v"), "Letters \"and\" \\ slashes") >= 0, "item view displays raw text");
   send_line("e"); send_line("a \"quoted\" sign"); send_line(".");
   verify(user->modal_stack_size() == 1, "existing-item completion preserves editor contents");
   send_line("e"); send_line("a \"quoted\" sign"); send_line("~q");
   verify(user->modal_stack_size() == 1, "existing-item abort returns to menu");
   send_line("m"); send_line("x"); send_line("a"); send_line("north");
   verify(user->modal_stack_size() == 2, "new-exit direction opens actual editor");
   send_line("/domains/std/room/Example_Room"); send_line(".");
   verify(user->modal_stack_size() == 1, "bound exit callback returns to exit menu");
   verify(strindex(send_line("v"), "north : /domains/std/room/Example_Room") >= 0,
          "exit view displays exact destination without a leading blank line");
   send_line("e"); send_line("north"); send_line(".");
   verify(user->modal_stack_size() == 1, "existing-exit completion preserves destination");
   send_line("e"); send_line("north"); send_line("~q");
   verify(user->modal_stack_size() == 1, "existing-exit abort returns to menu");
   send_line("a"); send_line("west"); send_line("/domains/std/room/Example_Room"); send_line(".");
   send_line("r"); send_line("west");
   verify(strindex(send_line("v"), "west :") < 0, "exit deletion removes selected direction");
   send_line("m"); send_line("o"); send_line("a"); send_line("/std/object");
   verify(user->modal_stack_size() == 2, "new object opens quantity prompt");
   send_line("2");
   verify(user->modal_stack_size() == 1, "bound quantity callback returns to object menu");
   send_line("e"); send_line("/std/object"); send_line("3");
   verify(strindex(send_line("v"), "/std/object : 3") >= 0, "object quantity edit updates the view");
   send_line("e"); send_line("/std/object"); send_line("2");
   send_line("a"); send_line("/std/container"); send_line("1");
   send_line("r"); send_line("/std/container");
   verify(strindex(send_line("v"), "/std/container :") < 0, "object deletion removes selected path");
   send_line("m");
   verify(strindex(send_line("v"), "a \"quoted\" sign") >= 0, "room view includes item summary");
   verify(strindex(save_room("composed.c"), "File saved") >= 0, "room with item exit and objects saves");
   string before = read_file("/generated/composed.c");
   send_line("d"); send_line("h"); send_line("4"); send_line("m");
   save_room("composed.c");
   verify(strindex(read_file("/generated/composed.c"), "set_light(4)") >= 0
          && read_file("/generated/composed.c") != before, "saving existing target overwrites source");
   before = read_file("/generated/composed.c");
   verify(strindex(save_room("absent/failure.c"), "File not saved.") >= 0,
          "failed write reports failure through menu");
   verify(read_file("/generated/composed.c") == before && file_size("/generated/absent/failure.c") == -1,
          "failed write leaves existing output intact");
   verify(user->modal_stack_size() == 1, "failed write keeps menu usable");
   object other = clone_object("/port_tests/roommaker/user", "otheraudit");
   set_this_player(other); other->run_command();
   object other_menu = other->menu_owner();
   verify(other_menu != menu && other->modal_stack_size() == 1, "second user gets an independent menu clone");
   object first = user;
   user = other;
   send_line("r"); send_line("f"); send_line("p"); send_line("/generated"); send_line("m");
   save_room("other.c");
   verify(strindex(read_file("/generated/other.c"), "RoomMaker Room") >= 0,
          "second menu retains independent default descriptions");
   send_line("q");
   verify(!objectp(other_menu) && !other->modal_stack_size() && objectp(menu), "second user can quit without removing first menu");
   user = first; set_this_player(first);
   verify(strindex(send_line("v"), "A \"quoted\" room") >= 0, "first menu retains its own edited state");
   send_line("q");
   verify(!objectp(menu) && !first->modal_stack_size(), "quit removes first menu and modal frame");
   verify(!sizeof(filter(objects(), (: clonep($1) && base_name($1) == EDIT_OB :))), "all text-editor clones retire");
   verify(!sizeof(filter(objects(), (: clonep($1) && base_name($1) == "/contrib/roommaker/buildmenu" :))),
          "all build-menu clones retire");
   efun::destruct(first); efun::destruct(other);
   finish();
}
