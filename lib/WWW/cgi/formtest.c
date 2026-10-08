/* Do not remove the headers from this file! see /USAGE for more info. */

// POST test.

varargs string main(mapping form_info, mapping headers)
{
   return sprintf("%O\n", form_info);
}
