/* Do not remove the headers from this file! see /USAGE for more info. */

#define I3_MAX_PAYLOAD 262144
#define I3_MAX_DEPTH 64
#define I3_MAX_VALUES 32768
#define I3_MAX_CHUNK 65536
#define I3_MAX_CHUNKS 64
#define I3_MAX_INPUT (I3_MAX_PAYLOAD + I3_MAX_CHUNK + 4)

private class i3_encoder
{
   int nodes;
   int *data;
   int at;
}

private class i3_decoder
{
   bytes data;
   int at;
   int end;
   int nodes;
}

nosave private bytes *i3_chunks = ({});
nosave private int i3_chunk_head;
nosave private int i3_chunk_offset;
nosave private int i3_queued_bytes;
nosave private int i3_header_bytes;
nosave private int i3_payload_length;
nosave private int *i3_body;
nosave private int i3_body_bytes;
nosave private int i3_input_failed;

private string i3_number(mixed value)
{
   string text = sprintf(floatp(value) ? "%f" : "%d", value);
   bytes data = to_bytes(text, "UTF-8");
   foreach (int ch in data)
      if ((ch < 48 || ch > 57) && ch != 45 && ch != 46)
         throw("I3: nonfinite number");
   return text;
}

private mixed i3_key(mixed value)
{
   if (value == nil)
      return 0;
   if (stringp(value) || intp(value) || floatp(value))
      return value;
   throw("I3: unsupported mapping key");
}

private int i3_measure(class i3_encoder state, mixed value, int depth)
{
   int size;
   if (++state.nodes > I3_MAX_VALUES)
      throw("I3: value limit");
   if (value == nil)
      return 1;
   if (intp(value) || floatp(value))
      return sizeof(i3_number(value));
   if (stringp(value))
   {
      bytes data = to_bytes(value, "UTF-8");
      size = sizeof(data) + 2;
      if (size >= I3_MAX_PAYLOAD)
         throw("I3: payload limit");
      foreach (int ch in data)
      {
         if (!ch)
            throw("I3: string contains NUL");
         if (ch == 34 || ch == 92 || ch == 13)
            size++;
      }
   }
   else if (arrayp(value))
   {
      if (depth >= I3_MAX_DEPTH)
         throw("I3: nesting limit");
      size = 4;
      foreach (mixed item in value)
      {
         size += i3_measure(state, item, depth + 1) + 1;
         if (size >= I3_MAX_PAYLOAD)
            throw("I3: payload limit");
      }
   }
   else if (mappingp(value))
   {
      mapping seen = ([]);
      if (depth >= I3_MAX_DEPTH)
         throw("I3: nesting limit");
      size = 4;
      foreach (mixed key, mixed item in value)
      {
         mixed wire_key = floatp(key) ? to_float(i3_number(key)) : i3_key(key);
         if (seen[wire_key])
            throw("I3: duplicate mapping key");
         seen[wire_key] = 1;
         size += i3_measure(state, key, depth + 1) + i3_measure(state, item, depth + 1) + 2;
         if (size >= I3_MAX_PAYLOAD)
            throw("I3: payload limit");
      }
   }
   else
      throw("I3: unsupported value");
   if (size >= I3_MAX_PAYLOAD)
      throw("I3: payload limit");
   return size;
}

private void i3_write_value(class i3_encoder state, mixed value)
{
   int *data = state.data;
   int at = state.at;
   if (value == nil || intp(value) || floatp(value))
   {
      bytes text = to_bytes(value == nil ? "0" : i3_number(value), "UTF-8");
      foreach (int ch in text)
         data[at++] = ch;
   }
   else if (stringp(value))
   {
      bytes text = to_bytes(value, "UTF-8");
      data[at++] = 34;
      foreach (int ch in text)
      {
         if (ch == 34 || ch == 92 || ch == 13)
            data[at++] = 92;
         data[at++] = ch == 10 ? 13 : ch;
      }
      data[at++] = 34;
   }
   else if (arrayp(value))
   {
      data[at++] = 40;
      data[at++] = 123;
      state.at = at;
      foreach (mixed item in value)
      {
         i3_write_value(state, item);
         data[state.at++] = 44;
      }
      at = state.at;
      data[at++] = 125;
      data[at++] = 41;
   }
   else
   {
      data[at++] = 40;
      data[at++] = 91;
      state.at = at;
      foreach (mixed key, mixed item in value)
      {
         i3_write_value(state, key);
         data[state.at++] = 58;
         i3_write_value(state, item);
         data[state.at++] = 44;
      }
      at = state.at;
      data[at++] = 93;
      data[at++] = 41;
   }
   state.at = at;
}

