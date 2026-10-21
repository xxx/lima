#include <config/stats.h>
inherit M_ACCESS;
protected inherit CLASS_STATMODS;
private int checks;
int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_destruct_request(object target) { return 1; }
int valid_destruct(object caller, object target, string program) { return 1; }
private void verify(int passed, string label)
{
    if (!passed) throw("BODY_STATS_FAIL: " + label);
    checks++;
    dump("BODY_STATS_OK: " + label + "\n");
}
private int same(string *left, string *right)
{
    if (sizeof(left) != sizeof(right)) return 0;
    for (int i = 0; i < sizeof(left); i++) if (left[i] != right[i]) return 0;
    return 1;
}
private int bonus() { return 3; }
private void check_race(object menu, string race, string *names)
{
    string path = menu->query_race_filename(race);
    object body = USER_OB->make_race(path, "stataudit" + race);
    verify(clonep(body) && base_name(body) == base_name(load_object(path)) && lower_case(body->query_name()) == "stataudit" + race,
           race + " actual body constructor and name");
    verify(body->query_race() == race && body->is_body(), race + " actual race/body identity");
    verify(same(sort_array(values(body->stat_abrev()), 1), names), race + " selected stat names");
    verify(sizeof(body->query_stats()) == 0, race + " constructor does not roll stats early");
    foreach (string stat in names) {
        mixed pure = body->query_stat_pure(stat);
        mixed current = body->query_stat(stat);
        mixed mod = body->query_mod_stat(stat);
        verify(intp(pure) && pure == 0 && intp(current) && current == 0 && intp(mod) && mod == 0,
               race + " empty stat defaults " + stat);
    }
#if BODY_STATS == BODY_STATS_LIMA
    int initial_points = body->query_level() * STAT_PTS_PER_LEVEL;
#else
    int initial_points = (body->query_level() / 4) * 2;
#endif
    verify(body->spare_points() == initial_points, race + " spare points before rolling");
    string primary = names[0];
    body->inc_mod_stat(primary);
    verify(body->query_mod_stat(primary) == 1, race + " increment absent modifier");
    body->init_stats();
    int valid_rolls = 1;
    for (int roll = 0; roll < 16; roll++) {
        if (roll) body->init_stats();
        foreach (string stat in names) {
            mixed pure = body->query_stat_pure(stat);
            mixed current = body->query_stat(stat);
            mixed mod = body->query_mod_stat(stat);
            valid_rolls = valid_rolls && intp(pure) && intp(current) && intp(mod) && mod == 0;
#if BODY_STATS == BODY_STATS_DND
            valid_rolls = valid_rolls && pure >= 3 && pure <= 18 && current == pure;
#elif BODY_STATS == BODY_STATS_RIFTS
            valid_rolls = valid_rolls && pure >= 3 && pure != 16 && current == pure;
#else
            valid_rolls = valid_rolls && pure >= 0 && pure <= 100 && current == pure;
#endif
        }
    }
    verify(valid_rolls, race + " repeated initialization, roll bounds and zero modifiers");
    verify(body->spare_points() == initial_points, race + " spare points after rolling");
    class stat_roll_mods mods = body->query_roll_mods();
    verify(intp(mods.str_adjust) && intp(mods.agi_adjust) && intp(mods.int_adjust) && intp(mods.wil_adjust),
           race + " race class override remains accessible");
    if (race == "human" || race == "dragon") verify(mods.str_adjust == 9, race + " existing strength adjustment");
    if (race == "elf") verify(mods.str_adjust == -8, race + " existing strength adjustment");
    verify(stringp(body->show_stats()) && strlen(body->show_stats()) > 20, race + " stat display");
    int original = body->query_stat_pure(primary);
    mapping copy = body->query_stats();
    copy["stat_" + primary] = -999;
    verify(body->query_stat_pure(primary) == original, race + " stat query copy isolation");
    body->set_stat(primary, 10);
    body->set_mod_stat(primary, -2);
    body->inc_mod_stat(primary);
    body->refresh_stats();
    verify(body->query_stat_pure(primary) == 10 && body->query_mod_stat(primary) == -1 && body->query_stat(primary) == 9,
           race + " explicit stat and negative modifier refresh");
    verify(body->spare_points() == initial_points + 1, race + " spare points use modified stat");
    function hook = bonus;
    body->add_hook(primary + "_bonus", hook);
    body->refresh_stats();
    verify(body->query_stat(primary) == 12 && body->query_stat_pure(primary) == 10,
           race + " real stat bonus hook");
    body->remove_hook(primary + "_bonus", hook);
    body->refresh_stats();
    verify(body->query_stat(primary) == 9, race + " removed bonus hook");
    body->reset_stat_increases();
    verify(body->query_mod_stat(primary) == 0 && body->query_stat(primary) == 10,
           race + " reset modifier retains base stat");
    verify(body->spare_points() == initial_points, race + " reset restores spare points");
    body->inc_mod_stat("auditmissing");
    verify(body->query_mod_stat("auditmissing") == 1 && body->query_stat_pure("auditmissing") == 0,
           race + " absent modifier after initialization");
    body->init_stats();
    verify(body->query_mod_stat("auditmissing") == 0 && body->query_stat_pure("auditmissing") == 0 &&
           body->query_stat("auditmissing") == 0 && body->query_stats()["mod_auditmissing"] == nil,
           race + " reinitialization drops old modifier");
    efun::destruct(body);
    verify(!body && sizeof(object_clones(load_object(path))) == 0, race + " clone cleanup");
}
void create()
{
    set_privilege(1);
    object menu = load_object("/obj/usermenu/usermenu");
#if BODY_STATS == BODY_STATS_LIMA
    string *races = ({"documentation", "elf", "human", "orc", "troll"});
    string *names = ({"agi", "int", "str", "wil"});
#elif BODY_STATS == BODY_STATS_DND
    string *races = ({"elf", "human"});
    string *names = ({"cha", "con", "dex", "int", "str", "wis"});
#elif BODY_STATS == BODY_STATS_RIFTS
    string *races = ({"dragon", "human"});
    string *names = ({"iq", "ma", "me", "pb", "pe", "pp", "ps", "spd"});
#endif
    verify(same(sort_array(menu->query_races(), 1), races), "actual menu selects compatible races");
    mapping race_data = menu->query_race_data();
    race_data["human"] = "/invalid";
    verify(base_name(load_object(menu->query_race_filename("human"))) == "/std/race/human", "menu race data copy isolation");
    foreach (string race in races) check_race(menu, race, names);
    dump("BODY_STATS_TOTAL: " + checks + "\n");
}
