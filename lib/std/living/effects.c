/* Do not remove the headers from this file! see /USAGE for more info. */

inherit CLASS_EFFECT;

private mixed *effects = ({});
nosave private int tag;
nosave private class effect_class scheduled_effect;
nosave private int processing;
nosave private int clear_generation;

//: MODULE
// Relative-delay effects queue with one owned call_out.
// Positive counters tick N times, then end on the following firing;
// zero ends on the first firing and negative counters repeat indefinitely.

private int effect_position(class effect_class effect)
{
   for (int i = 0; i < sizeof(effects); i++)
      if (effects[i] == effect)
         return i;
   return -1;
}

private void cancel_timer()
{
   if (tag)
      remove_call_out(tag);
   tag = 0;
   scheduled_effect = nil;
}

private void sync_queue()
{
   if (processing || !tag)
      return;
   mixed *timer = query_call_out(tag);
   if (!timer)
   {
      cancel_timer();
      if (sizeof(effects))
         effects[0].delay = 0;
      return;
   }
   // Stored delays are whole seconds; retain the live timer's finer deadline.
   int remaining = timer[2] > 0 ? timer[2] : 0;
   if (sizeof(effects))
      effects[0].delay = remaining / 1000 + (remaining % 1000 != 0);
}

private void schedule_next()
{
   if (processing)
      return;
   if (sizeof(effects) && tag && scheduled_effect == effects[0] && query_call_out(tag))
      return;
   cancel_timer();
   if (sizeof(effects))
   {
      scheduled_effect = effects[0];
      tag = call_out(next_effect, effects[0].delay);
   }
}

private void splice_effect(class effect_class effect, int pos)
{
   effects = (pos ? effects[0..pos - 1] : ({})) + ({effect}) + effects[pos..];
}

private void erase_effect(int pos)
{
   if (pos + 1 < sizeof(effects))
      effects[pos + 1].delay += effects[pos].delay;
   effects = (pos ? effects[0..pos - 1] : ({})) + effects[pos + 1..];
}

private void report_effect_error(class effect_class effect, mixed failure)
{
   catch(dump("Effect callback failed:", effect.effect_ob, failure));
}

//: FUNCTION time_to_next_effect
// Returns whole seconds remaining, or -1 when no owned timer exists.
int time_to_next_effect()
{
   mixed *timer = tag ? query_call_out(tag) : nil;
   if (!timer)
      return -1;
   return timer[2] < 0 ? 0 : timer[2] / 1000;
}

//: FUNCTION actual_period
// Accepts seconds, ({fixed, random_range}), or a function returning seconds.
int actual_period(mixed val)
{
   if (intp(val))
      return val;
   if (arrayp(val) && sizeof(val) == 2 && intp(val[0]) && intp(val[1]))
      return val[0] + random(val[1]);
   if (functionp(val))
   {
      mixed period = evaluate(val);
      if (!intp(period))
         error("Effect interval callback must return an integer");
      return period;
   }
   return 0;
}

//: FUNCTION remove_effect_at
// Removes an entry without an end hook; returns 0 for an invalid index.
int remove_effect_at(int pos)
{
   if (pos < 0 || pos >= sizeof(effects))
      return 0;
   sync_queue();
   erase_effect(pos);
   schedule_next();
   return 1;
}

//: FUNCTION find_effect_index
// Returns the first object-path match, or -1.
int find_effect_index(string ob)
{
   if (ob)
      for (int i = 0; i < sizeof(effects); i++)
         if (effects[i].effect_ob == ob)
            return i;
   return -1;
}

//: FUNCTION find_effect_name_index
// Returns the first exact-name match, or -1.
int find_effect_name_index(string name)
{
   if (name)
      for (int i = 0; i < sizeof(effects); i++)
         if (effects[i].name == name)
            return i;
   return -1;
}

//: FUNCTION find_effect_indexes_matching
// Returns indexes matching a nonempty name prefix.
int *find_effect_indexes_matching(string name)
{
   int *res = ({});
   if (name)
      for (int i = 0; i < sizeof(effects); i++)
         if (effects[i].name[0..strlen(name) - 1] == name)
            res += ({i});
   return res;
}

//: FUNCTION find_effect
// Returns the live effect class, or integer zero when absent.
mixed find_effect(string ob)
{
   int idx = find_effect_index(ob);
   return idx > -1 ? effects[idx] : 0;
}

//: FUNCTION query_effect_args
// Returns the live payload, or integer zero when absent.
mixed query_effect_args(string ob)
{
   int idx = find_effect_index(ob);
   return idx > -1 ? effects[idx].args : 0;
}

//: FUNCTION remove_effect
// Removes the first object-path match without an end hook.
int remove_effect(string ob)
{
   return remove_effect_at(find_effect_index(ob));
}

//: FUNCTION remove_effect_named
// Removes the first exact-name match without an end hook.
int remove_effect_named(string name)
{
   return remove_effect_at(find_effect_name_index(name));
}

//: FUNCTION remove_effects_matching
// Removes all prefix matches and reports whether any were found.
int remove_effects_matching(string name)
{
   int *matches = find_effect_indexes_matching(name);
   if (!sizeof(matches))
      return 0;
   sync_queue();
   foreach (int idx in sort_array(matches, -1))
      erase_effect(idx);
   schedule_next();
   return 1;
}

