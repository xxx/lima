/* Do not remove the headers from this file! see /USAGE for more info. */

inherit LIVING;
inherit M_ACTIONS;
inherit M_TRIGGERS;

int starts_with(string str, string pref)
{
   return str[0..strlen(pref)-1] == pref;
}

int ends_with(string str, string suff)
{
   return str[ < strlen(suff)..] == suff;
}

void pattern_setup()
{
   add_pattern(
       "%s says: %s.",
       (: [string name, mixed sub]
          if (functionp(sub))
          {
             if (evaluate(sub, lower_case(name)))
                return "say That's true.";
             else
                return "say You're lying.";
          }
          if (sub && intp(sub))
             return "say I can't figure out what you mean.";
          if (random(5))
             return 0;
          return "say That's boring.  I want to know things about your name.";
       :),
       nil, "say_rule");

   add_sub_pattern("say_rule", "%s name %s", ( : $2:), nil, "predicate");

   add_sub_pattern(
       "predicate", "%s or %s",
       (: [function f1, function f2]
          return ( : evaluate($(f1), $1) || evaluate($(f2), $1) :);
       :),
       "and_pred", "predicate");
   add_sub_pattern("predicate", "%s", ( : $1:), "and_pred");

   add_sub_pattern(
       "and_pred", "%s and %s",
       (: [function f1, function f2]
          return ( : evaluate($(f1), $1) && evaluate($(f2), $1) :);
       :),
       "simple_pred", "and_pred");
   add_sub_pattern("and_pred", "%s", ( : $1:), "simple_pred");

   add_sub_pattern("simple_pred", "is not %s", (: [string name] return ( : $1 != $(name) :); :));
   add_sub_pattern("simple_pred", "is %s", (: [string name] return ( : $1 == $(name) :); :));
   add_sub_pattern(
       "simple_pred", "starts with %s", (: [string prefix]
          return ( : starts_with($1, $(prefix)) :);
       :));
   add_sub_pattern("simple_pred", "ends with %s", (: [string suffix] return ( : ends_with($1, $(suffix)) :); :));
   add_sub_pattern("simple_pred", nil, 1);
}

void setup()
{
   set_name("Logical Genius");
   set_gender(1);
   set_proper_name("Logical Genius");
   set_in_room_desc("A Logical Genius eyes you suspiciously.");
   set_id("genius", "dude");
   set_long("Logical Genius peers at you.\nLogical Genius says: Tell me something about your name.");
   pattern_setup();
}
