/* Do not remove the headers from this file! see /USAGE for more info. */

// Regex associations use native Unicode patterns and search each remaining suffix.
private string association_span(string text, int begin, int end)
{
    return begin == end ? "" : text[begin..end - 1];
}

mixed *reg_assoc(string text, string *patterns, mixed *tokens, mixed fallback = 0)
{
    string *pieces = ({});
    mixed *labels = ({});
    int cursor = 0, emitted = 0, size = sizeof(text);

    if (!stringp(text))
        throw("reg_assoc: text must be a string.");
    if (!arrayp(patterns) || !arrayp(tokens))
        throw("reg_assoc: patterns and tokens must be arrays.");
    if (sizeof(patterns) != sizeof(tokens))
        throw("Pattern and token array sizes must be identical.");
    foreach (string pattern : patterns)
        regmatch("", pattern);

    while (cursor < size)
    {
        int winner = -1, begin = size + 1, end = size;
        string suffix = text[cursor..];
        for (int i = 0; i < sizeof(patterns); i++)
        {
            mixed found = regmatch(suffix, patterns[i], 0x1000);
            if (!arrayp(found))
                continue;
            int length = sizeof(found[0]);
            int finish = cursor + found[<1] - (length == 0);
            int start = finish - length;
            if (start < begin)
            {
                winner = i;
                begin = start;
                end = finish;
            }
        }
        if (winner == -1)
            break;
        pieces += ({ association_span(text, emitted, begin), association_span(text, begin, end) });
        labels += ({ fallback, tokens[winner] });
        emitted = end;
        cursor = end + (begin == end);
    }
    return ({ pieces + ({ association_span(text, emitted, size) }), labels + ({ fallback }) });
}
