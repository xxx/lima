/* Do not remove the headers from this file! see /USAGE for more info. */

// An NNTP client object. (RFC 977: Network News Transfer Protocol)
// Protocol by Brian Kantor and Phil Lapsley.
// Created: May 4, 1995 by John Viega (rust@virginia.edu)
// More user friendly date handling added Aug 8. by Edward Kmett (Harmless)
// ARTICLE response handling was removed by Edward Kmett.

//: MODULE
// Optional text NNTP client with ordered response callbacks.

#include <mudlib.h>
#include <socket.h>

#define NNTP_PORT 119
#define MAX_PORT 32000
#define BODY_CHUNK_CHARACTERS 1024

private class request {
   string kind;
   string wire;
   int success;
   int multiline;
   string *body;
};

private object news_socket;
private function response_callback;
private class request *pending = ({});
private class request active;
private string incoming = "";
private string *reply;
private int greeted;
private int can_post;
private int closed;
private int quitting;
private int notifying;
private int body_offset;
private int body_part;
private int body_sending;
private int body_started;

private void close_connection()
{
   object socket = news_socket;
   closed = 1;
   news_socket = nil;
   pending = ({});
   active = nil;
   incoming = "";
   reply = nil;
   body_sending = body_started = body_offset = body_part = 0;
   if (socket) catch(socket->remove());
}

private int report(mixed *arguments)
{
   notifying++;
   mixed failure = catch(evaluate(response_callback, arguments...));
   notifying--;
   if (failure) close_connection();
   return !failure && !closed;
}

private void fail(string message)
{
   if (closed) return;
   close_connection();
   report(({"nntp error: " + message}));
}

private int send(string text)
{
   if (catch(news_socket->send(text))) {
      fail("write failed.");
      return 0;
   }
   return 1;
}

private void start_next()
{
   while (!closed && greeted && !notifying && !active && sizeof(pending)) {
      active = pending[0];
      pending = pending[1..];
      if (active.kind == "XHDR") {
         active = nil;
         report(({"nntp error: not implemented yet."}));
         continue;
      }
      if ((active.kind == "POST" || active.kind == "IHAVE") && !can_post) {
         active = nil;
         report(({"nntp error: not confirmed for posting."}));
         continue;
      }
      body_started = 0;
      send(active.wire + "\r\n");
   }
}

private void enqueue(string kind, string command, int success, int multiline, string *body)
{
   if (closed || !news_socket) error("nntp: connection is closed.");
   if (quitting) error("nntp: QUIT is already queued.");
   if (kind == "QUIT") quitting = 1;
   pending += ({new(class request, kind: kind, wire: command, success: success,
                    multiline: multiline, body: body)});
   start_next();
}

private string next_body_chunk(object socket)
{
   if (closed || socket != news_socket || !body_sending) return nil;
   string chunk = "";
   while (sizeof(chunk) < BODY_CHUNK_CHARACTERS && body_part < sizeof(active.body)) {
      string part = active.body[body_part];
      int count = BODY_CHUNK_CHARACTERS - sizeof(chunk);
      if (count > sizeof(part) - body_offset) count = sizeof(part) - body_offset;
      if (count) chunk += part[body_offset..body_offset + count - 1];
      body_offset += count;
      if (body_offset == sizeof(part)) { body_part++; body_offset = 0; }
   }
   if (body_part == sizeof(active.body)) {
      body_sending = 0;
      active.body = nil;
   }
   return sizeof(chunk) ? chunk : nil;
}

private void complete(mixed *arguments)
{
   active = nil;
   reply = nil;
   body_started = body_sending = body_offset = body_part = 0;
   if (report(arguments)) start_next();
}

private int digits(string text)
{
   if (!sizeof(text)) return 0;
   foreach (int character in text)
      if (character < '0' || character > '9') return 0;
   return 1;
}

private void finish_reply()
{
   if (active.kind == "LIST") {
      mixed *groups = ({});
      foreach (string group in reply[1..]) groups += ({explode(group, " ")});
      complete(({groups}));
   } else complete(({reply}));
}

