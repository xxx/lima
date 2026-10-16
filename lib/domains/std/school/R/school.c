// May - 2000

#include "../school.h"
inherit INDOOR_ROOM;

void setup(){
  set_brief("Wizard's Academy");
  set_long("   This is an abnormally large propylaeum to the Wizards Academy here\n"
           "on Lima Bean. A large gate towers so high here that one has to crack \n"
           "their neck to see the top of it. On the left and right side of the gate\n"
           "are two massive Gamorrean guards, who grunt and snort while waiting \n"
           "for their next trespasser to prey upon. On the other side of the gate \n"
           "is a long stoa lined with tombs that contain sacred knowledge of the \n"
           "ancients only those who have become masters or gained tutelage under \n"
           "them can comprehend these texts. Impressive this vantage point may be, \n"
           "there are many things to learn.\n"
  );

  add_item("gate",  
   ([ "look" :("   The gate is approximately 30 to 40 feet high. It is made of Laboi \n"
               "Crystals. The form work of the gate is beyond measure. To begin to \n"
               "\"up\" : R + \"newwiz1\",\n"
               "describe it would fail to do it justice. Instead it can only be \n"
               "passionately explored to its indescrible detail. One aspect really \n"
               "paradoxically shines is a titanium doorbutton, which blips and flickers.\n"),
   ]) );

  add_item("crystals", 
   ([ "look" :("   These are the crystals of Laboi. They are highly sought through out \n"
               "the universe for its rarity and beauty. However, Laboi crystals can only \n"
               "be cultivated by a gentle mammalian race that live on Laboi II. The Laboi \n"
               "are a nomadic tribe who search for the dangerous yans. One important \n"
               "aspect of developing these crystals is they need telekinesis to affect \n"
               "how they grow and shape. Ponder the one who was powerful enough to form \n"
               "these crystals into a monolithic gate.\n"),
   ]) );

  add_item("doorbutton",
   ([ "look" :("   The titatium door button has different hues of blue and silver. It \n"
               "blips and bloops to a presence that approaches it. The lights glow into \n"
               "red and white making most familar sounds. Aside the light is a port. Its \n"
               "complex circuitry requires some knowledge.\n"),
   ]) );

  add_item("port",
   ([ "look" :("   The port is circular with varying lenths of radial angles. \n"
               "Interlocking plates whir momentarily and make fasciating geometric \n"
               "combinations. The answer to this intresting port lies only in the memory \n"
               "of the past. Also in the port is a hollowed out core.\n"),
   ]) );


  add_item("light",
   ([ "look" :("    Yep its a light, nothing fascinating about the light. Except, maybe \n"
               "the fact it changes colors from white to blue and red most fluidly. \n"
               "Apparently the light is filled with a vitreous fluid is remarkably cool \n"
               "to the touch. It is probably made of a crystalline type of fluid.\n"),
   ]) );


  add_item("guard",
   ([ "look" :("Which guard? There are two guards. left or right?\n"),
   ]) );

  add_item("left guard",
   ([ "look" :("  The short Gamorrean guard looks like a joke. However, it would not be \n"
               "wise to under estimate him. For he is not just short but very stout and \n"
               "heavy boned. He wields a 40 centimenter vibro blade. He is adorned with \n"
               "bare essentials and reeks of the most foul odors. His cousin guards the \n"
               "right side some thing is particular about the right guard.\n"),
   ]) );

  add_item("right guard",
   ([ "look" :("This is an abnormally large guard, especially, for a Gamorrean guard. He \n"
               "stands 2.5 meters tall. Yet, in addition to to being tall he is massively \n"
               "built. One would estimate he is about 500 kilograms. Certainly such a \n"
               "rarity in Gamorrean guard must come high priced. This leads to some \n"
               "assumptions about the size of the left guard.\n"),
   ]) );

  add_item("stoa",
   ([ "look" :("Alas, my friend, you can see good but you don't have x-ray vision through \n"
               "the gate. Also, its telekinetic nature hinders force-vision as well.\n"),
   ]) );

  add_item("tomes",
   ([ "look" :("Alas, my friend, you can see good but you don't have x-ray vision through \n"
               "the gate. Also, its telekinetic nature hinders force-vision as well.\n"),
   ]) );

  set_exits( ([
    "newwiz" : R "newwiz1",
    "forward" : R "corridor01",
   ]) );
  set_objects( ([
    O "map" : 1,
   ]) );
}
