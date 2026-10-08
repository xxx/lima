/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** gmcp.c -- gmcp handling
**
** 2024, Tsath created.
*/

object query_body();
private
int gmcp;

void gmcp_enable()
{
   TBUG("GMCP enabled");
   gmcp = 1;
}

int uses_gmcp()
{
   TBUG("uses_gmcp?");
   mapping connection = query_connection(this_object());
   return connection && connection["gmcp"];
}

void gmcp_message(string s)
{
   if (!s || s == "")
      return;
   if (uses_gmcp())
   {
      int split = strindex(s, " ");
      if (split == 0)
         return;
      send_gmcp(this_object(), split < 0 ? s : s[0..split-1], split < 0 ? "" : s[split+1..]);
   }
}
