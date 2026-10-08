/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** state_d.c -- Changes states for objects on the MUD
** Tsath 2020-07-10 (Created)
*/

//: MODULE
// This daemon periodically calls objects on the MUD to allow them to change state. This can
// be used to grow crops, make food rot, make critically wounded things die.
//
// This daemon checks due work every STATE_POLL_SECONDS using a repeating call_out.
// See M_STATEFUL, M_DECAY, LIGHT_OBJECT and ^std/consumable/hamburger.c for some examples of usage.

#define MAX_PROCESSED 10
#define INITIAL_SPREAD 5
#define STATE_POLL_SECONDS 2

private nosave int heartbeat_id;
private nosave int processing;

int last_time;

mapping queue = ([]);

//: MODULE in_queue
// Checks if ob is in the queue (anywhere in queue).
int in_queue(object ob)
{
   foreach (mixed *targets in values(queue))
      foreach (mixed *target in targets)
         if (target[0] == ob)
            return 1;
   return 0;
}

//: FUNCTION add_to_queue
// Schedules ob after its interval in minutes, plus add_to_time in seconds.
// If force=1, the object is added even if already in queue.
varargs void add_to_queue(object ob, int add_to_time, int force, mixed extra)
{
   int update_time;
   if (!extra)
      extra = "scheduled";

   if (ob && ob->query_call_interval() && (force || !in_queue(ob)))
   {
      update_time = (ob->query_call_interval() * 60 + time()) + add_to_time;

      if (!queue[update_time])
         queue[update_time] = ({});

      queue[update_time] += ({({ob, extra})});
   }
}

//: FUNCTION add_to_queue_secs
// Useful for faster testing, should probably not be used in the real mudlib.
// If it's this brief, then use a call_out(). See add_to_queue for documentation.
// This function adds seconds, and not minutes.
varargs void add_to_queue_secs(object ob, int add_to_time, int force, mixed extra)
{
   int update_time;
   if (!extra)
      extra = "scheduled";
   if (ob && ob->query_call_interval() && (force || !in_queue(ob)))
   {
      update_time = (time()) + add_to_time;

      if (!queue[update_time])
         queue[update_time] = ({});

      queue[update_time] += ({({ob, extra})});
   }
}

//: FUNCTION add_to_queue_at_time
// Add to queue at a specific time. No checks, no requirements.
varargs void add_to_queue_at_time(mixed ob, int update_time, mixed extra)
{
   if (!extra)
      extra = "scheduled";

   if (!queue[update_time])
      queue[update_time] = ({});

   foreach (mixed *ar in queue[update_time])
   {
      if (ar[0] == ob && ar[1] == extra)
         return;
   }

   queue[update_time] += ({({ob, extra})});
}

//: FUNCTION remove_from_queue
// Removes ob from the queue nomatter when scheduled.
varargs void remove_from_queue(object ob, mixed extra)
{
   foreach (int update_time, mixed * targets in queue)
   {
      foreach (mixed *target in targets)
      {
         if (target[0] == ob)
         {
            if (extra && target[1] != extra)
               continue;
            queue[update_time] -= ({target});
         }
      }
      if (!sizeof(queue[update_time]))
         m_delete(queue, update_time);
   }
}

private
object *find_targets(string t)
{
   object *t_ar = ({});
   t_ar = filter(objects(), ( : base_name($1) == $(t) :));
   if (sizeof(t_ar) > 1)
      t_ar = filter(t_ar, ( : clonep($1) :));
   return t_ar;
}

private
void update_target(object ob, mixed extra)
{
   if (ob && ob->is_stateful(extra) && ob->state_update(extra) && ob)
      add_to_queue(ob, 0, 1, extra);
}

private
void process_queue()
{
   int processed;
   int now = time();
   foreach (int update_time in sort_array(keys(queue), 1))
   {
      if (update_time > now)
         break;
      while (sizeof(queue[update_time]) && processed < MAX_PROCESSED)
      {
         mixed *target = queue[update_time][0];
         mixed *remaining = queue[update_time][1..];
         if (stringp(target[0]))
         {
            object *matches = find_targets(target[0]);
            mixed *expanded = ({});
            foreach (object ob in matches)
               expanded += ({({ob, target[1]})});
            queue[update_time] = expanded + remaining;
            if (!sizeof(matches))
               processed++;
            continue;
         }

         queue[update_time] = remaining;
         if (!sizeof(remaining))
            m_delete(queue, update_time);
         processed++;
         mixed failure = catch(update_target(target[0], target[1]));
         if (failure)
            debug_message(sprintf("STATE_D update failed: %O\n", failure));
      }
      if (!sizeof(queue[update_time]))
         m_delete(queue, update_time);
      if (processed >= MAX_PROCESSED)
         return;
   }
}

private
void capture_all_statefuls()
{
   object *statefuls = filter(objects(), ( : clonep($1) && $1->is_stateful() :));
   queue = ([]);
   foreach (object ob in statefuls)
      add_to_queue(ob, random(INITIAL_SPREAD));
}

void heart_beat()
{
   if (processing)
      return;
   processing = 1;
   mixed failure = catch(process_queue());
   processing = 0;
   if (failure)
      throw(failure);
}

void create()
{
   if (clonep())
   {
      destruct(this_object());
      return;
   }
   capture_all_statefuls();
   EVENT_D->run_scheduler();
   if (!query_call_out(heartbeat_id))
      heartbeat_id = call_out(heart_beat, STATE_POLL_SECONDS, STATE_POLL_SECONDS);
}

mapping queue()
{
   return queue;
}

object *owned(object body)
{
   if (!body)
      body = this_body();
   object *result = ({});
   foreach (mixed *targets in values(queue))
      foreach (mixed *target in targets)
      {
         mixed ob = target[0];
         if (objectp(ob) && clonep(ob) && ob->query_owner() == body && member_array(ob, result) < 0)
            result += ({ob});
      }
   return result;
}

int unique(string bname, object body)
{
   return !sizeof(filter(owned(body), ( : base_name($1) == $(bname) :)));
}

string stat_me()
{
   string squeue = "";
   mapping q_out = ([]), q_args = ([]);

   // Need to use some muscle first to get the mappings into a shape where we can print them.
   // Fairly simple transformations thanksfully.
   foreach (int update_time in sort_array(keys(queue), 1))
   {
      mixed *targets = queue[update_time];
      foreach (mixed *t in targets)
      {
         if (!arrayp(q_out[t[0]]))
            q_out[t[0]] = ({});
         if (!arrayp(q_args[t[0]]))
            q_args[t[0]] = ({});
         q_out[t[0]] += ({update_time});
         q_args[t[0]] += ({t[1]});
      }
   }

   // Now we're ready to iterate over objects instead of timestamps.
   foreach (mixed ob, int *times in q_out)
   {
      if (sizeof(times) == 1)
         squeue +=
             sprintf("%-55s%-25s%-10s\n", shorten_filename(ob), q_args[ob][0], time_to_string(times[0] - time(), 1));
      else
         squeue += sprintf("%-55s%-25s%-10s\n", shorten_filename(ob),
                           implode(clean_array(q_args[ob]), "/") + " (" + sizeof(times) + ")",
                           time_to_string(times[0] - time(), 1));
   }

   return sprintf("%-55s%-25s%-10s\n", "Object", "Arguments", "Next") + sprintf("%92'-'s\n", "") + squeue +
          "\nThere are " + sizeof(flatten_array(values(STATE_D->queue()))) / 2 + " items in queue.";
}