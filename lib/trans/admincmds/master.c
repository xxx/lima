/* Do not remove the headers from this file! see /USAGE for more info. */

/*
Added the 'master' command which reloads the master object.
Jezu @ LIMA July 6, 2023
*/

//: ADMINCOMMAND
// USAGE: ``master [<request-id>]``
//
// Queue an in-place master update, or inspect a previous request.
// Updates retain compatible state and do not rerun create().
//
// .. TAGS: RST

#include <mudlib.h>

inherit CMD;

private
void main(string arg)
{
   int id;
   mixed problem;
   mapping status;

   if (!this_user() || !check_privilege(1))
   {
      out("Permission denied.\n");
      return;
   }

   if (!arg || arg == "")
   {
      problem = catch(id = request_object_recompile("master"));
      if (problem)
         outf("Master update request failed: %s\n", problem);
      else
         outf("Master update %d queued. Use 'master %d' to check completion.\n", id, id);
      return;
   }

   problem = catch(id = to_int(arg));
   if (problem || id <= 0 || sprintf("%d", id) != arg)
   {
      out("Usage: master [<request-id>]\n");
      return;
   }

   problem = catch(status = query_object_recompile(id));
   if (problem)
   {
      outf("Cannot inspect master update %d: %s\n", id, problem);
      return;
   }
   if (!status || status["target"] != "master")
   {
      outf("No master update status available for request %d.\n", id);
      return;
   }

   if (status["state"] == "succeeded")
      outf("Master update %d succeeded (%d objects updated).\n", id, status["updated"]);
   else if (status["state"] == "failed")
      outf("Master update %d failed: %s\n", id, status["error"]);
   else
      outf("Master update %d is %s.\n", id, status["state"]);
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
