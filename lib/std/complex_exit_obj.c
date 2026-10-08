/* Do not remove the headers from this file! see /USAGE for more info. */

inherit M_COMPLEX_EXIT;
inherit OBJ base_object;

string stat_me()
{
   return base_object::stat_me() + m_complex_exit::stat_me();
}
