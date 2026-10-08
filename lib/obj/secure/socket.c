/* Do not remove the headers from this file! see /USAGE for more info. */

#include <mudlib.h>
#include <socket.h>

inherit "/std/modules/m_i3_codec";

#define MAX_OUTPUT_BYTES 524288
#define MAX_OUTPUT_FRAMES 64
#define WRITE_CHUNK_BYTES 8192
#define WRITE_BATCH 8
#define READ_BATCH 8
#define CLOSE_WAIT 10

nosave private int style;
nosave private int configured;
nosave private int fdOwned = -1;
nosave private int ready;
nosave private int closing;
nosave private int native_closing;
nosave private int suppress_close;
nosave private int close_notified;
nosave private int input_eof;
nosave private int output_active;
nosave private int input_timer;
nosave private int output_timer;
nosave private int close_timer;
nosave private object client;
nosave private object offer_parent;
nosave private function read_func;
nosave private function close_func;
nosave private function write_func;
nosave private mixed *remote_endpoint;
nosave private mixed *local_endpoint;
nosave private bytes *write_queue = ({});
nosave private int queued_bytes;
nosave private int write_offset;

private void require_client()
{
   if (previous_object() != client && previous_object() != this_object())
      throw("socket: caller does not control this connection");
}

private void cancel_work()
{
   if (input_timer) remove_call_out(input_timer);
   if (output_timer) remove_call_out(output_timer);
   if (close_timer) remove_call_out(close_timer);
   input_timer = output_timer = close_timer = 0;
}

private void notify_close()
{
   if (!close_notified && !suppress_close)
   {
      close_notified = 1;
      if (close_func) catch(evaluate(close_func, this_object()));
   }
}

private void finish_connection()
{
   fdOwned = -1;
   ready = 0;
   closing = 1;
   cancel_work();
   write_queue = ({});
   queued_bytes = write_offset = 0;
   i3_reset_input();
   notify_close();
   if (this_object()) destruct();
}

private void fail_connection()
{
   if (fdOwned > 0) catch(socket_close(fdOwned));
   finish_connection();
}

private void close_expired()
{
   close_timer = 0;
   fail_connection();
}

private void flush_output()
{
   if (!ready || native_closing || fdOwned < 1)
      return;
   for (int batch = 0; batch < WRITE_BATCH && sizeof(write_queue); batch++)
   {
      bytes frame = write_queue[0];
      int end = write_offset + WRITE_CHUNK_BYTES - 1;
      if (end >= sizeof(frame)) end = sizeof(frame) - 1;
      if (!socket_write(fdOwned, frame[write_offset..end]))
         return;
      write_offset = end + 1;
      if (write_offset == sizeof(frame))
      {
         queued_bytes -= sizeof(frame);
         write_queue = write_queue[1..];
         write_offset = 0;
      }
   }
   if (sizeof(write_queue))
   {
      if (!output_timer) output_timer = call_out(continue_output, 0);
   }
   else if (closing)
   {
      socket_close(fdOwned);
      native_closing = 1;
   }
}

private void continue_output()
{
   output_timer = 0;
   if (catch(flush_output())) fail_connection();
}

private void begin_close()
{
   if (closing) return;
   closing = 1;
   if (input_timer) remove_call_out(input_timer);
   input_timer = 0;
   i3_reset_input();
   close_timer = call_out(close_expired, CLOSE_WAIT);
   if (!ready || style == SKT_STYLE_LISTEN_M)
   {
      write_queue = ({});
      queued_bytes = write_offset = 0;
      socket_close(fdOwned);
      native_closing = 1;
   }
   else
      flush_output();
}

private void pump_input()
{
   for (int count = 0; count < READ_BATCH && !closing; count++)
   {
      mixed *packet = i3_next_packet();
      if (packet == nil)
      {
         if (input_eof)
         {
            i3_end_input();
            begin_close();
            notify_close();
         }
         return;
      }
      catch(evaluate(read_func, this_object(), packet));
      if (!this_object()) return;
   }
   if (!closing && !input_timer) input_timer = call_out(continue_input, 0);
}

private void continue_input()
{
   input_timer = 0;
   if (!closing && catch(pump_input())) fail_connection();
}

private void release_callback(int fd, object previous)
{
   if (origin() != "efun" || previous != offer_parent || previous_object() != offer_parent || fdOwned > 0)
      throw("socket: invalid ownership offer");
   socket_acquire(fd, event_callback);
   fdOwned = fd;
   ready = 1;
   mapping info = socket_info(fd);
   remote_endpoint = info["remote"];
   local_endpoint = info["local"];
   offer_parent = nil;
   catch(evaluate(read_func, this_object(), nil));
}

nomask function ownership_offer()
{
   if (previous_object() != offer_parent || fdOwned > 0)
      throw("socket: ownership offer is private to the listener");
   return release_callback;
}

private void accept_connection(int fd)
{
   object peer;
   int acquired;
   mixed failure = catch {
      peer = clone_object(SOCKET, SKT_STYLE_INT_ACQUIRE, read_func, close_func, this_object(), client);
      acquired = socket_release(fd, peer, peer->ownership_offer());
   };
   if (failure || !acquired)
   {
      if (peer) peer->remove();
      catch(socket_close(fd));
   }
}

