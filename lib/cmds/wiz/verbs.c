/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
// USAGE:
//
//    |  ``verbs``
//    |  ``verbs <verb>``
//
// This command will show you the rules that a certain verbs has,
// or a list of all the verbs and their rules
//
// ``verbs``
//
//   |  ffear: STR
//   |  move: OBJ
//   |  look: OBJ with OBJ, STR, at OBJ with OBJ, STR OBJ, for OBJ, for LIV, at OBJ, OBJ
//   |  open: OBJ with OBJ, up OBJ, OBJ
//   |  pour: OBJ in OBJ
//   |  get: OBJ with OBJ, OBJ out of OBJ, OBJ from OBJ, OBJ
//   |  wind: OBJ with OBJ, OBJ
//   |  put: OBJ in OBJ
//   |  give: OBJ to LIV
//   |  wait:
//   |  whisper: to LIV STR, STR to LIV, LIV STR
//   |  close: OBJ
//   |  fill: OBJ with OBJ, OBJ
//   |  exit: OBJ
//   |  go: in OBJ, into OBJ, on OBJ, over OBJ, to OBJ, around OBJ, up OBJ, down OBJ, STR
//
// ``verbs go``
//
//   |  go: in OBJ, into OBJ, on OBJ, over OBJ, to OBJ, around OBJ, up OBJ, down OBJ, STR
//
// .. TAGS: RST

inherit CMD;

private
void main(string str)
{
   mapping rules = ([]);
   string *verbs = ({});

   if (str == "")
      str = nil;

   foreach (mapping entry in query_parser_registry())
   {
      string verb = entry["verb"];

      if (str && verb != str)
         continue;
      if (!has_key(rules, verb))
      {
         verbs += ({ verb });
         rules[verb] = ({});
      }
      rules[verb] += ({ entry["rule"] });
   }

   foreach (string verb in verbs)
   {
      mapping emote = SOUL_D->query_emote(verb);
      int first = 1;

      foreach (string rule in rules[verb])
      {
         if (emote && emote[rule])
            continue;
         if (first)
         {
            outf("\n%s: ", verb);
            first = 0;
         }
         else
            out(", ");
         out(rule);
      }
   }
   out("\n");
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main(arg);
}
