/* Do not remove the headers from this file! see /USAGE for more info. */

//
// BOARD.C    970414 Fritti@TiamaT, Vette@Lima
//
// Generic bulletin board, with switching group capability.
//
// Added security checks using NEWS_D internal security
// Vette April 14, 1997

//: MODULE
//  Boards are simply set up from rooms by using
//
//   set_objects( ([
//      "/contrib/board" : ({ "name", "linked newsgroup", switchable })
//    ]) );

#include <edit.h>

inherit OBJ;
inherit M_READABLE;
inherit M_INPUT;
inherit M_SET;
inherit CLASS_NEWSMSG;

// The maximum nr of headers to show.
#define MAX_HEADERS 10

// Default to no switchable boards.
#define DEFAULT_SWITCHABLE 0

// The default mud newsgroup
#define DEFAULT_GROUP BUG_NEWSGROUP

// The default Name of the Board Object
#define DEFAULT_NAME "Bulletin Board"

// The group to which this board is linked. Set by setup().
private
nosave string linked_group;

// The name of this board. Set by setup().
private
nosave string board_name;

// Is this board switchable? Set by create().
private
int can_switch;

// Function prototypes.
private nomask int set_group(string new_group);
private
nomask string list_headers();
private nomask string do_desc();
private
nomask string switch_to_next_unread_group();

private nomask int *message_ids(string group)
{
   if (!check_group(group))
      return ({});
   int *ids = NEWS_D->get_messages(group, 1);
   return arrayp(ids) ? sort_array(ids, 1) : ({});
}

// Create a new board.
varargs void mudlib_setup(string name, string group, int switchable)
{
   if (!group || group == "")
      group = DEFAULT_GROUP;
   if (!name || name == "")
      name = DEFAULT_NAME;
   if (!switchable)
      switchable = DEFAULT_SWITCHABLE;

   can_switch = switchable;

   if (!set_group(group))
      error("Invalid board newsgroup: " + group + "\n");

   board_name = name;
   set_id("board", "bulletin board");
   set_long(list_headers);
   set_mass(to_float(SMALL));
}

// A check to test whether the current user is allowed to read
// from this group.
// Added by Vette  April 14, 1997
private
nomask int check_group(string g)
{
   if (member_array(g, NEWS_D->get_groups()) == -1)
   {
      return 0;
   }
   return 1;
}

// Set and query functions.
// Moved the group check into its own function and called that fn
// Vette  April 14, 1997
private
nomask int set_group(string new_group)
{
   if (!check_group(new_group))
      return 0;
   linked_group = new_group;
   return 1;
}

// Added Valid group check
nomask string query_group()
{
   return linked_group;
}

// Used for switchable boards.
nomask void set_switchable(int flag)
{
   can_switch = flag;
}

nomask int query_switchable()
{
   return can_switch;
}

// Format one message line
private
nomask varargs string format_message_line(int id)
{
   class news_msg msg;
   string subject;

   msg = NEWS_D->get_message(linked_group, id);

   if (!msg || !msg.body)
      return nil; // Do not display removed messages.
   else
      subject = msg.subject;

   return sprintf("%-48s [%-10s on %s]", // Display message subject/poster/time
                  subject[0..47], msg.poster, intp(msg.time) ? ctime(msg.time)[4..9] : msg.time);
}

// Format all message lines.
// Added some changes to support groups with less than MAX_HEADERS posts
// to them...it used to make negative ID's
// Added a * to unread posts
private
nomask string *query_message_lines()
{
   int *ids = message_ids(linked_group);
   int start = sizeof(ids) > MAX_HEADERS ? sizeof(ids) - MAX_HEADERS : 0;
   string read_ids = this_user() ? this_user()->get_news_id_read(linked_group) : "";
   string *lines = ({});
   for (int index = start; index < sizeof(ids); index++)
      lines += ({sprintf("%4d%s %s", index + 1, set_member(read_ids, ids[index]) ? " " : "*",
                         format_message_line(ids[index]))});
   return sizeof(lines) ? lines : ({"The board is bereft of posts.  Feel free to change this!\n"});
}

// Make the long description.
private
nomask string list_headers()
{
   if (!check_group(linked_group))
      return "You do not have permission.\n";

   return (can_switch ? "Group: " + linked_group + "\n" : "") +
          "ID    TITLE                                            POSTER      TIME\n" + ("-" * 78) + "\n" +
          implode(query_message_lines(), "\n");
}

// Needed for M_READABLE
nomask int has_entries()
{
   return 1;
}

// Read about an entry.
// Added a couple of changes to beef up security
// Vette  April 14, 1997
nomask mixed read_entry(string str)
{
   if (!this_user() || !check_group(linked_group))
      return "You do not have access.\n";
   int *ids = message_ids(linked_group);
   int id;
   if (str == "next")
   {
      string read_ids = this_user() ? this_user()->get_news_id_read(linked_group) : "";
      foreach (int candidate in ids)
         if (!set_member(read_ids, candidate))
         {
            id = candidate;
            break;
         }
      if (!id)
         return "No more messages.\n";
   }
   else
   {
      int number;
      if (sscanf(str, "%d", number) != 1)
         return "Specify the post number you want to read.";
      if (number <= 0 || number > sizeof(ids))
         return "You can only read posts that are on the bulletin board!\n";
      id = ids[number - 1];
   }
   class news_msg msg = NEWS_D->get_message(linked_group, id);
   this_user()->add_news_id_read(linked_group, id);
   return format_message_line(id) + "\n\n" + msg.body;
}

