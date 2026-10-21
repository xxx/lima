inherit ARMOUR;

private int unwear_calls;

void do_remove()
{
   unwear_calls++;
   ::do_remove();
}

int query_unwear_calls() { return unwear_calls; }