private void dispatch_event(int fd, string name, mixed data)
{
   switch (name)
   {
   case "ready":
      ready = 1;
      remote_endpoint = data["remote"];
      local_endpoint = data["local"];
      flush_output();
      break;
   case "accept":
      if (closing) socket_close(data);
      else accept_connection(data);
      break;
   case "read":
      if (!closing) { i3_feed(data); pump_input(); }
      break;
   case "eof":
      if (!closing) { input_eof = 1; pump_input(); }
      break;
   case "drained":
      flush_output();
      if (!closing && !sizeof(write_queue) && output_active)
      {
         output_active = 0;
         if (write_func) catch(evaluate(write_func, this_object()));
      }
      break;
   case "closed":
      finish_connection();
      break;
   }
}

private void event_callback(int fd, string name, mixed data)
{
   if (origin() != "driver" || fd != fdOwned)
      throw("socket: invalid native event");
   if (catch(dispatch_event(fd, name, data)) && this_object()) fail_connection();
}

nomask void set_write_callback(function f)
{
   require_client();
   write_func = f;
}

varargs nomask void send(mixed message, string address)
{
   require_client();
   if (closing || fdOwned < 1 || style == SKT_STYLE_LISTEN_M)
      throw("socket: connection is not writable");
   if (address) throw("socket: MUD packets have no per-message address");
   bytes frame = i3_encode(message);
   if (sizeof(write_queue) >= MAX_OUTPUT_FRAMES || sizeof(frame) > MAX_OUTPUT_BYTES - queued_bytes)
      throw("socket: output queue limit");
   write_queue += ({frame});
   queued_bytes += sizeof(frame);
   output_active = 1;
   flush_output();
}

void remove()
{
   if (previous_object() != client && previous_object() != this_object() && previous_object() != find_object(SIMUL_OB) && previous_object() != offer_parent)
      throw("socket: caller does not control this connection");
   suppress_close = 1;
   if (fdOwned < 1) { finish_connection(); return; }
   if (catch(begin_close())) fail_connection();
}

nomask mixed *address() { return remote_endpoint ? remote_endpoint[0..] : nil; }
nomask int local_port() { return local_endpoint ? local_endpoint[1] : 0; }
nomask string local_address() { return local_endpoint ? local_endpoint[0] : nil; }

nomask mapping query_status()
{
   mapping info = fdOwned > 0 ? socket_info(fdOwned) : nil;
   return (["handle":fdOwned, "ready":ready, "closing":closing,
            "queued_bytes":queued_bytes, "queued_frames":sizeof(write_queue),
            "native_output_pending":info ? info["output_pending"] : 0,
            "pending_input":i3_pending_input(), "local":local_endpoint ? local_endpoint[0..] : nil,
            "remote":remote_endpoint ? remote_endpoint[0..] : nil]);
}

string stat_me()
{
   return sprintf("%O: style=%d local=%O remote=%O ready=%d closing=%d queue=%d bytes/%d frames\n",
                  this_object(), style, local_endpoint, remote_endpoint, ready, closing, queued_bytes, sizeof(write_queue));
}

varargs void create(int skt_style, mixed p1, mixed p2, mixed p3, mixed p4)
{
   if (!clonep()) return;
   if (configured) throw("socket: already configured");
   configured = 1;
   style = skt_style;
   client = previous_object();
   if (style == SKT_STYLE_INT_ACQUIRE)
   {
      if (!client || base_name(client) != SOCKET || !clonep(client) || p3 != client || !objectp(p4))
         throw("socket: only a listener can construct an accepted connection");
      offer_parent = client;
      client = p4;
      read_func = p1;
      close_func = p2;
   }
   else
   {
      read_func = p2;
      close_func = p3;
   }
   if (!functionp(read_func) || (close_func != nil && !functionp(close_func)))
      throw("socket: expected callbacks");
   switch (style)
   {
   case SKT_STYLE_CONNECT_M:
      if (!check_previous_privilege("Mudlib:socket"))
         throw("Insufficient privs to open an outgoing socket.");
      if (!stringp(p1)) throw("socket: expected host and port");
      string *parts = explode(p1, " ");
      if (sizeof(parts) != 2 || parts[0] == "" || parts[1] == "")
         throw("socket: expected host and port");
      foreach (int ch in to_bytes(parts[1], "UTF-8"))
         if (ch < 48 || ch > 57) throw("socket: invalid port");
      fdOwned = socket_connect(parts[0], to_int(parts[1]), event_callback);
      break;
   case SKT_STYLE_LISTEN_M:
      if (!intp(p1) || (p4 != nil && !stringp(p4))) throw("socket: invalid listen address");
      fdOwned = socket_listen(p4 == nil ? "0.0.0.0" : p4, p1, event_callback);
      break;
   case SKT_STYLE_INT_ACQUIRE:
      break;
   default:
      throw("socket: only MUD-mode TCP is supported by this adapter");
   }
}