// Helpful text for those used to type 'read 1'.
nomask mixed direct_read_obj(object ob)
{
   return "Use 'read about <postnr>' to read the bulletin board.\n";
}

mixed direct_read_about_str_from_obj(string str, object ob)
{
   return 1;
}

// Short description.
private
nomask string do_desc()
{
   int *ids = message_ids(linked_group);
   string read_ids = this_user() ? this_user()->get_news_id_read(linked_group) : "";
   int unread;
   foreach (int id in ids)
      if (!set_member(read_ids, id)) unread++;
   return sprintf("A %s%s [%d unread, %d total].", board_name,
                  can_switch ? " set to " + linked_group : "", unread, sizeof(ids));
}

// For board verbs like post etc.
nomask int is_bulletin_board()
{
   return 1;
}

// Check if id is in range, for board verbs can_* functions.
nomask int valid_id(int id)
{
   int *ids = message_ids(linked_group);
   if (id <= 0 || id > sizeof(ids))
      return 0;

   return 1;
}

// Check if this_body() can post.
nomask int valid_post()
{
   return this_user() && check_group(linked_group) && !NEWS_D->is_write_restricted(linked_group);
}

// Verb interface. These get called by their respective verbs.

// Remove a note.
nomask void do_remove(int id)
{
   int *ids = message_ids(linked_group);
   if (id <= 0 || id > sizeof(ids))
   {
      write("No such note.\n");
      return;
   }
   class news_msg msg = NEWS_D->get_message(linked_group, ids[id - 1]);
   if (msg.userid != this_user()->query_userid() && !check_privilege(1))
   {
      write("You can only delete your own posts.\n");
      return;
   }
   mixed problem = catch(NEWS_D->remove_post(linked_group, ids[id - 1]));
   write(problem ? "Could not remove that post.\n" : "Post number " + id + " removed.\n");
}

mixed direct_remove_wrd_from_obj(string wrd, object ob)
{
   int id;

   if (!valid_post())
      return "You aren't permitted to remove posts from this board.\n";

   if (sscanf(wrd, "%d", id) != 1)
      return "Please specify a post number.\n";

   if (!valid_id(id))
      return "No such note.\n";

   return 1;
}

// Post a note.
private
void receive_post(string group, string subj, string *body)
{
   if (nilp(body))
   {
      write("Post aborted.\n");
      return;
   }
   int posted;
   mixed problem = catch(posted = NEWS_D->post(group, subj, implode(body, "\n")));
   if (problem || !posted)
   {
      write("Post not completed; check before resending. Text retained in the editor.\n");
      clone_object(EDIT_OB, EDIT_TEXT, body, &receive_post(group, subj));
      return;
   }
   write("Posted.\n");
}

private
void receive_subject(string group, mixed subj)
{
   if (subj == -1)
      return;
   if (!stringp(subj) || subj == "")
   {
      write("No subject, post aborted.\n");
      return;
   }
   if (sizeof(subj) > 50)
   {
      write("Subject too long. Please try again.\n");
      modal_simple(&receive_subject(group), "Subject: ");
      return;
   }
   clone_object(EDIT_OB, EDIT_TEXT, nil, &receive_post(group, subj));
}

nomask void do_post(string subj)
{
   if (!valid_post())
   {
      write("You aren't permitted to post on this board.\n");
      return;
   }
   if (!subj || subj == "")
      modal_simple(&receive_subject(linked_group), "Subject: ");
   else if (sizeof(subj) <= 50)
      clone_object(EDIT_OB, EDIT_TEXT, nil, &receive_post(linked_group, subj));
   else
      write("Subject too long.\n");
}

mixed direct_post_on_obj_about_str(string str, object ob)
{
   if (!valid_post())
      return "You aren't permitted to post on this board.\n";
   if (!check_group(linked_group))
      return "You lack the permissions.\n";

   if (sizeof(str) > 50)
      return "Subject too long.\n";

   return 1;
}

// Followup a note.
private
void receive_followup(string group, int followup_id, string *body)
{
   if (nilp(body))
   {
      write("Followup aborted.\n");
      return;
   }
   int posted;
   mixed problem = catch(posted = NEWS_D->followup(group, followup_id, implode(body, "\n")));
   if (problem || !posted)
   {
      write("Followup not completed; check before resending. Text retained in the editor.\n");
      clone_object(EDIT_OB, EDIT_TEXT, body, &receive_followup(group, followup_id));
      return;
   }
   write("Posted.\n");
}

nomask void do_followup(int id)
{
   int *ids = message_ids(linked_group);
   if (!valid_post() || id <= 0 || id > sizeof(ids))
   {
      write("No such note, or posting is not permitted.\n");
      return;
   }
   clone_object(EDIT_OB, EDIT_TEXT, nil, &receive_followup(linked_group, ids[id - 1]));
}