//: FUNCTION insert_effect_at
// Low-level positional splice; the supplied delay is relative to its predecessor.
void insert_effect_at(class effect_class effect, int pos)
{
   if (!effect || pos < 0 || pos > sizeof(effects) || effect_position(effect) >= 0)
      error("insert_effect_at: invalid position or duplicate effect");
   sync_queue();
   if (effect.delay < 0)
      effect.delay = 0;
   splice_effect(effect, pos);
   schedule_next();
}

//: FUNCTION insert_effect
// Inserts a delay measured from now, preserving arrival order at equal deadlines.
int insert_effect(class effect_class effect)
{
   if (!effect || effect_position(effect) >= 0)
      return 0;
   sync_queue();
   int delay = effect.delay > 0 ? effect.delay : 0;
   int pos;
   while (pos < sizeof(effects) && delay >= effects[pos].delay)
   {
      delay -= effects[pos].delay;
      pos++;
   }
   effect.delay = delay;
   if (pos < sizeof(effects))
      effects[pos].delay -= delay;
   splice_effect(effect, pos);
   schedule_next();
   return 1;
}

private mixed *invoke_effect(class effect_class effect)
{
   object ob = load_object(effect.effect_ob);
   if (!ob)
      error("Cannot load effect: " + effect.effect_ob);
   if (!effect.counter)
   {
      ob->end_effect(this_object(), effect.args, 0);
      return ({0});
   }
   if (effect.counter > 0)
      effect.counter--;
   mixed args = ob->do_effect(this_object(), effect.args, effect.counter);
   int delay = actual_period(effect.interval);
   return ({1, args, delay});
}

//: FUNCTION next_effect
// Fires one entry; a direct call also consumes the pending timer.
void next_effect()
{
   if (processing)
      return;
   cancel_timer();
   if (!sizeof(effects))
      return;
   class effect_class effect = effects[0];
   effect.delay = 0;
   int ending = !effect.counter;
   if (ending)
      erase_effect(0);
   processing++;
   mixed *result;
   mixed failure = catch(result = invoke_effect(effect));
   int pos = effect_position(effect);
   if (!ending && pos >= 0)
   {
      erase_effect(pos);
      if (!stringp(failure))
      {
         effect.args = result[1];
         effect.delay = result[2];
         insert_effect(effect);
      }
   }
   if (stringp(failure))
      report_effect_error(effect, failure);
   processing--;
   schedule_next();
}

//: FUNCTION clear_effects
// Detaches the queue before end hooks; effects those hooks add survive the clear.
void clear_effects()
{
   cancel_timer();
   clear_generation++;
   mixed *removed = effects;
   effects = ({});
   processing++;
   foreach (class effect_class effect in removed)
   {
      object ob = load_object(effect.effect_ob);
      if (ob && function_exists("end_effect", ob))
      {
         mixed failure = catch(ob->end_effect(this_object(), effect.args, effect.counter));
         if (stringp(failure))
            report_effect_error(effect, failure);
      }
   }
   processing--;
   schedule_next();
}

//: FUNCTION query_effects
// Returns a deep copy with the head delay synchronized to the live timer.
mixed *query_effects()
{
   sync_queue();
   return deep_copy(effects);
}

//: FUNCTION add_effect
// Omitted repeats/interval use effect defaults; explicit zeros remain zeros.
void add_effect(string ob, mixed args = nil, mixed *options...)
{
   if (sizeof(options) > 2)
      error("add_effect: too many arguments");
   object obj = load_object(ob);
   if (!obj)
      return;
   int generation = clear_generation;
   int repeats = sizeof(options) ? (int)options[0] : obj->query_repeats();
   mixed interval = sizeof(options) > 1 ? options[1] : obj->query_interval();
   class effect_class effect = new(class effect_class,
      effect_ob: ob, name: obj->query_name(), args: args,
      interval: interval, counter: repeats, delay: actual_period(interval));
   if (function_exists("start_effect", obj))
      effect.args = obj->start_effect(this_object(), args, repeats, interval);
   if (generation == clear_generation)
      insert_effect(effect);
}

//: FUNCTION reinstate_effects
// Restored queues tick immediately; active or reentrant reinstatement is a no-op.
void reinstate_effects()
{
   if (processing || (tag && query_call_out(tag)))
      return;
   cancel_timer();
   int generation = clear_generation;
   mixed *restored = effects + ({});
   processing++;
   foreach (class effect_class effect in restored)
   {
      if (effect_position(effect) < 0)
         continue;
      object ob = load_object(effect.effect_ob);
      mixed args;
      mixed failure;
      if (!ob)
         failure = "Cannot load effect: " + effect.effect_ob;
      else if (function_exists("reinstate_effect", ob))
         failure = catch(args = ob->reinstate_effect(this_object(), effect.args, effect.counter));
      else
         args = effect.args;
      int pos = effect_position(effect);
      if (pos >= 0)
      {
         if (stringp(failure))
            erase_effect(pos);
         else
            effect.args = args;
      }
      if (stringp(failure))
         report_effect_error(effect, failure);
   }
   processing--;
   if (generation == clear_generation)
      next_effect();
   else
      schedule_next();
}
