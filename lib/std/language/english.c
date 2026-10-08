// Local Lima port experiment; the driver does not select a language.
private mapping exceptions = ([
    "are": ({3, "is"}), "bus": ({0, "es"}), "bonus": ({0, "es"}),
    "child": ({0, "ren"}), "cliff": ({0, "s"}), "datum": ({2, "a"}),
    "die": ({1, "ce"}), "deer": ({0, ""}), "do": ({0, "es"}),
    "dynamo": ({0, "s"}), "foot": ({3, "eet"}), "fish": ({0, ""}),
    "forum": ({2, "a"}), "fife": ({0, "s"}), "glasses": ({0, ""}),
    "goose": ({4, "eese"}), "go": ({0, "es"}), "gum": ({0, "s"}),
    "giraffe": ({0, "s"}), "human": ({0, "s"}), "have": ({2, "s"}),
    "index": ({2, "ices"}), "louse": ({4, "ice"}), "lotus": ({0, "s"}),
    "mackerel": ({0, ""}), "moose": ({0, ""}), "mouse": ({4, "ice"}),
    "matrix": ({1, "ces"}), "mech": ({0, "s"}), "marquis": ({0, "es"}),
    "ox": ({0, "en"}), "pants": ({0, ""}), "penis": ({0, "es"}),
    "quaff": ({0, "s"}), "remains": ({0, ""}), "roof": ({0, "s"}),
    "sniff": ({0, "s"}), "sheep": ({0, ""}), "sphinx": ({1, "ges"}),
    "staff": ({2, "ves"}), "safe": ({0, "s"}), "shaman": ({0, "s"}),
    "thief": ({1, "ves"}), "tooth": ({4, "eeth"}), "talisman": ({0, "s"}),
    "vax": ({0, "en"}), "virus": ({0, "es"}), "was": ({2, "ere"})
]);

private string replace_ending(string word, int count, string suffix)
{
    int kept = sizeof(word) - count;
    return (kept > 0 ? word[0..kept - 1] : "") + suffix;
}

private string inflect_word(string word)
{
    mixed rule;
    int size = sizeof(word);
    int before;

    if (!size) return word;
    rule = exceptions[lower_case(word)];
    if (arrayp(rule)) return replace_ending(word, rule[0], rule[1]);
    before = size > 1 ? word[size - 2] : 0;

    // Case tests retain the reviewed legacy suffix behavior for compatibility.
    switch (word[size - 1]) {
    case 'E': case 'e':
        if (before == 'f' || before == 'F') return replace_ending(word, 2, "ves");
        break;
    case 'F': case 'f':
        if (before != 'e' && before != 'E' && before != 'f' && before != 'F')
            return replace_ending(word, 1, "ves");
        break;
    case 'H': case 'h':
        if (before == 'c' || before == 's') return word + "es";
        break;
    case 'N': case 'n':
        if (size > 2 && before == 'a' && word[size - 3] == 'm')
            return replace_ending(word, 3, "men");
        break;
    case 'O': case 'o':
        if (size > 1 && before != 'o') return word + "es";
        break;
    case 'S': case 's':
        if (before == 'i') return replace_ending(word, 2, "es");
        if (before == 'u') return replace_ending(word, 2, "i");
        if (before == 'a' || before == 'e' || before == 'o') return word + "ses";
        return word + "es";
    case 'X': case 'x':
        return word + "es";
    case 'Y': case 'y':
        if (size > 1 && member_array(before, "aeiou") == -1)
            return replace_ending(word, 1, "ies");
        break;
    case 'Z': case 'z':
        if (member_array(before, "aeiou") != -1) return word + "zes";
        return word + "es";
    }
    return word + "s";
}

string pluralize(string phrase)
{
    string tail = "";
    string prefix = "";
    int boundary = strindex(phrase, " of ");

    if (boundary >= 0) {
        tail = phrase[boundary..];
        phrase = boundary ? phrase[0..boundary - 1] : "";
    }
    if (phrase[0..1] == "a " || phrase[0..1] == "A ")
        phrase = phrase[2..];
    else if (phrase[0..2] == "an " || phrase[0..2] == "An ")
        phrase = phrase[3..];

    boundary = strindex(phrase, " ", 1);
    if (boundary >= 0) {
        prefix = phrase[0..boundary];
        phrase = phrase[boundary + 1..];
    }
    return prefix + inflect_word(phrase) + tail;
}
