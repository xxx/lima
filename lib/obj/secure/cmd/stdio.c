/* Do not remove the headers from this file! see /USAGE for more info. */

#include <more.h>

private
nosave string stdoutstr = nil;
private
nosave string buf;
private
nosave mixed next_cmd;
private
nosave mixed next_implode_info;
private
nosave int no_redirect = 0;
private
nosave int output_flags = 0;

protected
int set_output_flags(int x)
{
   output_flags = x;
}

protected
void no_redirection()
{
   no_redirect = 1;
}

protected
int restrict_redirection()
{
   return no_redirect;
}

mixed get_rest_of_args()
{
   mixed tmp = ({next_cmd, next_implode_info});

   next_cmd = nil;
   next_implode_info = nil;
   return tmp;
}

string get_output()
{
   return buf;
}

protected
int end_of_pipeline()
{
   return !next_cmd;
}

private
int determine_stdout(mixed args)
{
   string op = args[0];
   string stdoutfile;
   int clobber = 0;

   switch (op)
   {
   case ">":
      clobber = 1;
   case ">>":
      stdoutfile = evaluate_path(args[1]);
      if (!is_directory(base_path(stdoutfile)))
      {
         printf("%s is not a valid directory.\n", base_path(stdoutfile));
         return 0;
      }
      if (clobber)
      {
         if (is_file(stdoutfile) && (!rm(stdoutfile)))
         {
            printf("%s: couldn't remove.\n", stdoutfile);
            return 0;
         }
      }
      stdoutstr = stdoutfile;
      break;
   case "|":
      next_cmd = args[1..];
      break;
   default:
      error("Should never get here...");
   }
   return 1;
}

protected
void bare_init()
{
   stdoutstr = nil;
   buf = "";
   next_cmd = nil;
   output_flags = 0;
   next_implode_info = nil;
}

protected
mixed hello_stdio(mixed ininfo, mixed argstohold, mixed implode_info)
{
   string stdinfile;
   string stdin;
   bare_init();
   next_implode_info = implode_info;
   if (argstohold)
   {
      if (!arrayp(argstohold) || sizeof(argstohold) < 2 ||
          (argstohold[0] != "|" && argstohold[0] != ">" && argstohold[0] != ">>") ||
          (argstohold[0] != "|" && (sizeof(argstohold) != 2 || !stringp(argstohold[1]) || !strlen(argstohold[1]))))
      {
         write("syntax error: invalid output route.\n");
         return 0;
      }
   }
   if (arrayp(ininfo) && (sizeof(ininfo) != 1 || !stringp(ininfo[0]) || !strlen(ininfo[0])))
   {
      write("syntax error: invalid input route.\n");
      return 0;
   }
   /* First, if there's ininfo, read our std input */
   if (ininfo)
   {
      if (stringp(ininfo))
      {
         // We got stdin from a pipe
         stdin = ininfo;
      }
      else
      {
         stdinfile = evaluate_path(ininfo[0]);
         if (!is_file(stdinfile))
         {
            printf("%s is not a file.\n", stdinfile);
            return 0;
         }
         stdin = read_file(stdinfile);
      }
   }
   if (argstohold)
   {
      if (!determine_stdout(argstohold))
         return 0;
   }
   return stdin || 1;
}

protected
mixed done_outputing()
{
   object stdoutobj;
   string tmp;

   if ((!stdoutstr) && (!next_cmd))
   {
      if (buf == "" || !buf)
         return 0;
      string *lines = explode(buf, "\n");
      if (buf[<1] == '\n')
         lines = lines[0..<2];
      more(lines, 0, nil, output_flags);
      return 0;
   }
   if (stdoutstr)
   {
      if (!write_file(stdoutstr, buf))
      {
         printf("error: couldn't write to %s.\n", stdoutstr);
      }
      return 0;
   }
   tmp = buf;
   buf = nil;
   return ({next_cmd, tmp, next_implode_info});
}

protected
void out(string s)
{
   if (!buf)
      buf = "";
   buf += s;
}

protected
void outf(string format, mixed args...)
{
   buf += sprintf(format, args...);
}
