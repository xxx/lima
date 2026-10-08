/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** ed_session.c
**
** Represents an editing session.  This is used in conjunction with the
** input system to allow nested editing/more/help/etc.
**
** 23-Jan-95. Deathblade. Created.
*/

#include <clean_up.h>
#include <mudlib.h>
#include <playerflags.h>

inherit M_INPUT;
inherit M_ACCESS;
inherit M_ED;

private nosave function end_func;
private nosave function abort_func;
private nosave function input_handler;
private nosave object user;
private nosave object body;
private nosave int owns_editing_flag = 0;
private nosave int session_state = 0;

private void finish_editing(int completed)
{
   mixed problem, first_problem;
   function callback;
   function handler;
   if (session_state == 2)
      return;
   session_state = 2;
   callback = completed ? end_func : abort_func;
   handler = input_handler;
   end_func = abort_func = input_handler = nil;

   // Parent and completion callbacks may start another editor.
   if (owns_editing_flag && body)
      first_problem = catch(body->clear_flag(F_IN_EDIT));
   owns_editing_flag = 0;
   if (handler)
   {
      problem = catch(modal_remove(handler));
      if (!first_problem)
         first_problem = problem;
   }
   body = user = nil;
   if (callback)
   {
      problem = catch(evaluate(callback));
      if (!first_problem)
         first_problem = problem;
   }
   destruct();
   if (first_problem)
      throw(first_problem);
}

private nomask void receive_ed_input(mixed input)
{
   mixed problem;
   if (input == -1)
   {
      finish_editing(0);
      return;
   }
   if (session_state != 1)
      error("Editor session is not active");
   problem = catch(tell(user, ed_cmd(input), NO_ANSI));
   if (problem)
   {
      catch(finish_editing(0));
      throw(problem);
   }
   if (query_ed_mode() == -1)
      finish_editing(1);
}

private nomask string query_prompt()
{
   return query_ed_mode() == 0 ? ":" : "";
}

// Cancellation releases resources without importing an unfinished edit.
varargs nomask void begin_editing(string fname, int restricted, function finished, function cancelled)
{
   mixed problem;
   if (session_state)
      error("Editor session is already assigned");
   session_state = 1;
   user = this_user();
   body = this_body();
   end_func = finished;
   abort_func = cancelled;
   problem = catch {
      if (!user)
         error("Editor session requires a user");
      ed_start(fname, restricted);
      input_handler = receive_ed_input;
      modal_push(input_handler, query_prompt);
      if (body && !body->test_flag(F_IN_EDIT))
      {
         owns_editing_flag = 1;
         body->set_flag(F_IN_EDIT);
      }
      if (!restricted)
         printf("Editing: /%s", ed_cmd("f"));
      if (!is_file(fname))
         printf("[New file]\n");
   };
   if (problem)
   {
      catch(finish_editing(0));
      throw(problem);
   }
}

void set_ed_setup(int code)
{
   if (!user)
      error("Editor session is not active");
   user->set_ed_setup(code);
}

int query_ed_setup()
{
   return user ? user->query_ed_setup() : 0;
}

private void create()
{
   set_privilege(1);
}

varargs void remove(int reload)
{
   finish_editing(0);
}

private int clean_up()
{
   if (session_state != 1 || !user || query_ed_mode() == -1)
      catch(finish_editing(0));
   return ASK_AGAIN;
}