nomask void do_followup_with_message(int id)
{
   int *ids;
   class news_msg msg;
   string *lines;

   ids = message_ids(linked_group);

   if (!valid_post() || id <= 0 || id > sizeof(ids))
   {
      write("No such note, or posting is not permitted.\n");
      return;
   }
   msg = NEWS_D->get_message(linked_group, ids[id - 1]);
   if (!msg || !msg.body)
   {
      write("You may not followup to that message -- it was removed.\n");
      return;
   }

   lines = ({sprintf("On %s %s wrote post #%d:", intp(msg.time) ? ctime(msg.time) : msg.time, msg.poster,
                     ids[id - 1])});

   lines += map(explode(msg.body, "\n"), ( : "> " + $1:));

   clone_object(EDIT_OB, EDIT_TEXT, lines, &receive_followup(linked_group, ids[id - 1]));
}

mixed direct_post_wrd_to_wrd_on_obj(string wrd1, string wrd2, object ob)
{
   int id;

   if (!valid_post())
      return "You're not allowed to post on this board.\n";
   if (!check_group(linked_group))
      return "You lack the permissions.\n";
   if (sscanf(wrd2, "%d", id) != 1)
      return "Please specify a post number.\n";

   if (!valid_id(id))
      return "No such note.\n";

   return 1;
}

mixed direct_post_wrd_with_wrd_on_obj(string wrd1, string wrd2, object ob)
{
   return direct_post_wrd_to_wrd_on_obj(wrd1, wrd2, ob);
}

// Reply through the user-owned mail composer.
nomask void do_reply(int id)
{
   int *ids = message_ids(linked_group);
   if (id <= 0 || id > sizeof(ids))
   {
      write("No such note.\n");
      return;
   }
   this_user()->query_mailer()->begin_news_reply(linked_group, ids[id - 1]);
}

mixed direct_answer_to_wrd_on_obj(string wrd, object ob)
{
   int id;

   if (!check_group(linked_group))
      return "You lack the permissions.\n";
   if (sscanf(wrd, "%d", id) != 1)
      return "Please specify a post number to reply to.\n";

   if (!valid_id(id))
      return "No such note.\n";

   return 1;
}

// Switch board newsgroup.
nomask void do_switch(string group)
{
   if (!can_switch || !this_user() || NEWS_D->is_write_restricted(linked_group))
   {
      write("You aren't permitted to switch this board.\n");
      return;
   }
   if (group == "next")
      if ((group = switch_to_next_unread_group()) == "" || !group)
      {
         write("No new news.\n");
         return;
      }
   if (!set_group(group))
   {
      write("Couldn't change to group <" + group + ">.\n");
      return;
   }

   this_body()->simple_action("$N $vchange " + the_short() + " to group <" + group + ">.\n");
}

mixed direct_switch_obj_to_str(object ob, string str)
{
   if (!can_switch)
      return 0;

   if (!this_user() || NEWS_D->is_write_restricted(linked_group))
      return "You aren't permitted to switch this board.\n";

   if ((member_array(str, NEWS_D->get_groups()) == -1) && (str != "next"))
      return "That is not a valid newsgroup name.\n";

   return 1;
}

/*
=============================================================================
*/
/* Functions to Support Switching to Next Unread and Listing Unread Groups */

/* Lifted from newsreader.c */
private
int count_unread_messages(string group, int all_messages)
{
   int *ids = message_ids(group);
   if (all_messages)
      return sizeof(ids);
   string read_ids = this_user() ? this_user()->get_news_id_read(group) : "";
   int count;
   foreach (int id in ids)
      if (!set_member(read_ids, id)) count++;
   return count;
}

/* Lifted from newsreader.c */
private
nomask int test_for_new(string group)
{
   if (count_unread_messages(group, 0))
      return 1;
   return 0;
}

/* Lifted from newsreader.c */
private
nomask string format_group_line(string group)
{
   int unread = count_unread_messages(group, 1);
   return sprintf("  %-40s (%d %s, %d unread)", group, unread, unread == 1 ? "message" : "messages",
                  count_unread_messages(group, 0));
}

/* Lifted from newsreader.c (slightly modified) */
private
nomask void display_groups_with_new()
{
   string *groups;

   groups = filter(this_user()->subscribed_groups(), test_for_new);

   if (!sizeof(groups))
   {
      write("No new news.\n");
   }
   else
   {
      string *list;
      list = ({"Groups with new messages:", ""}) + map(groups, format_group_line);
      more(list);
   }
}

private
nomask string switch_to_next_unread_group()
{
   string *groups;

   groups = filter(this_user()->subscribed_groups(), test_for_new);
   if (!sizeof(groups))
   {
      return "";
   }
   else
   {
      return groups[0];
   }
}

string query_in_room_desc()
{
   return is_visible() ? do_desc() : "";
}

mixed indirect_post_on_obj_about_str(string str, object ob)
{
   return direct_post_on_obj_about_str(str, ob);
}
