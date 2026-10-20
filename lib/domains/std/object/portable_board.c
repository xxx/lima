/* Do not remove the headers from this file! see /USAGE for more info. */

// This is a portable bulletin board
// Its simple, but since it took me a while to get it working right I
// figured I would save you the trouble.
// Vette  April 16, 1997

inherit "/contrib/board";
inherit M_GETTABLE;

void mudlib_setup()
{
   ::mudlib_setup("Portable Bulletin Board", BUG_NEWSGROUP, 1);
}

string a_short()
{
   return query_in_room_desc();
}