private void i3_require_packet(mixed value)
{
   if (!arrayp(value) || !sizeof(value) || !stringp(value[0]))
      throw("I3: packet must be an array with a string type");
}

protected bytes i3_encode(mixed *packet)
{
   class i3_encoder state = new(class i3_encoder);
   int length;
   i3_require_packet(packet);
   length = i3_measure(state, packet, 0) + 1;
   if (length > I3_MAX_PAYLOAD)
      throw("I3: payload limit");
   state.data = allocate(length + 4);
   state.data[0] = (length >> 24) & 255;
   state.data[1] = (length >> 16) & 255;
   state.data[2] = (length >> 8) & 255;
   state.data[3] = length & 255;
   state.at = 4;
   i3_write_value(state, packet);
   state.data[state.at] = 0;
   return to_bytes(state.data);
}

private int i3_peek(class i3_decoder state)
{
   if (state.at >= state.end)
      throw("I3: truncated value");
   return state.data[state.at];
}

private void i3_expect(class i3_decoder state, int ch)
{
   if (i3_peek(state) != ch)
      throw("I3: malformed value");
   state.at++;
}

private string i3_read_string(class i3_decoder state)
{
   bytes data = state.data;
   int start = ++state.at;
   int at = start;
   int count;
   int *result;
   // Scan before allocation so short strings do not reserve the rest of the frame.
   while (at < state.end && data[at] != 34)
   {
      if (data[at++] == 92 && at++ >= state.end)
         throw("I3: truncated string escape");
      count++;
   }
   if (at >= state.end)
      throw("I3: unterminated string");
   result = allocate(count);
   at = start;
   for (int i = 0; i < count; i++)
   {
      int ch = data[at++];
      if (ch == 92)
         ch = data[at++];
      else if (ch == 13)
         ch = 10;
      result[i] = ch;
   }
   state.at = at + 1;
   return to_text(to_bytes(result), "UTF-8");
}

private mixed i3_read_number(class i3_decoder state)
{
   bytes data = state.data;
   int start = state.at;
   int at = start;
   int real;
   string token;
   if (data[at] == 45)
      at++;
   if (at >= state.end || data[at] < 48 || data[at] > 57)
      throw("I3: malformed number");
   while (at < state.end && data[at] >= 48 && data[at] <= 57)
      at++;
   if (at < state.end && data[at] == 46)
   {
      real = 1;
      at++;
      if (at >= state.end || data[at] < 48 || data[at] > 57)
         throw("I3: malformed fraction");
      while (at < state.end && data[at] >= 48 && data[at] <= 57)
         at++;
   }
   if (at < state.end && data[at] == 101)
   {
      real = 1;
      at++;
      if (at >= state.end || (data[at] != 43 && data[at] != 45))
         throw("I3: malformed exponent");
      at++;
      if (at >= state.end || data[at] < 48 || data[at] > 57)
         throw("I3: malformed exponent");
      while (at < state.end && data[at] >= 48 && data[at] <= 57)
         at++;
   }
   if (at - start > 400)
      throw("I3: numeric token limit");
   token = to_text(data[start..at - 1], "UTF-8");
   state.at = at;
   if (real)
   {
      float value = to_float(token);
      i3_number(value);
      return value;
   }
   return to_int(token);
}

private mixed i3_read_value(class i3_decoder state, int depth)
{
   int ch;
   if (++state.nodes > I3_MAX_VALUES)
      throw("I3: value limit");
   ch = i3_peek(state);
   if (ch == 34)
      return i3_read_string(state);
   if (ch == 45 || (ch >= 48 && ch <= 57))
      return i3_read_number(state);
   if (ch != 40)
      throw("I3: unsupported value syntax");
   if (depth >= I3_MAX_DEPTH)
      throw("I3: nesting limit");
   state.at++;
   ch = i3_peek(state);
   state.at++;
   if (ch == 123)
   {
      mixed *items = ({});
      int count;
      while (i3_peek(state) != 125)
      {
         mixed item = i3_read_value(state, depth + 1);
         i3_expect(state, 44);
         if (count == sizeof(items))
            items += allocate(count ? count : 8);
         items[count++] = item;
      }
      state.at++;
      i3_expect(state, 41);
      return count ? items[0..count - 1] : ({});
   }
   if (ch == 91)
   {
      mapping result = ([]);
      mapping seen = ([]);
      while (i3_peek(state) != 93)
      {
         mixed key = i3_key(i3_read_value(state, depth + 1));
         mixed value;
         i3_expect(state, 58);
         value = i3_read_value(state, depth + 1);
         i3_expect(state, 44);
         if (seen[key])
            throw("I3: duplicate mapping key");
         seen[key] = 1;
         result[key] = value;
      }
      state.at++;
      i3_expect(state, 41);
      return result;
   }
   throw("I3: unsupported container syntax");
}

