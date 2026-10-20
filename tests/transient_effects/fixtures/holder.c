inherit "/std/container";

private int living = 1;
private int bound;
private int stuns;
private int heals;
private int commands;
private int actions;

void setup()
{
   add_relation("in", 10000);
   set_default_relation("in");
}

int is_living() { return living; }
void set_living(int value) { living = value; }
void set_bound(int value) { bound = value; }
int query_stuns() { return stuns; }
int query_heals() { return heals; }
int query_commands() { return commands; }
int query_actions() { return actions; }

void stun_us(int amount)
{
   if (amount < 0 || amount >= max(1, bound))
      throw("EFFECT_FAIL: stun amount outside strength bounds");
   stuns++;
}

void heal_us(int amount)
{
   if (amount < 0 || amount >= max(1, bound))
      throw("EFFECT_FAIL: healing amount outside strength bounds");
   heals++;
}

void force_game_command(string command)
{
   if (member_array(command, ({"trip", "stumble", "appear dizzy", "burp", "hiccup", "puke"})) == -1)
      throw("EFFECT_FAIL: unexpected drunkenness command");
   commands++;
}

void simple_action(string action)
{
   if (action != "$N $vdrool.")
      throw("EFFECT_FAIL: unexpected drunkenness action");
   actions++;
}
