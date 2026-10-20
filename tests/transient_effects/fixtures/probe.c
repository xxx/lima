inherit "/contrib/transient_effect/transient_effect";

private int remove_during_effect;

void setup()
{
   set_effect_type("probe");
   set_effect_name("probe");
}

void set_remove_during_effect() { remove_during_effect = 1; }

void do_effect(object affected)
{
   "/secure/master"->record_effect(this_object(), affected, efun::this_player());
   if (remove_during_effect)
      remove();
}