protected mixed *i3_decode(bytes payload)
{
   int length = sizeof(payload);
   class i3_decoder state;
   mixed value;
   if (length < 1 || length > I3_MAX_PAYLOAD || payload[length - 1] != 0)
      throw("I3: invalid payload length or terminator");
   for (int i = 0; i < length - 1; i++)
      if (!payload[i])
         throw("I3: interior NUL");
   state = new(class i3_decoder, data: payload, end: length - 1);
   value = i3_read_value(state, 0);
   if (state.at != state.end)
      throw("I3: trailing payload");
   i3_require_packet(value);
   return value;
}

protected void i3_reset_input()
{
   i3_chunks = ({});
   i3_chunk_head = 0;
   i3_chunk_offset = 0;
   i3_queued_bytes = 0;
   i3_header_bytes = 0;
   i3_payload_length = 0;
   i3_body = nil;
   i3_body_bytes = 0;
   i3_input_failed = 0;
}

private void i3_fail_input(string reason)
{
   i3_reset_input();
   i3_input_failed = 1;
   throw("I3: " + reason);
}

private void i3_require_input()
{
   if (i3_input_failed)
      throw("I3: failed input requires reset");
}

protected int i3_pending_input()
{
   return i3_queued_bytes + i3_header_bytes + i3_body_bytes;
}

protected void i3_feed(bytes chunk)
{
   int length = sizeof(chunk);
   i3_require_input();
   if (!length)
      return;
   if (length > I3_MAX_CHUNK || length > I3_MAX_INPUT - i3_pending_input())
      i3_fail_input("input byte limit");
   if (i3_chunk_head)
   {
      i3_chunks = i3_chunks[i3_chunk_head..];
      i3_chunk_head = 0;
   }
   if (sizeof(i3_chunks) >= I3_MAX_CHUNKS)
      i3_fail_input("input chunk limit");
   i3_chunks += ({chunk});
   i3_queued_bytes += length;
}

private void i3_advance_chunk(int amount)
{
   i3_chunk_offset += amount;
   i3_queued_bytes -= amount;
   if (i3_chunk_offset == sizeof(i3_chunks[i3_chunk_head]))
   {
      i3_chunk_head++;
      i3_chunk_offset = 0;
      if (i3_chunk_head == sizeof(i3_chunks))
      {
         i3_chunks = ({});
         i3_chunk_head = 0;
      }
   }
}

protected mixed *i3_next_packet()
{
   i3_require_input();
   while (i3_queued_bytes)
   {
      bytes chunk = i3_chunks[i3_chunk_head];
      if (i3_header_bytes < 4)
      {
         i3_payload_length = (i3_payload_length << 8) | chunk[i3_chunk_offset];
         i3_header_bytes++;
         i3_advance_chunk(1);
         if (i3_header_bytes == 4)
         {
            if (i3_payload_length < 1 || i3_payload_length > I3_MAX_PAYLOAD)
               i3_fail_input("invalid frame length");
            i3_body = allocate(i3_payload_length);
         }
         continue;
      }
      int count = sizeof(chunk) - i3_chunk_offset;
      int needed = i3_payload_length - i3_body_bytes;
      if (count > needed)
         count = needed;
      for (int i = 0; i < count; i++)
         i3_body[i3_body_bytes + i] = chunk[i3_chunk_offset + i];
      i3_body_bytes += count;
      i3_advance_chunk(count);
      if (i3_body_bytes == i3_payload_length)
      {
         mixed *packet;
         mixed failure = catch(packet = i3_decode(to_bytes(i3_body)));
         if (failure)
         {
            i3_reset_input();
            i3_input_failed = 1;
            throw(failure);
         }
         i3_header_bytes = 0;
         i3_payload_length = 0;
         i3_body = nil;
         i3_body_bytes = 0;
         return packet;
      }
   }
   return nil;
}

protected void i3_end_input()
{
   i3_require_input();
   if (i3_pending_input())
      i3_fail_input("incomplete or unconsumed input at EOF");
}
