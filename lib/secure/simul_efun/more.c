/* Do not remove the headers from this file! see /USAGE for more info. */

#include <more.h>

object this_body();
varargs void tell(object, string, int);
mixed get_user_variable(string varname);

private
nomask int default_num()
{
   int t = get_user_variable("MORE");
   int num = DEFAULT_SCREEN_HEIGHT;

   if (stringp(t))
      t = to_int(t);
   if (t)
      num = t;
   return num;
}

//: FUNCTION tail
// This version is slightly extended and compatible in spirit, but doesn't
// reproduce the oddities of the original tail() efun.  Note that it also
// returns the string, so write(tail(fname)) is needed for strict
// compatibility.

varargs string tail(string fname, int nlines)
{
   if (!stringp(fname))
      error("tail: No file name supplied.");
   if (nlines < 1)
      nlines = 10;

   mixed input = read_bytes(fname);
   if (input == nil)
      return "";
   bytes data = input;
   int pos = sizeof(data) - 1;
   if (pos >= 0 && data[pos] == '\n')
      pos--;
   for (; pos >= 0; pos--)
   {
      if (data[pos] == '\n' && --nlines == 0)
         return to_text(data[pos + 1..], "UTF-8");
   }
   return to_text(data, "UTF-8");
}

//: FUNCTION more
// more(arg) starts up more to display the array of lines 'arg'.  If arg is
// a string, it is exploded around "\n".  An optional second argument gives
// the number of lines per chunk.  An optional continuation function will
// be evaluated when the "more" is completed.
varargs nomask void more(mixed arg, int num, function continuation, int output_flags)
{
   if (stringp(arg))
      arg = explode(arg, "\n");
   else if (!arrayp(arg))
      return;

   if (!sizeof(arg))
      return;

   if (!num)
      num = default_num();

   if (sizeof(arg) < num)
   {
      foreach (string line in arg)
      {
         tell(this_user(), line + "\n", output_flags);
      }
      if (continuation)
         evaluate(continuation);
      return;
   }

   clone_object (MORE_OB, MORE_LINES, arg, num, continuation, output_flags);
}

//: FUNCTION more_file
// more_file(arg) starts up more to display the single file 'arg' if 'arg'
// is a string, or more than one file if 'arg' is an array of strings.
// An optional second argument gives the number of lines per chunk.  An
// optional continuation function will be evaluated when the "more" is
// completed.
varargs nomask void more_file(mixed arg, int num, function continuation, int output_flags)
{
   if (stringp(arg))
      arg = ({arg});
   else if (!arrayp(arg) || !sizeof(arg))
      return;

   if (!num)
      num = default_num();

   clone_object (MORE_OB, MORE_FILES, arg, num, continuation, output_flags);
}