private void status_line(string line)
{
   if (sizeof(line) < 3 || !digits(line[0..2]) ||
       (sizeof(line) > 3 && line[3] != ' ')) {
      fail("invalid response: " + line + ".");
      return;
   }
   int code = to_int(line[0..2]);
   if (!greeted) {
      if (code != 200 && code != 201) {
         fail("unexpected greeting: " + line);
         return;
      }
      greeted = 1;
      can_post = code == 200;
      start_next();
      return;
   }
   if (code == 400) {
      fail("server closed connection: " + line);
      return;
   }
   if (!active) {
      fail("unsolicited response: " + line);
      return;
   }
   if (active.kind == "POST" || active.kind == "IHAVE") {
      int continuation = active.kind == "POST" ? 340 : 335;
      if (!body_started && code == continuation) {
         body_started = body_sending = 1;
         body_offset = body_part = 0;
         if (send(next_body_chunk(news_socket))) report(({line}));
         return;
      }
      if (body_sending || (code < 400 && (!body_started || code != active.success))) {
         fail("unexpected article response: " + line);
         return;
      }
      complete(({line}));
      return;
   }
   if (code != active.success && code < 400) {
      fail("unexpected reply: " + line);
      return;
   }
   if (code == active.success && active.multiline) {
      reply = ({line});
      return;
   }
   if (active.kind == "QUIT") {
      close_connection();
      report(({line}));
      return;
   }
   string kind = active.kind;
   if (kind == "GROUP" || kind == "STAT") {
      if (code != active.success) {
         complete(({"nntp error: unexpected reply: " + code}));
         return;
      }
      string *fields = filter(explode(line, " "), (: sizeof($1) :));
      if (kind == "GROUP") {
         if (sizeof(fields) < 5 || !digits(fields[1]) ||
             !digits(fields[2]) || !digits(fields[3])) {
            fail("invalid GROUP response: " + line);
            return;
         }
         complete(({code, to_int(fields[1]), to_int(fields[2]), to_int(fields[3]), fields[4]}));
      } else {
         if (sizeof(fields) < 3 || !digits(fields[1])) {
            fail("invalid article number: " + line);
            return;
         }
         complete(({code, to_int(fields[1]), fields[2]}));
      }
      return;
   }
   if (kind == "LIST" && code != 215)
      complete(({"nntp error: unexpected reply: " + code}));
   else complete(({line}));
}

private void read_data(string data)
{
   incoming += data;
   int end;
   while (!closed && (end = strindex(incoming, "\r\n")) >= 0) {
      string line = end ? incoming[0..end - 1] : "";
      incoming = incoming[end + 2..];
      if (reply != nil) {
         if (line == ".") finish_reply();
         else {
            if (sizeof(line) && line[0] == '.') line = line[1..];
            reply += ({line});
         }
      } else status_line(line);
   }
}

private void nntp_read_callback(object socket, string data)
{
   if (closed || socket != news_socket || data == nil) return;
   if (catch(read_data(data))) fail("could not process response.");
}

private void nntp_close_callback(object socket)
{
   if (socket == news_socket && !closed) fail("connection closed before QUIT reply.");
}

varargs void create(function callback, string host, mixed port)
{
   if (!callback && !host && !port) return;
   if (!functionp(callback) || !stringp(host) || !sizeof(host))
      error("nntp: expected a response callback and host.");
   if (!port) port = NNTP_PORT;
   if (stringp(port) && digits(port)) port = to_int(port);
   if (!intp(port) || port < 1 || port > MAX_PORT)
      error("nntp: invalid port.");
   response_callback = callback;
   news_socket = clone_object(SOCKET, SKT_STYLE_CONNECT, host + " " + port,
                              nntp_read_callback, nntp_close_callback);
   if (catch(news_socket->set_write_callback(next_body_chunk))) {
      close_connection();
      error("nntp: could not configure output callback.");
   }
}

