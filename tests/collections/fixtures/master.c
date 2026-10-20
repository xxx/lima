inherit "/contrib/last_queue";
inherit "/contrib/stack";
private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
private void verify_collection(int passed, string label) {
    if (!passed) throw("COLLECTION_FAIL: " + label);
    checks++;
    dump("COLLECTION_OK: " + label + "\n");
}
private int contents(mixed *actual, mixed *expected) {
    if (sizeof(actual) != sizeof(expected)) return 0;
    for (int i = 0; i < sizeof(actual); i++)
        if (actual[i] != expected[i]) return 0;
    return 1;
}
void create() {
    class lqueue q = lqueue_new(3);
    verify_collection(q.size == 3 && q.index == 0 && sizeof(q.elems) == 3, "queue initialization");
    verify_collection(contents(lqueue_list(q), ({nil, nil, nil})), "queue empty fixed-size window");
    verify_collection(nilp(lqueue_member(q, 1)) && nilp(lqueue_member(q, 3)), "queue unfilled members");
    lqueue_add(q, "first");
    verify_collection(lqueue_member(q, 1) == "first" && q.index == 1, "queue newest member");
    verify_collection(contents(lqueue_list(q), ({nil, nil, "first"})), "queue partial window");
    lqueue_add(q, 0);
    verify_collection(lqueue_member(q, 1) == 0 && !nilp(lqueue_member(q, 1)), "queue preserves numeric zero");
    verify_collection(lqueue_member(q, 2) == "first", "queue previous member");
    mapping value = (["name":"shared"]);
    lqueue_add(q, value);
    verify_collection(q.index == 0 && lqueue_member(q, 1) == value, "queue wraps at capacity");
    verify_collection(contents(lqueue_list(q), ({"first", 0, value})), "queue full wrap has one window");
    mixed *first_list = lqueue_list(q);
    mixed *second_list = lqueue_list(q);
    verify_collection(first_list != second_list && first_list != q.elems, "queue wrap returns fresh arrays");
    first_list[0] = "only in copy";
    verify_collection(lqueue_member(q, 3) == "first", "queue wrap copy isolation");
    mixed *nested = ({"nested"});
    lqueue_add(q, nested);
    verify_collection(contents(lqueue_list(q), ({0, value, nested})), "queue rotates oldest to newest");
    verify_collection(lqueue_member(q, 1) == nested && lqueue_member(q, 2) == value
        && lqueue_member(q, 3) == 0, "queue member order after wrap");
    mixed *view = lqueue_list(q);
    view[0] = "another copy";
    verify_collection(lqueue_member(q, 3) == 0, "queue rotated copy isolation");
    view[1]["name"] = "changed";
    verify_collection(value["name"] == "changed" && lqueue_member(q, 2)["name"] == "changed", "queue shallow mapping references");
    view[2][0] = "also changed";
    verify_collection(nested[0] == "also changed", "queue shallow array references");
    lqueue_add(q, 4);
    lqueue_add(q, 5);
    verify_collection(q.index == 0 && contents(lqueue_list(q), ({nested, 4, 5})), "queue repeated full wrap");
    class lqueue one = lqueue_new(1);
    verify_collection(contents(lqueue_list(one), ({nil})), "queue capacity-one empty window");
    lqueue_add(one, "single");
    verify_collection(contents(lqueue_list(one), ({"single"})) && lqueue_member(one, 1) == "single", "queue capacity-one full window");
    lqueue_add(one, nil);
    verify_collection(contents(lqueue_list(one), ({nil})) && nilp(lqueue_member(one, 1)), "queue explicit nil value");
    verify_collection(contents(lqueue_list(lqueue_new(0)), ({})), "queue zero-capacity listing");
    class lqueue independent = lqueue_new(3);
    verify_collection(independent.elems != q.elems && contents(lqueue_list(independent), ({nil, nil, nil})), "queue instance independence");

    class stack s = stack_new(3);
    verify_collection(s.which == 0 && contents(s.elems, ({nil, nil, nil})), "stack initialization");
    mixed problem = catch(stack_pop(s));
    verify_collection(stringp(problem) && strindex(problem, "Stack underflow.") >= 0 && s.which == 0, "stack empty underflow");
    stack_push(s, "bottom");
    stack_push(s, 0);
    stack_push(s, value);
    verify_collection(s.which == 3, "stack reaches capacity");
    problem = catch(stack_push(s, "overflow"));
    verify_collection(stringp(problem) && strindex(problem, "Stack overflow.") >= 0 && s.which == 3, "stack overflow preserves depth");
    verify_collection(stack_pop(s) == value && s.which == 2, "stack pops newest mapping");
    mixed zero = stack_pop(s);
    verify_collection(zero == 0 && !nilp(zero) && s.which == 1, "stack preserves numeric zero");
    verify_collection(stack_pop(s) == "bottom" && s.which == 0, "stack LIFO order");
    problem = catch(stack_pop(s));
    verify_collection(stringp(problem) && strindex(problem, "Stack underflow.") >= 0 && s.which == 0, "stack drained underflow");
    stack_push(s, nil);
    verify_collection(nilp(stack_pop(s)) && s.which == 0, "stack explicit nil value");
    stack_push(s, nested);
    mixed *popped = stack_pop(s);
    popped[0] = "from stack";
    verify_collection(popped == nested && nested[0] == "from stack", "stack shallow array reference");
    stack_push(s, 2.5);
    verify_collection(stack_pop(s) == 2.5, "stack float value");
    stack_push(s, this_object());
    verify_collection(stack_pop(s) == this_object(), "stack object value");
    function callback = (: 17 :);
    stack_push(s, callback);
    function received = stack_pop(s);
    verify_collection(received == callback && received() == 17, "stack function value");
    class stack other = stack_new(3);
    stack_push(s, 9);
    verify_collection(other.elems != s.elems && other.which == 0 && stack_pop(s) == 9, "stack instance independence");
    class stack disabled = stack_new(0);
    problem = catch(stack_push(disabled, 1));
    verify_collection(stringp(problem) && strindex(problem, "Stack overflow.") >= 0 && disabled.which == 0, "stack zero-capacity overflow");
    dump("COLLECTION_TOTAL: " + checks + "\n");
}
