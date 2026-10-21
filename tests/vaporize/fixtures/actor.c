inherit ADVERSARY;

void setup()
{
   set_name("vaporization test creature");
   set_mass(5.0);
}

void trigger_death_hook() { die(); }