private int leap_year(int year)
{
   return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

// Gregorian calendars repeat every 146097 days, including before the epoch.
//: FUNCTION dt_stamp
// Return a UTC yymmdd hhmmss date for NEWGROUPS and NEWNEWS.

string dt_stamp(int timestamp)
{
   int day = timestamp / 86400;
   int seconds = timestamp % 86400;
   if (seconds < 0) { day--; seconds += 86400; }
   int era = day / 146097;
   day %= 146097;
   if (day < 0) { era--; day += 146097; }
   int year = 1970 + era * 400;
   while (day >= 365 + leap_year(year)) {
      day -= 365 + leap_year(year);
      year++;
   }
   int *months = ({31, 28 + leap_year(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31});
   int month;
   while (day >= months[month]) { day -= months[month]; month++; }
   return sprintf("%02d%02d%02d %02d%02d%02d", (year % 100 + 100) % 100,
                  month + 1, day + 1, seconds / 3600, seconds % 3600 / 60, seconds % 60);
}

//: FUNCTION newgroups
// Send an nntp NEWGROUPS command.

void newgroups(int datetime) { enqueue("NEWGROUPS", "NEWGROUPS " + dt_stamp(datetime) + " GMT", 231, 1, nil); }

//: FUNCTION newnews
// Send an nntp NEWNEWS command.

void newnews(string group, int datetime) { enqueue("NEWNEWS", "NEWNEWS " + group + " " + dt_stamp(datetime) + " GMT", 230, 1, nil); }

//: FUNCTION list
// Send an nntp LIST command.

void list() { enqueue("LIST", "LIST", 215, 1, nil); }

//: FUNCTION group
// Send an nntp GROUP command.

void group(string name) { enqueue("GROUP", "GROUP " + name, 211, 0, nil); }

//: FUNCTION help
// Send an nntp HELP command.

void help() { enqueue("HELP", "HELP", 100, 1, nil); }

//: FUNCTION stat
// Send an nntp STAT command.

void stat(string id) { enqueue("STAT", "STAT " + id, 223, 0, nil); }

//: FUNCTION next
// Send an nntp NEXT command.

void next() { enqueue("STAT", "NEXT", 223, 0, nil); }

//: FUNCTION last
// Send an nntp LAST command.

void last() { enqueue("STAT", "LAST", 223, 0, nil); }

//: FUNCTION head
// Send an nntp HEAD command.

varargs void head(string id) { enqueue("HEAD", id ? "HEAD " + id : "HEAD", 221, 1, nil); }

//: FUNCTION body
// Send an nntp BODY command.

varargs void body(string id) { enqueue("BODY", id ? "BODY " + id : "BODY", 222, 1, nil); }

//: FUNCTION article
// Send an nntp ARTICLE command.

varargs void article(string id) { enqueue("ARTICLE", id ? "ARTICLE " + id : "ARTICLE", 220, 1, nil); }

//: FUNCTION xhdr
// Report that XHDR is not implemented.

void xhdr(string hdr, string str) { enqueue("XHDR", "", 0, 0, nil); }

//: FUNCTION slave
// Send an nntp SLAVE command.

void slave() { enqueue("SLAVE", "SLAVE", 202, 0, nil); }

//: FUNCTION quit
// Send an nntp QUIT command, and clean up.

void quit() { enqueue("QUIT", "QUIT", 205, 0, nil); }

private string *article_body(mixed file)
{
   if (!stringp(file)) error("nntp: expected a local filename.");
   string contents = read_file(file);
   if (contents == nil) error("nntp: missing local file.");
   contents = replace_string(contents, "\r\n", "\n");
   string *lines = sizeof(contents) ? explode(contents, "\n") : ({});
   if (sizeof(lines) && lines[<1] == "") lines = lines[..<2];
   string *parts = ({});
   foreach (string line in lines) {
      if (sizeof(line) && line[0] == '.') parts += ({"."});
      parts += ({line, "\r\n"});
   }
   return parts + ({".\r\n"});
}

//: FUNCTION post
// Send an nntp POST command.

void post(string file)
{
   string *body;
   if (catch(body = article_body(file))) {
      report(({"nntp error: couldn't find local file."}));
      return;
   }
   enqueue("POST", "POST", 240, 0, body);
}

//: FUNCTION ihave
// Send an nntp IHAVE command.

void ihave(string id, mixed file)
{
   string *body;
   if (catch(body = article_body(file))) {
      report(({"nntp error: couldn't find local file."}));
      return;
   }
   enqueue("IHAVE", "IHAVE " + id, 235, 0, body);
}

void remove() { close_connection(); }
