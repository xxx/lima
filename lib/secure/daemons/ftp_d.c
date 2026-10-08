/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** First draft / quickly hacked FTPD.
**
** Rust (rust@lima.mudlib.org) July 12, 1996
**
** There are commands this doesn't support.  If your client
** Seems to want some other command to work for some weird reason,
** and this FTPD won't do it, let me know and I'll add it in.
**
** Myth@Icon of Sin - Jan 19, 1997
**  o Fixed STOR:
**    o ascii: removed \r's
**    o binary: added a fileposition flag
**  o Added the SYST command.
**  o Added line checking to the read callback.
**  o Fixed send so that larger files can be handled without difficulty.
** Jan 21, 1997
**  o Fixed the mkd command.
**
** Tigran Sept 16, 1997
**  o Fixed nlst and list (which are now identicle) so that they accept
**    flags.  This ftp server should now be able to be used by most
**    GUI based clients, and should also work w/ ange-ftp and efs (from
**    emacs and Xemacs.  Note there is still work that can be done here.
**    Acceptable flags are -a -l -C and -F
**
** Naebator (sgt@israel.ru) Jan 9, 1998
**  o Fixed LIST and NLST:
**    o Added -1 flag
**    o unless specified otherwize, forces -l flag on LIST and -1 flag on NLST.
**    o directories report size 0.
**  o If home directory does not exist, it defaults to "/".
** Tigran Jan 15, 1999
**  o Added passive mode connections.
** Loriel July 2001
**  o Tidied up passive mode to close old passive pipes etc.
*/

#include <assert.h>
#include <clean_up.h>
#include <log.h>
#include <ports.h>
#include <socket.h>

inherit M_ACCESS;

#include <ftp_d.h>

#define LTYPE_LIST 0
#define LTYPE_NLST 1

#define RESTRICT_PORTS
#define MIN_PORT 40000
#define MAX_PORT 40100
#define MAX_TRIES 10

private
int lastport = MIN_PORT;
private
nosave int idlecall;
private
string iphost;
private
nosave mapping passives = ([]);
private
nosave mapping sessions = ([]);
private
nosave mapping dataports = ([]);
private
nosave mapping dispatch = (["user":FTP_CMD_user,
                            "pass":FTP_CMD_pass,
                            "retr":FTP_CMD_retr,
                            "stor":FTP_CMD_stor,
                            "nlst":FTP_CMD_nlst,
                            "list":FTP_CMD_list,
                             "pwd":FTP_CMD_pwd,
                            "cdup":FTP_CMD_cdup,
                             "cwd":FTP_CMD_cwd,
                            "quit":FTP_CMD_quit,
                            "type":FTP_CMD_type,
                             "mkd":FTP_CMD_mkd,
                            "port":FTP_CMD_port,
                            "noop":FTP_CMD_noop,
                            "dele":FTP_CMD_dele,
                            "syst":FTP_CMD_syst,
                            "pasv":FTP_CMD_pasv,
]);
private
object sock;

private
mixed outfile = ([]);

#ifdef ALLOW_ANON_FTP
private
nosave string *anon_logins = ({"anonymous", "ftp"});
#endif

int query_lastport()
{
   return lastport;
}

varargs private string find_flags(string incoming)
{
   string *parts;
   string *flags = ({});
   if (!incoming)
      return nil;
   parts = explode(incoming, " ");
   parts = filter(parts, ( : $1[0] == '-' :));
   foreach (string part in parts)
      flags += explode(part, "");
   flags = clean_array(flags);
   return implode(flags -= ({"-"}), "");
}

varargs private string strip_flags(string incoming)
{
   string *parts;
   if (!incoming)
      return nil;
   parts = explode(incoming, " ");
   parts = filter(parts, ( : $1[0] != '-' :));
   return implode(parts, " ");
}

private
void FTPLOGF(string format, mixed *args...)
{
   FTPLOG(sprintf(format, args...));
}

private
string FTP_get_welcome_msg()
{
   return sprintf("220- %s FTP server ready.\n%s"
                  "220 Anonymous and wizard logins accepted.\n",
                  mud_name(),
                  is_file(FTP_WELCOME) ? "220- " + replace_string(read_file(FTP_WELCOME), "\n", "\n220- ") + "\n" : "");
}

private
void FTP_handle_idlers()
{
   m_delete(sessions, 0); /* Just make sure there's no stale stuff here */
   if (!sizeof(sessions))
      return;
   foreach (object socket, class ftp_session info in sessions)
   {
      if (info.dataPipe) /* Data connections are still active. */
      {
         info.idleTime = 0;
         continue;
      }
      info.idleTime += 60;
      if (info.idleTime >= MAX_IDLE_TIME + 60)
      {
         FTPLOGF("%s idled out at %s.\n", capitalize(info.user), ctime(time()));
         socket->send(sprintf("421 Timeout. (%i seconds): closing control connection.\n", MAX_IDLE_TIME));
         m_delete(sessions, socket);
         FTP_close_socket(socket);
      }
   }
   if (sizeof(keys(sessions)))
      idlecall = call_out(FTP_handle_idlers, 60);
}

private
void FTP_addSession(object socket)
{
   class ftp_session newSession;

   newSession = new (class ftp_session);
   newSession.cmdPipe = socket;
   iphost = socket->local_address();
#ifdef ALLOW_ANON_FTP
   socket->send(FTP_get_welcome_msg());
#else
   socket->send(sprintf("220- %s FTP server ready.\n220 Please login with your"
                        " wizard name.\n",
                        mud_name()));
#endif
   m_delete(sessions, 0); /* Just make sure there's no stale stuff here */
   if (!sizeof(sessions))
   {
      idlecall = call_out(FTP_handle_idlers, 60);
   }
   sessions[socket] = newSession;
}

private
void FTP_close_socket(object socket)
{
   if (socket) socket->remove();
}

private
void FTP_write(object socket)
{
   class ftp_session info;

   info = sessions[dataports[socket]];
   info.cmdPipe->send("226 Transfer complete.\n");
   info.dataPipe->remove();
}

private
void FTP_read(object socket, string data)
{
   string cmd, arg;
   class ftp_session thisSession;
   function dispatchTo;
   int i;
   mixed err;
   /* DEBUG FTP kobol */
   FTPLOGF("SOCKET %O\n", socket);
   FTPLOGF("SOCKET_ADDR %O\n", socket->address());
   /* END DEBUG */

   /* If there is no data, it's a new connection. */
   if (!data)
   {
      assert(!sessions[socket]);
      FTP_addSession(socket);
      return;
   }

   thisSession = sessions[socket];

   /* Check to make sure that the time idle is not greater than the maximum
    * idle time.  If it is remove the session and everything attached to it */
   /*   if(thisSession.idleTime>MAX_IDLE_TIME) { */

   /*     //    call_out((:destruct($(socket) ):),5); */
   /*     return; */
   /*   } */

   thisSession.idleTime = 0;

   if (!thisSession.command)
      thisSession.command = "";

   data = replace_string(data, "\r", "");
   thisSession.command += data;
   if ((i = strsrch(thisSession.command, "\n")) == -1)
      return;
   data = thisSession.command[0..i - 1];
   thisSession.command = thisSession.command[i + 1..];

   thisSession.command = trim(thisSession.command);

   /* get the command and argument. */
   if (!sscanf(data, "%s %s", cmd, arg))
   {
      cmd = data;
   }

   /* lower_case the command... */
   cmd = lower_case(cmd);

   /* If we're not connected, these are valid commands... */
   if (!thisSession.connected)
   {
      switch (cmd)
      {
      case "user":
         FTP_CMD_user(thisSession, arg);
         return;
      case "pass":
         FTP_CMD_pass(thisSession, arg);
         return;
      case "quit":
         FTP_CMD_quit(thisSession, arg);
         return;
      case "noop":
         FTP_CMD_noop(thisSession, arg);
         return;
      default:
         socket->send("503 Log in with USER first.\n");
         return;
      }
   }
   /* We are connected, so dispatch to the proper handler. */
   dispatchTo = dispatch[cmd];
   if (!dispatchTo)
   {
      /* Log command so we know what clients are trying to use */
      socket->send(sprintf("502 Unknown command %s.\n", cmd));
      return;
   }
   if (err = catch (evaluate(dispatchTo, thisSession, arg)))
   {
      FTPLOGF("%s caused a FAILURE with command '%s'.\n", capitalize(thisSession.user), data);
      FTPLOGF("Error caught was '%O'\n", err);
      socket->send("550 Unknown failure.  Please report what you were doing "
                   "to the mud admin.\n");
   }
   return;
}

private
void FTP_PASV_read(class ftp_session info, object socket, bytes data)
{
   if (data == nil)
   {
      info.dataPipe = socket;
      socket->set_write_callback(FTP_write);
      dataports[socket] = info.cmdPipe;
      object listener = passives[info.cmdPipe];
      m_delete(passives, info.cmdPipe);
      // The accept event must commit before retiring its listener.
      call_out(&FTP_close_socket(listener), 0);
      return;
   }
   FTP_DATA_read(socket, data);
}

private
void FTP_close(object socket)
{
   m_delete(sessions, socket);
}

private
void create()
{
   sock = clone_object (SOCKET, SKT_STYLE_LISTEN, PORT_FTP, FTP_read, FTP_close);
}

private
void FTP_DATA_read(object socket, bytes data)
{
   if (!data) return;
   class ftp_session info = sessions[dataports[socket]];
   if (!info.binary)
      data = to_bytes(replace_string(to_text(data, "ISO-8859-1"), "\r", ""), "ISO-8859-1");
   unguarded(info.priv, (: write_bytes($(info.targetFile), $(info.filepos), $(data)) :));
   info.filepos += sizeof(data);
}

private
void FTP_DATA_close(object socket)
{
   class ftp_session info;
   info = sessions[dataports[socket]];
   if (info)
      info.cmdPipe->send("226 Transfer complete.\n");
}

private
void FTP_CMD_user(class ftp_session info, string arg)
{
   NEEDS_ARG();
   arg = lower_case(arg);
   if (info.connected)
   {
      info.cmdPipe->send(sprintf("530 User %s access denied.\n", arg));
      return;
   }
   info.user = arg;

#ifdef ALLOW_ANON_FTP
   if (member_array(arg, anon_logins) != -1)
   {
      info.cmdPipe->send("331 Guest login ok, send your complete e-mail "
                          "address as password.\n");
      return;
   }
#endif

   info.cmdPipe->send(sprintf("331 Password required for %s.\n", arg));
   return;
}

private
void FTP_CMD_pass(class ftp_session info, string arg)
{
   string password;
   mixed *userinfo;

   NEEDS_ARG();
   if (info.connected || !info.user)
   {
      info.cmdPipe->send("503 Login with USER first.\n");
      return;
   }

#ifdef ALLOW_ANON_FTP
   if (ANON_USER())
   {
      info.cmdPipe->send("230 guest login ok, access restrictions apply.\n");
      info.connected = 1;
      info.priv = 0;
      info.pwd = ANON_PREFIX;
      FTPLOGF("Anomymous login from %s (email = %s)\n", info.cmdPipe->address()[0], arg);
      return;
   }
#endif

   userinfo = unguarded(1, ( : USER_D->query_variable($(info.user), ({"password"})) :));
   if (arrayp(userinfo) && sizeof(userinfo))
      password = userinfo[0];
   else
   {
      info.cmdPipe->send("530 Login incorrect.\n");
      return;
   }
   if (crypt(arg, password) != password || !wizardp(info.user))
   {
      info.cmdPipe->send("530 Login incorrect.\n");
      return;
   }
   info.cmdPipe->send(sprintf("230 User %s logged in.\n", info.user));
   info.connected = 1;
   FTPLOGF("%s connected from %s.\n", capitalize(info.user), info.cmdPipe->address()[0]);

   if (adminp(info.user))
      info.priv = 1;
   else
      info.priv = info.user;

   info.pwd = join_path(WIZ_DIR, info.user);
   if (!is_directory(info.pwd))
      info.pwd = "/";
   return;
}

private
void FTP_CMD_quit(class ftp_session info, string arg)
{
   info.cmdPipe->send("221 Goodbye.\n");
   if (info.dataPipe)
      FTP_close_socket(info.dataPipe);
   if (member_array(info.cmdPipe, keys(passives)) > -1)
   {
      FTP_close_socket(passives[info.cmdPipe]);
      m_delete(passives, info.cmdPipe);
   }
   FTPLOGF("%s QUIT at %s.\n", capitalize(info.user), ctime(time()));
   m_delete(sessions, info.cmdPipe);
   FTP_close_socket(info.cmdPipe);
}

int next_port()
{
#ifdef RESTRICT_PORTS
   lastport++;
   if (lastport > MAX_PORT + 1)
      lastport = MIN_PORT + 1;
   return lastport - 1;
#endif
   return 0;
}

private
void FTP_PASV_start(class ftp_session info, int attempt)
{
   object listener = unguarded(1, (: clone_object(SOCKET, SKT_STYLE_LISTEN_B,
      next_port(), &FTP_PASV_read($(info)), FTP_DATA_close, $(info).cmdPipe->local_address()) :));
   passives[info.cmdPipe] = listener;
   call_out(&FTP_PASV_ready(info, listener, attempt, 0), 0.01);
}

private
void FTP_PASV_ready(class ftp_session info, object listener, int attempt, int waited)
{
   if (!info.cmdPipe || passives[info.cmdPipe] != listener) return;
   if (!listener || listener->query_status()["closing"] || waited >= 1000)
   {
      FTP_close_socket(listener);
      m_delete(passives, info.cmdPipe);
      if (attempt + 1 < MAX_TRIES) FTP_PASV_start(info, attempt + 1);
      else info.cmdPipe->send("425 Cannot open passive data connection.\n");
      return;
   }
   int port = listener->local_port();
   if (!port)
   {
      call_out(&FTP_PASV_ready(info, listener, attempt, waited + 1), 0.01);
      return;
   }
   info.cmdPipe->send(sprintf("227 Entering Passive mode. (%s,%i,%i)\n",
      replace_string(info.cmdPipe->local_address(), ".", ","), port >> 8, port % 256));
}

private
void FTP_CMD_pasv(class ftp_session info, string arg)
{
   FTPLOGF("PASV rec'd %O\n", info.cmdPipe);
   if (arg)
   {
      info.cmdPipe->send("500 command not understood.\n");
      return;
   }
   FTP_close_socket(info.dataPipe);
   FTP_close_socket(passives[info.cmdPipe]);
   if (sizeof(sessions))
   {
      foreach (object skt, class ftp_session session in sessions)
      {
         if (session && session.user == info.user && skt != info.cmdPipe)
         {
            FTP_close_socket(passives[skt]);
            m_delete(sessions, skt);
            m_delete(passives, skt);
            skt->remove();
         }
      }
   }
   FTP_PASV_start(info, 0);
}

private
void FTP_CMD_port(class ftp_session info, string arg)
{
   string ip, *parts;
   int port;
   NEEDS_ARG();

   parts = explode(arg, ",");
   if (sizeof(parts) != 6)
   {
      info.cmdPipe->send("550 Failed command.\n");
      FTP_close_socket(info.dataPipe);
      return;
   }
   ip = implode(parts[0..3], ".");
   /* Make a 16 bit port # out of 2 8 bit values.  */
   port = (to_int(parts[4]) << 8) + to_int(parts[5]);

   // tell_user("loriel", sprintf("info %O", info));
   if (info.dataPipe)
      FTP_close_socket(info.dataPipe);

   switch (info.binary)
   {
   case 0:
      info.dataPipe = unguarded(1, (
                                       : clone_object (SOCKET, SKT_STYLE_CONNECT_B, sprintf("%s %d", $(ip), $(port)),
                                              FTP_DATA_read,
                                              FTP_DATA_close)
                                       :));
      break;
   case 1:
      info.dataPipe = unguarded(1, (
                                       : clone_object (SOCKET, SKT_STYLE_CONNECT_B, sprintf("%s %d", $(ip), $(port)),
                                              FTP_DATA_read,
                                              FTP_DATA_close)
                                       :));
      break;
   default:
      return;
   }
   /* map the data port to the cmd port so we can find the cmd port when
    * we're given the data port. */
   dataports[info.dataPipe] = info.cmdPipe;
   info.dataPipe->set_write_callback(FTP_write);
   info.cmdPipe->send("200 PORT command successful.\n");
   // tell_user("loriel", sprintf("info %O", info));
   return;
}

private
void do_list(class ftp_session info, string arg, int ltype)
{
   mixed *files;
   string flags;
   string output;
   int isfile;

   flags = find_flags(arg);
   arg = strip_flags(arg);
   if (!arg || arg == "")
      arg = ".";

   /* Check to make sure we aren't really looking at a file.
    * canonical_file() mucks this up a bit later on so the check
    * is necessary */
   if (arg[ < 2..] == "/.")
      if (is_file(arg[0.. < 3]))
         isfile = 1;

   arg = evaluate_path(arg, info.pwd);
   ANON_CHECK(arg);
   if (unguarded(1, ( : is_directory($(arg)) :)))
      arg = join_path(arg, "*");
   if (unguarded(info.priv, ( : file_size(base_path($(arg))) :)) == -1)
   {
      info.cmdPipe->send(sprintf("550 %s: No such file OR directory.\n", arg));
      FTP_close_socket(info.dataPipe);
      return;
   }
   if (isfile)
      files = ({});
   else
      files = unguarded(info.priv, ( : map(get_dir($(arg)), (: [string name] ({name, file_size(join_path(base_path($(arg)), name)), file_time(join_path(base_path($(arg)), name))}) :)) :));
   if (!files && !isfile)
   {
      info.cmdPipe->send(sprintf("550 %s: Permission denied.\n", arg));
      FTP_close_socket(info.dataPipe);
      return;
   }
   if (flags)
      if (strsrch(flags, 'a') == -1)
         files = filter(files, ( : member_array($1[0], ({".", ".."})) == -1 :));
   if (!sizeof(files) && !isfile)
   {
      info.cmdPipe->send("550 No files found.\n");
      FTP_close_socket(info.dataPipe);
      return;
   }

   /* in case of LIST imply -l */
   /* in case of NLST imply -1 */
   if (ltype == LTYPE_LIST)
   {
      if (flags)
      {
         if ((strsrch(flags, 'l') == -1) && (strsrch(flags, 'C') == -1) && (strsrch(flags, '1') == -1))
            flags += "l";
      }
      else
         flags = "l";
   }
   else
   {
      if (flags)
      {
         if ((strsrch(flags, 'l') == -1) && (strsrch(flags, 'C') == -1) && (strsrch(flags, '1') == -1))
            flags += "1";
      }
      else
         flags = "1";
   }

   /* Check the flags for output now */

   /* Check the F flag */
   if (strsrch(flags, 'F') > -1)
   {
      foreach (mixed *file in files)
         if (file[1] == -2)
            file[0] = sprintf("%s/", file[0]);
   }
   /* The C flag */
   if (strsrch(flags, 'C') > -1)
   {
      int lines;
      int size;
      int i;
      if ((strsrch(flags, 'l') > -1) || (strsrch(flags, '1') > -1))
      {
         info.cmdPipe->send("550: LIST -C flag is incompatible with -1 or -l.\n");
         FTP_close_socket(info.dataPipe);
         return;
      }
      lines = ((size = sizeof(files)) / 3) + 1;
      output = "";
      for (i = 0; i < lines; i++)
      {
         mixed *these_files;
         if ((i * 3 + 2) < size)
            these_files = files[(i * 3)..(i * 3 + 2)];
         else if (i * 3 < size)
         {
            these_files = files[(i * 3)..];
            while (sizeof(these_files) < 3)
               these_files += ({({"", 0, 0})});
         }
         else
            break;
         output = sprintf("%s%-=25s %-=25s %-=25s\n", output, these_files[0][0], these_files[1][0], these_files[2][0]);
      }
   }
   if (strsrch(flags, 'l') > -1)
   {
      if (strsrch(flags, '1') > -1)
      {
         info.cmdPipe->send("550: LIST -l and -1 flags incompatible.\n");
         FTP_close_socket(info.dataPipe);
         return;
      }

      output = implode(
          map(files, (
                         : sprintf("%s %3i %=9s %=8s %=7s %s%5s %s", $1[1] == -2 ? "drwxrwxr-x" : "-rw-rw-r--", 1,
                                   lower_case(replace_string(mud_name(), " ", "_"))[0..7],
                                   lower_case(replace_string(mud_name(), " ", "_"))[0..7],
                                   $1[1] == -2 ? "0" : sprintf("%d", $1[1]), ctime($1[2])[4..10],
                                   (time() - $1[2]) > 31536000 ? ctime($1[2])[20..] : ctime($1[2])[11..15], $1[0])
                         :)),
          "\n");
   }

   if (strsrch(flags, '1') > -1)
      output = implode(map(files, ( : sprintf("%s", $1[0]) :)), "\n");

   info.cmdPipe->send("150 Opening ascii mode data connection for file list\n");
   info.dataPipe->send(implode(explode(output, "\n"), "\r\n") + "\r\n");
   return;
}

private
void FTP_CMD_list(class ftp_session info, string arg)
{
   /* Make sure the dataPort is connected, otherwise this will error */
   if (!info.dataPipe)
   {
      call_out(&FTP_CMD_list(info, arg), 2);
      return;
   }
   do_list(info, arg, LTYPE_LIST);
}

private
void FTP_CMD_nlst(class ftp_session info, string arg)
{
   /* Make sure the dataPort is connected, otherwise this will error */
   if (!info.dataPipe)
   {
      call_out(&FTP_CMD_nlst(info, arg), 2);
      return;
   }

   do_list(info, arg, LTYPE_NLST);
}

private
void FTP_CMD_retr(class ftp_session info, string arg)
{
   string target_file;
   int i;

   /* Make sure the dataPort is connected, otherwise this will error */
   if (!info.dataPipe)
   {
      call_out(&FTP_CMD_retr(info, arg), 2);
      return;
   }
   NEEDS_ARG();

   target_file = evaluate_path(arg, info.pwd);

   ANON_CHECK(target_file);

   if (unguarded(info.priv, ( : is_directory($(target_file)) :)))
   {
      info.cmdPipe->send(sprintf("550 %s: Can't retrieve (it's a directory)."
                                  "\n",
                                  target_file));
      FTP_close_socket(info.dataPipe);
      return;
   }
   if (!unguarded(info.priv, ( : is_file($(target_file)) :)))
   {
      info.cmdPipe->send(sprintf("550 %s: No such file OR directory.\n", target_file));
      FTP_close_socket(info.dataPipe);
      return;
   }
   if (!unguarded(info.priv, ( : file_size($(target_file)) :)))
   {
      info.cmdPipe->send(sprintf("550 %s: File contains nothing.\n", target_file));
      FTP_close_socket(info.dataPipe);
      return;
   }

   switch (info.binary)
   {
   case 0:
      i = file_size(target_file);
      FTPLOGF("%s GOT %s.\n", capitalize(info.user), target_file);

      outfile[info.dataPipe] = ({target_file, 0, 0, info.cmdPipe});
      info.dataPipe->set_write_callback(FTP_CMD_retr_callback);

      info.cmdPipe->send(sprintf("150 Opening ascii mode data connection for "
                                  "%s (%d bytes).\n",
                                  target_file, i));

      info.dataPipe->send(FTP_CMD_retr_callback(info.dataPipe));
      break;
   case 1:
      i = file_size(target_file);

      outfile[info.dataPipe] = ({target_file, 1, 0, info.cmdPipe});
      info.dataPipe->set_write_callback(FTP_CMD_retr_callback);

      info.cmdPipe->send(sprintf("150 Opening binary mode data connection "
                                  "for %s (%d bytes).\n",
                                  target_file, i));

      info.dataPipe->send(FTP_CMD_retr_callback(info.dataPipe));
      break;
   default:
      ENSURE(0);
   }
}

void FTP_CMD_pwd(class ftp_session info, string arg)
{
   info.cmdPipe->send(sprintf("257 \"%s\" is current directory.\n", info.pwd));
}

void FTP_CMD_noop(class ftp_session info, string arg)
{
   info.cmdPipe->send("221 NOOP command successful.\n");
}

private
void FTP_CMD_stor(class ftp_session info, string arg)
{
   /* Make sure the dataPort is connected, otherwise this will error */
   if (!info.dataPipe)
   {
      call_out(&FTP_CMD_stor(info, arg), 2);
      return;
   }

   NEEDS_ARG();

   arg = evaluate_path(arg, info.pwd);

#ifndef ANON_CAN_PUT
#ifdef ALLOW_ANON_FTP
   if (ANON_USER())
   {
      info.cmdPipe->send("550 Permission denied.\n");
      FTP_close_socket(info.dataPipe);
      return;
   }
#endif
#else
   ANON_CHECK(arg);
#endif

   if (!unguarded(info.priv, ( : is_directory(base_path($(arg))) :)))
   {
      info.cmdPipe->send("553 No such directory to store into.\n");
      FTP_close_socket(info.dataPipe);
      return;
   }
   info.targetFile = arg;
   if (unguarded(info.priv, ( : is_file($(arg)) :)))
   {
      if (!unguarded(info.priv, ( : rm($(arg)) :)))
      {
         info.cmdPipe->send(sprintf("550 %s: Permission denied.\n", arg));
         FTP_close_socket(info.dataPipe);
         return;
      }
   }
   if (!unguarded(info.priv, ( : write_file($(arg), "") :)))
   {
      info.cmdPipe->send(sprintf("550 %s: Permission denied.\n", arg));
      FTP_close_socket(info.dataPipe);
      return;
   }
   FTPLOGF("%s PUT %s.\n", capitalize(info.user), arg);

   /* Reset the file position flag. */
   info.filepos = 0;
   info.cmdPipe->send(sprintf("150 Opening %s mode data connection for %s.\n", info.binary ? "binary" : "ascii", arg));
}

private
void FTP_CMD_cdup(class ftp_session info, string arg)
{
   FTP_CMD_cwd(info, "..");
}

private
void FTP_CMD_cwd(class ftp_session info, string arg)
{
   string newpath;

   NEEDS_ARG();
   newpath = evaluate_path(arg, info.pwd);

   ANON_CHECK(newpath);

   if (!unguarded(info.priv, ( : is_directory($(newpath)) :)))
   {
      info.cmdPipe->send(sprintf("550 %s: No such file or directory.\n", newpath));
      return;
   }
   info.pwd = newpath;
   info.cmdPipe->send("250 CWD command successful.\n");
}

private
void FTP_CMD_mkd(class ftp_session info, string arg)
{
   NEEDS_ARG();

   arg = evaluate_path(arg, info.pwd);

#ifndef ANON_CAN_PUT
#ifdef ALLOW_ANON_FTP
   if (ANON_USER())
   {
      info.cmdPipe->send("550 Permission denied.\n");
      FTP_close_socket(info.dataPipe);
      return;
   }
#endif
#else
   ANON_CHECK(arg);
#endif

   if (!unguarded(info.priv, ( : is_directory(base_path($(arg))) :)))
   {
      info.cmdPipe->send(sprintf("550 %s: No such directory.\n", base_path(arg)));
      return;
   }
   if (unguarded(info.priv, ( : file_size($(arg)) :)) != -1)
   {
      info.cmdPipe->send(sprintf("550 %s: File exists.\n", arg));
      return;
   }

   if (!unguarded(info.priv, ( : mkdir($(arg)) :)))
   {
      info.cmdPipe->send(sprintf("550 %s: Permission denied.\n", arg));
      return;
   }
   info.cmdPipe->send("257 MKD command successful.\n");
   return;
}

private
void FTP_CMD_type(class ftp_session info, string arg)
{
   NEEDS_ARG();
   switch (arg)
   {
   case "a":
   case "A":
      info.binary = 0;
      info.cmdPipe->send("200 Type set to A.\n");
      return;
   case "i":
   case "I":
      info.binary = 1;
      info.cmdPipe->send("200 Type set to I.\n");
      return;
   default:
      info.cmdPipe->send("550 Unknown file type.\n");
      return;
   }
}

private
void FTP_CMD_dele(class ftp_session info, string arg)
{
   NEEDS_ARG();
   arg = evaluate_path(arg, info.pwd);

   ANON_CHECK(arg);

   if (!unguarded(info.priv, ( : is_file($(arg)) :)))
   {
      info.cmdPipe->send(sprintf("550 %s: No such file OR directory.\n", arg));
      return;
   }
   if (!unguarded(info.priv, ( : rm($(arg)) :)))
   {
      info.cmdPipe->send(sprintf("550 %s: Permission denied.\n", arg));
      return;
   }
   FTPLOGF("%s DELETED %s.\n", capitalize(info.user), arg);
   info.cmdPipe->send("250 DELE command successful.\n");
}

void remove()
{
   class ftp_session item;

   foreach (item in values(sessions))
   {
      if (objectp(item.cmdPipe))
         FTP_close_socket(item.cmdPipe);

      if (objectp(item.dataPipe))
         FTP_close_socket(item.dataPipe);
   }

   FTP_close_socket(sock);
   remove_call_out(idlecall);
}

int clean_up()
{
   return 0;
}

string *list_users()
{
   return map(values(sessions), ( : ((class ftp_session)$1).connected ? ((class ftp_session)$1).user : "(login)" :));
}

private
void FTP_CMD_syst(class ftp_session info, string arg)
{
   info.cmdPipe->send("215 UNIX Mud Name: " + mud_name() + "\n");
}

bytes FTP_CMD_retr_callback(object ob)
{
   int start, length;
   mixed ret;

   if (!ob || !has_key(outfile, ob))
      return nil;

   start = outfile[ob][2];
   length = FTP_BLOCK_SIZE;
   outfile[ob][2] += length;

   if (start + length > file_size(outfile[ob][0]))
      length = file_size(outfile[ob][0]) - start;

   ret = unguarded(sessions[outfile[ob][3]].priv, (: read_bytes($(outfile[ob][0]), $(start), $(length)) :));

   if (start + length >= file_size(outfile[ob][0]))
   {
      m_delete(outfile, ob);
      ob->set_write_callback(FTP_write);
   }

   return bytesp(ret) ? ret : nil;
}

mapping query_passives()
{
   return deep_copy(passives);
}

mapping query_sessions()
{
   return deep_copy(sessions);
}

mapping query_dataports()
{
   return deep_copy(dataports);
}

string query_iphost()
{
   return iphost;
}
