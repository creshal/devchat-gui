/*
 * devchat_window.c
 * Copyright (C) Samuel Vincent Creshal 2010 <creshal@arcor.de>
 *
 * devchat-gui is free software: you can redistribute it and/or modify
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * devchat-gui is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "devchat_window.h"
#include "devchat_cb_data.h"
#include "devchat_html_tag.h"
#include "devchat_html_attr.h"
#include "devchat_conversation.h"
#include "devchat_url_tag.h"
#include "HTMLent.h"

#include <string.h>
#include <time.h>
#include <inttypes.h>

gchar* dbg_msg;


enum {
  SETTINGS_BROWSER = 1,
  SETTINGS_COLOR_FONT,
  SETTINGS_COLOR_L1,
  SETTINGS_COLOR_L3,
  SETTINGS_COLOR_L5,
  SETTINGS_COLOR_L6,
  SETTINGS_COLOR_GOLDIES,
  SETTINGS_COLOR_GREENS,
  SETTINGS_COLOR_BLUES,
  SETTINGS_COLOR_TIME,
  SETTINGS_COLOR_URL,
  SETTINGS_COLOR_URL_VISITED,
  SETTINGS_COLOR_URL_HOVER,
  SETTINGS_COLOR_HIGHLIGHT,
  SETTINGS_USER,
  SETTINGS_PASS,
  SETTINGS_SHOWID,
  SETTINGS_STEALTHJOIN,
  SETTINGS_AUTOJOIN,
  SETTINGS_SHOWHIDDEN,
  SETTINGS_COLORUSER,
  SETTINGS_NOTIFY,
  SETTINGS_VNOTIFY,
  SETTINGS_HANDLE_WIDTH,
  SETTINGS_WIDTH,
  SETTINGS_HEIGHT,
  SETTINGS_X,
  SETTINGS_Y,
  SETTINGS_TRAYICON,
  SETTINGS_MAXIMIZED,
  SETTINGS_COLOR_RED,
  SETTINGS_COLOR_CYAN,
  SETTINGS_COLOR_YELLOW,
  SETTINGS_COLOR_BLUE,
  SETTINGS_COLOR_GREEN,
  SETTINGS_COLOR_MAGENTA
} params;

#ifdef INGAME
enum {
  INGAME_STATUS_DISCONNECTED,
  INGAME_STATUS_ONLINE,
  INGAME_STATUS_SILENCED,
  INGAME_STATUS_KICKED
} ingame_status;
#endif

static void devchat_window_set_property (GObject* object, guint id, const GValue* value, GParamSpec* pspec);
static void devchat_window_get_property (GObject* object, guint id, GValue* value, GParamSpec* pspec);
gboolean track_window_state (GtkWidget* widget, GdkEventWindowState* s, DevchatCBData* data);
gboolean paned_update (DevchatCBData* data);
void save_settings (DevchatWindow* w);
void tray_status_change (GtkWidget* w, DevchatCBData* data);
void url_tag_nv_color_cb (GtkTextTag* t, gchar* value);
void url_tag_v_color_cb (GtkTextTag* t, gchar* value);
void show_tray_menu (GtkStatusIcon* icon, guint button, guint activate_time, DevchatCBData* data);
void notify(gchar* title, gchar* body, GdkPixbuf* icon, DevchatCBData* data);
#ifdef NOTIFY
void notify_cb ();
#endif
void urlopen ();
void user_list_get();
void message_list_get();
void login_cb ();
void remote_level ();
gboolean hotkey_cb (GtkWidget* w, GdkEventKey* key, DevchatCBData* data);
void destroy (GtkObject* widget, DevchatCBData* data);
void login (GtkWidget* widget, DevchatCBData* data);
void config_cb (GtkWidget* widget, DevchatCBData* data);
void go_forum (GtkWidget* widget, DevchatCBData* data);
void reconnect (GtkWidget* widget, DevchatCBData* data);
void tab_changed (GtkWidget* widget, GtkNotebook* nb, guint pagenum, DevchatCBData* data);
gboolean scroll_mark_onscreen (DevchatConversation* c);
void level_changed (GtkWidget* widget, DevchatCBData* data);
void next_tab (GtkWidget* widget, DevchatCBData* data);
void prev_tab (GtkWidget* widget, DevchatCBData* data);
void show_his (GtkWidget* widget, DevchatCBData* data);
void about_cb (GtkWidget* widget, DevchatCBData* data);
void at_cb (GtkWidget* widget, DevchatCBData* data);
DevchatConversation* pm_cb (GtkWidget* widget, DevchatCBData* data);
void user_list_clear_cb (GtkWidget* child, DevchatCBData* data);
void find (GtkWidget* widget, DevchatCBData* data);
void launch_browser (GtkWidget* fnord, gchar* uri, DevchatCBData* data);
void update_tags (gchar* key, DevchatConversation* value, DevchatCBData* data);
void add_smilie_cb (gpointer key, gpointer value, DevchatCBData* data);
void ins_smilie (GtkWidget* widget, DevchatCBData* data);
void ins_preset (GtkWidget* widget, DevchatCBData* data);
gboolean badass (gchar* name, DevchatCBData* data);
gint user_lookup (gchar* a, gchar* b);
void search_ava_cb (SoupSession* s, SoupMessage* m, DevchatCBData* data);
void his_cb (SoupSession* s, SoupMessage* m, DevchatCBData* data);
gboolean user_list_poll (DevchatCBData* data);
gboolean message_list_poll (DevchatCBData* data);
void ce_parse (gchar* data, DevchatCBData* self, gchar* date);
gchar* parse_message (gchar* message_d, DevchatCBData* data);
void toggle_tray_minimize (GtkStatusIcon* icon, DevchatCBData* data);
gchar* current_time ();
gboolean get_pos_size (DevchatWindow* window);
void message_list_chunk (SoupMessage* m, SoupBuffer* chunk, DevchatCBData* data);
gboolean message_list_timeout (DevchatCBData* data);
gboolean user_list_timeout (DevchatCBData* data);
void popup_open_link (GtkWidget* w, DevchatCBData* data);
void popup_copy_stuff (GtkWidget* w, DevchatCBData* data);
void popup_insert_text (GtkWidget* w, DevchatCBData* data);
gboolean quit_timeout_cb (DevchatCBData* data);
void quit_cb (SoupSession* s, SoupMessage* m, DevchatCBData* data);
void msg_sent_cb (SoupSession* s, SoupMessage* m, DevchatCBData* data);
void devchat_toggle_ignore_user (GtkTextBuffer* target_buffer, gchar* name);
#ifdef INGAME
gboolean get_ingame_messages (DevchatCBData* data);
void ingame_update_status (DevchatCBData* data, gint status);
void ingame_clear_user_list (DevchatCBData* data);
void ingame_clear_message_list (DevchatCBData* data);
void ingame_append_user (DevchatCBData* data, gchar* user);
void ingame_append_message (DevchatCBData* data, gchar* author, gchar* mode, gchar* time_attr, gchar* lid, gchar* message);
void ingame_flush_data (DevchatCBData* data);
#endif

G_DEFINE_TYPE (DevchatWindow, devchat_window, G_TYPE_OBJECT)

DevchatWindow*
devchat_window_new (void)
{
  DevchatWindow* window = g_object_new (DEVCHAT_TYPE_WINDOW, NULL);

  return window;
}

static void
devchat_window_init (DevchatWindow* self)
{

  GtkVBox* vbox0 = GTK_VBOX(gtk_vbox_new(FALSE,0));
  GtkWidget* menu = gtk_menu_bar_new();
  GtkWidget* hpaned1 = gtk_hpaned_new();

  init_entities ();

  self->smilies = g_hash_table_new (g_str_hash, g_str_equal);
  self->users = g_hash_table_new (g_str_hash, g_str_equal);
  self->avatars = g_hash_table_new (g_str_hash, g_str_equal);
  self->conversations = g_hash_table_new (g_str_hash, g_str_equal);

  self->settings.browser = g_strdup("<native>");
  self->settings.color_font = g_strdup("#fff");
  self->settings.color_l1 = g_strdup("#222");
  self->settings.color_l3 = g_strdup("#533");
  self->settings.color_l5 = g_strdup("#344");
  self->settings.color_l6 = g_strdup("#453");
  self->settings.color_goldies = g_strdup ("#fc0");
  self->settings.color_greens = g_strdup("#0c7");
  self->settings.color_blues = g_strdup("#47f");
  self->settings.color_time = g_strdup("#999");
  self->settings.color_url = g_strdup("#ff0");
  self->settings.color_url_visited = g_strdup("#ff0");
  self->settings.color_url_hover = g_strdup("#fff");
  self->settings.color_highlight = g_strdup("#c00");
  self->settings.color_red = g_strdup ("#f00");
  self->settings.color_green = g_strdup ("#0f0");
  self->settings.color_blue = g_strdup ("#00f");
  self->settings.color_magenta = g_strdup ("#f0f");
  self->settings.color_yellow = g_strdup ("#ff0");
  self->settings.color_cyan = g_strdup ("#0ff");
  self->settings.user = g_strdup(g_get_user_name());
  self->settings.pass = g_strdup("hidden");
  self->settings.store_pass = FALSE;
  self->settings.showid = FALSE;
  self->settings.stealthjoin = FALSE;
  self->settings.autojoin = FALSE;
  self->settings.showhidden = TRUE;
  self->settings.coloruser = TRUE;
  self->settings.showtray = FALSE;
  self->settings.jumptab = FALSE;
  self->settings.notify = g_strdup("<native>");
  self->settings.vnotify = g_strdup("<native>");
  self->settings.servername = g_strdup ("SovietServer");
  self->settings.width = 600;
  self->settings.height = 400;
  self->settings.x = 0;
  self->settings.y = 0;
  self->settings.avatar_size = 12;
  self->settings.update_time = 1000;
  self->settings.keywords = NULL;
  self->firstrun = TRUE;
  self->hovertag = NULL;
  self->buf_current = 0;
  self->search_start_set = FALSE;
  self->dnd = FALSE;
  self->settings.maximized = FALSE;
  self->message_buffer = "";
  self->moderators = g_hash_table_new (g_str_hash, g_str_equal);
  self->jarfile = g_build_filename (g_get_user_config_dir(),"devchat_cookies.csv", NULL);
  self->last_notification = 0;
  self->settings.proxy = "";
#ifdef INGAME
  self->settings.TCFolder = "";
  self->ingame_lid = -1;
  self->ingame_userlist = "";
  self->ingame_messagelist = "";
  self->ingame_usercount = 0;
#endif

  gint j;

  for (j=0; j <= MAX_BUF; j++)
    self->buffer[j] = "";

  for (j=0; j < 10; j++)
    self->settings.presets[j] = "";

  self->settings_backup = self->settings;


  DevchatCBData* self_data = devchat_cb_data_new (self, NULL);

  self->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gchar* title = g_strdup_printf ("%s %s",APPNAME, VERSION);
  gtk_window_set_title (GTK_WINDOW(self->window), title);
  g_free (title);
  gtk_widget_set_size_request (self->window, 600,400);
  gtk_window_move(GTK_WINDOW(self->window), self->settings.x, self->settings.y);
  self->accelgroup = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(self->window), self->accelgroup);
  g_signal_connect(self->window, "destroy", G_CALLBACK(destroy), self_data);
  g_signal_connect (self->window, "window-state-event", G_CALLBACK (track_window_state), self_data);

  GtkMenuItem* menu_main = GTK_MENU_ITEM (gtk_menu_item_new_with_mnemonic (_("_Main")));
  GtkMenuItem* menu_edit = GTK_MENU_ITEM (gtk_menu_item_new_with_mnemonic (_("_Edit")));
  GtkMenuItem* menu_insert = GTK_MENU_ITEM (gtk_menu_item_new_with_mnemonic (_("_Insert")));
  GtkMenuItem* menu_view = GTK_MENU_ITEM (gtk_menu_item_new_with_mnemonic (_("_Go")));
  GtkMenuItem* menu_about = GTK_MENU_ITEM (gtk_menu_item_new_with_mnemonic (_("_Help")));

  self->item_connect = gtk_image_menu_item_new_from_stock(GTK_STOCK_CONNECT,self->accelgroup);
  g_signal_connect (self->item_connect, "activate", G_CALLBACK (login),self_data);

  self->item_status = gtk_menu_item_new_with_label (_("Change Status..."));
  gtk_widget_set_no_show_all (self->item_status, TRUE);

  GtkWidget* item_status_online = gtk_menu_item_new_with_label (_("Online"));
  g_signal_connect (item_status_online, "activate", G_CALLBACK (tray_status_change), devchat_cb_data_new (self, GINT_TO_POINTER (0)));
  GtkWidget* item_status_away = gtk_menu_item_new_with_label (_("Away"));
  g_signal_connect (item_status_away, "activate", G_CALLBACK (tray_status_change), devchat_cb_data_new (self, GINT_TO_POINTER (1)));
  GtkWidget* item_status_dnd = gtk_menu_item_new_with_label (_("DND"));
  g_signal_connect (item_status_dnd, "activate", G_CALLBACK (tray_status_change), devchat_cb_data_new (self, GINT_TO_POINTER (2)));

  GtkMenuShell* sub_status = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(sub_status, item_status_online);
  gtk_menu_shell_append(sub_status, item_status_away);
  gtk_menu_shell_append(sub_status, item_status_dnd);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (self->item_status), GTK_WIDGET (sub_status));


  GtkWidget* item_prefs = gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES,self->accelgroup);
  g_signal_connect (item_prefs, "activate", G_CALLBACK (config_cb),self_data);
  gtk_widget_add_accelerator(item_prefs, "activate", self->accelgroup, GDK_P, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* edit_profile = devchat_cb_data_new (self, GINT_TO_POINTER (URL_PROFILE_EDIT));

  GtkWidget* item_profile = gtk_image_menu_item_new_with_label (_("Edit profile..."));
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_profile), gtk_image_new_from_icon_name("system-users",GTK_ICON_SIZE_MENU));
  g_signal_connect (item_profile, "activate", G_CALLBACK (go_forum), edit_profile);

  GtkWidget* item_find = gtk_image_menu_item_new_from_stock (GTK_STOCK_FIND, self->accelgroup);
  g_signal_connect (item_find, "activate", G_CALLBACK (find), self_data);

  DevchatCBData* view_devnet = devchat_cb_data_new (self, GINT_TO_POINTER (URL_VISIT_L3));

  self->item_l3 = gtk_menu_item_new_with_label (_("Open DevNet fora..."));
  g_signal_connect (self->item_l3, "activate", G_CALLBACK (go_forum), view_devnet);
  gtk_widget_set_no_show_all(self->item_l3,TRUE);

  DevchatCBData* view_beta = devchat_cb_data_new (self, GINT_TO_POINTER (URL_VISIT_L5));

  self->item_l5 = gtk_menu_item_new_with_label (_("Open betatest fora..."));
  g_signal_connect (self->item_l5, "activate", G_CALLBACK (go_forum), view_beta);
  gtk_widget_set_no_show_all(self->item_l5,TRUE);

  DevchatCBData* view_forum = devchat_cb_data_new (self, GINT_TO_POINTER (URL_VISIT_L1));

  GtkWidget* item_forum = gtk_menu_item_new_with_label (_("Open forum..."));
  g_signal_connect (item_forum, "activate", G_CALLBACK (go_forum), view_forum);

  GtkWidget* item_tabclose = gtk_image_menu_item_new_with_label (_("Close tab"));
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_tabclose), gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_tabclose, "activate", G_CALLBACK (devchat_window_close_tab), self_data);
  gtk_widget_add_accelerator(item_tabclose, "activate", self->accelgroup, GDK_W, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  self->item_reconnect = gtk_image_menu_item_new_with_label (_("Reconnect"));
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (self->item_reconnect), gtk_image_new_from_stock(GTK_STOCK_REFRESH,GTK_ICON_SIZE_MENU));
  g_signal_connect (self->item_reconnect, "activate", G_CALLBACK (reconnect), self_data);
  gtk_widget_set_no_show_all(self->item_reconnect,TRUE);
  gtk_widget_add_accelerator(self->item_reconnect, "activate", self->accelgroup, GDK_R, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_disconnect = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT,self->accelgroup);
  g_signal_connect (item_disconnect, "activate", G_CALLBACK (destroy), self_data);
  gtk_widget_add_accelerator(item_disconnect, "activate", self->accelgroup, GDK_Q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_tab_next = gtk_image_menu_item_new_with_label (_("Next Tab"));
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_tab_next), gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_tab_next, "activate", G_CALLBACK (next_tab), self_data);
  gtk_widget_add_accelerator(item_tab_next, "activate", self->accelgroup, GDK_Page_Down, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(item_tab_next, "activate", self->accelgroup, GDK_Tab, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_tab_prev = gtk_image_menu_item_new_with_label (_("Previous Tab"));
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_tab_prev), gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_tab_prev, "activate", G_CALLBACK (prev_tab), self_data);
  gtk_widget_add_accelerator(item_tab_prev, "activate", self->accelgroup, GDK_Page_Up, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_his = gtk_image_menu_item_new_with_label (_("Show History..."));
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_his), gtk_image_new_from_icon_name("appointment-new",GTK_ICON_SIZE_MENU));
  g_signal_connect (item_his, "activate", G_CALLBACK (show_his), self_data);

  GtkWidget* item_about = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT,self->accelgroup);
  g_signal_connect (item_about, "activate", G_CALLBACK (about_cb), self_data);

  DevchatCBData* report_data = devchat_cb_data_new (self, "http://dev.yaki-syndicate.de/bugs/");
  GtkWidget* item_report = gtk_menu_item_new_with_label (_("Report a bug..."));
  g_signal_connect (item_report, "activate", G_CALLBACK (popup_open_link), report_data);

#ifdef G_OS_WIN32
  DevchatCBData* update_data = devchat_cb_data_new (self, g_strconcat ("http://dev.yaki-syndicate.de/update-check/update.py/check?OS=Windows&ver=", VERSION, NULL));
#else
  DevchatCBData* update_data = devchat_cb_data_new (self, g_strconcat ("http://dev.yaki-syndicate.de/update-check/update.py/check?OS=Unix&ver=", VERSION, NULL));
#endif
  GtkWidget* item_update = gtk_menu_item_new_with_label (_("Check for update..."));
  g_signal_connect (item_update, "activate", G_CALLBACK (popup_open_link), update_data);

  DevchatCBData* faq_data = devchat_cb_data_new (self, "http://dev.yaki-syndicate.de/index.php?/faq.html");
  GtkWidget* item_faq = gtk_menu_item_new_with_label (_("Frequently asked questions..."));
  g_signal_connect (item_faq, "activate", G_CALLBACK (popup_open_link), faq_data);

  DevchatCBData* format_b = devchat_cb_data_new (self, "b");

  GtkWidget* item_bold = gtk_image_menu_item_new_from_stock (GTK_STOCK_BOLD,self->accelgroup);
  g_signal_connect (item_bold, "activate", G_CALLBACK (devchat_window_btn_format), format_b);
  gtk_widget_add_accelerator(item_bold, "activate", self->accelgroup, GDK_B, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_i = devchat_cb_data_new (self, "i");

  GtkWidget* item_italic = gtk_image_menu_item_new_from_stock (GTK_STOCK_ITALIC,self->accelgroup);
  g_signal_connect (item_italic, "activate", G_CALLBACK (devchat_window_btn_format), format_i);
  gtk_widget_add_accelerator(item_italic, "activate", self->accelgroup, GDK_I, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_u = devchat_cb_data_new (self, "u");

  GtkWidget* item_line = gtk_image_menu_item_new_from_stock (GTK_STOCK_UNDERLINE,self->accelgroup);
  g_signal_connect (item_line, "activate", G_CALLBACK (devchat_window_btn_format), format_u);
  gtk_widget_add_accelerator(item_line, "activate", self->accelgroup, GDK_U, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_img = devchat_cb_data_new (self, "img");

  GtkWidget* item_pict = gtk_image_menu_item_new_with_mnemonic (_("I_mage"));
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_pict), gtk_image_new_from_icon_name("image-x-generic",GTK_ICON_SIZE_MENU));
  g_signal_connect (item_pict, "activate", G_CALLBACK (devchat_window_btn_format), format_img);
  gtk_widget_add_accelerator(item_pict, "activate", self->accelgroup, GDK_M, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_url = devchat_cb_data_new (self, "url");

  GtkWidget* item_link = gtk_image_menu_item_new_with_mnemonic (_("_Link"));
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_link), gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_link, "activate", G_CALLBACK (devchat_window_btn_format), format_url);
  gtk_widget_add_accelerator(item_link, "activate", self->accelgroup, GDK_L, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  self->item_smilies = gtk_menu_item_new_with_mnemonic (_("_Smilies..."));
  self->item_presets = gtk_menu_item_new_with_mnemonic (_("_Preset texts..."));



  GtkMenuShell* main_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(main_sub, self->item_connect);
  gtk_menu_shell_append(main_sub, self->item_status);
  gtk_menu_shell_append(main_sub, item_tabclose);
  gtk_menu_shell_append(main_sub, gtk_menu_item_new());
  gtk_menu_shell_append(main_sub, self->item_reconnect);
  gtk_menu_shell_append(main_sub, item_disconnect);
  gtk_menu_item_set_submenu(menu_main,GTK_WIDGET(main_sub));

  GtkMenuShell* insert_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(insert_sub, item_bold);
  gtk_menu_shell_append(insert_sub, item_italic);
  gtk_menu_shell_append(insert_sub, item_line);
  gtk_menu_shell_append(insert_sub, item_pict);
  gtk_menu_shell_append(insert_sub, item_link);
  gtk_menu_shell_append(insert_sub, gtk_menu_item_new());
  gtk_menu_shell_append(insert_sub, self->item_smilies);
  gtk_menu_shell_append(insert_sub, self->item_presets);
  gtk_menu_item_set_submenu(menu_insert,GTK_WIDGET(insert_sub));

  GtkMenuShell* edit_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(edit_sub, item_find);
  gtk_menu_shell_append(edit_sub, item_profile);
  gtk_menu_shell_append(edit_sub, gtk_menu_item_new());
  gtk_menu_shell_append(edit_sub, item_prefs);
  gtk_menu_item_set_submenu(menu_edit,GTK_WIDGET(edit_sub));

  GtkMenuShell* view_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(view_sub, item_tab_next);
  gtk_menu_shell_append(view_sub, item_tab_prev);
  gtk_menu_shell_append(view_sub, gtk_menu_item_new());
  gtk_menu_shell_append(view_sub, self->item_l3);
  gtk_menu_shell_append(view_sub, self->item_l5);
  gtk_menu_shell_append(view_sub, item_forum);
  gtk_menu_shell_append(view_sub, gtk_menu_item_new());
  gtk_menu_shell_append(view_sub, item_his);
  gtk_menu_item_set_submenu(menu_view,GTK_WIDGET(view_sub));

  GtkMenuShell* about_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(about_sub, item_faq);
  gtk_menu_shell_append(about_sub, item_report);
  gtk_menu_shell_append(about_sub, item_update);
  gtk_menu_shell_append(about_sub, gtk_menu_item_new());
  gtk_menu_shell_append(about_sub, item_about);
  gtk_menu_item_set_submenu(menu_about,GTK_WIDGET(about_sub));

  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_main));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_edit));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_insert));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_view));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_about));

  gtk_box_pack_start (GTK_BOX(vbox0), GTK_WIDGET(menu), FALSE,FALSE,0);


  self->notebook = gtk_notebook_new ();
  g_signal_connect(self->notebook, "switch-page", G_CALLBACK (tab_changed), self_data);
  gtk_box_pack_start(GTK_BOX(vbox0), self->notebook, TRUE,TRUE,0);
  gtk_container_add(GTK_CONTAINER(self->window),GTK_WIDGET(vbox0));
  g_signal_connect(self->window, "focus-in-event", G_CALLBACK(devchat_window_tab_changed_win),self_data);

  self->statusbar = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(self->statusbar),TRUE);
  self->statuslabel = gtk_label_new (_("Not connected."));
  self->userlabel = gtk_label_new ("");
  gtk_box_pack_end ( GTK_BOX(self->statusbar), self->userlabel,FALSE,FALSE,0);
  gtk_box_pack_end ( GTK_BOX(self->statusbar), gtk_vseparator_new(),FALSE,FALSE,1);
  gtk_box_pack_end ( GTK_BOX(self->statusbar), self->statuslabel,FALSE,FALSE,0);
  gtk_box_pack_end ( GTK_BOX(self->statusbar), gtk_vseparator_new(),FALSE,FALSE,1);
  gtk_box_pack_start(GTK_BOX(vbox0), self->statusbar, FALSE,FALSE,1);

  self->output = gtk_text_buffer_new (NULL);
  self->outputwidget = gtk_text_view_new_with_buffer (self->output);
  GdkColor l1;
  GdkColor font;
  gdk_color_parse (self->settings.color_l1, &l1);
  gdk_color_parse (self->settings.color_font, &font);
  gtk_widget_modify_base (self->outputwidget, GTK_STATE_NORMAL, &l1);
  gtk_widget_modify_text (self->outputwidget, GTK_STATE_NORMAL, &font);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(self->outputwidget), FALSE);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(self->outputwidget), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(self->outputwidget), GTK_WRAP_WORD_CHAR);
  gtk_widget_set_size_request(self->outputwidget, 300,100);
  g_signal_connect (self->output, "mark-set", G_CALLBACK(devchat_window_on_mark_set_cb),self_data);
  g_signal_connect (self->outputwidget, "motion-notify-event", G_CALLBACK(devchat_window_on_motion_cb),self_data);
  g_signal_connect (self->outputwidget, "button-press-event", G_CALLBACK(devchat_window_button_press_cb),self_data);

  GtkWidget* scroller1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller1),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroller1),GTK_SHADOW_ETCHED_IN);
  g_object_set (self->outputwidget, "left-margin", 2, "right-margin", 2, "pixels-below-lines", 1, "pixels-above-lines", 1, NULL);
  gtk_container_add (GTK_CONTAINER(scroller1), self->outputwidget);

  GtkWidget* search_box = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (search_box), scroller1, TRUE, TRUE, 0);

  self->searchbar = gtk_hbox_new (FALSE, 1);
  self->search_entry = gtk_entry_new ();
  self->search_button = gtk_button_new_from_stock (GTK_STOCK_FIND);
  g_signal_connect (self->search_button, "clicked", G_CALLBACK (devchat_window_find), devchat_cb_data_new (self, self->search_entry));
  GtkWidget* btn_bar_close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  g_signal_connect (btn_bar_close, "clicked", G_CALLBACK (devchat_window_close_search), devchat_cb_data_new (self, self->searchbar));
  gtk_widget_add_accelerator (btn_bar_close, "clicked", self->accelgroup, GDK_Escape, 0, 0);
  gtk_box_pack_start (GTK_BOX (self->searchbar), self->search_entry, TRUE, TRUE, 1);
  gtk_box_pack_start (GTK_BOX (self->searchbar), self->search_button, FALSE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX (self->searchbar), btn_bar_close, FALSE, FALSE, 1);
  gtk_widget_set_no_show_all (self->searchbar, TRUE);

  gtk_box_pack_start (GTK_BOX (search_box), self->searchbar, FALSE, FALSE, 0);

  gtk_paned_pack1 (GTK_PANED(hpaned1), search_box, TRUE,TRUE);

  GtkWidget* scroller2 = gtk_scrolled_window_new (NULL, NULL);
  self->userlist = gtk_vbox_new (FALSE,1);
  gtk_widget_set_size_request (self->userlist, 180, -1);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(scroller2), self->userlist);
  self->userlist_port = gtk_bin_get_child(GTK_BIN(scroller2));

  if (self->settings.coloruser == TRUE)
  {
    gtk_widget_modify_bg (self->userlist_port, GTK_STATE_NORMAL, &l1);
  }

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller2),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroller2),GTK_SHADOW_ETCHED_IN);
  gtk_paned_pack2 (GTK_PANED(hpaned1), scroller2, FALSE, FALSE);

  GtkWidget* vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_size_request (vbox2, -1, 74);

  self->inputbar = gtk_hbox_new (FALSE, 0);
  self->level_box = gtk_combo_box_new_text ();
  gtk_widget_set_no_show_all (self->level_box,TRUE);
  g_signal_connect(self->level_box, "changed", G_CALLBACK (level_changed), self_data);
  gtk_box_pack_start(GTK_BOX(self->inputbar),self->level_box,FALSE,FALSE,1);
  GtkWidget* btn_bold = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_bold), gtk_image_new_from_stock(GTK_STOCK_BOLD,GTK_ICON_SIZE_SMALL_TOOLBAR));
  GtkWidget* btn_ital = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_ital), gtk_image_new_from_stock(GTK_STOCK_ITALIC,GTK_ICON_SIZE_SMALL_TOOLBAR));
  GtkWidget* btn_line = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_line), gtk_image_new_from_stock(GTK_STOCK_UNDERLINE,GTK_ICON_SIZE_SMALL_TOOLBAR));
  GtkWidget* btn_url = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_url), gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,GTK_ICON_SIZE_SMALL_TOOLBAR));
  gtk_widget_set_tooltip_text (btn_url, "Insert URL");
  GtkWidget* btn_img = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (btn_img), gtk_image_new_from_icon_name("image-x-generic",GTK_ICON_SIZE_SMALL_TOOLBAR));
  gtk_widget_set_tooltip_text (btn_img, _("Insert Image\nNote that you will be killed if you insert images larger than 32*32px. You have been warned."));

  g_signal_connect (btn_bold, "clicked", G_CALLBACK (devchat_window_btn_format), format_b);
  g_signal_connect (btn_ital, "clicked", G_CALLBACK (devchat_window_btn_format), format_i);
  g_signal_connect (btn_line, "clicked", G_CALLBACK (devchat_window_btn_format), format_u);
  g_signal_connect (btn_url, "clicked", G_CALLBACK (devchat_window_btn_format), format_url);
  g_signal_connect (btn_img, "clicked", G_CALLBACK (devchat_window_btn_format), format_img);

  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_bold,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_ital,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_line,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_url,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_img,FALSE,FALSE,0);

  self->color_box = gtk_combo_box_new_text ();
  g_signal_connect (self->color_box, "changed", G_CALLBACK (devchat_window_color_changed), self_data);

  gtk_combo_box_append_text (GTK_COMBO_BOX (self->color_box), _("Text color"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (self->color_box), "red");
  gtk_combo_box_append_text (GTK_COMBO_BOX (self->color_box), "green");
  gtk_combo_box_append_text (GTK_COMBO_BOX (self->color_box), "blue");
  gtk_combo_box_append_text (GTK_COMBO_BOX (self->color_box), "cyan");
  gtk_combo_box_append_text (GTK_COMBO_BOX (self->color_box), "magenta");
  gtk_combo_box_append_text (GTK_COMBO_BOX (self->color_box), "yellow");

  gtk_combo_box_set_active (GTK_COMBO_BOX (self->color_box), 0);

  gtk_box_pack_start (GTK_BOX(self->inputbar),self->color_box,FALSE,FALSE,1);

  self->filter_ul = gtk_combo_box_new_text ();
  gtk_widget_set_no_show_all (self->filter_ul,TRUE);
  g_signal_connect(self->filter_ul, "changed", G_CALLBACK (devchat_window_filter_ul_changed), self->output);
  gtk_box_pack_start(GTK_BOX(self->inputbar),self->filter_ul,FALSE,FALSE,0);

  self->filter_ml = gtk_combo_box_new_text ();
  gtk_widget_set_no_show_all (self->filter_ml,TRUE);
  g_signal_connect(self->filter_ml, "changed", G_CALLBACK (devchat_window_filter_ml_changed), self->output);
  gtk_box_pack_start(GTK_BOX(self->inputbar),self->filter_ml,FALSE,FALSE,0);

  self->btn_send = gtk_button_new_from_stock(GTK_STOCK_OK);
  g_signal_connect (self->btn_send, "clicked", G_CALLBACK (devchat_window_btn_send),self_data);
  gtk_widget_add_accelerator(self->btn_send, "clicked", self->accelgroup, GDK_Return, 0, 0);
  gtk_widget_add_accelerator(self->btn_send, "clicked", self->accelgroup, GDK_KP_Enter, 0, 0);

  GtkWidget* btn_quit = gtk_button_new_from_stock(GTK_STOCK_QUIT);
  g_signal_connect (btn_quit, "clicked", G_CALLBACK (destroy),self_data);
  gtk_widget_add_accelerator(btn_quit, "clicked", self->accelgroup, GDK_Q, GDK_CONTROL_MASK, 0);

  gtk_box_pack_end (GTK_BOX(self->inputbar),btn_quit,FALSE,FALSE,0);
  gtk_box_pack_end (GTK_BOX(self->inputbar),gtk_vseparator_new(),FALSE,FALSE,0);
  gtk_box_pack_end (GTK_BOX(self->inputbar),self->btn_send,FALSE,FALSE,0);

  self->chk_raw = gtk_check_button_new_with_label (_("Raw mode"));
  gtk_widget_set_tooltip_text (self->chk_raw, _("Send raw HTML text. Needed e.g. for browser-kicks and <!-- comments -->. Not recommended for daily use."));
  gtk_box_pack_end (GTK_BOX(self->inputbar),self->chk_raw,FALSE,FALSE,0);
  gtk_widget_set_no_show_all (self->chk_raw,TRUE);

  gtk_box_pack_start (GTK_BOX(vbox2),self->inputbar,FALSE,FALSE,0);


  GtkWidget* scroller3 = gtk_scrolled_window_new (NULL,NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller3),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroller3),GTK_SHADOW_ETCHED_IN);

  self->inputwidget = gtk_text_view_new ();
  self->input = gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->inputwidget));
  gtk_widget_modify_base (self->inputwidget, GTK_STATE_NORMAL, &l1);
  gtk_widget_modify_text (self->inputwidget, GTK_STATE_NORMAL, &font);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(self->inputwidget), GTK_WRAP_WORD_CHAR);
  g_object_set (self->inputwidget, "left-margin", 2, "right-margin", 2, "pixels-below-lines", 1, "pixels-above-lines", 1, NULL);
  gtk_container_add(GTK_CONTAINER(scroller3),self->inputwidget);

#ifdef SPELLCHECK
  const gchar* const* langs = g_get_language_names ();
  if (!no_spellcheck && !gtkspell_new_attach (GTK_TEXT_VIEW (self->inputwidget), langs[0], NULL))
    err (_("Error S: Error initialising spell checker!"));
#endif

  gtk_box_pack_start (GTK_BOX(vbox2),scroller3,TRUE,TRUE,0);

  devchat_window_create_tags (self->output, self_data);

  self->loginbar = gtk_vbox_new (FALSE,0);
  GtkWidget* hbox2 = gtk_hbox_new (FALSE,0);
  self->user_entry = gtk_entry_new ();
  self->pass_entry = gtk_entry_new ();
  gtk_entry_set_text( GTK_ENTRY (self->user_entry), self->settings.user);
  gtk_entry_set_text( GTK_ENTRY (self->pass_entry), self->settings.pass);
  gtk_entry_set_visibility (GTK_ENTRY (self->pass_entry), FALSE);

  self->btn_connect = gtk_button_new_from_stock (GTK_STOCK_CONNECT);
  g_signal_connect (self->btn_connect, "clicked", G_CALLBACK (login), self_data);
  gtk_widget_add_accelerator(self->btn_connect, "activate", self->accelgroup, GDK_Return, 0, 0);
  gtk_widget_add_accelerator(self->btn_connect, "activate", self->accelgroup, GDK_KP_Enter, 0, 0);

  gtk_box_pack_start (GTK_BOX(hbox2),self->user_entry,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX(hbox2),self->pass_entry,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX(hbox2),self->btn_connect,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (self->loginbar), hbox2,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox2), self->loginbar,FALSE,FALSE,0);

  GtkWidget* vpaned = gtk_vpaned_new ();
  gtk_paned_pack1 (GTK_PANED(vpaned),hpaned1,TRUE,TRUE);
  gtk_paned_pack2 (GTK_PANED(vpaned),vbox2,FALSE,FALSE);

  GtkWidget* main_label = gtk_label_new (_("X-DEVCHAT"));
  GtkWidget* event_box = gtk_event_box_new ();
  gtk_event_box_set_above_child (GTK_EVENT_BOX (event_box), TRUE);
  gtk_event_box_set_visible_window (GTK_EVENT_BOX (event_box), FALSE);
  gtk_container_add (GTK_CONTAINER (event_box), main_label);
  g_signal_connect (event_box, "button-release-event", G_CALLBACK (devchat_window_tab_changed_win), self_data);

  gtk_widget_show_all (event_box);

  gtk_notebook_append_page_menu (GTK_NOTEBOOK (self->notebook), vpaned, event_box, gtk_label_new (_("X-DEVCHAT")));

  gtk_notebook_set_tab_reorderable (GTK_NOTEBOOK(self->notebook), vpaned, FALSE);

  self->trayicon = gtk_status_icon_new ();
  gtk_status_icon_set_visible (GTK_STATUS_ICON (self->trayicon), self->settings.showtray);
  g_signal_connect (self->trayicon, "activate", G_CALLBACK (toggle_tray_minimize), self_data);
  g_signal_connect (self->trayicon, "popup-menu", G_CALLBACK (show_tray_menu), self_data);

  gtk_widget_show_all (self->window);
  gtk_widget_hide_all (self->inputbar);

  gtk_window_resize (GTK_WINDOW (self->window), self->settings.width,self->settings.height);
  gtk_paned_set_position( GTK_PANED(vpaned),-1);

  gtk_widget_grab_focus (self->user_entry);

  gtk_about_dialog_set_url_hook ((GtkAboutDialogActivateLinkFunc) launch_browser, self_data, NULL);

#ifdef NOTIFY
  notify_init(APPNAME);
#endif

  self->users_without_avatar = NULL;
  self->firstrun = TRUE;
  self->no_halt_requested = TRUE;
  self->lastid = g_strdup("1");
  if (debug)
    dbg("Initalising libsoup...");

  self->session = soup_session_async_new ();
  self->jar = soup_cookie_jar_text_new (self->jarfile, FALSE);
  soup_session_add_feature (self->session, SOUP_SESSION_FEATURE (self->jar));
  g_object_set (self->session, SOUP_SESSION_USER_AGENT, "Mozilla/5.0 (compatible)",
                               SOUP_SESSION_PROXY_URI, soup_uri_new (self->settings.proxy), NULL);

  gchar* cookies = soup_cookie_jar_get_cookies (self->jar, soup_uri_new ("http://www.egosoft.com"), FALSE);
  if (cookies)
  {
    /*Check whether we're really logged in.*/
    if (!g_strstr_len (cookies, -1, "s%3A6%3A%22userid%22%3Bi%3A-1"))
    {
      /*Still logged in. */
      gtk_widget_set_sensitive(self->btn_connect,FALSE);
      gtk_widget_set_sensitive(self->user_entry,FALSE);
      gtk_widget_set_sensitive(self->pass_entry,FALSE);

      if (debug)
        dbg ("Found cookies still edible.");

      gtk_label_set_text (GTK_LABEL (self->statuslabel), _("Determining user level..."));
      SoupMessage* step2 = soup_message_new ("GET", "http://www.egosoft.com");

      if (debug)
        dbg ("Trying to determine userlevel...");

      soup_session_queue_message (self->session, step2, SOUP_SESSION_CALLBACK(remote_level), self_data);
    }
    else
    {
      /*Thank you very much for spamming every user with useless cookies, phpBB.*/
      GSList* cookie_list = soup_cookie_jar_all_cookies (self->jar);
      while (cookie_list)
      {
        soup_cookie_jar_delete_cookie (self->jar, (SoupCookie*) cookie_list->data);
        GSList* tmp = cookie_list;
        cookie_list = cookie_list->next;
        g_slist_free_1 (tmp);
      }
    }
    g_free (cookies);
  }

  g_timeout_add_seconds (4, (GSourceFunc) get_pos_size, self);

}

static void
devchat_window_class_init (DevchatWindowClass* klass)
{
  GObjectClass* gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = devchat_window_set_property;
  gobject_class->get_property = devchat_window_get_property;

  g_object_class_install_property (gobject_class, SETTINGS_BROWSER, g_param_spec_string
                                                     ( "browser", "Browser",
                                                       "Browser used to display links.", "<native>",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_FONT, g_param_spec_string
                                                     ( "color_font", "Font Color",
                                                       "Color used for normal text.", "#fff",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_L1, g_param_spec_string
                                                     ( "color_l1", "L1 background",
                                                       "Color used for L1 backgrounds.", "#171717",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_L3, g_param_spec_string
                                                     ( "color_l3", "L3 background",
                                                       "Color used for L3 backgrounds.", "#441818",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_L5, g_param_spec_string
                                                     ( "color_l5", "L5 background",
                                                       "Color used for L5 backgrounds.", "#242454",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_L6, g_param_spec_string
                                                     ( "color_l6", "L6 background",
                                                       "Color used for L6 backgrounds.", "#144414",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_GOLDIES, g_param_spec_string
                                                     ( "color_goldies", "Goldie color",
                                                       "Color used for L7+ users.", "#fc0",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_GREENS, g_param_spec_string
                                                     ( "color_greens", "Greenie color",
                                                       "Color used for L6 users.", "#0c9",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_BLUES, g_param_spec_string
                                                     ( "color_blues", "Peasant color",
                                                       "Color used for L5- users.", "#47f",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_TIME, g_param_spec_string
                                                     ( "color_time", "Time color",
                                                       "Color used for aux. messages, e.g. time.", "#999",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_URL, g_param_spec_string
                                                     ( "color_url", "URI Color",
                                                       "Color used to display links.", "#ee6",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_URL_VISITED, g_param_spec_string
                                                     ( "color_url_visited", "Visited URI color",
                                                       "Color used to display visited links.", "#ec6",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_URL_HOVER, g_param_spec_string
                                                     ( "color_url_hover", "Hover Link color",
                                                       "Color used to display hovered links.", "#fff",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_HIGHLIGHT, g_param_spec_string
                                                     ( "color_highlight", "Tab highlight color",
                                                       "Color used to display highlighted tabs.", "#b00",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_USER, g_param_spec_string
                                                     ( "user", "User name",
                                                       "Username.", g_get_user_name (),
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_PASS, g_param_spec_string
                                                     ( "pass", "Password",
                                                       "Password.", "<hidden>",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_SHOWID, g_param_spec_boolean
                                                     ( "showid", "Show post id",
                                                       "Shows the internal post id next to (new) posts.", FALSE,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_SHOWHIDDEN, g_param_spec_boolean
                                                     ( "showhidden", "Show hidden usernames",
                                                       "Shows user names when normally required to be hidden (stealth goldies, stealth posts by L6+).", FALSE,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_AUTOJOIN, g_param_spec_boolean
                                                     ( "autojoin", "Auto-login",
                                                       "Logs in automatically when starting the client.", FALSE,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_STEALTHJOIN, g_param_spec_boolean
                                                     ( "stealthjoin", "Stealth login",
                                                       "Supress join/quit messages.", FALSE,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_TRAYICON, g_param_spec_boolean
                                                     ( "trayicon", "Show Tray Icon",
                                                       "Whether to show a tray icon.", FALSE,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLORUSER, g_param_spec_boolean
                                                     ( "coloruser", "Tint user list",
                                                       "Tints the userlist with L1 background color. Recommended for bright themes.", TRUE,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_NOTIFY, g_param_spec_string
                                                     ( "notify", "Play audio notifications",
                                                       "Plays notifications, per default audio files.", "<native>",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_VNOTIFY, g_param_spec_string
                                                     ( "vnotify", "Show visible notifications",
                                                       "Shows notifications, per default libnotify visual ones.", "<native>",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_HANDLE_WIDTH, g_param_spec_int
                                                     ( "handle_width", "Position of the text view/user list separator",
                                                       "Determines the width of the text output widget.", 0, INT_MAX, 0,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_WIDTH, g_param_spec_int
                                                     ( "width", "Window width",
                                                       "Width of the window.", 480, INT_MAX, 600,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_HEIGHT, g_param_spec_int
                                                     ( "height", "Window height",
                                                       "Height of the window.", 320, INT_MAX, 400,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_X, g_param_spec_int
                                                     ( "x", "Window x position",
                                                       "X position of the window.", 0, INT_MAX, 0,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_Y, g_param_spec_int
                                                     ( "y", "Window y position",
                                                       "Y position of the window.", 0, INT_MAX, 0,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_MAXIMIZED, g_param_spec_boolean
                                                     ( "maximized", "Maximize Window",
                                                       "Indicates whether the window should be maximized.", FALSE,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_RED, g_param_spec_string
                                                     ( "color_red", "Red Color",
                                                       "Color used for red text.", "#f00",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_GREEN, g_param_spec_string
                                                     ( "color_green", "Green Color",
                                                       "Color used for green text.", "#0f0",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_BLUE, g_param_spec_string
                                                     ( "color_blue", "Blue Color",
                                                       "Color used for blue text.", "#00f",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_CYAN, g_param_spec_string
                                                     ( "color_cyan", "Cyan color",
                                                       "Color used for cyan text.", "#0ff",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_YELLOW, g_param_spec_string
                                                     ( "color_yellow", "Yellow color",
                                                       "Color used for yellow text.", "#ff0",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
  g_object_class_install_property (gobject_class, SETTINGS_COLOR_MAGENTA, g_param_spec_string
                                                     ( "color_magenta", "Magenta color",
                                                       "Color used for magenta text.", "#f0f",
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));
}

static void devchat_window_set_property (GObject* object, guint id, const GValue* value, GParamSpec* pspec)
{
  DevchatWindow* window = DEVCHAT_WINDOW (object);
  GdkColor color;
  GtkTextTagTable* t;
  GtkTextTag* tag;
  switch (id)
  {
    case SETTINGS_BROWSER: window->settings.browser = g_value_dup_string (value); break;
    case SETTINGS_COLOR_FONT: window->settings.color_font = g_value_dup_string (value);
                              gdk_color_parse (g_value_dup_string (value), &color);
                              gtk_widget_modify_text (window->outputwidget, GTK_STATE_NORMAL, &color);
                              g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id)));
                              gtk_widget_modify_text (window->inputwidget, GTK_STATE_NORMAL, &color); break;
    case SETTINGS_COLOR_L1: window->settings.color_l1 = g_value_dup_string (value);
                            t = gtk_text_buffer_get_tag_table (window->output);
                            tag = gtk_text_tag_table_lookup (t, "l1");
                            g_object_set (tag, "paragraph-background", g_value_dup_string (value), NULL);
                            gdk_color_parse (g_value_dup_string (value), &color);
                            gtk_widget_modify_base (window->outputwidget, GTK_STATE_NORMAL, &color);
                            gtk_widget_modify_bg (window->userlist_port, GTK_STATE_NORMAL, &color);
                            g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id)));
                            gtk_widget_modify_base (window->inputwidget, GTK_STATE_NORMAL, &color); break;
    case SETTINGS_COLOR_L3: window->settings.color_l3 = g_value_dup_string (value);
                            t = gtk_text_buffer_get_tag_table (window->output);
                            tag = gtk_text_tag_table_lookup (t, "l3");
                            g_object_set (tag, "paragraph-background", g_value_dup_string (value), NULL);
                            g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_L5: window->settings.color_l5 = g_value_dup_string (value);
                            t = gtk_text_buffer_get_tag_table (window->output);
                            tag = gtk_text_tag_table_lookup (t, "l5");
                            g_object_set (tag, "paragraph-background", g_value_dup_string (value), NULL);
                            g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_L6: window->settings.color_l6 = g_value_dup_string (value);
                            t = gtk_text_buffer_get_tag_table (window->output);
                            tag = gtk_text_tag_table_lookup (t, "l6");
                            g_object_set (tag, "paragraph-background", g_value_dup_string (value), NULL);
                            g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_GOLDIES: window->settings.color_goldies = g_value_dup_string (value);
                                t = gtk_text_buffer_get_tag_table (window->output);
                                tag = gtk_text_tag_table_lookup (t, "goldie");
                                g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                                g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_GREENS: window->settings.color_greens = g_value_dup_string (value);
                                t = gtk_text_buffer_get_tag_table (window->output);
                                tag = gtk_text_tag_table_lookup (t, "greenie");
                                g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                                g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_BLUES: window->settings.color_blues = g_value_dup_string (value);
                               t = gtk_text_buffer_get_tag_table (window->output);
                               tag = gtk_text_tag_table_lookup (t, "peasant");
                               g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                               g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_TIME: window->settings.color_time = g_value_dup_string (value);
                              t = gtk_text_buffer_get_tag_table (window->output);
                              tag = gtk_text_tag_table_lookup (t, "time");
                              g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                              g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_URL: window->settings.color_url = g_value_dup_string (value);
                             t = gtk_text_buffer_get_tag_table (window->output);
                             gtk_text_tag_table_foreach (t, (GtkTextTagTableForeach) url_tag_nv_color_cb, g_value_dup_string (value));
                             g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_URL_VISITED: window->settings.color_url_visited = g_value_dup_string (value);
                                     t = gtk_text_buffer_get_tag_table (window->output);
                                     gtk_text_tag_table_foreach (t, (GtkTextTagTableForeach) url_tag_v_color_cb, g_value_dup_string (value));
                                     g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_URL_HOVER: window->settings.color_url_hover = g_value_dup_string (value); break;
    case SETTINGS_COLOR_HIGHLIGHT: window->settings.color_highlight = g_value_dup_string (value); break;
    case SETTINGS_USER: window->settings.user = g_value_dup_string (value); gtk_entry_set_text( GTK_ENTRY (window->user_entry), window->settings.user); break;
    case SETTINGS_PASS: window->settings.pass = g_value_dup_string (value); gtk_entry_set_text( GTK_ENTRY (window->pass_entry), window->settings.pass); break;
    case SETTINGS_SHOWID: window->settings.showid = g_value_get_boolean (value); break;
    case SETTINGS_STEALTHJOIN: window->settings.stealthjoin = g_value_get_boolean (value); break;
    case SETTINGS_AUTOJOIN: window->settings.autojoin = g_value_get_boolean (value);
                            if (window->firstrun && window->settings.autojoin && !soup_cookie_jar_get_cookies (window->jar, soup_uri_new ("http://www.egosoft.com"), FALSE))
                              login (window->btn_connect, devchat_cb_data_new (window, NULL));
                            break;
    case SETTINGS_SHOWHIDDEN: window->settings.showhidden = g_value_get_boolean (value); break;
    case SETTINGS_COLORUSER: window->settings.coloruser = g_value_get_boolean (value); break;
    case SETTINGS_NOTIFY: window->settings.notify = g_value_dup_string (value); break;
    case SETTINGS_VNOTIFY: window->settings.vnotify = g_value_dup_string (value); break;
    case SETTINGS_HANDLE_WIDTH: window->settings.handle_width = g_value_get_int (value);
                         g_timeout_add (500, (GSourceFunc) paned_update, devchat_cb_data_new (window, NULL)); break;
    case SETTINGS_WIDTH: window->settings.width = g_value_get_int (value);
                         gtk_window_resize (GTK_WINDOW (window->window), window->settings.width, window->settings.height); break;
    case SETTINGS_HEIGHT: window->settings.height = g_value_get_int (value);
                         gtk_window_resize (GTK_WINDOW (window->window), window->settings.width, window->settings.height); break;
    case SETTINGS_X: window->settings.x = g_value_get_int (value);
                         gtk_window_move (GTK_WINDOW (window->window), window->settings.x, window->settings.y); break;
    case SETTINGS_Y: window->settings.y = g_value_get_int (value);
                         gtk_window_move (GTK_WINDOW (window->window), window->settings.x, window->settings.y); break;
    case SETTINGS_TRAYICON: window->settings.showtray = g_value_get_boolean (value);
                            gtk_status_icon_set_visible (GTK_STATUS_ICON (window->trayicon), window->settings.showtray); break;
    case SETTINGS_MAXIMIZED: window->settings.maximized = g_value_get_boolean (value);
                             if (window->settings.maximized)
                               gtk_window_maximize (GTK_WINDOW (window->window));
                             else
                               gtk_window_unmaximize (GTK_WINDOW (window->window)); break;
    case SETTINGS_COLOR_RED: window->settings.color_red = g_value_dup_string (value);
                                t = gtk_text_buffer_get_tag_table (window->output);
                                tag = gtk_text_tag_table_lookup (t, "red");
                                g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                                g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_GREEN: window->settings.color_green = g_value_dup_string (value);
                                t = gtk_text_buffer_get_tag_table (window->output);
                                tag = gtk_text_tag_table_lookup (t, "green");
                                g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                                g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_BLUE: window->settings.color_blue = g_value_dup_string (value);
                                t = gtk_text_buffer_get_tag_table (window->output);
                                tag = gtk_text_tag_table_lookup (t, "blue");
                                g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                                g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_YELLOW: window->settings.color_yellow = g_value_dup_string (value);
                                t = gtk_text_buffer_get_tag_table (window->output);
                                tag = gtk_text_tag_table_lookup (t, "yellow");
                                g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                                g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_CYAN: window->settings.color_cyan = g_value_dup_string (value);
                                t = gtk_text_buffer_get_tag_table (window->output);
                                tag = gtk_text_tag_table_lookup (t, "cyan");
                                g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                                g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
    case SETTINGS_COLOR_MAGENTA: window->settings.color_magenta = g_value_dup_string (value);
                                t = gtk_text_buffer_get_tag_table (window->output);
                                tag = gtk_text_tag_table_lookup (t, "magenta");
                                g_object_set (tag, "foreground", g_value_dup_string (value), NULL);
                                g_hash_table_foreach (window->conversations, (GHFunc) update_tags, devchat_cb_data_new(window, GINT_TO_POINTER(id))); break;
  }
}

gboolean paned_update (DevchatCBData* data)
{
  GtkWidget* hpaned1 = gtk_widget_get_parent (gtk_widget_get_parent (data->window->userlist_port));
  gtk_paned_set_position (GTK_PANED(hpaned1), data->window->settings.handle_width);
  return FALSE;
}

void update_tags (gchar* key, DevchatConversation* value, DevchatCBData* data)
{
  GtkTextTagTable* t;
  GtkTextTag* tag;
  GdkColor color;
  switch (GPOINTER_TO_INT (data->data))
  {
    case SETTINGS_COLOR_FONT: gdk_color_parse (data->window->settings.color_font, &color);
                              gtk_widget_modify_text (value->out_widget, GTK_STATE_NORMAL, &color);
                              if (value->in_widget) {gtk_widget_modify_text (value->in_widget, GTK_STATE_NORMAL, &color);} break;
    case SETTINGS_COLOR_L1: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                            tag = gtk_text_tag_table_lookup (t, "l1");
                            g_object_set (tag, "paragraph-background", data->window->settings.color_l1, NULL);
                            gdk_color_parse (data->window->settings.color_l1, &color);
                            gtk_widget_modify_base (value->out_widget, GTK_STATE_NORMAL, &color);
                            if (value->in_widget) {gtk_widget_modify_base (value->in_widget, GTK_STATE_NORMAL, &color);} break;
    case SETTINGS_COLOR_GOLDIES: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                                tag = gtk_text_tag_table_lookup (t, "goldie");
                                g_object_set (tag, "foreground", data->window->settings.color_goldies, NULL); break;
    case SETTINGS_COLOR_GREENS: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                                tag = gtk_text_tag_table_lookup (t, "greenie");
                                g_object_set (tag, "foreground", data->window->settings.color_greens, NULL); break;
    case SETTINGS_COLOR_BLUES: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                               tag = gtk_text_tag_table_lookup (t, "peasant");
                               g_object_set (tag, "foreground", data->window->settings.color_blues, NULL); break;
    case SETTINGS_COLOR_TIME: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                              tag = gtk_text_tag_table_lookup (t, "time");
                              g_object_set (tag, "foreground", data->window->settings.color_time, NULL); break;
    case SETTINGS_COLOR_URL: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                             gtk_text_tag_table_foreach (t, (GtkTextTagTableForeach) url_tag_nv_color_cb, data->window->settings.color_url); break;
    case SETTINGS_COLOR_URL_VISITED: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                             gtk_text_tag_table_foreach (t, (GtkTextTagTableForeach) url_tag_nv_color_cb, data->window->settings.color_url_visited); break;
    case SETTINGS_COLOR_RED: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                                tag = gtk_text_tag_table_lookup (t, "red");
                                g_object_set (tag, "foreground", data->window->settings.color_red, NULL); break;
    case SETTINGS_COLOR_GREEN: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                                tag = gtk_text_tag_table_lookup (t, "green");
                                g_object_set (tag, "foreground", data->window->settings.color_green, NULL); break;
    case SETTINGS_COLOR_BLUE: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                                tag = gtk_text_tag_table_lookup (t, "blue");
                                g_object_set (tag, "foreground", data->window->settings.color_blue, NULL); break;
    case SETTINGS_COLOR_MAGENTA: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                                tag = gtk_text_tag_table_lookup (t, "magenta");
                                g_object_set (tag, "foreground", data->window->settings.color_magenta, NULL); break;
    case SETTINGS_COLOR_CYAN: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                                tag = gtk_text_tag_table_lookup (t, "cyan");
                                g_object_set (tag, "foreground", data->window->settings.color_cyan, NULL); break;
    case SETTINGS_COLOR_YELLOW: t = gtk_text_buffer_get_tag_table (value->out_buffer);
                                tag = gtk_text_tag_table_lookup (t, "yellow");
                                g_object_set (tag, "foreground", data->window->settings.color_yellow, NULL); break;
  }
}

void url_tag_nv_color_cb (GtkTextTag* t, gchar* value)
{
  if (DEVCHAT_IS_URL_TAG (t))
  {
    DevchatURLTag* tag = DEVCHAT_URL_TAG (t);
    if (!tag->visited)
      g_object_set (tag, "foreground", value, NULL);
  }
}

void url_tag_v_color_cb (GtkTextTag* t, gchar* value)
{
  if (DEVCHAT_IS_URL_TAG (t))
  {
    DevchatURLTag* tag = DEVCHAT_URL_TAG (t);
    if (tag->visited)
      g_object_set (tag, "foreground", value, NULL);
  }
}

static void devchat_window_get_property (GObject* object, guint id, GValue* value, GParamSpec* pspec)
{

}

void destroy (GtkObject* widget, DevchatCBData* data)
{
  save_settings (data->window);

  gtk_status_icon_set_visible (GTK_STATUS_ICON (data->window->trayicon), FALSE);

#ifdef NOTIFY
  notify_uninit ();
#endif

  if (!(data->window->firstrun))
  {
    SoupMessage* msg = soup_message_new ("GET", "http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?cmd=logout_silent");
    soup_session_queue_message (data->window->session, msg, SOUP_SESSION_CALLBACK (quit_cb), data);
  #ifdef INGAME
    ingame_update_status (data, INGAME_STATUS_DISCONNECTED);
  #endif
    g_timeout_add_seconds (2, (GSourceFunc) quit_timeout_cb, data);
  }
  else
    gtk_main_quit ();
}

void quit_cb (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  soup_session_abort (s);
  gtk_main_quit ();
}

gboolean quit_timeout_cb (DevchatCBData* data)
{
  soup_session_abort (data->window->session);
  gtk_main_quit ();
  return FALSE;
}


void save_settings (DevchatWindow* w)
{
  if (no_config)
    return;

  gchar* settingsfile = g_build_filename(g_get_user_config_dir(),"devchat", NULL);

  if (debug && !g_file_test (settingsfile, G_FILE_TEST_EXISTS))
  {
    gchar* dbg_msg = g_strdup_printf ("Settings file not found, search path was %s. Creating.\n", settingsfile);
    dbg (dbg_msg);
    g_free (dbg_msg);
  }

  GSList* tmp_kw = w->settings.keywords;
  gchar* keywords_string = g_strdup("KEYWORDS=");

  if (tmp_kw)
  {
    keywords_string = g_strconcat (keywords_string, (gchar*) tmp_kw->data, NULL);

    tmp_kw = tmp_kw->next;

    while (tmp_kw)
    {
      keywords_string = g_strconcat (keywords_string, "|", (gchar*) tmp_kw->data, NULL);
      tmp_kw = tmp_kw->next;
    }
  }

  gchar* presets_string = g_strdup("BOILERPLATES=");
  gint i;
  for (i=0;i < 10 && g_strcmp0 ("",w->settings.presets[i]) != 0;i++)
  {
    presets_string = g_strconcat (presets_string, w->settings.presets[i], "|", NULL);
  }

  gchar* bools_string = g_strdup_printf ("SHOWID=%s\nSTEALTHJOIN=%s\nAUTOJOIN=%s\nSHOWHIDDEN=%s\nCOLORUSER=%s\nSTORE_PASS=%s\nSHOW_TRAY=%s\nJUMP_TAB=%s\nMAXIMIZED=%s\n", w->settings.showid? "TRUE":"FALSE",
                                         w->settings.stealthjoin? "TRUE" : "FALSE",
                                         w->settings.store_pass? (w->settings.autojoin? "TRUE" : "FALSE") : "FALSE",
                                         w->settings.showhidden? "TRUE" : "FALSE",
                                         w->settings.coloruser? "TRUE" : "FALSE",
                                         w->settings.store_pass? "TRUE" : "FALSE",
                                         w->settings.showtray? "TRUE" : "FALSE",
                                         w->settings.jumptab? "TRUE" : "FALSE",
                                         w->settings.maximized? "TRUE":"FALSE");

  gchar* settings = g_strconcat (_("#Settings file for DevchatGUI. Please do not alter the key names.\n \
#Note: This behaviour is different from python version 0.x, in which the order of the values was the only thing important.\n \
#Keywords and boilerplates are separated by | (u007C, vertical line).\n \
#Truth Values (SHOWID,SHOWHIDDEN,STEALTHJOIN,AUTOJOIN) must be written as \"true\" for true, everything else (1/yes/...) will be regarded as false.\n"), "[Devchat]\n",
                                 "BROWSER=",w->settings.browser, "\n",
                                 "COLOR_FONT=",w->settings.color_font, "\n",
                                 "COLOR_TIME=",w->settings.color_time, "\n",
                                 "COLOR_L1=",w->settings.color_l1, "\n",
                                 "COLOR_L3=",w->settings.color_l3, "\n",
                                 "COLOR_L5=",w->settings.color_l5, "\n",
                                 "COLOR_L6=",w->settings.color_l6, "\n",
                                 "COLOR_GOLDIES=",w->settings.color_goldies, "\n",
                                 "COLOR_GREENS=",w->settings.color_greens, "\n",
                                 "COLOR_BLUES=",w->settings.color_blues, "\n",
                                 "COLOR_URL=",w->settings.color_url, "\n",
                                 "COLOR_URL_VISITED=",w->settings.color_url_visited, "\n",
                                 "COLOR_URL_HOVER=",w->settings.color_url_hover, "\n",
                                 "COLOR_HIGHLIGHT=",w->settings.color_highlight, "\n",
                                 "COLOR_GREEN=",w->settings.color_green, "\n",
                                 "COLOR_BLUE=",w->settings.color_blue, "\n",
                                 "COLOR_RED=",w->settings.color_red, "\n",
                                 "COLOR_MAGENTA=",w->settings.color_magenta, "\n",
                                 "COLOR_CYAN=",w->settings.color_cyan, "\n",
                                 "COLOR_YELLOW=",w->settings.color_yellow, "\n",
                                 "USER=",w->settings.user, "\n",
                                 "PASS=",w->settings.store_pass? w->settings.pass : "<none>", "\n",
                                 "NOTIFY=",w->settings.notify, "\n",
                                 "VNOTIFY=",w->settings.vnotify, "\n",
                                 "SERVER_NAME=",w->settings.servername, "\n",
                                 "PROXY=",w->settings.proxy, "\n",
                               #ifdef INGAME
                                 "TC_FOLDER=",w->settings.TCFolder, "\n",
                               #endif
                                 g_strdup_printf("WIDTH=%i\n", w->settings.width),
                                 g_strdup_printf("HEIGHT=%i\n", w->settings.height),
                                 g_strdup_printf("X=%i\n", w->settings.x),
                                 g_strdup_printf("Y=%i\n", w->settings.y),
                                 g_strdup_printf("UPDATE_TIME=%i\n", w->settings.update_time),
                                 g_strdup_printf("AVATARSIZE=%i\n", w->settings.avatar_size),
                                 g_strdup_printf("HANDLE_WIDTH=%i\n", w->settings.handle_width),
                                 keywords_string, "\n",
                                 presets_string, "\n",
                                 bools_string,
                                 NULL);
  if (!g_file_set_contents (settingsfile, settings, -1, NULL) && debug)
    dbg ("Error writing settings file.");

  g_free (settings);
  g_free (bools_string);
  g_free (keywords_string);
  g_free (presets_string);
}

void login (GtkWidget* widget, DevchatCBData* data)
{
  gtk_widget_set_sensitive(data->window->btn_connect,FALSE);
  gtk_widget_set_sensitive(data->window->user_entry,FALSE);
  gtk_widget_set_sensitive(data->window->pass_entry,FALSE);
  if (debug)
    dbg ("Logging in...");

  gtk_label_set_text (GTK_LABEL (data->window->statuslabel), _("Logging in..."));
  data->window->settings.user = g_strdup (gtk_entry_get_text(GTK_ENTRY(data->window->user_entry)));
  data->window->settings.pass = g_strdup (gtk_entry_get_text(GTK_ENTRY(data->window->pass_entry)));
  SoupMessage* loginparams = soup_form_request_new("POST", "http://forum.egosoft.com/login.php","username",data->window->settings.user,"password",data->window->settings.pass,"autologin","on","redirect","","webroot","0","login","Log in",NULL);
  soup_session_queue_message (data->window->session, loginparams, SOUP_SESSION_CALLBACK (login_cb), data);
}

void login_cb (SoupSession* session, SoupMessage* msg, DevchatCBData* data)
{
  if (debug)
    dbg ("Got login response from server.");

  if (g_strrstr(msg->response_body->data,"invalid password"))
  {
    err (_("Error L: Login failed."));
    gtk_label_set_text (GTK_LABEL (data->window->statuslabel), _("Login failed."));
    gtk_widget_hide_all (data->window->inputbar);
    gtk_widget_show_all (data->window->loginbar);
    gtk_widget_set_sensitive(data->window->btn_connect,TRUE);
    gtk_widget_set_sensitive(data->window->user_entry,TRUE);
    gtk_widget_set_sensitive(data->window->pass_entry,TRUE);
  }
  else if (g_strrstr(msg->response_body->data,"Visual Confirmation"))
  {
    err (_("Error C: Login failed, limit reached."));
    gtk_widget_hide_all (data->window->inputbar);
    gtk_widget_hide_all (data->window->loginbar);
    gtk_label_set_text (GTK_LABEL (data->window->statuslabel), _("Login failed, account locked. Please visit the forum and re-activate manually."));
    gtk_widget_set_sensitive(data->window->btn_connect,TRUE);
    gtk_widget_set_sensitive(data->window->user_entry,TRUE);
    gtk_widget_set_sensitive(data->window->pass_entry,TRUE);
  }
  else
  {
    if (debug)
      dbg ("Login successful.");

    gtk_label_set_text (GTK_LABEL (data->window->statuslabel), _("Login successful! Determining user level..."));
    SoupMessage* step2 = soup_message_new("GET","http://www.egosoft.com");

    if (debug)
      dbg ("Trying to determine userlevel...");

    soup_session_queue_message (data->window->session, step2, SOUP_SESSION_CALLBACK(remote_level), data);
  }
}

void remote_level (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{

  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->level_box), 3);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->level_box), 2);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->level_box), 1);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->level_box), 0);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->filter_ul), 3);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->filter_ul), 2);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->filter_ul), 1);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->filter_ul), 0);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->filter_ml), 3);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->filter_ml), 2);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->filter_ml), 1);
  gtk_combo_box_remove_text (GTK_COMBO_BOX(data->window->filter_ml), 0);

  if (g_strrstr (m->response_body->data,"User-Level: 5"))
  {
    data->window->userlevel = 5;
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->level_box), _("Level 1"));
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->level_box), _("Level 3"));
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->level_box), _("Level 5"));
    gtk_combo_box_set_active (GTK_COMBO_BOX(data->window->level_box), 0);

    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ul), _("Filter Users"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ul), _("Userlevel <3"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ul), _("Userlevel <5"));
    gtk_combo_box_set_active (GTK_COMBO_BOX(data->window->filter_ul), 0);

    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ml), _("Filter Messages"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ml), _("Messagelevel <3"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ml), _("Messagelevel <5"));
    gtk_combo_box_set_active (GTK_COMBO_BOX(data->window->filter_ml), 0);

    gtk_widget_show (data->window->filter_ml);
    gtk_widget_show (data->window->filter_ul);

    gtk_widget_show (data->window->level_box);
    gtk_widget_show (data->window->item_l3);
    gtk_widget_show (data->window->item_l5);
  }
  else if (g_strrstr (m->response_body->data,"User-Level: 3"))
  {
    data->window->userlevel = 3;
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->level_box), _("Level 1"));
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->level_box), _("Level 3"));
    gtk_combo_box_set_active (GTK_COMBO_BOX(data->window->level_box), 0);

    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ul), _("Filter Users"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ul), _("Userlevel <3"));
    gtk_combo_box_set_active (GTK_COMBO_BOX(data->window->filter_ul), 0);

    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ml), _("Filter Messages"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ml), _("Messagelevel <3"));
    gtk_combo_box_set_active (GTK_COMBO_BOX(data->window->filter_ml), 0);

    gtk_widget_show (data->window->filter_ml);
    gtk_widget_show (data->window->filter_ul);


    gtk_widget_show (data->window->level_box);
    gtk_widget_show (data->window->item_l3);
  }
  else if (g_strrstr (m->response_body->data,"User-Level: "))
  {
    data->window->userlevel = 6;
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->level_box), _("Level 1"));
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->level_box), _("Level 3"));
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->level_box), _("Level 5"));
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->level_box), _("Level 6"));
    gtk_combo_box_set_active (GTK_COMBO_BOX(data->window->level_box), 0);

    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ul), _("Filter Users"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ul), _("Userlevel <3"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ul), _("Userlevel <5"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ul), _("Userlevel <6"));
    gtk_combo_box_set_active (GTK_COMBO_BOX(data->window->filter_ul), 0);

    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ml), _("Filter Messages"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ml), _("Messagelevel <3"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ml), _("Messagelevel <5"));
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->filter_ml), _("Messagelevel <6"));
    gtk_combo_box_set_active (GTK_COMBO_BOX(data->window->filter_ml), 0);

    gtk_widget_show (data->window->filter_ml);
    gtk_widget_show (data->window->filter_ul);

    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->color_box), "chatname_green");
    gtk_combo_box_append_text (GTK_COMBO_BOX(data->window->color_box), "chatname_blue");

    gtk_widget_show (data->window->level_box);
    gtk_widget_show (data->window->chk_raw);
    gtk_widget_show (data->window->item_l3);
    gtk_widget_show (data->window->item_l5);
  }
  else
  {
    data->window->userlevel = 1;
  }
  if (debug) {
    dbg_msg = g_strdup_printf("Determined userlevel to be %i.", data->window->userlevel);
    dbg (dbg_msg);
    g_free (dbg_msg);
  }

  if (!data->window->settings.stealthjoin)
  {
    gchar* joinmsg;
    if (data->window->userlevel < 6)
      joinmsg = g_strdup_printf ("[cyan](%s):[/cyan] /me has joined.", data->window->settings.servername);
    else
      joinmsg = g_strdup_printf ("<span class=\"chatname_green\">(%s):</span> /me has joined.", data->window->settings.servername);
    devchat_window_text_send (data, g_strdup (joinmsg), NULL, "1", TRUE);
    g_free (joinmsg);
    gtk_notebook_set_current_page (GTK_NOTEBOOK (data->window->notebook), 0);
  }

#ifdef INGAME
  g_timeout_add_seconds (2, (GSourceFunc) get_ingame_messages, data);
  ingame_update_status (data, INGAME_STATUS_ONLINE);
#endif

  g_signal_connect(data->window->window, "key-press-event", G_CALLBACK (hotkey_cb), data);
  gtk_widget_grab_focus(data->window->inputwidget);
  gtk_widget_hide_all (data->window->loginbar);
  gtk_widget_show_all (data->window->inputbar);
  gtk_widget_hide (data->window->item_connect);
  gtk_widget_show (data->window->item_reconnect);
  gtk_widget_set_no_show_all (data->window->item_status, FALSE);
  gtk_widget_show_all (data->window->item_status);
  if (debug)
    dbg ("Starting requests...");

  gtk_label_set_text (GTK_LABEL (data->window->statuslabel), _("Waiting for messages..."));
  data->window->msg_list_parsed = TRUE;
  data->window->usr_list_parsed = TRUE;
  data->window->usr_list_getter = g_timeout_add ((data->window->settings.update_time * 2), (GSourceFunc) user_list_poll, data);
  data->window->msg_list_getter = g_timeout_add (data->window->settings.update_time, (GSourceFunc) message_list_poll, data);
}

gboolean
user_list_poll (DevchatCBData* data)
{
  if (data->window->usr_list_parsed)
  {
   if (debug)
    dbg ("Starting user list poll...");


    GSList* tmp = data->window->users_online;

    gchar* uri = g_strdup ("http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?users=");

    while (tmp)
    {
      uri = g_strconcat (uri, (gchar*) tmp->data, ";", NULL);
      tmp = tmp->next;
    }

    data->window->user_message = soup_message_new ("GET", uri);
    soup_session_queue_message (data->window->session, data->window->user_message, SOUP_SESSION_CALLBACK (user_list_get), data);
    data->window->user_timeout_id = g_timeout_add_seconds (data->window->settings.update_time * 20, (GSourceFunc) user_list_timeout, data);
    data->window->usr_list_parsed = FALSE;
  }
  return TRUE;
}

gboolean
message_list_poll (DevchatCBData* data)
{
  if (data->window->msg_list_parsed)
  {
    if (debug)
      dbg ("Starting message list poll...");

    data->window->message_message = soup_message_new ("GET", g_strdup_printf("http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?lid=%s",data->window->lastid));
    g_signal_connect (data->window->message_message, "got-chunk", G_CALLBACK (message_list_chunk), data);
    soup_session_queue_message (data->window->session, data->window->message_message, SOUP_SESSION_CALLBACK (message_list_get), data);
    data->window->message_timeout_id = g_timeout_add_seconds (data->window->settings.update_time * 10, (GSourceFunc) message_list_timeout, data);
    data->window->msg_list_parsed = FALSE;
  }
  return TRUE;
}

gboolean user_list_timeout (DevchatCBData* data)
{
  g_slist_free (data->window->users_online);
  data->window->users_online = NULL;
  data->window->usr_list_parsed = TRUE;
  if (data->window->user_message)
  {
    soup_session_cancel_message (data->window->session, data->window->user_message, SOUP_STATUS_CANCELLED);
    data->window->user_message = NULL;
  }

  return FALSE;
}

gboolean message_list_timeout (DevchatCBData* data)
{
  unsigned long last_ce_end;
  gchar* useable_message_chunk = NULL;

  if (!g_str_has_suffix (data->window->message_buffer, "/>") && !g_str_has_suffix (data->window->message_buffer, "</devchat>"))
  {
    gchar* endpointer = g_strrstr (data->window->message_buffer, "<ce");
    if (endpointer)
    {
      last_ce_end = endpointer - data->window->message_buffer;

      useable_message_chunk = g_strndup (data->window->message_buffer, last_ce_end);
      useable_message_chunk = g_strconcat (useable_message_chunk, "</ces>\n</devchat>", NULL);
      ce_parse (useable_message_chunk, data, "");
    }
  }
  else
  {
    useable_message_chunk = g_strdup (data->window->message_buffer);
    ce_parse (useable_message_chunk, data, "");
  }

  soup_session_cancel_message (data->window->session, data->window->message_message, SOUP_STATUS_CANCELLED);
  if (g_strcmp0 (data->window->message_buffer, "") != 0)
    g_free (data->window->message_buffer);
  data->window->message_buffer = "";

  data->window->msg_list_parsed = TRUE;

  return FALSE;
}

void message_list_chunk (SoupMessage* m, SoupBuffer* chunk, DevchatCBData* data)
{
  gchar* real_chunk = g_strndup (chunk->data, chunk->length);
  data->window->message_buffer = g_strconcat (data->window->message_buffer, real_chunk, NULL);
  g_free (real_chunk);
}

void user_list_clear_cb (GtkWidget* child, DevchatCBData* data)
{
  gtk_container_remove (GTK_CONTAINER (data->window->userlist), child);
}

void user_list_get (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  if (debug) {
    dbg_msg = g_strdup_printf ("(XX) User list response:\n\nStatus code: %i -> Status Message: %s.\n\nResponse Body: %s.\n\n\n", m->status_code, m->reason_phrase, m->response_body->data);
    dbg (dbg_msg);
    g_free (dbg_msg);
  }

  if (m->status_code == 200)
  {
    data->window->errorcount = MAX (0, data->window->errorcount-1);
    if (data->window->users_online)
    {
      g_slist_free (data->window->users_online);
      data->window->users_online = NULL;
    }

    data->window->usr_list_parsed = TRUE;
    data->window->user_message = NULL;
    g_source_remove (data->window->user_timeout_id);

    gchar* userlist = g_strdup (m->response_body->data);
    if (userlist)
    {
      if (debug)
        dbg ("Got non-empty userlist.");

    #ifdef INGAME
      ingame_clear_user_list (data);
    #endif


      xmlTextReaderPtr userparser = xmlReaderForMemory (userlist,strlen(userlist),"",NULL,(XML_PARSE_RECOVER|XML_PARSE_NOENT));
      xmlParserCtxtPtr ctxt = xmlCreateDocParserCtxt ((xmlChar*) userlist);

      GdkColor l1;
      gdk_color_parse (data->window->settings.color_l1, &l1);

      gtk_container_foreach (GTK_CONTAINER (data->window->userlist), (GtkCallback) user_list_clear_cb, data);

      GtkSizeGroup* sg = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

      guint usercount = 0;
      while (xmlTextReaderRead (userparser) > 0)
      {
        gchar* node = (gchar*) xmlTextReaderLocalName(userparser);

        if (node && (g_strcmp0 (node,"cu") == 0))
        {
          usercount++;
          gchar* name = (gchar*) xmlTextReaderGetAttribute(userparser, (xmlChar*) "n");
          gchar* uid = (gchar*) xmlTextReaderGetAttribute(userparser, (xmlChar*) "uid");
          gchar* level = (gchar*) xmlTextReaderGetAttribute(userparser, (xmlChar*) "l");
          gchar* status = (gchar*) xmlTextReaderGetAttribute(userparser, (xmlChar*) "s");

        #ifdef INGAME
          gchar* name_ingame;
        #endif

          g_hash_table_insert (data->window->users, g_strdup (name), g_strdup (uid));
          data->window->users_online = g_slist_prepend (data->window->users_online, g_strdup (uid));

          if (data->window->settings.showhidden == TRUE || ( (g_strcmp0("STEALTH",status) != 0) && (g_strcmp0("Away: STEALTH",status) != 0)))
          {

            if (!g_slist_find_custom (data->window->users_without_avatar,uid, (GCompareFunc) user_lookup) && !g_hash_table_lookup (data->window->avatars, uid))
            {
              gchar* ava_filename = g_build_filename (data->window->avadir,uid,NULL);

              if (debug) {
                dbg_msg = g_strdup_printf ("Searching for avatar %s...", ava_filename);
                dbg (dbg_msg);
                g_free (dbg_msg);
              }

              if (!g_file_test (ava_filename, G_FILE_TEST_EXISTS))
              {
                if (debug) {
                  dbg_msg = g_strdup_printf ("Avatar %s not found. Searching...",uid);
                  dbg (dbg_msg);
                  g_free (dbg_msg);
                }

                SoupMessage* ava_get = soup_message_new ("GET",g_strdup_printf("http://forum.egosoft.com/profile.php?mode=viewprofile&u=%s",uid));
                soup_session_queue_message (data->window->session, ava_get, SOUP_SESSION_CALLBACK (search_ava_cb), devchat_cb_data_new (data->window, g_strdup(uid)));
                if (debug)
                  dbg ("Search request queued, will be executed when idling.");

                data->window->users_without_avatar = g_slist_prepend (data->window->users_without_avatar,g_strdup(uid));
              }
              else
              {
                if (debug) {
                  dbg_msg = g_strdup_printf ("Found avatar for %s, checking whether avatar is too old...",name);
                  dbg (dbg_msg);
                  g_free (dbg_msg);
                }

                struct stat buf;
                if (g_stat (ava_filename, &buf) == 0)
                {
                  if ((time(NULL) - buf.st_mtime) > 86400)
                  {
                    /*Avatar was last modified more than one day ago, checking for new one.*/
                    SoupMessage* ava_get = soup_message_new ("GET",g_strdup_printf("http://forum.egosoft.com/profile.php?mode=viewprofile&u=%s",uid));
                    soup_session_queue_message (data->window->session, ava_get, SOUP_SESSION_CALLBACK (search_ava_cb), devchat_cb_data_new (data->window, g_strdup(uid)));
                    data->window->users_without_avatar = g_slist_prepend (data->window->users_without_avatar,g_strdup(uid));
                  }
                  else
                  {
                    g_hash_table_insert (data->window->avatars, g_strdup(uid), gdk_pixbuf_new_from_file_at_size (ava_filename,data->window->settings.avatar_size,data->window->settings.avatar_size,NULL));
                  }
                }
                else
                {
                  err (_("Error D: Error stat()ing avatar file! Trying to re-download it. If problem persists, check permissions for avatar directory."));

                  SoupMessage* ava_get = soup_message_new ("GET",g_strdup_printf("http://forum.egosoft.com/profile.php?mode=viewprofile&u=%s",uid));
                  soup_session_queue_message (data->window->session, ava_get, SOUP_SESSION_CALLBACK (search_ava_cb), devchat_cb_data_new (data->window, g_strdup(uid)));
                  data->window->users_without_avatar = g_slist_prepend (data->window->users_without_avatar,g_strdup(uid));
                }

              }
              g_free (ava_filename);
            }
            if (debug)
            {
              dbg_msg = g_strdup_printf("Adding user %s.",name);
              dbg (dbg_msg);
              g_free (dbg_msg);
            }

            GtkWidget* label = gtk_label_new(NULL);
            GtkWidget* container = gtk_hbox_new(FALSE,0);
            GtkWidget* at_btn = gtk_button_new();
            GtkWidget* profile_btn = gtk_button_new();
            GtkWidget* pm_btn = gtk_button_new ();
            GtkWidget* pm_label = gtk_label_new (NULL);

            gtk_label_set_markup (GTK_LABEL (pm_label), "<span font_size='x-small'>PM</span>");
            gtk_container_add (GTK_CONTAINER (pm_btn), pm_label);

            gulong real_level = strtoll(g_strndup(level,1),NULL,10);
            gchar* color;
            gchar* style;
            gchar* strike;


            gtk_button_set_relief (GTK_BUTTON (profile_btn), GTK_RELIEF_NONE);

            gchar* at_text = g_strdup_printf (_("View the forum profile of %s."),name);
            gtk_widget_set_tooltip_text (profile_btn, at_text);
            g_free (at_text);

            GdkPixbuf* ava = (GdkPixbuf*) g_hash_table_lookup (data->window->avatars, uid);

            if (!(ava))
              ava = (GdkPixbuf*) g_hash_table_lookup (data->window->avatars, "default");

            gtk_button_set_image (GTK_BUTTON (profile_btn), gtk_image_new_from_pixbuf (ava));
            gtk_size_group_add_widget (sg, profile_btn);
            gint uid_i = (int) strtoll (uid, NULL, 10);
            g_signal_connect (profile_btn, "clicked", G_CALLBACK (go_forum), devchat_cb_data_new (data->window, GINT_TO_POINTER (uid_i)));

            if (real_level > 6)
              color = data->window->settings.color_goldies;
            else if (real_level == 6)
              color = data->window->settings.color_greens;
            else
              color = data->window->settings.color_blues;

            if (g_strcmp0("",status) != 0)
            {
              style = "italic";
              gtk_widget_set_has_tooltip(label, TRUE);
              gchar* status_d;
              if (g_strcmp0 ("DND",status) == 0)
              {
                strike = "true";
                status_d = g_strdup (_("Do NOT disturb."));
              #ifdef INGAME
                name_ingame = "[d]";
              #endif
              }
              else
              {
                status_d = (gchar*) xmlStringDecodeEntities (ctxt, (xmlChar*) status, XML_SUBSTITUTE_BOTH, 0,0,0);
                strike = "false";
              #ifdef INGAME
                name_ingame = "[a]";
              #endif
              }
              gtk_widget_set_tooltip_text(at_btn, status_d);
              g_free (status_d);

              color = data->window->settings.color_time;
            }
            else
            {
              style = "normal";
              strike = "false";
              gchar* at_text = g_strdup_printf (_("Poke %s"),name);
              gtk_widget_set_tooltip_text(at_btn, at_text);
              g_free (at_text);
            #ifdef INGAME
              name_ingame = "[o]";
            #endif
            }

          #ifdef INGAME
            name_ingame = g_strconcat (name_ingame, name, "[", level, "]", NULL);
          #endif

            gchar* markup = g_markup_printf_escaped ("<span foreground='%s' style='%s' strikethrough='%s'>%s</span> <span foreground='%s'>(%s)</span>",color,style,strike,name,data->window->settings.color_font,level);
            gtk_label_set_markup (GTK_LABEL (label),markup);
            g_free (markup);

            g_signal_connect (at_btn, "clicked", G_CALLBACK (at_cb), devchat_cb_data_new (data->window,g_strdup(name)));
            gtk_container_add (GTK_CONTAINER (at_btn), label);
            gtk_button_set_relief (GTK_BUTTON(at_btn), GTK_RELIEF_NONE);

            g_signal_connect (pm_btn, "clicked", G_CALLBACK (pm_cb), devchat_cb_data_new (data->window,g_strdup(name)));

            gchar* pm_text = g_strdup_printf (_("Open a conversation with %s."),name);
            gtk_widget_set_tooltip_text (pm_btn, pm_text);
            g_free (pm_text);

            if (data->window->settings.coloruser)
            {
              gtk_widget_modify_bg (at_btn, GTK_STATE_PRELIGHT, &l1);
              gtk_widget_modify_bg (profile_btn, GTK_STATE_PRELIGHT, &l1);
            }

            gtk_box_pack_start (GTK_BOX (container),profile_btn,FALSE,FALSE,0);
            gtk_box_pack_start (GTK_BOX (container),at_btn,TRUE,TRUE,0);
            gtk_box_pack_start (GTK_BOX (container),pm_btn,FALSE,FALSE,0);

            gtk_box_pack_start (GTK_BOX (data->window->userlist),container,FALSE,FALSE,0);

          #ifdef INGAME
            ingame_append_user (data, name_ingame);
          #endif
          }
          g_free (uid);
          g_free (name);
          g_free (level);
          g_free (status);
          g_free (node);
        }
      }

      gchar* ul_text = g_strdup_printf (usercount==1? _("%i user online"):_("%i users online"), usercount);
      gtk_label_set_text (GTK_LABEL (data->window->userlabel), ul_text);
      g_free (ul_text);

      /*If the server failed to submit an incremental update, request complete list.*/
      if (usercount == 0)
      {
        if (data->window->users_online)
          g_slist_free (data->window->users_online);
        data->window->users_online = NULL;
      }
      else
      {
      #ifdef INGAME
        ingame_flush_data (data);
      #endif

        dbg_msg = g_strdup_printf(_("Last Update: %s"), current_time ());
        gtk_label_set_text (GTK_LABEL (data->window->statuslabel), dbg_msg);
        g_free (dbg_msg);
      }

      gtk_widget_show_all (data->window->userlist);
      xmlFreeTextReader (userparser);
      xmlFreeParserCtxt (ctxt);
      g_free (userlist);
    }
    else
    {
      /*If the server failed to submit an incremental update, request complete list.*/
      if (data->window->users_online)
        g_slist_free (data->window->users_online);
      data->window->users_online = NULL;
    }
  }
  else
  {
    data->window->errorcount++;
    if (data->window->errorcount > (TIMEOUT/data->window->settings.update_time))
    {
      gtk_label_set_text (GTK_LABEL (data->window->statuslabel), _("Connection Lost!"));
      reconnect (NULL, data);
    }
    else
    {
      if (data->window->users_online)
      {
        g_slist_free (data->window->users_online);
        data->window->users_online = NULL;
      }
      data->window->usr_list_parsed = TRUE;
    }
  }
}

gint user_lookup (gchar* a, gchar* b)
{
  return g_strcmp0 (a,b);
}


void search_ava_cb (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  gchar* profile = g_strdup (m->response_body->data);
  gboolean found = FALSE;

  if (profile)
  {
    if (debug) {
      dbg_msg = g_strdup_printf ("Got the forum profile of %s. Now searching for the avatar...", (gchar*) data->data);
      dbg (dbg_msg);
      g_free (dbg_msg);
    }

    if (g_regex_match_simple ("<span class=\"postdetails\">(Moderator|Site Admin|EGOSOFT)</span>", profile, G_REGEX_CASELESS, 0))
    {
      g_hash_table_insert (data->window->moderators, g_strdup ((gchar*) data->data), g_strdup ("y"));
    }
    else
      g_hash_table_insert (data->window->moderators, g_strdup ((gchar*) data->data), g_strdup ("n"));

    GRegex* regex = g_regex_new ("<img src=\"http:\\/\\/.*\\.(jpg|png|gif)", G_REGEX_UNGREEDY, 0, NULL);
    gchar** profile_lines = g_strsplit (profile, "\n",-1);

    gchar* filename = g_build_filename (data->window->avadir, data->data, NULL);

    gchar* line;
    guint i;
    gchar* ava_url = NULL;

    for (i = 0; profile_lines[i] != NULL && !found; i++)
    {
      line = profile_lines[i];
      GMatchInfo* result;
      if (g_regex_match (regex, line, 0, &result))
      {
        gchar* match = g_match_info_fetch(result,0);

        if (!g_str_has_prefix(match,"<img src=\"http://www.egosoft.com/") && !g_str_has_prefix(match,"<img src=\"http://stats.big-boards.com/"))
        {
          if (debug) {
            dbg_msg = g_strdup_printf ("Found something remotely resembling an avatar: %s", match+10);
            dbg (dbg_msg);
            g_free (dbg_msg);
          }
          found = TRUE;
          ava_url = g_strdup (match+10);
        }
        g_free (match);
      }
      g_match_info_free (result);
    }

    if(found)
    {
      if (debug) {
        dbg_msg = g_strdup_printf ("Now commencing avatar write: %s", ava_url);
        dbg (dbg_msg);
        g_free (dbg_msg);
      }
      data->window->users_without_avatar = g_slist_delete_link (data->window->users_without_avatar,
        g_slist_find_custom (data->window->users_without_avatar,data->data, (GCompareFunc) user_lookup));
      SoupMessage* a_m = soup_message_new ("GET", ava_url);
      if (soup_session_send_message (s, a_m) == 200)
      {
        /*Workaround for imageshack being TOO MOTHERFUCKING RETARDED to return a 404.*/
        if (g_strcmp0 ("404", a_m->response_body->data) != 0)
        {
          GError* error = NULL;
          if (!g_file_set_contents (filename, a_m->response_body->data, a_m->response_body->length, &error))
          {
            err (g_strdup_printf (_("Error D: Error while saving avatar: %s."), error->message));
            g_error_free (error);
            found = FALSE;
          }
          g_free (filename);
        }
        else
        {
          found = FALSE;
        }
      }
      else
      {
        found = FALSE;
        if (debug)
          dbg ("Error downloading avatar!");
      }
    }

    g_regex_unref (regex);
    g_strfreev (profile_lines);

    g_free (profile);

    if (debug)
      dbg ("Avatar search done.");

    if (!found)
    {
      data->window->users_without_avatar = g_slist_prepend (data->window->users_without_avatar, g_strdup ((gchar*) data->data));
      if (g_file_test (filename, G_FILE_TEST_EXISTS))
        g_remove (filename);
    }
    else
      user_list_timeout (data);
  }
}

void message_list_get (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  if (debug) {
    dbg_msg = g_strdup_printf ("(XX) Message list response:\n\nStatus code: %i -> Status Message: %s.\n", m->status_code, m->reason_phrase);
    dbg (dbg_msg);
    g_free (dbg_msg);
  }

  if (m->status_code == 200)
  {
    data->window->errorcount = MAX (0, data->window->errorcount-1);
    gchar* msglist = g_strdup (m->response_body->data);
    if (msglist)
    {
      ce_parse (msglist, data, "");
    }
    data->window->msg_list_parsed = TRUE;
    if (data->window->message_timeout_id)
      g_source_remove (data->window->message_timeout_id);
    data->window->message_timeout_id = 0;
    g_free (data->window->message_buffer);
    data->window->message_buffer = "";
    data->window->message_message = NULL;
  }
  else
  {
    data->window->errorcount++;

    if (data->window->errorcount > (TIMEOUT/data->window->settings.update_time))
    {
      gtk_label_set_text (GTK_LABEL (data->window->statuslabel), _("Connection Lost!"));
      reconnect (NULL, data);
    }
    else
      data->window->msg_list_parsed = TRUE;
  }
}

void ce_parse (gchar* msglist, DevchatCBData* self, gchar* date)
{
  gboolean message_found = FALSE;

  GSList* scroll_tos = NULL;

  GtkTextIter old_end;
  gtk_text_buffer_get_end_iter (self->window->output, &old_end);

  GtkTextMark* scroll_to = gtk_text_mark_new ("scrollTo", FALSE);
  gtk_text_buffer_add_mark (self->window->output, scroll_to, &old_end);

  xmlTextReaderPtr msgparser = xmlReaderForMemory (msglist, strlen (msglist), "", NULL, 0);
  gchar* currenttime = current_time ();

  while (xmlTextReaderRead (msgparser) == 1)
  {
    gchar* node = (gchar*) xmlTextReaderLocalName(msgparser);

    if (node && (g_strcmp0 (node,"lastconn") == 0))
    {
      gchar** time_cmps = g_strsplit ((gchar*) xmlTextReaderGetAttribute (msgparser, (xmlChar*) "value"), " ", 2);
      currenttime = g_strdup (time_cmps[1]);
      g_strfreev (time_cmps);

      if (g_strcmp0 (date, "") == 0)
      {
        gchar* labeltext = g_strdup_printf(_("Last Update: %s"), current_time());
        gtk_label_set_text (GTK_LABEL (self->window->statuslabel), labeltext);
        g_free (labeltext);
      }
    }
    else if (node && (g_strcmp0 (node,"ce") == 0))
    {
      message_found = TRUE;
      gchar* name = (gchar*) xmlTextReaderGetAttribute (msgparser, (xmlChar*) "a");
      gchar* mode = (gchar*) xmlTextReaderGetAttribute (msgparser, (xmlChar*) "u");
      gchar* time_attr = (gchar*) xmlTextReaderGetAttribute (msgparser, (xmlChar*) "t");
      gchar* lid = (gchar*) xmlTextReaderGetAttribute (msgparser, (xmlChar*) "i");
      gchar* message = (gchar*) xmlTextReaderGetAttribute (msgparser, (xmlChar*) "m");

      if (debug) {
        dbg_msg = g_strdup_printf ("Message parameters: username %s, mode %s, time %s, lid %s, message %s.", name, mode, time_attr, lid, message);
        dbg (dbg_msg);
        g_free (dbg_msg);
      }

      gint show_name = g_strcmp0 ("0", g_strndup(mode,1));
      gulong user_level = strtoll (g_strndup(mode+1,1),NULL,10);
      gulong msg_level = strtoll (mode+2,NULL,10);

      GtkTextBuffer* buf;
      GtkWidget* view;

      if (g_strcmp0 (date, "") == 0)
      {
        g_free (self->window->lastid);
        self->window->lastid = g_strdup (lid);

        if (msg_level == 0)
        {
          DevchatConversation* conv;
          if (g_strcmp0 (name, "(ChatServer)") == 0 && g_strstr_len (message, 80, "You sent a private message successfully to"))
          {
            gchar** msg_cmps = g_strsplit (message+66, "</font>", 2);
            gchar* target_name = g_strdup (msg_cmps[0]);
            g_strfreev (msg_cmps);
            name = g_strdup (self->window->settings.user);
            user_level = self->window->userlevel;
            if (g_strcmp0 (name, target_name) == 0) /*Make self-PMs only appear once and cancel processing in that case*/
              continue;
            conv = pm_cb (NULL, devchat_cb_data_new (self->window, target_name));
            gchar* tmp = message;
            message = g_strdup (message+77+strlen(target_name));
            g_free (tmp);
            buf = conv->out_buffer;
            view = conv->out_widget;

            if (!conv->scroll_to)
            {
              GtkTextIter pm_end;
              gtk_text_buffer_get_end_iter (buf, &pm_end);
              conv->scroll_to = gtk_text_mark_new ("scrollTo", FALSE);
              gtk_text_buffer_add_mark (buf, conv->scroll_to, &pm_end);

              scroll_tos = g_slist_prepend (scroll_tos, conv);
            }
          }
          else
          {
            if (g_strcmp0 (name, "(ChatServer)") != 0)
            {
              gchar* tmp = message;
              message = g_strdup (message+66);
              g_free (tmp);
              conv = pm_cb (NULL, devchat_cb_data_new (self->window, name));

              buf = conv->out_buffer;
              view = conv->out_widget;

              gchar* markup = g_markup_printf_escaped ("<span foreground=\"%s\" weight=\"bold\">%s</span>", self->window->settings.color_highlight, name);
              gtk_label_set_markup (GTK_LABEL (gtk_notebook_get_menu_label (GTK_NOTEBOOK (self->window->notebook), conv->child)), markup);
              g_free (markup);

              GdkPixbuf* icon = NULL;
              if (g_hash_table_lookup (self->window->users, name))
                icon = (GdkPixbuf*) g_hash_table_lookup (self->window->avatars, g_hash_table_lookup (self->window->users, name));
              if (!icon)
                icon = gtk_widget_render_icon (self->window->window, GTK_STOCK_INFO, GTK_ICON_SIZE_DIALOG, NULL);
              notify (name, "...sent you a private message", NULL, self);

              if (!conv->scroll_to)
              {
                GtkTextIter pm_end;
                gtk_text_buffer_get_end_iter (buf, &pm_end);
                conv->scroll_to = gtk_text_mark_new ("scrollTo", FALSE);
                gtk_text_buffer_add_mark (buf, conv->scroll_to, &pm_end);

                scroll_tos = g_slist_prepend (scroll_tos, conv);
              }
            }
            else
            {
              buf = self->window->output;
              view = self->window->outputwidget;
            }
          }
        }
        else
        {
          buf = self->window->output;
          view = self->window->outputwidget;
        }
      }
      else
      {
        DevchatConversation* conv;
        conv = pm_cb (NULL, devchat_cb_data_new (self->window, g_strconcat ("History for ", date, NULL)));
        buf = conv->out_buffer;
        view = conv->out_widget;
      }

      GtkTextIter end, e2;
      GtkTextTagTable* table = gtk_text_buffer_get_tag_table (buf);
      gtk_text_buffer_get_end_iter (buf, &end);

      e2 = end;
      gtk_text_iter_backward_char (&e2);

      gchar* last_char = gtk_text_buffer_get_text (buf, &end, &e2, TRUE);
      if (g_strcmp0 (last_char, "\xe2\x80\x8b") == 0) /*Last char mark, to prevent display corruption on filtering*/
      {
        gtk_text_buffer_delete (buf, &end, &e2);
        gtk_text_buffer_get_end_iter (buf, &end);
      }

      GtkTextMark* old_start = gtk_text_mark_new (NULL, TRUE);
      gtk_text_buffer_add_mark (buf, old_start, &end);

      gchar* tagname = g_strconcat ("user-", name, NULL);

      if (!gtk_text_tag_table_lookup (table, tagname))
        gtk_text_buffer_create_tag (buf, tagname, NULL);


      gtk_text_buffer_get_end_iter (buf, &end);
      gtk_text_buffer_insert (buf, &end, "\n", -1);
      gtk_text_buffer_get_end_iter (buf, &end);
      gchar* time_tag;
      if (self->window->settings.showid)
      {
        time_tag = g_strdup_printf ("%s %s", lid+(strlen(lid)-3), time_attr);
      }
      else
      {
        time_tag = g_strdup_printf ("%s", time_attr);
      }

      gchar* id_tag = g_strconcat ("lid::", lid, NULL);

      if (!gtk_text_tag_table_lookup (table, id_tag))
        gtk_text_buffer_create_tag (buf, id_tag, NULL);

      gchar* a_tag = "";
      if (!show_name)
      {
        a_tag = g_strconcat ("tma::", name, NULL);
        if (!gtk_text_tag_table_lookup (table, a_tag))
          gtk_text_buffer_create_tag (buf, a_tag, NULL);
      }

      gchar* tt_name;
      gtk_text_buffer_get_end_iter (buf, &end);

      if (g_strcmp0 ("(ChatServer)", name) == 0)
        tt_name = "greenie";
      else
        tt_name = "time";

      gtk_text_buffer_insert_with_tags (buf, &end, time_tag, -1, gtk_text_tag_table_lookup (table, tt_name), gtk_text_tag_table_lookup (table, id_tag), NULL);
      g_free (time_tag);
      g_free (id_tag);

      gchar* message_t = g_strdup_printf ("<p>%s</p>", message);

      gboolean kw_found = FALSE;
      if (g_strcmp0 (date, "") == 0)
      {
        if (g_strcmp0 (name, self->window->settings.user) != 0 && !self->window->firstrun)
        {
          GSList* tmp_kw = self->window->settings.keywords;

          while (tmp_kw && !kw_found)
          {
            if (g_strstr_len (g_utf8_strup(message_t,-1), -1, g_utf8_strup((gchar*) tmp_kw->data,-1)))
            {
              GdkPixbuf* icon = NULL;
              if (g_hash_table_lookup (self->window->users, name))
                icon = (GdkPixbuf*) g_hash_table_lookup (self->window->avatars, g_hash_table_lookup (self->window->users, name));
              if (!icon)
                icon = gtk_widget_render_icon (self->window->window, GTK_STOCK_INFO, GTK_ICON_SIZE_DIALOG, NULL);
              notify ((gchar*) tmp_kw->data, g_strdup_printf (_("...was mentioned by %s."), name), icon, self);
              kw_found = TRUE;
            }
            tmp_kw = tmp_kw->next;
          }
        }
      }

      gchar* name_color_tag = "peasant";
      if (user_level > 6)
        name_color_tag = "goldie";
      else if (user_level == 6)
        name_color_tag = "greenie";

      gtk_text_buffer_get_end_iter (buf, &end);
      gchar* name_t = (show_name != 0)? g_strdup_printf (" %s: ", name) : g_strdup (" ");
      gtk_text_buffer_insert_with_tags (buf, &end, name_t, -1, gtk_text_tag_table_lookup (table, name_color_tag), (kw_found)? gtk_text_tag_table_lookup (table, "bold") : NULL, NULL);
      g_free (name_t);

      if (g_strcmp0 (date, "") == 0)
      {
        gchar* kickmsg;
        if (msg_level != 0)
          kickmsg = g_strconcat ("!KICK ", g_ascii_strup (self->window->settings.user, -1), NULL);
        else
          kickmsg = g_strdup ("!KICK");
        gchar* message_up = g_ascii_strup (message, -1);

        if (g_strstr_len (message_up, -1, kickmsg) && badass (name, self) && !(self->window->firstrun))
        {
          gchar* kickmsg;
          if (g_strcmp0 (self->window->settings.servername, "SovietServer") == 0)
            kickmsg = g_strdup ("[red](SovietServer):[/red] In Soviet Russia, chat kicks /me …");
          else
            kickmsg = g_strdup_printf ("[red](%s):[/red] /me has been kicked.", self->window->settings.servername);
          devchat_window_text_send (self, g_strdup (kickmsg), NULL, "1", TRUE);
        #ifdef INGAME
          ingame_update_status (self, INGAME_STATUS_KICKED);
        #endif
          g_free (kickmsg);
          destroy (NULL, self);
        }

        gchar* silmsg;
        if (msg_level != 0)
          silmsg = g_strconcat ("!SILENCE ", g_ascii_strup (self->window->settings.user, -1), NULL);
        else
          silmsg = g_strdup ("!SILENCE");

        if (g_strstr_len (message_up, -1, silmsg) && badass (name, self))
        {
            gchar** current_time_cmps = g_strsplit (currenttime, ":", -1);
            gulong current_hour = g_ascii_strtoll (current_time_cmps[0],NULL,10);
            gulong current_minute = g_ascii_strtoll (current_time_cmps[1],NULL,10);
            gulong current_time_long = (current_hour * 60) + current_minute;
            g_strfreev (current_time_cmps);

            gchar** silent_time_cmps = g_strsplit (time_attr, ":", -1);
            gulong silent_hour = g_ascii_strtoll (silent_time_cmps[0],NULL,10);
            gulong silent_minute = g_ascii_strtoll (silent_time_cmps[1],NULL,10);
            gulong silent_time_long = (silent_hour * 60) + silent_minute;
            g_strfreev (silent_time_cmps);

            if (current_time_long < silent_time_long)
            {
              silent_time_long = silent_time_long - 1440;
            }
            if (current_time_long == silent_time_long)
            {
              gtk_widget_hide (self->window->inputbar);
              g_timeout_add_seconds (300, (GSourceFunc) gtk_widget_show_all, self->window->inputbar);
            }
            else if (current_time_long - silent_time_long < 5)
            {
              gtk_widget_hide (self->window->inputbar);
              g_timeout_add_seconds ((current_time_long - silent_time_long) * 60, (GSourceFunc) gtk_widget_show_all, self->window->inputbar);
            }
            /*XXX: TODO: (Un-)silence ingame.*/
        }

        if (g_strstr_len (message_up, -1, "STATUS CHANGED. NEW STATUS: DND"))
        {
          self->window->dnd = TRUE;
        }
        else if (g_strstr_len (message_up, -1, "STATUS CHANGED. NEW STATUS: ACTIVE"))
        {
          self->window->dnd = FALSE;
        }

        g_free (kickmsg);
        g_free (message_up);
        g_free (silmsg);
      }

    #ifndef INGAME
      parse_message (message_t, devchat_cb_data_new (self->window, view));
    #else
      gchar* ingame_message = parse_message (message_t, devchat_cb_data_new (self->window, view));
      ingame_append_message (self, name, mode, time_attr, lid, ingame_message);
    #endif

      g_free (message_t);

      GtkTextIter start;
      gtk_text_buffer_get_iter_at_mark (buf, &start, old_start);
      gtk_text_buffer_get_end_iter (buf, &end);

      gchar* taglevel;
      gchar* tagulevel;

      switch (user_level)
      {
        case 1: tagulevel = g_strdup ("ul1"); break;
        case 3: tagulevel = g_strdup ("ul3"); break;
        case 5: tagulevel = g_strdup ("ul5"); break;
        default: tagulevel = g_strdup ("ul6"); break;
      }

      switch (msg_level)
      {
        case 0:
        case 1: taglevel = g_strdup ("l1"); break;
        case 3: taglevel = g_strdup ("l3"); break;
        case 5: taglevel = g_strdup ("l5"); break;
        default: taglevel = g_strdup ("l6"); break;
      }

      gtk_text_buffer_apply_tag_by_name (buf, taglevel, &start, &end);
      gtk_text_buffer_apply_tag_by_name (buf, tagulevel, &start, &end);
      gtk_text_buffer_apply_tag_by_name (buf, tagname, &start, &end);

      gtk_text_iter_forward_chars (&start, 6);

      if (!show_name)
        gtk_text_buffer_apply_tag_by_name (buf, a_tag, &start, &end);

      g_free (taglevel);
      g_free (tagulevel);
      gtk_text_buffer_delete_mark (buf, old_start);
      g_free (name);
      g_free (tagname);
      g_free (mode);
      g_free (time_attr);
      g_free (lid);
      g_free (message);

      GtkTextIter e;
      gtk_text_buffer_get_end_iter (buf, &e);
      gtk_text_buffer_insert (buf, &e, "\xe2\x80\x8b", -1);
    }

    g_free (node);
  }
  if (debug)
    dbg ("Message list parsed.");

  if (message_found)
  {
  #ifdef INGAME
    ingame_flush_data (self);
  #endif

    GtkAdjustment* a = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (gtk_widget_get_parent (self->window->outputwidget)));
    if ((a->upper - (a->value + a->page_size)) < 90)
      gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (self->window->outputwidget), scroll_to);

    while (scroll_tos)
    {
      GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (gtk_widget_get_parent (((DevchatConversation*) scroll_tos->data)->out_widget)));
      GtkTextIter ed;
      gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (((DevchatConversation*) scroll_tos->data)->out_buffer), &ed);
      if (((adj->upper - (adj->value + adj->page_size)) < 90) || (self->window->firstrun == TRUE))
      {
        g_timeout_add (50, (GSourceFunc) scroll_mark_onscreen, (DevchatConversation*) scroll_tos->data);
      }
      else
      {
        gtk_text_buffer_delete_mark (GTK_TEXT_BUFFER (((DevchatConversation*) scroll_tos->data)->out_buffer), ((DevchatConversation*) scroll_tos->data)->scroll_to);
        ((DevchatConversation*) scroll_tos->data)->scroll_to = NULL;
      }

      GSList* tmp = scroll_tos;
      scroll_tos = scroll_tos->next;
      g_slist_free_1 (tmp);
    }
  }

  gtk_text_buffer_delete_mark (self->window->output, scroll_to);

  if (g_strcmp0 (date, "") == 0)
    self->window->firstrun = FALSE;

  xmlFreeTextReader (msgparser);

  g_free (msglist);
}

gboolean scroll_mark_onscreen (DevchatConversation* c)
{
  gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (c->out_widget), c->scroll_to);
  gtk_text_buffer_delete_mark (c->out_buffer, c->scroll_to);
  c->scroll_to = NULL;
  return FALSE;
}

void devchat_window_create_tags (GtkTextBuffer* buf, DevchatCBData* data)
{
  gtk_text_buffer_create_tag (buf, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_buffer_create_tag (buf, "italic", "style", PANGO_STYLE_ITALIC, NULL);
  gtk_text_buffer_create_tag (buf, "underline", "underline", PANGO_UNDERLINE_SINGLE, NULL);
  gtk_text_buffer_create_tag (buf, "time", "foreground", data->window->settings.color_time, NULL);
  gtk_text_buffer_create_tag (buf, "peasant", "foreground", data->window->settings.color_blues, NULL);
  gtk_text_buffer_create_tag (buf, "goldie", "foreground", data->window->settings.color_goldies, NULL);
  gtk_text_buffer_create_tag (buf, "greenie", "foreground", data->window->settings.color_greens, NULL);
  gtk_text_buffer_create_tag (buf, "l1", "paragraph-background", data->window->settings.color_l1, NULL);
  gtk_text_buffer_create_tag (buf, "l3", "paragraph-background", data->window->settings.color_l3, NULL);
  gtk_text_buffer_create_tag (buf, "l5", "paragraph-background", data->window->settings.color_l5, NULL);
  gtk_text_buffer_create_tag (buf, "l6", "paragraph-background", data->window->settings.color_l6, NULL);
  gtk_text_buffer_create_tag (buf, "ul1", NULL);
  gtk_text_buffer_create_tag (buf, "ul3", NULL);
  gtk_text_buffer_create_tag (buf, "ul5", NULL);
  gtk_text_buffer_create_tag (buf, "ul6", NULL);
  gtk_text_buffer_create_tag (buf, "red", "foreground", data->window->settings.color_red, NULL);
  gtk_text_buffer_create_tag (buf, "green", "foreground", data->window->settings.color_green, NULL);
  gtk_text_buffer_create_tag (buf, "blue", "foreground", data->window->settings.color_blue, NULL);
  gtk_text_buffer_create_tag (buf, "cyan", "foreground", data->window->settings.color_cyan, NULL);
  gtk_text_buffer_create_tag (buf, "yellow", "foreground", data->window->settings.color_yellow, NULL);
  gtk_text_buffer_create_tag (buf, "magenta", "foreground", data->window->settings.color_magenta, NULL);
}

gboolean badass (gchar* name, DevchatCBData* data)
{
  gchar* uid = g_hash_table_lookup (data->window->users, name);

  if (uid)
  {
    gchar* known_permission = (gchar*) g_hash_table_lookup (data->window->moderators, uid);
    if (!known_permission)
    {
      SoupMessage* pro_get = soup_message_new ("GET",g_strdup_printf("http://forum.egosoft.com/profile.php?mode=viewprofile&u=%s",uid));
      guint status = soup_session_send_message (data->window->session, pro_get);

      if (status == 200)
      {
        const gchar* message = pro_get->response_body->data;

        if (message)
        {
          /*Quote Belisarius: »Nein, die Säcke von Deep Silver und Koch Media bekommen keine Kickrechte.« Yessir.*/
          if (g_regex_match_simple ("<span class=\"postdetails\">(Moderator|Site Admin|EGOSOFT)</span>", message, G_REGEX_CASELESS, 0))
          {
            g_hash_table_insert (data->window->moderators, g_strdup (uid), g_strdup ("y"));
            return TRUE;
          }
          else
            g_hash_table_insert (data->window->moderators, g_strdup (uid), g_strdup ("n"));
        }
      }
    }
    else
      return g_strcmp0 (known_permission, "y") == 0;
  }

  return FALSE;
}

gchar* parse_message (gchar* message_d, DevchatCBData* data)
{
  /*Abandon all hope, ye who enter here.*/
  enum
  {
    STATE_DATA,
    STATE_TYPECHECK,
    STATE_CLOSETAG,
    STATE_OPENTAG,
    STATE_ATTR,
    STATE_ATTRCONT
  };

#ifdef INGAME
  gchar* retval = "";
#endif

  GtkTextIter old_end;

  GtkTextTagTable* table = gtk_text_buffer_get_tag_table (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)));

  GRegex* plus = g_regex_new ("&#43;", 0, 0, NULL);

  gchar current[2];
  current[0] = 32;
  current[1] = 0;
  gchar* content = "";
#ifdef INGAME
  gchar* ingame_content = "";
#endif
  GSList* taglist = NULL;
  GSList* stack = g_slist_prepend (NULL, "Γ");
  gchar* known_tags[] = {"font","i","u","b","br","span","img","a","!--","div","p",NULL};

  DevchatHTMLTag* current_tag = devchat_html_tag_new ();
  DevchatHTMLAttr* current_attr = devchat_html_attr_new ();

  gint state = STATE_DATA;

  gint i;

  if (debug)
    dbg ("Starting parser loop...");

  for (i=0; i < strlen (message_d); i++)
  {
    current[0] = message_d[i];

    if (real_debug) {
      dbg_msg = g_strdup_printf ("Current char: %s.", current);
      dbg (dbg_msg);
      g_free (dbg_msg);
    }

    if (state == STATE_DATA)
    {
      if (real_debug)
        dbg ("State: Data");

      if (g_strcmp0 (current, "<") == 0)
      {
        if (real_debug) {
          dbg_msg = g_strdup_printf ("Detected <, switching to state typecheck and dumping content %s.", content);
          dbg (dbg_msg);
          g_free (dbg_msg);
        }

        GtkTextIter end;
        gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end);

        if (g_strcmp0 (content, "") != 0)
        {
          gtk_text_buffer_insert (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end, content, -1);
        #ifdef INGAME
          retval = g_strconcat (retval, ingame_content, NULL);
          ingame_content = "";
        #endif
          content = "";
        }

        state = STATE_TYPECHECK;
      }
      else if (g_strcmp0 (current, "&") == 0)
      {
        /* Entity */
        int j;
        gchar ent_current[2];
        ent_current[0] = 32;
        ent_current[1] = 0;
        gchar* entity_name = "";
        gboolean found = FALSE;
        for (j=i; j < i+8 && found == FALSE; j++)
        {
          ent_current[0] = message_d[j];
          if (ent_current[0] == 45 || ent_current[0] == 46
          || (ent_current[0] > 47 && ent_current[0] < 58) || (ent_current[0] > 64 && ent_current[0] < 91) || ent_current[0] == 95
          || (ent_current[0] > 96 && ent_current[0] < 123) || ent_current[0] == 35)
            entity_name = g_strconcat (entity_name, ent_current, NULL);
          if (ent_current[0] == 59)
            found = TRUE;
        }

        if (!found)
        {
          /* Not an entity, just a stray &. Every day, dozens of & are set astray by their heartless owners just because they're too lazy to care for them and wrap them in a warm, cozy, standard-compliant &amp;. Have a heart and FUCKING STOP TO BREAK MY PARSER! */
          content = g_strconcat (content, "&", NULL);
        #ifdef INGAME
          ingame_content = g_strconcat (ingame_content, "&amp;", NULL);
        #endif
        }
        else
        {
          guint64 charval = 0;
          if (entity_name[0] == 35)
          {
            charval = g_ascii_strtoull (entity_name+1,NULL,10);
          #ifdef INGAME
            ingame_content = g_strconcat (ingame_content, "&", entity_name, ";", NULL);
          #endif
          }
          else
          {
            charval = GPOINTER_TO_UINT (g_hash_table_lookup (entities, entity_name));
          #ifdef INGAME
            ingame_content = g_strdup_printf ("%s&#%"PRIu64";", ingame_content, charval);
          #endif
          }
          if (charval)
          {
            gchar c[6];
            c[g_unichar_to_utf8 (charval, c)] = 0;
            content = g_strconcat (content, c, NULL);
          }

          i = i + strlen(entity_name) + 1;
        }
      }
      else
      {
        if (real_debug)
          dbg ("Adding char to content.");

        content = g_strconcat (content, current, NULL);
      #ifdef INGAME
        ingame_content = g_strconcat (ingame_content, current, NULL);
      #endif
      }
    }
    else if (state == STATE_TYPECHECK)
    {
      if (real_debug)
        dbg ("State: Type check.");

      if (g_strcmp0 (current, "/") == 0 && g_strcmp0 (stack->data, "O") == 0)
      {
        if (real_debug)
          dbg ("Detecting closing tag.");

        GSList* tmp = stack;
        stack = stack->next;
        g_slist_free_1 (tmp);

        state = STATE_CLOSETAG;
      }
      else if (g_strcmp0 (current, " ") != 0)
      {
        if (real_debug)
          dbg ("Adding current to tag name and switching to state open tag.");

        current_tag->name = g_strconcat (current_tag->name, current, NULL);

        stack = g_slist_prepend (stack, g_strdup("O"));
        state = STATE_OPENTAG;
      }
      else
      {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end);

        gtk_text_buffer_insert (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end, "< ", -1);
        state = STATE_DATA;
      }
    }
    else if (state == STATE_CLOSETAG)
    {
      if (real_debug)
        dbg ("State: Close tag.");

      if (g_strcmp0 (current, ">") == 0 && g_strcmp0 (stack->data,"O") == 0)
      {
        GSList* tmp = taglist;
        if (taglist->next)
        {
          taglist = taglist->next;
        }

        DevchatHTMLTag* top = tmp->data;
        /*TODO: Close actually closed tag, not the last one.*/

        if (real_debug) {
          dbg_msg = g_strdup_printf ("Closing Tag %s.", top->name);
          dbg (dbg_msg);
          g_free (dbg_msg);
        }

        gchar* tagname = NULL;

        if (g_strcmp0 (top->name,"font")==0)
        {
        if (real_debug) {
          dbg_msg = g_strdup_printf ("Found font tag! Attribute: %s\n",((DevchatHTMLAttr*) top->attrs->data)->name);
          dbg (dbg_msg);
          g_free (dbg_msg);
        }

          if (g_strcmp0 ( ((DevchatHTMLAttr*) top->attrs->data)->name, "color") == 0)
          {
            gchar* raw_color = ((DevchatHTMLAttr*) top->attrs->data)->value;

            if (g_strcmp0 (raw_color, "#ff0000") == 0)
              tagname = "red";
            else if (g_strcmp0 (raw_color, "#00ff00") == 0)
              tagname = "green";
            else if (g_strcmp0 (raw_color, "#0000ff") == 0)
              tagname = "blue";
            else if (g_strcmp0 (raw_color, "#ffff00") == 0)
              tagname = "yellow";
            else if (g_strcmp0 (raw_color, "#00ffff") == 0)
              tagname = "cyan";
            else if (g_strcmp0 (raw_color, "#ff00ff") == 0)
              tagname = "magenta";
          }
        }
        else if (g_strcmp0 (top->name,"i")==0)
        {
          tagname = "italic";
        }
        else if (g_strcmp0 (top->name,"u")==0)
        {
          tagname = "underline";
        }
        else if (g_strcmp0 (top->name,"b")==0)
        {
          tagname = "bold";
        }
        else if (g_ascii_strcasecmp (top->name,"BR")==0)
        {
          GtkTextIter fnord;

          gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &fnord);
          gtk_text_buffer_insert (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &fnord, "\n", -1);
        }
        else if (g_strcmp0 (top->name,"span")==0)
        {
          if (g_strcmp0 ( ((DevchatHTMLAttr*) top->attrs->data)->name, "class") == 0
              && g_strcmp0 ( ((DevchatHTMLAttr*) top->attrs->data)->value, "chatname_green") == 0)
            tagname = "greenie";
          else if (g_strcmp0 ( ((DevchatHTMLAttr*) top->attrs->data)->name, "class") == 0
              && g_strcmp0 ( ((DevchatHTMLAttr*) top->attrs->data)->value, "chatname_blue") == 0)
            tagname = "peasant";
        }
        else if (g_strcmp0 (top->name,"img")==0)
        {
          if (real_debug)
            dbg ("Parsing img tag...");

          gchar* smilie = NULL;
          gchar* uri = NULL;

          if (top->attrs->next && top->attrs->next->next)
            smilie = (gchar*) g_hash_table_lookup (data->window->smilies, (gchar*) ((DevchatHTMLAttr*) top->attrs->next->next->data)->value);
          uri = (gchar*) ((DevchatHTMLAttr*) top->attrs->next->data)->value;

          if (smilie)
          {
            if (real_debug)
              dbg ("Found smilie in database.");
          #ifdef INGAME
            retval = g_strconcat(retval, (gchar*) ((DevchatHTMLAttr*) top->attrs->next->next->data)->value, NULL);
          #endif

            GtkWidget* img = gtk_image_new_from_file (smilie);
            gtk_widget_set_tooltip_text (img, (gchar*) ((DevchatHTMLAttr*) top->attrs->next->next->data)->value);

            GtkTextIter fnord;
            gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &fnord);

            GtkTextChildAnchor* a = gtk_text_buffer_create_child_anchor (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &fnord);

            gtk_text_view_add_child_at_anchor (GTK_TEXT_VIEW (data->data), img, a);
            gtk_widget_show (img);
          }
          else if (uri && g_regex_match_simple ("http://.*?\\.([a-z][a-z]+)/.*?\\.(gif|jpg|png|tga|tif|tiff|webp)", uri, 0, 0)) /*Only attempt downloading if it's really an image. I will refrain from insulting Alex this time.*/
          {
            if (real_debug) {
              dbg_msg = g_strdup_printf ("Searching for image %s... ", uri);
              dbg (dbg_msg);
              g_free (dbg_msg);
            }
          #ifdef INGAME
            retval = g_strconcat(retval, "[img]", uri, "[/img]", NULL);
          #endif

            gchar** uri_parts = g_strsplit_set (uri, "/\\:*?\"<>|", 0); /*Stupid Win32 doesn't allow these chars in file names...*/

            gchar* filename = NULL;

          #ifdef G_OS_WIN32
            filename = g_build_filename (g_getenv ("TEMP"), g_strjoinv ("_",uri_parts), NULL);
          #else
            filename = g_build_filename ("/tmp", g_strjoinv ("_",uri_parts), NULL);
          #endif
            g_strfreev (uri_parts);

            if (real_debug) {
              dbg_msg = g_strdup_printf ("Writing image to %s.", filename);
              dbg (dbg_msg);
              g_free (dbg_msg);
            }

            if (!g_file_test (filename, G_FILE_TEST_EXISTS))
            {
              if (real_debug)
                dbg ("File not in cache, downloading...");

              SoupMessage* i_m = soup_message_new ("GET", uri);
              if (soup_session_send_message (data->window->session, i_m) == 200)
              {
                GError* error = NULL;
                if (!g_file_set_contents (filename, i_m->response_body->data, i_m->response_body->length, &error))
                {
                  err (g_strdup_printf (_("Error D: Error while saving image: %s.\n"), error->message));
                  g_error_free (error);
                }
              }
              else
                err (g_strdup_printf (_("Error D: Error downloading image %s: %i %s.\n"), uri, i_m->status_code, i_m->reason_phrase));
            }

            GtkTextIter fnord;
            gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &fnord);

            GtkWidget* img = gtk_image_new_from_file (filename);

            GdkPixbuf* p;
            GdkPixbufAnimation* a;

            switch (gtk_image_get_storage_type (GTK_IMAGE (img)))
            {
              case GTK_IMAGE_ANIMATION:
                a = gtk_image_get_animation (GTK_IMAGE (img));
                p = gdk_pixbuf_animation_get_static_image (a);
              break;
              case GTK_IMAGE_PIXBUF:
                p = gtk_image_get_pixbuf (GTK_IMAGE (img));
                a = NULL;
              break;
              default:
                err (_("Error U: Unknown image type!"));
                p = NULL;
                a = NULL;
              break;
            }

            if (p && (gdk_pixbuf_get_width (p) > 320 || gdk_pixbuf_get_height (p) > 240))
            {
              if (gdk_pixbuf_get_width (p) > 320)
              {
                p = gdk_pixbuf_scale_simple (p, 320, 320 / (gdk_pixbuf_get_width (p)*gdk_pixbuf_get_height (p)), GDK_INTERP_BILINEAR);
              }
              else
              {
                p = gdk_pixbuf_scale_simple (p, 240 / (gdk_pixbuf_get_width (p)*gdk_pixbuf_get_height (p)), 240, GDK_INTERP_BILINEAR);
              }
              gtk_text_buffer_insert_pixbuf (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &fnord, p);
            }
            else
            {
              GtkTextChildAnchor* a = gtk_text_buffer_create_child_anchor (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &fnord);

              gtk_text_view_add_child_at_anchor (GTK_TEXT_VIEW (data->data), img, a);
              gtk_widget_show (img);
            }

            gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &fnord);
            GtkTextIter fnord2 = fnord;
            gtk_text_iter_backward_char (&fnord2);
            tagname = g_strconcat ("img::", uri, NULL);

            if (!gtk_text_tag_table_lookup (table, tagname))
            {
              GtkTextTag* tag = gtk_text_tag_new (tagname);

              gtk_text_tag_table_add (table, GTK_TEXT_TAG (tag));
            }

            gtk_text_buffer_apply_tag_by_name (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), tagname, &fnord, &fnord2);

            g_free (filename);
          }
        }
        else if (g_strcmp0 (top->name,"a")==0)
        {
          if (top->attrs->next)
            tagname = g_strconcat ("url::", ((DevchatHTMLAttr*) top->attrs->next->data)->value, NULL);
          else
            tagname = g_strconcat ("url::", ((DevchatHTMLAttr*) top->attrs->data)->value, NULL); /*Mailto*/

          /*Stupid server always interpreting forum. as link.*/
          if (g_strcmp0 (tagname, "url::http://forum.") == 0)
          {
            g_free (tagname);
            tagname = NULL;
          }
          else
          {
            gchar* tagname_d = g_regex_replace (plus, tagname, -1, 0, "+", 0, NULL);

            g_free (tagname);

            tagname = tagname_d;

            if (real_debug) {
              dbg_msg = g_strdup_printf ("Inserting link to %s.", tagname);
              dbg (dbg_msg);
              g_free (dbg_msg);
            }

          #ifdef INGAME
            retval = g_strconcat(retval, "[url]", tagname_d, "[/url]", NULL);
          #endif

            if (!gtk_text_tag_table_lookup (table, tagname))
            {
              DevchatURLTag* tag = devchat_url_tag_new (tagname, data->window->settings.color_url);

              gtk_text_tag_table_add (table, GTK_TEXT_TAG (tag));
            }
          }
        }
        else if (g_strcmp0 (top->name,"!--") == 0)
        {
          if (real_debug)
            dbg ("Detected comment.");

          if (data->window->settings.showhidden)
          {
            top->attrs = g_slist_reverse (top->attrs);

            gchar* comment = "";

            while (top->attrs && g_strcmp0 (( (DevchatHTMLAttr*) top->attrs->data)->name, "--"))
            {
              comment = g_strconcat (comment, ( (DevchatHTMLAttr*) top->attrs->data)->name, " ", NULL);
              top->attrs = top->attrs->next;
            }
            GtkTextIter end;

            gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end);

          #ifdef INGAME
            retval = g_strconcat(retval, "\\033Z", comment, "\\033X", NULL);
          #endif

            gtk_text_buffer_insert_with_tags_by_name (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end, comment, -1, "time", NULL);
          }
        }

        if (tagname)
        {
          if (debug) {
            dbg_msg = g_strdup_printf ("Applying tag %s.", tagname);
            dbg (dbg_msg);
            g_free (dbg_msg);
          }

          GtkTextIter end;
          GtkTextIter start;
          gtk_text_buffer_get_iter_at_mark (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &start, top->start_mark);
          gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end);

          GtkTextIter start_pp = start;
          gtk_text_iter_forward_char (&start_pp);
          GtkTextIter start_prev = start;
          gtk_text_iter_backward_char (&start_prev);
          if (g_strcmp0 (gtk_text_buffer_get_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &start, &start_pp, FALSE), " ") != 0
              && g_strcmp0 (gtk_text_buffer_get_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &start_prev, &start, FALSE), " ") != 0)
          {
            gtk_text_buffer_get_iter_at_mark (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &start, top->start_mark);
            gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end);
          }

          gtk_text_buffer_apply_tag_by_name (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), tagname, &start, &end);
        }

        if (top->start_mark)
          gtk_text_buffer_delete_mark (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), top->start_mark);
        g_slist_free_1 (tmp);

        current_tag = devchat_html_tag_new ();

        state = STATE_DATA;
      }
      else if (g_strcmp0 (stack->data,"O") == 0)
      {
        if (real_debug)
          dbg ("Adding current to tag name.");

        current_tag->name = g_strconcat (current_tag->name, current, NULL);
      }
      else
      {
        /*FUUUUUUUUUUUUUUUUU Oger!*/
        state = STATE_DATA;
      }
    }
    else if (state == STATE_OPENTAG)
    {
      if (real_debug)
        dbg ("State: Open tag.");

      if (g_strcmp0 (current, ">") == 0)
      {
        if (real_debug) {
          dbg_msg = g_strdup_printf ("Detecting closing of %s tag definition, going back to data state or close tag, if tag is void.",current_tag->name);
          dbg (dbg_msg);
          g_free (dbg_msg);
        }

        /*Non-closing tags: HR, BR, area, img, param, input, option, col*/
        if (g_ascii_strcasecmp (current_tag->name, "BR") == 0 || g_strcmp0 (current_tag->name, "img") == 0 || g_strcmp0 (current_tag->name, "!--") == 0)
        {
          if (real_debug)
            dbg ("Closing void tag.");

          state = STATE_CLOSETAG;
          i--;
        }
        else
        {

          int i;
          gboolean known = FALSE;

          for (i=0; known_tags[i] && !known; i++)
          {
            known = g_strcmp0 (current_tag->name, known_tags[i]) == 0;
          }

          if (!known)
          {
            if (debug)
            {
              dbg_msg = g_strdup_printf ("Unknown tag »%s«!\n", current_tag->name);
              dbg (dbg_msg);
              g_free (dbg_msg);
            }

            GSList* tmp = stack;
            stack = stack->next;
            g_slist_free_1 (tmp);

            content = g_strconcat ("<", current_tag->name, ">", NULL);
          #ifdef INGAME
            retval = g_strconcat (retval, "&lt;", current_tag->name, "&gt;", NULL);
          #endif

            GtkTextIter end;
            gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end);
            gtk_text_buffer_insert (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end, content, -1);

            content = "";

            current_tag = devchat_html_tag_new ();
          }

          state = STATE_DATA;
        }

        gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &old_end);
        current_tag->start_mark = gtk_text_mark_new (NULL, TRUE);
        gtk_text_buffer_add_mark (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), current_tag->start_mark, &old_end);

        taglist = g_slist_prepend (taglist, current_tag);
        current_tag = devchat_html_tag_new ();
      }
      else if (g_strcmp0 (current, " ") == 0)
      {
        if (real_debug)
          dbg ("Detecting end of tag name definition, switching to state attribute.");

        int i;
        gboolean known = FALSE;

        for (i=0; known_tags[i] && !known; i++)
        {
          known = g_strcmp0 (current_tag->name, known_tags[i]) == 0;
        }

        if (!known)
        {
          if (debug)
          {
            dbg_msg = g_strdup_printf ("Unknown tag »%s«!\n", current_tag->name);
            dbg (dbg_msg);
            g_free (dbg_msg);
          }
          state = STATE_DATA;

          GSList* tmp = stack;
          stack = stack->next;
          g_slist_free_1 (tmp);

          content = g_strconcat ("<", current_tag->name, " ", NULL);
        #ifdef INGAME
          retval = g_strconcat (retval, "&lt;", current_tag->name, " ", NULL);
        #endif

          GtkTextIter end;
          gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end);
          gtk_text_buffer_insert (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &end, content, -1);

          content = "";

          current_tag = devchat_html_tag_new ();
        }
        else
          state = STATE_ATTR;
      }
      else
      {
        if (real_debug)
          dbg ("Adding current to tag name.");


        current_tag->name = g_strconcat (current_tag->name, current, NULL);
      }
    }
    else if (state == STATE_ATTR)
    {
      if (real_debug)
        dbg ("State: Attribute.");

      if (g_strcmp0 (current, "=") == 0)
      {
        if (real_debug)
          dbg ("Detecting value definition start. Switching to state attribute content.");

        i++;
        stack = g_slist_prepend (stack, g_strdup ("\""));
        state = STATE_ATTRCONT;
      }
      else if (g_strcmp0 (current, " ") == 0)
      {
        if (real_debug)
          dbg ("Detecting end of attribute, switching back to state open tag.");

        current_tag->attrs = g_slist_prepend (current_tag->attrs, current_attr);
        current_attr = devchat_html_attr_new ();
        state = STATE_OPENTAG;
        i--;
      }
      else if (g_strcmp0 (current, ">") == 0)
      {
        if (real_debug) {
          dbg_msg = g_strdup_printf ("Detecting closing of %s tag definition, going back to data state or close tag, if tag is void.",current_tag->name);
          dbg (dbg_msg);
          g_free (dbg_msg);
        }

        /*Non-closing tags: HR, BR, area, img, param, input, option, col*/
        if (g_strcmp0 (current_tag->name, "BR") == 0 || g_strcmp0 (current_tag->name, "img") == 0 || g_strcmp0 (current_tag->name, "!--") == 0)
        {
          if (real_debug)
            dbg ("Closing void tag.");

          state = STATE_CLOSETAG;
          i--;
        }
        else
        {
          state = STATE_DATA;
        }

        current_tag->attrs = g_slist_prepend (current_tag->attrs, current_attr);
        current_attr = devchat_html_attr_new ();

        gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &old_end);
        current_tag->start_mark = gtk_text_mark_new (NULL, TRUE);
        gtk_text_buffer_add_mark (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), current_tag->start_mark, &old_end);

        taglist = g_slist_prepend (taglist, current_tag);
        current_tag = devchat_html_tag_new ();
      }
      else
      {
        if (real_debug)
          dbg ("Adding current to attribute name.");

        current_attr->name = g_strconcat (current_attr->name, current, NULL);
      }
    }
    else if (state == STATE_ATTRCONT)
    {
      if (g_strcmp0 (current, "\"") == 0)
      {
        if (real_debug)
          dbg ("Detected \", switching back to state attribute and pop()'ing stack.");

        state = STATE_ATTR;

        GSList* tmp = stack;
        stack = stack->next;
        g_slist_free_1 (tmp);
      }
      else if (g_strcmp0 (current, "&") == 0)
      {
        /* Entity */
        int j;
        gchar ent_current[2];
        ent_current[0] = 32;
        ent_current[1] = 0;
        gchar* entity_name = "";
        gboolean found = FALSE;
        for (j=i; j < i+8 && found == FALSE; j++)
        {
          ent_current[0] = message_d[j];
          if (ent_current[0] == 45 || ent_current[0] == 46
          || (ent_current[0] > 47 && ent_current[0] < 58) || (ent_current[0] > 64 && ent_current[0] < 91) || ent_current[0] == 95
          || (ent_current[0] > 96 && ent_current[0] < 123) || ent_current[0] == 35)
            entity_name = g_strconcat (entity_name, ent_current, NULL);
          if (ent_current[0] == 59)
            found = TRUE;
        }

        if (!found)
        {
          /* Not an entity, just a stray &. Every day, dozens of & are set astray by their heartless owners just because they're too lazy to care for them and wrap them in a warm, cozy, standard-compliant &amp;. Have a heart and FUCKING STOP TO BREAK MY PARSER! */
          current_attr->value = g_strconcat (current_attr->value, "&", NULL);
        }
        else
        {
          guint64 charval = 0;
          if (entity_name[0] == 35)
          {
            charval = g_ascii_strtoull (entity_name+1,NULL,10);
          }
          else
          {
            charval = GPOINTER_TO_UINT (g_hash_table_lookup (entities, entity_name));
          }
          if (charval)
          {
            gchar c[6];
            c[g_unichar_to_utf8 (charval, c)] = 0;
            current_attr->value = g_strconcat (current_attr->value, c, NULL);
          }


          i = i + strlen(entity_name) + 1;
        }
      }
      else
      {
        if (real_debug)
          dbg ("Adding current to attribute valute.");

        current_attr->value = g_strconcat (current_attr->value, current, NULL);
      }
    }
  }

  g_free (plus);

  if (debug)
    dbg ("Removing trailing whitespace from message...");

  GtkTextIter chomp_iter, chomp_end;
  gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &chomp_iter);
  chomp_end = chomp_iter;
  gtk_text_iter_backward_char (&chomp_iter);

  while (g_unichar_isspace (gtk_text_iter_get_char (&chomp_iter)))
    gtk_text_iter_backward_char (&chomp_iter);

  if (gtk_text_iter_forward_char (&chomp_iter))
    gtk_text_buffer_delete (gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->data)), &chomp_iter, &chomp_end);

  if (debug)
    dbg ("Parsing done.");

#ifdef INGAME
  return retval;
#else
  return NULL;
#endif
}

gboolean hotkey_cb (GtkWidget* w, GdkEventKey* key, DevchatCBData* data)
{
  if (key->type == GDK_KEY_PRESS && (key->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
  {
    if (key->keyval == GDK_Up) /*Shamelessly copied from browser client.*/
    {
      gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
      GtkTextBuffer* buf;
      GtkTextIter start;
      GtkTextIter end;

      if (pagenum == 0)
      {
        buf = data->window->input;
      }
      else
      {
        const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
        DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
        buf = conv->in_buffer;
      }
      gtk_text_buffer_get_start_iter (buf, &start);
      gtk_text_buffer_get_end_iter (buf, &end);

      data->window->buffer[data->window->buf_current] = g_strdup (gtk_text_buffer_get_text (buf, &start, &end, FALSE));
      data->window->buf_current++;
      if (data->window->buf_current > MAX_BUF)
        data->window->buf_current = MAX_BUF;
      else if (data->window->buf_current < 0)
      {
        if (g_strcmp0 (data->window->buffer[0], "") != 0)
        {
          gint i = MAX_BUF-1;
          if (g_strcmp0 (data->window->buffer[MAX_BUF], "") != 0)
            g_free (data->window->buffer[MAX_BUF]);
          while (i >= 0)
          {
            data->window->buffer[i+1] = data->window->buffer[i];
            i--;
          }
          data->window->buffer[0] = "";
        }
        data->window->buf_current = 0;
      }

      gtk_text_buffer_set_text (buf, data->window->buffer[data->window->buf_current], -1);
    }
    else if (key->keyval == GDK_Down) /*Shamelessly copied from browser client.*/
    {
      gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
      GtkTextBuffer* buf;
      GtkTextIter start;
      GtkTextIter end;

      if (pagenum == 0)
      {
        buf = data->window->input;
      }
      else
      {
        const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
        DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
        buf = conv->in_buffer;
      }
      gtk_text_buffer_get_start_iter (buf, &start);
      gtk_text_buffer_get_end_iter (buf, &end);

      data->window->buffer[data->window->buf_current] = g_strdup (gtk_text_buffer_get_text (buf, &start, &end, FALSE));
      data->window->buf_current--;
      if (data->window->buf_current > MAX_BUF)
        data->window->buf_current = MAX_BUF;
      else if (data->window->buf_current < 0)
      {
        if (g_strcmp0 (data->window->buffer[0], "") != 0)
        {
          gint i = MAX_BUF-1;
          if (g_strcmp0 (data->window->buffer[MAX_BUF], "") != 0)
            g_free (data->window->buffer[MAX_BUF]);
          while (i >= 0)
          {
            data->window->buffer[i+1] = data->window->buffer[i];
            i--;
          }
          data->window->buffer[0] = "";
        }
        data->window->buf_current = 0;
      }

      gtk_text_buffer_set_text (buf, data->window->buffer[data->window->buf_current], -1);
    }
    else if (data->window->userlevel > 1)
    {
      GdkColor new_bg;
      if (key->keyval == GDK_1 || key->keyval == GDK_KP_1)
      {
        gtk_combo_box_set_active (GTK_COMBO_BOX (data->window->level_box), 0);
        gdk_color_parse (data->window->settings.color_l1, &new_bg);
        gtk_widget_modify_base (data->window->inputwidget, GTK_STATE_NORMAL, &new_bg);
      }
      else if (key->keyval == GDK_3 || key->keyval == GDK_KP_3)
      {
        gtk_combo_box_set_active (GTK_COMBO_BOX (data->window->level_box), 1);
        gdk_color_parse (data->window->settings.color_l3, &new_bg);
        gtk_widget_modify_base (data->window->inputwidget, GTK_STATE_NORMAL, &new_bg);
      }
      else if (data->window->userlevel > 3)
      {
        if (key->keyval == GDK_5 || key->keyval == GDK_KP_5)
        {
          gtk_combo_box_set_active (GTK_COMBO_BOX (data->window->level_box), 2);
          gdk_color_parse (data->window->settings.color_l5, &new_bg);
          gtk_widget_modify_base (data->window->inputwidget, GTK_STATE_NORMAL, &new_bg);
        }
        else if (data->window->userlevel > 5 && (key->keyval == GDK_6 || key->keyval == GDK_KP_6))
        {
          gtk_combo_box_set_active (GTK_COMBO_BOX (data->window->level_box), 3);
          gdk_color_parse (data->window->settings.color_l6, &new_bg);
          gtk_widget_modify_base (data->window->inputwidget, GTK_STATE_NORMAL, &new_bg);
        }
      }
    }
  }
  return FALSE;
}

void config_cb(GtkWidget* widget, DevchatCBData* data)
{
  GtkWidget* dialog = gtk_dialog_new ();

  GtkWidget* note_label = gtk_label_new (_("Note: Some settings will apply on restart."));
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area( GTK_DIALOG (dialog))), note_label, FALSE, FALSE, 0);

  gtk_window_set_title (GTK_WINDOW (dialog), _("Devchat settings"));
  gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (data->window->window));
  gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);

  gtk_dialog_add_buttons (GTK_DIALOG (dialog), _("Reset settings"), GTK_RESPONSE_REJECT,
                                             #ifndef G_OS_WIN32
                                               GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                             #endif
                                               GTK_STOCK_OK, GTK_RESPONSE_OK,
                                             #ifdef G_OS_WIN32
                                               GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                             #endif
                                               NULL);

  GtkWidget* nb = gtk_notebook_new ();

  GdkColor c;

  GtkWidget* hbox1 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_col_time = gtk_label_new (_("Timestamp color:"));
  gdk_color_parse (data->window->settings.color_time, &c);
  GtkWidget* btn_col_time = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_font = gtk_label_new (_("Normal font color:"));
  gdk_color_parse (data->window->settings.color_font, &c);
  GtkWidget* btn_col_font = gtk_color_button_new_with_color (&c);
  gtk_box_pack_start (GTK_BOX(hbox1), label_col_time,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox1), btn_col_time,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox1), label_col_font,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox1), btn_col_font,FALSE,FALSE,0);

  GtkWidget* hbox2 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_col_green = gtk_label_new (_("Greenie color:"));
  gdk_color_parse (data->window->settings.color_greens, &c);
  GtkWidget* btn_col_green = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_blue = gtk_label_new (_("Normal user color:"));
  gdk_color_parse (data->window->settings.color_blues, &c);
  GtkWidget* btn_col_blue = gtk_color_button_new_with_color (&c);
  gtk_box_pack_start (GTK_BOX(hbox2), label_col_green,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox2), btn_col_green,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox2), label_col_blue,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox2), btn_col_blue,FALSE,FALSE,0);

  GtkWidget* hbox3 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_col_l1 = gtk_label_new (_("L1 background color:"));
  gdk_color_parse (data->window->settings.color_l1, &c);
  GtkWidget* btn_col_l1 = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_l3 = gtk_label_new (_("L3 background color:"));
  gdk_color_parse (data->window->settings.color_l3, &c);
  GtkWidget* btn_col_l3 = gtk_color_button_new_with_color (&c);
  gtk_box_pack_start (GTK_BOX(hbox3), label_col_l1,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox3), btn_col_l1,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox3), label_col_l3,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox3), btn_col_l3,FALSE,FALSE,0);

  GtkWidget* hbox4 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_col_l5 = gtk_label_new (_("L5 background color:"));
  gdk_color_parse (data->window->settings.color_l5, &c);
  GtkWidget* btn_col_l5 = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_l6 = gtk_label_new (_("L6 background color:"));
  gdk_color_parse (data->window->settings.color_l6, &c);
  GtkWidget* btn_col_l6 = gtk_color_button_new_with_color (&c);
  gtk_box_pack_start (GTK_BOX(hbox4), label_col_l5,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox4), btn_col_l5,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox4), label_col_l6,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox4), btn_col_l6,FALSE,FALSE,0);

  GtkWidget* hbox5 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_col_url = gtk_label_new (_("Unvisited url color:"));
  gdk_color_parse (data->window->settings.color_url, &c);
  GtkWidget* btn_col_url = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_vurl = gtk_label_new (_("Visited url color:"));
  gdk_color_parse (data->window->settings.color_url_visited, &c);
  GtkWidget* btn_col_vurl = gtk_color_button_new_with_color (&c);
  gtk_box_pack_start (GTK_BOX(hbox5), label_col_url,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox5), btn_col_url,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox5), label_col_vurl,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox5), btn_col_vurl,FALSE,FALSE,0);

  GtkWidget* hbox7 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_col_hurl = gtk_label_new (_("Hover url color:"));
  gdk_color_parse (data->window->settings.color_url_hover, &c);
  GtkWidget* btn_col_hurl = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_high = gtk_label_new (_("Highlighted tab color:"));
  gdk_color_parse (data->window->settings.color_highlight, &c);
  GtkWidget* btn_col_high = gtk_color_button_new_with_color (&c);
  gtk_box_pack_start (GTK_BOX(hbox7), label_col_hurl,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox7), btn_col_hurl,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox7), label_col_high,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox7), btn_col_high,FALSE,FALSE,0);

  GtkWidget* hbox12 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_col_gold = gtk_label_new (_("Goldie color:"));
  gdk_color_parse (data->window->settings.color_goldies, &c);
  GtkWidget* btn_col_gold = gtk_color_button_new_with_color (&c);
  gtk_box_pack_start (GTK_BOX(hbox12), label_col_gold,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox12), btn_col_gold,FALSE,FALSE,0);

  GtkWidget* hbox15 = gtk_hbox_new (TRUE, 1);
  GtkWidget* hbox16 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_col_cyan = gtk_label_new ("[cyan]");
  gdk_color_parse (data->window->settings.color_cyan, &c);
  GtkWidget* btn_col_cyan = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_red = gtk_label_new ("[red]");
  gdk_color_parse (data->window->settings.color_red, &c);
  GtkWidget* btn_col_red = gtk_color_button_new_with_color (&c);
  gtk_box_pack_start (GTK_BOX (hbox15), label_col_cyan, FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox15), btn_col_cyan, FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox12), hbox15,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox16), label_col_red, FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox16), btn_col_red, FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(hbox12), hbox16,FALSE,FALSE,0);

  GtkWidget* hbox17 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_col_yellow = gtk_label_new ("[yellow]");
  gdk_color_parse (data->window->settings.color_yellow, &c);
  GtkWidget* btn_col_yellow = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_greenX = gtk_label_new ("[green]");
  gdk_color_parse (data->window->settings.color_green, &c);
  GtkWidget* btn_col_greenX = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_blueX = gtk_label_new ("[blue]");
  gdk_color_parse (data->window->settings.color_blue, &c);
  GtkWidget* btn_col_blueX = gtk_color_button_new_with_color (&c);
  GtkWidget* label_col_magenta = gtk_label_new ("[magenta]");
  gdk_color_parse (data->window->settings.color_magenta, &c);
  GtkWidget* btn_col_magenta = gtk_color_button_new_with_color (&c);
  gtk_box_pack_start (GTK_BOX (hbox17), label_col_yellow,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox17), btn_col_yellow,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox17), label_col_greenX,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox17), btn_col_greenX,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox17), label_col_blueX,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox17), btn_col_blueX,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox17), label_col_magenta,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox17), btn_col_magenta,FALSE,FALSE,0);

  GtkWidget* vbox1 = gtk_vbox_new (FALSE, 1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox4,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox5,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox7,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox12,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox17,FALSE,FALSE,0);

  gtk_notebook_append_page ( GTK_NOTEBOOK (nb), vbox1, gtk_label_new (_("Color settings")));


  GtkWidget* hbox6 = gtk_hbox_new (TRUE, 1);
  GtkWidget* chk_id = gtk_check_button_new_with_label (_("Show message ID"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_id), data->window->settings.showid);
  gtk_widget_set_tooltip_text (chk_id, _("Shows the internal ID of messages.\nMakes pointing at a certain post easier (and allows you to see whether people are talking in private messages and/or on higher levels).\nNote that this setting only applies on newly received messages."));
  GtkWidget* chk_hd = gtk_check_button_new_with_label (_("Show hidden usernames"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_hd), data->window->settings.showhidden);
  gtk_widget_set_tooltip_text (chk_hd, _("Shows usernames e.g. for /me actions, stealthing greenies and HTML comments <!-- --> (read: Greenie stealth posts).\nChanging this setting will apply on new tabs and newly received messages."));
  GtkWidget* chk_sj = gtk_check_button_new_with_label (_("Stealth join"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_sj), data->window->settings.stealthjoin);
  gtk_widget_set_tooltip_text (chk_sj, _("Suppress own join/quit messages."));
  gtk_box_pack_start (GTK_BOX (hbox6), chk_id,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX (hbox6), chk_hd,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX (hbox6), chk_sj,TRUE,TRUE,0);

  GtkWidget* hbox10 = gtk_hbox_new (TRUE, 1);
  GtkWidget* chk_aj = gtk_check_button_new_with_label (_("Automatic join"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_aj), data->window->settings.autojoin);
  GtkWidget* chk_cu = gtk_check_button_new_with_label (_("Tint user list"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_cu), data->window->settings.coloruser);
  gtk_widget_set_tooltip_text (chk_cu, _("Whether the userlist should be colored in the same color as the TextViews. Recommended if the font contrast would be too low else (read: bright themes like Aero/Classic for Windows®)."));
  GtkWidget* chk_sp = gtk_check_button_new_with_label (_("Remember password"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_sp), data->window->settings.store_pass);
  gtk_widget_set_tooltip_text (chk_sp, _("Whether the password shall be saved to disk."));
  gtk_box_pack_start (GTK_BOX (hbox10), chk_aj,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX (hbox10), chk_cu,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX (hbox10), chk_sp,TRUE,TRUE,0);

  GtkWidget* hbox14 = gtk_hbox_new (TRUE, 1);
  GtkWidget* chk_tray = gtk_check_button_new_with_label (_("Show tray icon"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_tray), data->window->settings.showtray);
  GtkWidget* chk_jmp = gtk_check_button_new_with_label (_("Jump to unread PMs"));
  gtk_widget_set_tooltip_text (chk_jmp, _("Whether the focus should jump to tabs with unread PMs or not."));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_jmp), data->window->settings.jumptab);
  gtk_box_pack_start (GTK_BOX (hbox14), chk_tray,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX (hbox14), chk_jmp,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX (hbox14), gtk_vbox_new (FALSE,0),TRUE,TRUE,0);

  GtkWidget* hbox8 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_notify = gtk_label_new (_("Audio notifications:"));
  GtkWidget* entry_notify = gtk_combo_box_entry_new_text ();
  gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_notify), 0, "<native>");
  gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_notify), 1, "<none>");
  if (g_strcmp0 (data->window->settings.notify, "<native>") == 0)
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_notify), 0);
  else if (g_strcmp0 (data->window->settings.notify, "<none>") == 0)
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_notify), 1);
  else
  {
    gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_notify), 2, data->window->settings.notify);
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_notify), 2);
  }
  gtk_widget_set_tooltip_text (entry_notify, _("This notification will be played on keyword match or PNs. <native> is a built-in audio notification, <none> deactivates it.\nYou can also specify custom commands to execute."));

  GtkWidget* label_vnotify = gtk_label_new (_("Visual Notifications:"));
  GtkWidget* entry_vnotify = gtk_combo_box_entry_new_text ();
  gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_vnotify), 0, "<native>");
  gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_vnotify), 1, "<none>");
  if (g_strcmp0 (data->window->settings.vnotify, "<native>") == 0)
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_vnotify), 0);
  else if (g_strcmp0 (data->window->settings.vnotify, "<none>") == 0)
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_vnotify), 1);
  else
  {
    gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_vnotify), 2, data->window->settings.vnotify);
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_vnotify), 2);
  }
  gtk_widget_set_tooltip_text (entry_vnotify, _("This notification will be played on keyword match or PNs. <native> is a built-in visual notification, <none> deactivates it.\nYou can also specify custom commands to execute."));


  gtk_box_pack_start (GTK_BOX (hbox8), label_notify,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox8), entry_notify,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX (hbox8), label_vnotify,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox8), entry_vnotify,TRUE,TRUE,0);

  GtkWidget* hbox9 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_keywords = gtk_label_new (_("Beep on keywords:"));
  GtkWidget* entry_keywords = gtk_entry_new ();
  gtk_widget_set_tooltip_text (entry_keywords, _("List of words which will trigger a notification, separated by | (u007C, vertical line)"));

  GSList* tmp_kw = data->window->settings.keywords;
  gchar* keywords_string = "";
  if (tmp_kw)
  {
    keywords_string = g_strconcat (keywords_string, (gchar*) tmp_kw->data, NULL);

    tmp_kw = tmp_kw->next;

    while (tmp_kw)
    {
      keywords_string = g_strconcat (keywords_string, "|", (gchar*) tmp_kw->data, NULL);
      tmp_kw = tmp_kw->next;
    }
  }
  gtk_entry_set_text (GTK_ENTRY(entry_keywords), keywords_string);
  if (g_strcmp0 (keywords_string, "") != 0)
    g_free (keywords_string);

  GtkWidget* label_browser = gtk_label_new (_("Browser:"));
  GtkWidget* entry_browser = gtk_combo_box_entry_new_text ();
  gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_browser), 0, "<native>");
  gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_browser), 1, "<none>");
  if (g_strcmp0 (data->window->settings.browser, "<native>") == 0)
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_browser), 0);
  else if (g_strcmp0 (data->window->settings.browser, "<none>") == 0)
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_browser), 1);
  else
  {
    gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_browser), 2, data->window->settings.browser);
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_browser), 2);
  }
  gtk_widget_set_tooltip_text (entry_browser, _("Please note that <native> is only supported on Microsoft Windows®, Debian and Debian derivates. You can specify the uri with %s if it's *not* the last parameter (i.e.: weird-browser %s --some-option)."));

  gtk_box_pack_start (GTK_BOX (hbox9), label_keywords,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox9), entry_keywords,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX (hbox9), label_browser,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox9), entry_browser,TRUE,TRUE,0);

  GtkWidget* hbox11 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_update = gtk_label_new (_("Time between updates (in ms):"));
  GtkWidget* scale_update = gtk_spin_button_new_with_range (200.0, 2000.0, 50.0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (scale_update), data->window->settings.update_time);
  GtkWidget* label_avas = gtk_label_new (_("Avatar size:"));
  GtkWidget* scale_avas = gtk_spin_button_new_with_range (8.0, 80.0, 4.0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (scale_avas), data->window->settings.avatar_size);

  gtk_box_pack_start (GTK_BOX (hbox11), label_update,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (hbox11), scale_update,TRUE,TRUE,0);
  gtk_box_pack_end (GTK_BOX (hbox11), scale_avas,TRUE,TRUE,0);
  gtk_box_pack_end (GTK_BOX (hbox11), label_avas,FALSE,FALSE,0);


  GtkWidget* vbox2 = gtk_vbox_new (FALSE, 1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox6,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox10,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox14,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox2), gtk_hseparator_new (),FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox8,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox9,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox11,FALSE,FALSE,0);


  GtkWidget* hbox13 = gtk_hbox_new (TRUE, 1);
  GtkWidget* label_msg = gtk_label_new (_("Chatserver string: "));
  GtkWidget* entry_msg = gtk_combo_box_entry_new_text ();
  gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_msg), 0, "SovietServer");
  gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_msg), 1, "ChatServer");
  if (g_strcmp0 (data->window->settings.servername, "SovietServer") == 0)
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_msg), 0);
  else if (g_strcmp0 (data->window->settings.servername, "ChatServer") == 0)
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_msg), 1);
  else
  {
    gtk_combo_box_insert_text (GTK_COMBO_BOX (entry_msg), 2, data->window->settings.servername);
    gtk_combo_box_set_active (GTK_COMBO_BOX (entry_msg), 2);
  }
  gtk_box_pack_start (GTK_BOX (hbox13), label_msg, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox13), entry_msg, FALSE, FALSE, 0);

#ifdef INGAME
  GtkWidget* label_tc = gtk_label_new (_("Terran Conflict folder:"));
  GtkWidget* entry_tc = gtk_file_chooser_button_new ("Select the Terran Conflice folder", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
  gtk_widget_set_tooltip_text (entry_tc, _("Enter the full path to the TC folder, if you want to use the ingame client."));
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (entry_tc), data->window->settings.TCFolder);
  gtk_box_pack_start (GTK_BOX (hbox13), label_tc, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox13), entry_tc, FALSE, FALSE, 0);
#else
  gtk_box_pack_start (GTK_BOX (hbox13), gtk_label_new (NULL), FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox13), gtk_label_new (NULL), FALSE, FALSE, 0);
#endif

  gtk_box_pack_start (GTK_BOX (vbox2), hbox13,FALSE,FALSE,0);

  GtkWidget* hbox_proxy = gtk_hbox_new (FALSE, 1);
  GtkWidget* label_proxy = gtk_label_new (_("Proxy settings: "));
  GtkWidget* entry_proxy = gtk_entry_new ();
  gtk_widget_set_tooltip_text (entry_proxy, _("Only HTTP-proxies are supported. Format is http://[username[:password]@]proxy-name[:Port]."));
  gtk_box_pack_start (GTK_BOX (hbox_proxy), label_proxy, FALSE, FALSE, 4);
  gtk_box_pack_start (GTK_BOX (hbox_proxy), entry_proxy, TRUE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (vbox2), hbox_proxy,FALSE,FALSE,0);

  gtk_notebook_append_page (GTK_NOTEBOOK (nb), vbox2, gtk_label_new (_("Misc")));

  GtkWidget* entry_preset[10];
  GtkWidget* label_preset[10];
  GtkWidget* hbox_preset[5];

  GtkWidget* vbox_preset = gtk_vbox_new (TRUE,1);

  gint i_p;

  for (i_p = 0; i_p < 5; i_p++)
  {
    label_preset[i_p] = gtk_label_new (g_strdup_printf (_("Preset text %i:"), i_p+1));
    entry_preset[i_p] = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY (entry_preset[i_p]), data->window->settings.presets[i_p]);
    label_preset[i_p+5] = gtk_label_new (g_strdup_printf (_("Preset text %i:"), i_p+6));
    entry_preset[i_p+5] = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY (entry_preset[i_p+5]), data->window->settings.presets[i_p+5]);
    hbox_preset[i_p] = gtk_hbox_new (FALSE, 1);
    gtk_box_pack_start (GTK_BOX (hbox_preset[i_p]), label_preset[i_p], FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox_preset[i_p]), entry_preset[i_p], TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (hbox_preset[i_p]), label_preset[i_p+5], FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox_preset[i_p]), entry_preset[i_p+5], TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox_preset), hbox_preset[i_p], FALSE, FALSE, 0);
  }

  gtk_notebook_append_page (GTK_NOTEBOOK (nb), vbox_preset, gtk_label_new (_("Preset texts")));


  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), nb, TRUE, TRUE, 1);


  GtkWidget* chk_reset_1 = gtk_check_button_new_with_label (_("Reset color"));
  GtkWidget* chk_reset_2 = gtk_check_button_new_with_label (_("Reset Misc"));
  GtkWidget* chk_reset_3 = gtk_check_button_new_with_label (_("Reset preset texts"));

  GtkWidget* hboxBB = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hboxBB), chk_reset_1, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hboxBB), chk_reset_2, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hboxBB), chk_reset_3, FALSE, FALSE, 0);

  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), hboxBB, FALSE, FALSE, 1);

  gtk_widget_show_all (dialog);

  gint result = gtk_dialog_run (GTK_DIALOG (dialog));

  GdkColor color_time, color_font, color_l1, color_l3, color_l5, color_l6, color_greens, color_blues, color_url,
           color_url_visited, color_url_hover, color_highlight, color_gold, color_green, color_blue, color_yellow,
           color_magenta, color_cyan, color_red;

  switch (result)
  {
    case GTK_RESPONSE_REJECT:
      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_reset_1)))
      {
        g_object_set (data->window, "color_time", data->window->settings_backup.color_time,
                                    "color_font", data->window->settings_backup.color_font,
                                    "color_l1", data->window->settings_backup.color_l1,
                                    "color_l3", data->window->settings_backup.color_l3,
                                    "color_l5", data->window->settings_backup.color_l5,
                                    "color_l6", data->window->settings_backup.color_l6,
                                    "color_goldies", data->window->settings_backup.color_goldies,
                                    "color_greens", data->window->settings_backup.color_greens,
                                    "color_blues", data->window->settings_backup.color_blues,
                                    "color_url", data->window->settings_backup.color_url,
                                    "color_url_visited", data->window->settings_backup.color_url_visited,
                                    "color_url_hover", data->window->settings_backup.color_url_hover,
                                    "color_highlight", data->window->settings_backup.color_highlight,
                                    "color_green", data->window->settings_backup.color_green,
                                    "color_red", data->window->settings_backup.color_red,
                                    "color_blue", data->window->settings_backup.color_blue,
                                    "color_cyan", data->window->settings_backup.color_cyan,
                                    "color_yellow", data->window->settings_backup.color_yellow,
                                    "color_magenta", data->window->settings_backup.color_magenta,
                                    NULL);
      }
      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_reset_2)))
      {
        g_object_set (data->window, "showid", data->window->settings_backup.showid,
                                    "showhidden", data->window->settings_backup.showhidden,
                                    "autojoin", data->window->settings_backup.autojoin,
                                    "stealthjoin", data->window->settings_backup.stealthjoin,
                                    "coloruser", data->window->settings_backup.coloruser,
                                    "browser", data->window->settings_backup.browser,
                                    "notify", data->window->settings_backup.notify,
                                    "vnotify", data->window->settings_backup.vnotify,
                                    "trayicon", data->window->settings_backup.showtray,
                                    NULL);
        g_slist_free (data->window->settings.keywords);
        data->window->settings.keywords = NULL;
        data->window->settings.proxy = "";
        data->window->settings.update_time = data->window->settings_backup.update_time;
        data->window->settings.avatar_size = data->window->settings_backup.avatar_size;
        data->window->settings.store_pass = data->window->settings_backup.store_pass;
        data->window->settings.servername = data->window->settings_backup.servername;
        data->window->settings.jumptab = data->window->settings_backup.jumptab;
      }

      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_reset_3)))
      {
        for (i_p=0;i_p<10;i_p++)
        {
          if (g_strcmp0 ("", data->window->settings.presets[i_p]) != 0)
            g_free (data->window->settings.presets[i_p]);
          data->window->settings.presets[i_p] = "";
        }
      }
      break;
    case GTK_RESPONSE_OK:
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_time), &color_time);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_font), &color_font);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_l1), &color_l1);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_l3), &color_l3);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_l5), &color_l5);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_l6), &color_l6);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_green), &color_greens);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_blue), &color_blues);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_url), &color_url);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_vurl), &color_url_visited);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_hurl), &color_url_hover);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_high), &color_highlight);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_gold), &color_gold);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_greenX), &color_green);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_blueX), &color_blue);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_yellow), &color_yellow);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_magenta), &color_magenta);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_cyan), &color_cyan);
      gtk_color_button_get_color (GTK_COLOR_BUTTON (btn_col_red), &color_red);
      g_object_set (data->window, "color_time", gdk_color_to_string (&color_time),
                                  "color_font", gdk_color_to_string (&color_font),
                                  "color_l1", gdk_color_to_string (&color_l1),
                                  "color_l3", gdk_color_to_string (&color_l3),
                                  "color_l5", gdk_color_to_string (&color_l5),
                                  "color_l6", gdk_color_to_string (&color_l6),
                                  "color_goldies", gdk_color_to_string (&color_gold),
                                  "color_greens", gdk_color_to_string (&color_greens),
                                  "color_blues", gdk_color_to_string (&color_blues),
                                  "color_url", gdk_color_to_string (&color_url),
                                  "color_url_visited", gdk_color_to_string (&color_url_visited),
                                  "color_url_hover", gdk_color_to_string (&color_url_hover),
                                  "color_highlight", gdk_color_to_string (&color_highlight),
                                  "showid", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_id)),
                                  "showhidden", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_hd)),
                                  "autojoin", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_aj)),
                                  "stealthjoin", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_sj)),
                                  "coloruser", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_cu)),
                                  "browser", gtk_combo_box_get_active_text (GTK_COMBO_BOX (entry_browser)),
                                  "notify", gtk_combo_box_get_active_text (GTK_COMBO_BOX (entry_notify)),
                                  "vnotify", gtk_combo_box_get_active_text (GTK_COMBO_BOX (entry_vnotify)),
                                  "trayicon", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_tray)),
                                  "color_green", gdk_color_to_string (&color_green),
                                  "color_red", gdk_color_to_string (&color_red),
                                  "color_blue", gdk_color_to_string (&color_blue),
                                  "color_cyan", gdk_color_to_string (&color_cyan),
                                  "color_yellow", gdk_color_to_string (&color_yellow),
                                  "color_magenta", gdk_color_to_string (&color_magenta),
                                  NULL);

      data->window->settings.proxy = g_strdup (gtk_entry_get_text(GTK_ENTRY(entry_proxy)));
      g_object_set (data->window->session, SOUP_SESSION_PROXY_URI, soup_uri_new (data->window->settings.proxy), NULL);
    #ifdef INGAME
      data->window->settings.TCFolder = g_strdup (gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (entry_tc)));
    #endif

      gchar** keywords = g_strsplit (gtk_entry_get_text(GTK_ENTRY(entry_keywords)), "|", 0);
      gint i;
      g_slist_free (data->window->settings.keywords);
      data->window->settings.keywords = NULL;
      for (i = 0; keywords[i] != NULL; i++)
        data->window->settings.keywords = g_slist_append (data->window->settings.keywords, g_strdup(keywords[i]));
      g_strfreev (keywords);

      for (i_p = 0;i_p < 10; i_p++)
        data->window->settings.presets[i_p] = g_strdup (gtk_entry_get_text (GTK_ENTRY (entry_preset[i_p])));

      data->window->settings.update_time = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (scale_update));
      data->window->settings.avatar_size = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (scale_avas));
      data->window->settings.store_pass = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_sp));
      data->window->settings.servername = gtk_combo_box_get_active_text (GTK_COMBO_BOX (entry_msg));
      data->window->settings.jumptab = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_jmp));
      devchat_window_refresh_presets (data->window);
      save_settings (data->window);
    break;
    default: break;
  }
  gtk_widget_destroy (dialog);
}

void find (GtkWidget* widget, DevchatCBData* data)
{
  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkWidget* bar;
  GtkWidget* entry;
  GtkWidget* button;
  GtkWidget* orig_button;
  gboolean start_set;

  if (pagenum == 0)
  {
    bar = data->window->searchbar;
    entry = data->window->search_entry;
    button = data->window->search_button;
    orig_button = data->window->btn_send;
    start_set = data->window->search_start_set;
  }
  else
  {
    const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
    DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
    bar = conv->searchbar;
    button = conv->search_button;
    entry = conv->search_entry;
    orig_button = conv->btn_send;
    start_set = conv->search_start_set;
  }

  gboolean is_visible;

  g_object_get (bar, "visible", &is_visible, NULL);

  if (!is_visible)
  {
    gtk_widget_set_no_show_all (bar, FALSE);
    gtk_widget_show_all (bar);
    gtk_widget_grab_focus (entry);
    gtk_entry_set_icon_from_stock (GTK_ENTRY (entry), GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_FIND);

    gtk_widget_remove_accelerator (orig_button, data->window->accelgroup, GDK_Return, 0);
    gtk_widget_remove_accelerator (orig_button, data->window->accelgroup, GDK_KP_Enter, 0);
    gtk_widget_add_accelerator (button, "clicked", data->window->accelgroup, GDK_Return, 0, 0);
    gtk_widget_add_accelerator (button, "clicked", data->window->accelgroup, GDK_KP_Enter, 0, 0);

    start_set = FALSE;
  }
  else
  {
    gtk_widget_remove_accelerator (button, data->window->accelgroup, GDK_Return, 0);
    gtk_widget_remove_accelerator (button, data->window->accelgroup, GDK_KP_Enter, 0);
    gtk_widget_add_accelerator(orig_button, "clicked", data->window->accelgroup, GDK_Return, 0, 0);
    gtk_widget_add_accelerator(orig_button, "clicked", data->window->accelgroup, GDK_KP_Enter, 0, 0);
    gtk_widget_hide_all (bar);
    gtk_widget_set_no_show_all (bar, TRUE);
    start_set = FALSE;
  }
}

void devchat_window_find (GtkWidget* widget, DevchatCBData* data)
{
  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  DevchatConversation* conv;
  GtkWidget* entry = GTK_WIDGET (data->data);
  GtkTextBuffer* buf;
  GtkWidget* view;
  GtkTextIter start;
  gboolean start_set;

  if (pagenum == 0)
  {
    buf = data->window->output;
    view = data->window->outputwidget;
    entry = data->window->search_entry;
    start = data->window->search_start;
    start_set = data->window->search_start_set;
    conv = NULL;
  }
  else
  {
    const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
    conv = g_hash_table_lookup (data->window->conversations, target);
    buf = conv->out_buffer;
    view = conv->out_widget;
    entry = conv->search_entry;
    start = conv->search_start;
    start_set = conv->search_start_set;
  }

  gtk_entry_set_icon_from_stock (GTK_ENTRY (entry), GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_FIND);

  const gchar* searchtext = gtk_entry_get_text (GTK_ENTRY (entry));

  GtkTextIter end;
  if (!start_set)
  {
    gtk_text_buffer_get_start_iter (buf, &start);
    start_set = TRUE;
  }

  if (gtk_text_iter_forward_search (&start, searchtext, GTK_TEXT_SEARCH_VISIBLE_ONLY | GTK_TEXT_SEARCH_TEXT_ONLY, &start, &end, NULL))
  {
    gtk_text_buffer_move_mark (buf, gtk_text_buffer_get_insert (buf), &start);
    gtk_text_buffer_move_mark_by_name (buf, "selection_bound", &end);
    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (view), gtk_text_buffer_get_insert (buf));
  }
  else
  {
    gtk_entry_set_icon_from_stock (GTK_ENTRY (entry), GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_STOP);
    start_set = FALSE;
  }

  gtk_text_iter_forward_chars (&start, strlen (searchtext));

  if (pagenum == 0)
  {
    data->window->search_start = start;
    data->window->search_start_set = start_set;
  }
  else
  {
    conv->search_start_set = start_set;
    conv->search_start = start;
  }
}

void devchat_window_close_search (GtkWidget* widget, DevchatCBData* data)
{
  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkWidget* orig_button;
  GtkWidget* button;

  if (pagenum == 0)
  {
    data->window->search_start_set = FALSE;
    orig_button = data->window->btn_send;
    button = data->window->search_button;
  }
  else
  {
    const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
    DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
    conv->search_start_set = FALSE;
    orig_button = conv->btn_send;
    button = conv->search_button;
  }

  gtk_widget_remove_accelerator (button, data->window->accelgroup, GDK_Return, 0);
  gtk_widget_remove_accelerator (button, data->window->accelgroup, GDK_KP_Enter, 0);
  gtk_widget_add_accelerator(orig_button, "clicked", data->window->accelgroup, GDK_Return, 0, 0);
  gtk_widget_add_accelerator(orig_button, "clicked", data->window->accelgroup, GDK_KP_Enter, 0, 0);

  gtk_widget_hide_all (data->data);
  gtk_widget_set_no_show_all (data->data, TRUE);
}

void go_forum(GtkWidget* widget, DevchatCBData* data)
{
  gchar* url = g_strdup ("http://forum.egosoft.com/");
  switch (GPOINTER_TO_INT (data->data))
  {
    case -255: url = g_strconcat (url, "profile.php?mode=editprofile", NULL); break;
    case -5: url = g_strconcat (url, "index.php?c=16", NULL); break;
    case -3: url = g_strconcat (url, "index.php?c=9", NULL); break;
    case -1: url = g_strconcat (url, "index.php", NULL); break;
    default: url = g_strdup_printf ("%sprofile.php?mode=viewprofile&u=%i", url, GPOINTER_TO_INT (data->data)); break;
  }

  if (debug) {
    dbg_msg = g_strdup_printf ("URL to open: %s\n", url);
    dbg (dbg_msg);
    g_free (dbg_msg);
  }

  launch_browser (NULL, url, data);
  g_free (url);
}

void devchat_window_close_tab(GtkWidget* widget, DevchatCBData* data)
{
  const gchar* target_name;
  GtkWidget* notebook_child;
  if (data->data)
  {
    notebook_child = data->data;
  }
  else
  {
    if (gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook)) != 0)
      notebook_child = gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook)));
    else
      return;
  }
  target_name = g_strdup (gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), notebook_child));

  gtk_widget_hide_all (notebook_child);
}

void reconnect(GtkWidget* widget, DevchatCBData* data)
{
  data->window->errorcount = 0;
  if (debug)
    dbg ("Killing soup session... WITH A SPOON.\n(Killing soup with a spoon, get it? Oh, the wit... *ahem* Sorry, I'll continue.)\n");

  data->window->firstrun = TRUE;
  data->window->hovertag = NULL;
  soup_session_abort (data->window->session);
  g_source_remove (data->window->msg_list_getter);
  g_source_remove (data->window->usr_list_getter);
  data->window->session = soup_session_async_new ();
  soup_session_add_feature (data->window->session, SOUP_SESSION_FEATURE(soup_cookie_jar_new()));
  login (NULL, data);
}

void tab_changed (GtkWidget* widget, GtkNotebook* nb, guint pagenum, DevchatCBData* data)
{
  if (pagenum > 0)
  {
    GtkLabel* l = GTK_LABEL (gtk_notebook_get_menu_label (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum)));
    gtk_label_set_markup (l, gtk_label_get_text (l));
  }
}

gboolean devchat_window_tab_changed_win (GtkWidget* widget, GdkEvent* ev, DevchatCBData* data)
{
  guint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkWidget* w;

  if (pagenum == 0)
  {
    w = data->window->inputwidget;
  }
  else
  {
    const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
    DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
    w = conv->in_widget;
    tab_changed (NULL, GTK_NOTEBOOK (data->window->notebook), pagenum, data);
  }

  if (w)
  {
    gtk_widget_grab_focus (w);
  }

  return FALSE;
}

gboolean devchat_window_on_motion_cb (GtkWidget* widget, GdkEventMotion* m, DevchatCBData* data)
{
  gint buf_x;
  gint buf_y;

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (widget), GTK_TEXT_WINDOW_TEXT, m->x, m->y, &buf_x, &buf_y);

  GtkTextIter iter;
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (widget), &iter, buf_x, buf_y);

  gboolean found = FALSE;

  GSList* tag = gtk_text_iter_get_tags (&iter);

  while (tag && !(found))
  {
    gchar* name;
    g_object_get (tag->data, "name", &name, NULL);

    if (name && g_str_has_prefix (name, "img::"))
    {
      gtk_widget_set_tooltip_text (widget, name+5);
      found = TRUE;
      gtk_statusbar_pop (GTK_STATUSBAR (data->window->statusbar),
                         gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "img")
                        );
      gtk_statusbar_push (GTK_STATUSBAR (data->window->statusbar),
                          gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "img"),
                          name+5
                         );
    }
    else if (name && g_str_has_prefix (name, "url::"))
    {
      gtk_widget_set_tooltip_text (widget, name+5);
      g_object_set (tag->data, "foreground", data->window->settings.color_url_hover, NULL);
      data->window->hovertag = tag->data;
      found = TRUE;
      gtk_statusbar_pop (GTK_STATUSBAR (data->window->statusbar),
                         gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "link")
                        );
      gtk_statusbar_push (GTK_STATUSBAR (data->window->statusbar),
                          gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "link"),
                          name+5
                         );
    }
    else if (name && g_str_has_prefix (name, "lid::"))
    {
      gtk_widget_set_tooltip_text (widget, name+5);
      found = TRUE;
      gtk_statusbar_pop (GTK_STATUSBAR (data->window->statusbar),
                         gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "lid")
                        );
      gtk_statusbar_push (GTK_STATUSBAR (data->window->statusbar),
                          gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "lid"),
                          name+5
                         );
    }
    else if (name && g_str_has_prefix (name, "tma::"))
    {
      gtk_widget_set_tooltip_text (widget, name+5);
      found = TRUE;
      gtk_statusbar_pop (GTK_STATUSBAR (data->window->statusbar),
                         gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "tma")
                        );
      gtk_statusbar_push (GTK_STATUSBAR (data->window->statusbar),
                          gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "tma"),
                          name+5
                         );
    }

    tag = tag->next;
  }

  if (!found)
  {
    gtk_widget_set_has_tooltip (widget, FALSE);
    gtk_statusbar_pop (GTK_STATUSBAR (data->window->statusbar),
                       gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "link")
                      );
    gtk_statusbar_pop (GTK_STATUSBAR (data->window->statusbar),
                       gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "img")
                      );
    gtk_statusbar_pop (GTK_STATUSBAR (data->window->statusbar),
                       gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "lid")
                      );
    gtk_statusbar_pop (GTK_STATUSBAR (data->window->statusbar),
                       gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "tma")
                      );
    if (data->window->hovertag)
    {
      gboolean visited;
      g_object_get (data->window->hovertag, "visited", &visited, NULL);

      g_object_set (data->window->hovertag, "foreground", visited? data->window->settings.color_url_visited:data->window->settings.color_url, NULL);
      data->window->hovertag = NULL;
    }
  }

  return FALSE;
}

void devchat_window_on_mark_set_cb (GtkTextBuffer* buffer, GtkTextIter* iter, GtkTextMark* mark, DevchatCBData* data)
{
  if (g_strcmp0 (gtk_text_mark_get_name (mark), "selection_bound") == 0)
  {
    GSList* tag = gtk_text_iter_get_tags (iter);

    while (tag)
    {
      gchar* name;
      g_object_get (tag->data, "name", &name, NULL);

      if (name && g_str_has_prefix (name, "url::"))
      {
      #ifndef G_OS_WIN32
        gchar* uri = g_shell_quote (name+5);
      #else
        gchar* uri = name+5;
      #endif

        if (debug)
          dbg_msg = g_strdup_printf ("Quoted URI: %s\n", uri);

        launch_browser (NULL, uri, data);
        g_free (uri);

        g_object_set (tag->data, "visited", TRUE, NULL);
        g_object_set (tag->data, "foreground", data->window->settings.color_url_visited, NULL);
      }
      tag = tag->next;
    }
  }
}

void launch_browser (GtkWidget* fnord, gchar* uri, DevchatCBData* data)
{
  if (g_strcmp0 (data->window->settings.browser,"<native>") == 0)
  {
  #ifdef G_OS_WIN32
    ShellExecute (NULL, "open", uri, NULL, NULL, SW_SHOWNORMAL);
  #else
    if (!g_file_test ("/usr/bin/x-www-browser", G_FILE_TEST_EXISTS))
    {
      err (_("Error B: Your system doesn't have a native browser!"));
      data->window->settings.browser = "<none>";
    }
    else
    {
      gchar* commandline = g_strdup_printf ("/usr/bin/x-www-browser %s", uri);
      g_spawn_command_line_async (commandline, NULL);
      g_free (commandline);
    }
  #endif
  }
  else if (g_strcmp0 (data->window->settings.browser,"<none>") != 0)
  {
    gchar* commandline;
    if (g_strstr_len (data->window->settings.browser, -1, "%s"))
      commandline = g_strdup_printf (data->window->settings.browser, uri);
    else
      commandline = g_strconcat (data->window->settings.browser," ", uri, NULL);
    g_spawn_command_line_async (commandline, NULL);
    g_free (commandline);
  }
}

void devchat_window_filter_ul_changed (GtkWidget* widget, GtkTextBuffer* data)
{
  GtkTextTagTable* t = gtk_text_buffer_get_tag_table (data);
  GtkTextTag* tag;

  tag = gtk_text_tag_table_lookup (t, "ul1");
  g_object_set (tag, "invisible-set", FALSE, NULL);
  tag = gtk_text_tag_table_lookup (t, "ul3");
  g_object_set (tag, "invisible-set", FALSE, NULL);
  tag = gtk_text_tag_table_lookup (t, "ul5");
  g_object_set (tag, "invisible-set", FALSE, NULL);

  switch (gtk_combo_box_get_active (GTK_COMBO_BOX (widget)))
  {
    case 3: tag = gtk_text_tag_table_lookup (t, "ul5");
            g_object_set (tag, "invisible", TRUE, NULL);
    case 2: tag = gtk_text_tag_table_lookup (t, "ul3");
            g_object_set (tag, "invisible", TRUE, NULL);
    case 1: tag = gtk_text_tag_table_lookup (t, "ul1");
            g_object_set (tag, "invisible", TRUE, NULL);
    default: break;
  }
}

void devchat_window_filter_ml_changed (GtkWidget* widget, GtkTextBuffer* data)
{
  GtkTextTagTable* t = gtk_text_buffer_get_tag_table (data);
  GtkTextTag* tag;

  tag = gtk_text_tag_table_lookup (t, "l1");
  g_object_set (tag, "invisible-set", FALSE, NULL);
  g_object_set (tag, "paragraph-background-set", TRUE, NULL);
  tag = gtk_text_tag_table_lookup (t, "l3");
  g_object_set (tag, "invisible-set", FALSE, NULL);
  g_object_set (tag, "paragraph-background-set", TRUE, NULL);
  tag = gtk_text_tag_table_lookup (t, "l5");
  g_object_set (tag, "invisible-set", FALSE, NULL);
  g_object_set (tag, "paragraph-background-set", TRUE, NULL);

  switch (gtk_combo_box_get_active (GTK_COMBO_BOX (widget)))
  {
    case 3: tag = gtk_text_tag_table_lookup (t, "l5");
            g_object_set (tag, "invisible", TRUE, NULL);
            g_object_set (tag, "paragraph-background-set", FALSE, NULL);
    case 2: tag = gtk_text_tag_table_lookup (t, "l3");
            g_object_set (tag, "invisible", TRUE, NULL);
            g_object_set (tag, "paragraph-background-set", FALSE, NULL);
    case 1: tag = gtk_text_tag_table_lookup (t, "l1");
            g_object_set (tag, "invisible", TRUE, NULL);
            g_object_set (tag, "paragraph-background-set", FALSE, NULL);
    default: break;
  }
}

void level_changed (GtkWidget* widget, DevchatCBData* data)
{
  if (data->window->userlevel > 1)
  {
    GdkColor new_bg;
    if (gtk_combo_box_get_active (GTK_COMBO_BOX (widget)) == 0)
    {
      gdk_color_parse (data->window->settings.color_l1, &new_bg);
      gtk_widget_modify_base (data->window->inputwidget, GTK_STATE_NORMAL, &new_bg);
    }
    else if (gtk_combo_box_get_active (GTK_COMBO_BOX (widget)) == 1)
    {
      gdk_color_parse (data->window->settings.color_l3, &new_bg);
      gtk_widget_modify_base (data->window->inputwidget, GTK_STATE_NORMAL, &new_bg);
    }
    else if (gtk_combo_box_get_active (GTK_COMBO_BOX (widget)) == 2)
    {
      gdk_color_parse (data->window->settings.color_l5, &new_bg);
      gtk_widget_modify_base (data->window->inputwidget, GTK_STATE_NORMAL, &new_bg);
    }
    else if (gtk_combo_box_get_active (GTK_COMBO_BOX (widget)) == 3)
    {
      gdk_color_parse (data->window->settings.color_l6, &new_bg);
      gtk_widget_modify_base (data->window->inputwidget, GTK_STATE_NORMAL, &new_bg);
    }
  }
}

/*
  Target: non-null for PMs.
  level: non-zero for non-PMs.
*/
void devchat_window_text_send (DevchatCBData* data, gchar* text, gchar* target, gchar* sendlevel, gboolean raw)
{
  const int smilie_count = 19;

  GRegex* custom_smilies[smilie_count];

  custom_smilies[0] = g_regex_new (":cube:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[1] = g_regex_new (":ugly:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[2] = g_regex_new (":fp:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[3] = g_regex_new (":wub:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[4] = g_regex_new (":keks:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[5] = g_regex_new (":eg:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[6] = g_regex_new (":giggle:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[7] = g_regex_new (":shocked:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[8] = g_regex_new (":confused:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[9] = g_regex_new (":mad:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[10] = g_regex_new (":cry:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[11] = g_regex_new (":evil:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[12] = g_regex_new (":twisted:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[13] = g_regex_new (":idea:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[14] = g_regex_new (":arrow:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[15] = g_regex_new (":think:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[16] = g_regex_new (":doh:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[17] = g_regex_new (":rant:", G_REGEX_UNGREEDY, 0, NULL);
  custom_smilies[18] = g_regex_new (":star:", G_REGEX_UNGREEDY, 0, NULL);

  gchar* custom_smilie_replacements[smilie_count];

  custom_smilie_replacements[0] = "[img]http://dl.creshal.de/dc/cube.png[/img]";
  custom_smilie_replacements[1] = "[img]http://dl.creshal.de/dc/ugly.gif[/img]";
  custom_smilie_replacements[2] = "[img]http://dl.creshal.de/dc/fp1.gif[/img]";
  custom_smilie_replacements[3] = "[img]http://dl.creshal.de/dc/wub.gif[/img]";
  custom_smilie_replacements[4] = "[img]http://dl.creshal.de/dc/atomkeks.png[/img]";
  custom_smilie_replacements[5] = "[img]http://dl.creshal.de/dc/icon_evillaugh.gif[/img]";
  custom_smilie_replacements[6] = "[img]http://dl.creshal.de/dc/giggle.gif[/img]";
  custom_smilie_replacements[7] = "[img]http://forum.egosoft.com/images/smiles/icon_eek.gif[/img]";
  custom_smilie_replacements[8] = "[img]http://forum.egosoft.com/images/smiles/icon_confused.gif[/img]";
  custom_smilie_replacements[9] = "[img]http://forum.egosoft.com/images/smiles/icon_mad.gif[/img]";
  custom_smilie_replacements[10] = "[img]http://forum.egosoft.com/images/smiles/icon_cry.gif[/img]";
  custom_smilie_replacements[11] = "[img]http://forum.egosoft.com/images/smiles/icon_evil.gif[/img]";
  custom_smilie_replacements[12] = "[img]http://forum.egosoft.com/images/smiles/icon_twisted.gif[/img]";
  custom_smilie_replacements[13] = "[img]http://forum.egosoft.com/images/smiles/icon_idea.gif[/img]";
  custom_smilie_replacements[14] = "[img]http://forum.egosoft.com/images/smiles/icon_arrow.gif[/img]";
  custom_smilie_replacements[15] = "[img]http://forum.egosoft.com/images/smiles/icon_think.gif[/img]";
  custom_smilie_replacements[16] = "[img]http://forum.egosoft.com/images/smiles/icon_doh.gif[/img]";
  custom_smilie_replacements[17] = "[img]http://forum.egosoft.com/images/smiles/icon_rant.gif[/img]";
  custom_smilie_replacements[18] = "[img]http://forum.egosoft.com/images/smiles/icon_star.gif[/img]";

  int i;
  for (i=0; i < smilie_count; i++)
  {
    gchar* tmp2 = text;
    text = g_regex_replace (custom_smilies[i], text, -1, 0, custom_smilie_replacements[i], 0, NULL);
    g_free (tmp2);
  }

  if (target && g_strcmp0 (sendlevel, "0") == 0)
    text = g_strconcat ("/msg ", target, " ", text, NULL);


  gchar* enc_text = "";

  guchar current[2];
  current[0] = 32;
  current[1] = 0;

  gint max = strlen (text);

  for (i=0; i < max; i++)
  {
    current[0] = text[i];

    if (real_debug) {
      dbg_msg = g_strdup_printf ("Current char: %i\n", current[0]);
      dbg (dbg_msg);
      g_free (dbg_msg);
    }

    /*Allowed: 45, 46, 48-57, 65-90, 95, 97-122*/
    if (current[0] == 45 || current[0] == 46
        || (current[0] > 47 && current[0] < 58) || (current[0] > 64 && current[0] < 91) || current[0] == 95
        || (current[0] > 96 && current[0] < 123))
    {
      enc_text = g_strconcat (enc_text, current, NULL);
    }
    else if (current[0] > 31 && current[0] < 128)
    {
      /*Restricted char, but valid ASCII. %escape*/
      if (current[0] == 43)
        enc_text = g_strconcat (enc_text, "%26%2343%3B", NULL);
      else if (current[0] == 60)
        if (!raw)
          enc_text = g_strconcat (enc_text, "%26%2360%3B", NULL);
        else
          enc_text = g_strdup_printf ("%s%%%X", enc_text, current[0]);
      else if (current[0] == 62)
        if (!raw)
          enc_text = g_strconcat (enc_text, "%26%2362%3B", NULL);
        else
          enc_text = g_strdup_printf ("%s%%%X", enc_text, current[0]);
      else
        enc_text = g_strdup_printf ("%s%%%X", enc_text, current[0]);
    }
    else if (current[0] > 193 && current[0] < 245)
    {
      /*UTF8 char start. Use g_utf8_get_char to get the real char, insert as %uxxxx. Illegal by RFC and W3C, but if the server wants it...*/

      enc_text = g_strdup_printf ("%s%%u%.4X", enc_text, g_utf8_get_char (text+i));

      i++;
      if (current[0] > 223)
      {
        i++;
        if (current[0] > 239)
          i++;
      }
    }
    else if (current[0] == 10)
    {
      enc_text = g_strconcat (enc_text, "%0D%0A", NULL);
    }
    else if (debug)
    {
      dbg ("Invalid char in sent text. Stop that!");
    }
  }

  if (debug) {
    dbg_msg = g_strdup_printf ("Parsed message: %s.\n", enc_text);
    dbg (dbg_msg);
    g_free (dbg_msg);
  }

  SoupMessage* post = soup_message_new("GET", g_strconcat ("http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?cmd=post&chatlevel=",sendlevel,"&textinput=", enc_text, NULL));
  soup_session_queue_message (data->window->session, post, SOUP_SESSION_CALLBACK (msg_sent_cb), devchat_cb_data_new (data->window, g_strconcat ("&chatlevel=",sendlevel,"&textinput=", enc_text, NULL)));

  g_free (text);
  g_free (enc_text);
}

void devchat_window_btn_send (GtkWidget* widget, DevchatCBData* data)
{
  if (debug)
    dbg ("Sending message...");

  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkTextBuffer* buf;
  gchar* text;
  GtkTextIter start;
  GtkTextIter end;
  GtkWidget* chk_raw;
  gchar* target = NULL;
  gchar* sendlevel;

  if (pagenum == 0)
  {
    buf = data->window->input;
    gtk_text_buffer_get_start_iter (buf, &start);
    gtk_text_buffer_get_end_iter (buf, &end);
    text = g_strdup (gtk_text_buffer_get_text (buf, &start, &end, FALSE));
    chk_raw = data->window->chk_raw;
    gint level = gtk_combo_box_get_active (GTK_COMBO_BOX (data->window->level_box));

    switch (level)
    {
      case -1:
      case 0: sendlevel = "1"; break;
      case 1: sendlevel = "3"; break;
      case 2: sendlevel = "5"; break;
      default: sendlevel = "6"; break;
    }
  }
  else
  {
    target = g_strdup (gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum)));
    DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
    buf = conv->in_buffer;
    chk_raw = conv->chk_raw;
    gtk_text_buffer_get_start_iter (buf, &start);
    gtk_text_buffer_get_end_iter (buf, &end);
    text = g_strdup (gtk_text_buffer_get_text (buf, &start, &end, FALSE));
    sendlevel = "0";
  }
  text = g_strstrip (text);

  if (g_strcmp0("",text) != 0)
  {
    gint i = MAX_BUF-1;

    if (g_strcmp0 (data->window->buffer[MAX_BUF], "") != 0)
      g_free (data->window->buffer[MAX_BUF]);
    /*Shamelessly copied from browser chat.*/
    while (i>0)
    {
      data->window->buffer[i+1] = data->window->buffer[i];
      i--;
    }
    data->window->buffer[1] = g_strdup (text);

    if (g_strcmp0 (data->window->buffer[0], "") != 0)
    {
      gtk_text_buffer_set_text (buf, data->window->buffer[0], -1);
      g_free(data->window->buffer[0]);
      data->window->buffer[0] = "";
    }
    else
    {
      GtkTextIter s, e;
      gtk_text_buffer_get_bounds (buf, &s, &e);
      gtk_text_buffer_delete (buf, &s, &e); /*_set_text (,"",) causes visual artefacts when SPELLCHECK is enabled*/
    }
    data->window->buf_current = 0;

  #ifdef OTR
    if (target)
    {
      gchar* new_text = NULL;
      if (otrl_message_sending (data->window->otr_state, &(data->window->otr_funcs), data->window, data->window->settings.username, "x-devchat", target, text, NULL, &new_text, NULL, NULL) != 0)
      {
        err (_("Error O: OTR encryption failed!"));
        return;
      }
      if (new_text)
      {
        g_free (text);
        text = new_text;
      }
    }
  #endif

    devchat_window_text_send (data, g_strdup (text), target, sendlevel, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_raw)));

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_raw), FALSE);
  }
#ifndef OTR
  g_free (text);
#else
  otrl_message_free (text);
#endif
}

void msg_sent_cb (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  if (m->status_code != 200)
  {
    err (_("Error M: Error sending message, trying to resend..."));

    SoupMessage* post = soup_message_new("GET", g_strconcat ("http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?cmd=post", (gchar*) data->data, NULL));
    if (soup_session_send_message (s, post) != 200)
      err (_("Error M2: Error sending message, retry failed. Aborting."));
  }
}

void devchat_window_btn_format (GtkWidget* widget, DevchatCBData* data)
{
  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkTextBuffer* buf;
  GtkWidget* w;
  GtkTextIter start;
  GtkTextIter end;

  if (pagenum == 0)
  {
    buf = data->window->input;
    w = data->window->inputwidget;
  }
  else
  {
    const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
    DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
    buf = conv->in_buffer;
    w = conv->in_widget;
  }

  if (gtk_text_buffer_get_selection_bounds (buf, &start, &end))
  {
    GtkTextMark* ed = gtk_text_buffer_create_mark (buf, "tmp", &end, TRUE);
    gchar* tag_open = g_strconcat ("[", (gchar*) data->data, "]", NULL);
    gtk_text_buffer_insert (buf, &start, tag_open, -1);
    g_free (tag_open);
    gtk_text_buffer_get_iter_at_mark (buf, &end, ed);
    gchar* tag_close = g_strconcat ("[/", (gchar*) data->data, "]", NULL);
    gtk_text_buffer_insert (buf, &end, tag_close, -1);
    g_free (tag_close);
    gtk_text_iter_backward_chars (&end, strlen((gchar*) data->data)+3);
    gtk_text_buffer_place_cursor (buf, &end);
    gtk_text_buffer_delete_mark (buf, ed);
  }
  else
  {
    gchar* tag = g_strconcat ("[", (gchar*) data->data, "][/", (gchar*) data->data, "]", NULL);
    gtk_text_buffer_insert_at_cursor (buf, tag, -1);
    g_free (tag);

    GtkTextIter cursor;

    gtk_text_buffer_get_iter_at_mark (buf, &cursor, gtk_text_buffer_get_insert (buf));
    gtk_text_iter_backward_chars (&cursor, strlen ((gchar*) data->data) + 3);
    gtk_text_buffer_move_mark (buf, gtk_text_buffer_get_insert (buf), &cursor);
    gtk_text_buffer_move_mark_by_name (buf, "selection_bound", &cursor);
  }
  gtk_widget_grab_focus (w);
}

void next_tab (GtkWidget* widget, DevchatCBData* data)
{
  if (gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook)) == (gtk_notebook_get_n_pages (GTK_NOTEBOOK (data->window->notebook))-1))
    gtk_notebook_set_current_page (GTK_NOTEBOOK (data->window->notebook), 0);
  else
    gtk_notebook_next_page (GTK_NOTEBOOK (data->window->notebook));
}

void prev_tab (GtkWidget* widget, DevchatCBData* data)
{
  if (gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook)) == 0)
    gtk_notebook_set_current_page (GTK_NOTEBOOK (data->window->notebook), (gtk_notebook_get_n_pages (GTK_NOTEBOOK (data->window->notebook))-1));
  else
    gtk_notebook_prev_page (GTK_NOTEBOOK (data->window->notebook));
}

void show_his (GtkWidget* widget, DevchatCBData* data)
{
  guint day;
  guint month;
  guint year;
  gchar* uri;

  GtkWidget* cal = gtk_calendar_new ();
  GtkWidget* dialog = gtk_dialog_new_with_buttons (_("Choose a date"), GTK_WINDOW (data->window->window), GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                                   NULL);
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG(dialog))), cal, FALSE, FALSE, 0);
  gtk_widget_show (cal);

  switch (gtk_dialog_run (GTK_DIALOG (dialog)))
  {
    case GTK_RESPONSE_ACCEPT:
      gtk_calendar_get_date (GTK_CALENDAR (cal), &year, &month, &day);
      month++;
      uri = g_strdup_printf ("http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?date=%02i/%02i/%04i",month,day,year);

      DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, g_strdup_printf ("History for %02i/%02i/%04i",month,day,year));
      if (conv)
        gtk_widget_show_all (conv->child);
      else
      {
        SoupMessage* get = soup_message_new("GET", uri);
        soup_session_queue_message (data->window->session, get, SOUP_SESSION_CALLBACK (his_cb), devchat_cb_data_new(data->window, g_strdup_printf("%02i/%02i/%04i",month,day,year)));
        gtk_statusbar_push (GTK_STATUSBAR (data->window->statusbar),
                            gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "his"),
                            _("History loading...")
                           );
      }
    break;
    default: break;
  }
  gtk_widget_destroy (dialog);
}

void his_cb (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  gchar* messagelist = g_strdup (m->response_body->data);

  gtk_statusbar_pop (GTK_STATUSBAR (data->window->statusbar),
                     gtk_statusbar_get_context_id (GTK_STATUSBAR (data->window->statusbar), "his")
                    );

  if (messagelist)
  {
    ce_parse (messagelist, data, (gchar*) data->data);
  }
}

void about_cb (GtkWidget* widget, DevchatCBData* data)
{
  const gchar* authors[] = {"Samuel Vincent Creshal", "Fanchen", 0};
  const gchar* artists[] = {"Silvio Walther",0};

  GtkWidget* dialog = gtk_about_dialog_new ();
  gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (dialog), APPNAME);
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), VERSION);
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), "© Samuel Creshal 2010\nPortions © Egosoft\nPortions © International Organization for Standardization 1986");
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), "http://dev.yaki-syndicate.de");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), _(" - Gtk+ client for X-Devchat."));
  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
  gtk_about_dialog_set_artists (GTK_ABOUT_DIALOG (dialog), artists);
  gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog), _("translator-credits"));
  gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), gdk_pixbuf_new_from_file (g_build_filename (data->window->workingdir, "dcgui_48.png", NULL), NULL));

  gchar* license_text;

  const gchar* const* dirs = g_get_system_data_dirs ();

  int i;
  gchar* license_filename = NULL;

  for (i=0; dirs[i]; i++)
  {
    license_filename = g_build_filename (dirs[i], "licenses", "common", "GPL2", "license.txt", NULL);
    if (g_file_test (license_filename, G_FILE_TEST_EXISTS))
    {
      g_file_get_contents (license_filename, &license_text, NULL, NULL);
      break;
    }
    else
    {
      license_filename = g_build_filename (dirs[i], "common-licenses", "GPL-2", NULL);
      if (g_file_test (license_filename, G_FILE_TEST_EXISTS))
      {
        g_file_get_contents (license_filename, &license_text, NULL, NULL);
        break;
      }
      else
      {
        license_text = "This program is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n\
GNU General Public License for more details.";
      }
    }
  }
  g_free (license_filename);

  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), license_text);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

void at_cb (GtkWidget* widget, DevchatCBData* data)
{
  GRegex* re = g_regex_new (" ", 0, 0, NULL);
  gchar* msg_r = g_regex_replace (re, (gchar*) data->data, -1, 0, "\xc2\xa0", 0, NULL);

  gchar* text = g_strconcat ("@", msg_r, " ", NULL);

  gtk_text_buffer_insert_at_cursor (data->window->input, (gchar*) text, strlen (text));

  g_free (re);
  g_free (msg_r);
  g_free (text);

  gtk_widget_grab_focus (data->window->inputwidget);
}

DevchatConversation* pm_cb (GtkWidget* widget, DevchatCBData* data)
{
  gboolean is_history = g_str_has_prefix ((gchar*) data->data, "History for ");

  DevchatConversation* conv = (DevchatConversation*) g_hash_table_lookup (data->window->conversations, (gchar*) data->data);
  if (!conv)
  {
    conv = devchat_conversation_new (is_history, data->window);

    g_hash_table_insert (data->window->conversations, g_strdup ((gchar*) data->data), conv);

    GtkWidget* labelbox = gtk_hbox_new (FALSE, 0);
    GtkWidget* avatar;

    /*Determine icon*/
    if (is_history)
      avatar = gtk_image_new_from_stock (GTK_STOCK_INFO, GTK_ICON_SIZE_MENU);
    else if (g_hash_table_lookup (data->window->users, (gchar*) data->data)
             && g_hash_table_lookup (data->window->avatars, g_hash_table_lookup (data->window->users, (gchar*) data->data)))
      avatar = gtk_image_new_from_pixbuf (g_hash_table_lookup (data->window->avatars, g_hash_table_lookup (data->window->users, (gchar*) data->data)));
    else if (g_strcmp0 ((gchar*) data->data, "(ChatServer):") == 0)
      avatar = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_MENU);
    else
      avatar = gtk_image_new_from_pixbuf (g_hash_table_lookup (data->window->avatars, "default"));

    GtkWidget* avatar_event_box = gtk_event_box_new ();
    gtk_event_box_set_above_child (GTK_EVENT_BOX (avatar_event_box), TRUE);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (avatar_event_box), FALSE);
    gtk_container_add (GTK_CONTAINER (avatar_event_box), avatar);
    gtk_box_pack_start (GTK_BOX (labelbox), avatar_event_box, FALSE, FALSE, 0);
    g_signal_connect (avatar_event_box, "button-release-event", G_CALLBACK (devchat_window_tab_changed_win), data);

    GtkWidget* label = gtk_label_new ((gchar*) data->data);
    GtkWidget* event_box = gtk_event_box_new ();
    gtk_event_box_set_above_child (GTK_EVENT_BOX (event_box), TRUE);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (event_box), FALSE);
    gtk_container_add (GTK_CONTAINER (event_box), label);
    gtk_box_pack_start (GTK_BOX (labelbox), event_box, TRUE, TRUE, 0);

    GtkWidget* tab_close = gtk_button_new ();
    gtk_widget_set_size_request (tab_close, 15, 8);
    GdkPixbuf* rendered_icon = gtk_widget_render_icon (tab_close, GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU, NULL);
    GdkPixbuf* real_icon = gdk_pixbuf_scale_simple (rendered_icon, 8, 8, GDK_INTERP_BILINEAR);

    gtk_button_set_image (GTK_BUTTON (tab_close), gtk_image_new_from_pixbuf (real_icon));

    g_signal_connect (tab_close, "clicked", G_CALLBACK (devchat_window_close_tab), devchat_cb_data_new (data->window, conv->child));
    gtk_box_pack_end (GTK_BOX (labelbox), tab_close, FALSE, TRUE, 0);
    g_signal_connect (event_box, "button-release-event", G_CALLBACK (devchat_window_tab_changed_win), data);
    gtk_widget_show_all (labelbox);
    gtk_widget_show_all (conv->child);
    gtk_notebook_append_page_menu (GTK_NOTEBOOK (data->window->notebook), conv->child, labelbox, label);
    gtk_notebook_set_tab_reorderable (GTK_NOTEBOOK (data->window->notebook), conv->child, TRUE);
  }
  gtk_widget_show_all (conv->child);
  if (data->window->settings.jumptab || widget)
  {
    gtk_notebook_set_current_page (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_page_num (GTK_NOTEBOOK (data->window->notebook), conv->child));
    if (conv->in_widget)
      gtk_widget_grab_focus (conv->in_widget);
  }

  return conv;
}

void devchat_window_refresh_smilies (DevchatWindow* self)
{
  GtkWidget* smilie_sub = gtk_menu_new();
  GtkWidget* tearoff = gtk_tearoff_menu_item_new ();
  gtk_menu_shell_append (GTK_MENU_SHELL (smilie_sub), tearoff);
  g_hash_table_foreach (self->smilies, (GHFunc) add_smilie_cb, devchat_cb_data_new (self, smilie_sub));
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (self->item_smilies), smilie_sub);
  gtk_widget_show_all (self->item_smilies);
}

void add_smilie_cb (gpointer key, gpointer value, DevchatCBData* data)
{
  gchar* name = (gchar*) key;
  gchar* icon = (gchar*) value;

  GtkWidget* item = gtk_image_menu_item_new_with_label (name);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), gtk_image_new_from_file (icon));

  g_signal_connect (item, "activate", G_CALLBACK (ins_smilie), devchat_cb_data_new (data->window, name));
  gtk_menu_shell_append (data->data, item);
}

void devchat_window_refresh_presets (DevchatWindow* self)
{
  GtkWidget* preset_sub = gtk_menu_new ();
  gint i;
  for (i = 0; i < 10 && g_strcmp0 ("", self->settings.presets[i]) != 0; i++)
  {
    GtkWidget* item = gtk_menu_item_new_with_label (self->settings.presets[i]);
    g_signal_connect (item, "activate", G_CALLBACK (ins_preset), devchat_cb_data_new (self, self->settings.presets[i]));
    gtk_widget_add_accelerator (item, "activate", self->accelgroup, i==9? GDK_0 : GDK_1+i, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append (GTK_MENU_SHELL (preset_sub), item);
  }
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (self->item_presets), preset_sub);
  gtk_widget_show_all (self->item_presets);
}

void ins_smilie (GtkWidget* widget, DevchatCBData* data)
{
  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkTextBuffer* buf;

  if (pagenum == 0)
  {
    buf = data->window->input;
  }
  else
  {
    const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
    DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
    buf = conv->in_buffer;
  }

  gtk_text_buffer_insert_at_cursor (buf, (gchar*) data->data, -1);
}

void ins_preset (GtkWidget* widget, DevchatCBData* data)
{
  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkTextBuffer* buf;

  if (pagenum == 0)
  {
    buf = data->window->input;
  }
  else
  {
    const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
    DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
    buf = conv->in_buffer;
  }

  gtk_text_buffer_insert_at_cursor (buf, (gchar*) data->data, -1);
}

void notify(gchar* title, gchar* body, GdkPixbuf* icon, DevchatCBData* data)
{
  if (!data->window->dnd)
  {
    if (time (NULL) - data->window->last_notification < 2)
      return;
    else
      data->window->last_notification = time (NULL);


    if (g_strcmp0(data->window->settings.vnotify,"<native>") == 0)
    {
      if (!gtk_widget_get_visible (data->window->window))
        gtk_status_icon_set_blinking (GTK_STATUS_ICON (data->window->trayicon), TRUE);
  #ifdef NOTIFY
      NotifyNotification* note = notify_notification_new(title,body,NULL,NULL);
      if (icon)
        notify_notification_set_icon_from_pixbuf(note,icon);
      notify_notification_add_action(note, "0", "Show", NOTIFY_ACTION_CALLBACK (notify_cb), data, NULL);
      notify_notification_set_timeout (note, 3141);
      notify_notification_show (note, NULL);
  #endif
    }
    else if (g_strcmp0(data->window->settings.vnotify,"<none>") != 0)
    {
      gchar* vcmdline = g_strdup(data->window->settings.vnotify);
      if (!g_spawn_command_line_async (vcmdline, NULL))
      {
        err (_("Error N: Failed to launch visual notification process."));
        g_free (data->window->settings.vnotify);
        data->window->settings.vnotify = g_strdup("<none>");
      }
      g_free (vcmdline);
    }

    if (g_strcmp0(data->window->settings.notify,"<native>") == 0)
    {
      GRegex* fldr = g_regex_new ("pixmap", 0, 0, NULL);
      gchar* workingdir = g_regex_replace (fldr, data->window->workingdir, -1, 0, "sound", 0, NULL);

    #ifdef G_OS_UNIX
      gchar* cmdline = g_strdup_printf ("aplay -q %s/jingle.wav", workingdir);
      if (!g_spawn_command_line_async (cmdline, NULL))
      {
        g_free (cmdline);
        cmdline = g_strdup_printf ("ossplay -q %s/jingle.wav", workingdir);
        if (!g_spawn_command_line_async (cmdline, NULL))
        {
          err (_("Error N: Failed to launch audio notification process."));
          data->window->settings.notify = g_strdup("<none>");
        }
      }
      g_free (cmdline);
    #else
      #ifdef G_OS_WIN32
        sndPlaySound (g_build_filename (workingdir, "jingle.wav", NULL), SND_ASYNC);
      #endif
    #endif

      g_free (fldr);
      g_free (workingdir);
    }
    else if (g_strcmp0(data->window->settings.notify,"<none>") != 0)
    {
      gchar* cmdline = g_strdup(data->window->settings.notify);
      if (!g_spawn_command_line_async (cmdline, NULL))
      {
        err (_("Error N: Failed to launch audio notification process."));
        g_free (data->window->settings.notify);
        data->window->settings.notify = g_strdup("<none>");
      }
      g_free (cmdline);
    }
  }
}

#ifdef NOTIFY
void notify_cb(NotifyNotification* note, gchar* action, DevchatCBData* data)
{
  gtk_window_present (GTK_WINDOW(data->window->window));
}
#endif

void toggle_tray_minimize (GtkStatusIcon* icon, DevchatCBData* data)
{
  gtk_status_icon_set_blinking (icon, FALSE);
  if (gtk_widget_get_visible (data->window->window))
  {
    gtk_window_get_position (GTK_WINDOW (data->window->window), &data->window->settings.x, &data->window->settings.y);
    if (gtk_window_has_toplevel_focus (GTK_WINDOW (data->window->window)))
    {
      gtk_widget_hide (data->window->window);
      data->window->settings.update_time *= 10;
    }
    else
    {
      gtk_window_present (GTK_WINDOW (data->window->window));
      gtk_window_move (GTK_WINDOW (data->window->window), data->window->settings.x, data->window->settings.y);
    }
  }
  else
  {
    data->window->settings.update_time /= 10;
    gtk_widget_show (data->window->window);
    gtk_window_move (GTK_WINDOW (data->window->window), data->window->settings.x, data->window->settings.y);
  }
}

void show_tray_menu (GtkStatusIcon* icon, guint button, guint activate_time, DevchatCBData* data)
{
  GtkWidget* menu = gtk_menu_new ();

  GtkWidget* item_status = gtk_menu_item_new_with_label (_("Change Status..."));

  GtkWidget* item_status_online = gtk_menu_item_new_with_label (_("Online"));
  g_signal_connect (item_status_online, "activate", G_CALLBACK (tray_status_change), devchat_cb_data_new (data->window, GINT_TO_POINTER (0)));
  GtkWidget* item_status_away = gtk_menu_item_new_with_label (_("Away"));
  g_signal_connect (item_status_away, "activate", G_CALLBACK (tray_status_change), devchat_cb_data_new (data->window, GINT_TO_POINTER (1)));
  GtkWidget* item_status_dnd = gtk_menu_item_new_with_label (_("DND"));
  g_signal_connect (item_status_dnd, "activate", G_CALLBACK (tray_status_change), devchat_cb_data_new (data->window, GINT_TO_POINTER (2)));

  GtkMenuShell* sub_status = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(sub_status, item_status_online);
  gtk_menu_shell_append(sub_status, item_status_away);
  gtk_menu_shell_append(sub_status, item_status_dnd);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (item_status), GTK_WIDGET (sub_status));

  GtkWidget* item_quit = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
  g_signal_connect (item_quit, "activate", G_CALLBACK (destroy), data);

  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item_status);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), gtk_menu_item_new ());
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item_quit);

  gtk_widget_show_all (menu);

  gtk_menu_popup (GTK_MENU (menu), NULL, NULL, gtk_status_icon_position_menu, icon, button, activate_time);
}

void tray_status_change (GtkWidget* w, DevchatCBData* data)
{
  gchar* msg;
  switch (GPOINTER_TO_INT (data->data))
  {
    case 0: msg = "/back"; break;
    case 1: msg = "/away"; break;
    case 2: msg = "/dnd"; break;
    default: return;
  }
  devchat_window_text_send (data, g_strdup (msg), NULL, "1", FALSE);
}

gboolean track_window_state (GtkWidget* widget, GdkEventWindowState* s, DevchatCBData* data)
{
  if (s->changed_mask & GDK_WINDOW_STATE_MAXIMIZED)
  {
    if (s->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)
      data->window->settings.maximized = TRUE;
    else
      data->window->settings.maximized = FALSE;
  }

  return TRUE;
}

gboolean get_pos_size (DevchatWindow* window)
{
  if (gtk_widget_get_visible (window->window))
  {
    gtk_window_get_position (GTK_WINDOW (window->window), &window->settings.x, &window->settings.y);
    gtk_window_get_size (GTK_WINDOW (window->window), &window->settings.width, &window->settings.height);

    GtkPaned* hpaned1 = GTK_PANED (gtk_widget_get_parent (gtk_widget_get_parent (window->userlist_port)));
    window->settings.handle_width = gtk_paned_get_position (hpaned1);
  }
  return TRUE;
}

void devchat_window_color_changed (GtkWidget* widget, DevchatCBData* data)
{
  gchar* tag;
  gboolean html;
  switch (gtk_combo_box_get_active (GTK_COMBO_BOX (widget)))
  {
    case -1:
    case 0: return;
    case 7:
    case 8: html = TRUE; tag = gtk_combo_box_get_active_text (GTK_COMBO_BOX (widget)); break;
    default: html = FALSE; tag = gtk_combo_box_get_active_text (GTK_COMBO_BOX (widget)); break;
  }
  gtk_combo_box_set_active (GTK_COMBO_BOX (widget), 0);

  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkTextBuffer* buf;
  GtkWidget* w;
  GtkTextIter start;
  GtkTextIter end;

  if (pagenum == 0)
  {
    buf = data->window->input;
    w = data->window->inputwidget;
  }
  else
  {
    const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
    DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
    buf = conv->in_buffer;
    w = conv->in_widget;
  }

  if (gtk_text_buffer_get_selection_bounds (buf, &start, &end))
  {
    GtkTextMark* ed = gtk_text_buffer_create_mark (buf, "tmp", &end, TRUE);
    gchar* tag_open = g_strconcat ((html? "<span class=\"":"["), tag, (html? "\">":"]"), NULL);
    gtk_text_buffer_insert (buf, &start, tag_open, -1);
    g_free (tag_open);
    gtk_text_buffer_get_iter_at_mark (buf, &end, ed);
    gchar* tag_close = (html? g_strdup ("</span>") : g_strconcat ("[/", tag, "]", NULL));
    gtk_text_buffer_insert (buf, &end, tag_close, -1);
    g_free (tag_close);
    gtk_text_iter_backward_chars (&end, (html? 7 : strlen(tag)+3));
    gtk_text_buffer_place_cursor (buf, &end);
    gtk_text_buffer_delete_mark (buf, ed);
  }
  else
  {
    gchar* ctag = g_strconcat ((html? "<span class=\"":"["), tag, (html? "\"></span>":"][/"), (html? "" : tag), (html? "": "]"), NULL);
    gtk_text_buffer_insert_at_cursor (buf, ctag, -1);
    g_free (ctag);

    GtkTextIter cursor;

    gtk_text_buffer_get_iter_at_mark (buf, &cursor, gtk_text_buffer_get_insert (buf));
    gtk_text_iter_backward_chars (&cursor, strlen (tag) + 3);
    gtk_text_buffer_move_mark (buf, gtk_text_buffer_get_insert (buf), &cursor);
    gtk_text_buffer_move_mark_by_name (buf, "selection_bound", &cursor);
  }
  gtk_widget_grab_focus (w);
  g_free (tag);
}

gboolean devchat_window_button_press_cb (GtkWidget* w, GdkEventButton* event, DevchatCBData* data)
{
  if (event->button == 3 && event->type == GDK_BUTTON_PRESS)
  {
    return devchat_window_on_popup_menu (w, data);
  }
  return FALSE;
}

gboolean devchat_window_on_popup_menu (GtkWidget* view, DevchatCBData* data)
{
  gint x = 0;
  gint y = 0;
  gtk_widget_get_pointer (view, &x, &y);
  if (x && y)
  {
    gint buf_x, buf_y;
    GtkTextIter iter;
    GtkWidget* menu;

    gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (view), GTK_TEXT_WINDOW_TEXT, x, y, &buf_x, &buf_y);
    gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (view), &iter, buf_x, buf_y);

    gboolean found = FALSE;

    GSList* tag = gtk_text_iter_get_tags (&iter);

    menu = gtk_menu_new ();

    gchar* username = NULL;
    GRegex* r = g_regex_new (" ", 0, 0, NULL);

    while (tag && !(found))
    {
      gchar* name;
      g_object_get (tag->data, "name", &name, NULL);

      if (name && g_str_has_prefix (name, "img::"))
      {
        found = TRUE;
        GtkWidget* item_open_link = gtk_image_menu_item_new_with_label (_("Open image in browser"));
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item_open_link), gtk_image_new_from_stock (GTK_STOCK_JUMP_TO, GTK_ICON_SIZE_MENU));
        g_signal_connect (item_open_link, "activate", G_CALLBACK (popup_open_link), devchat_cb_data_new (data->window, g_strdup (name+5)));

        GtkWidget* item_copy_link = gtk_image_menu_item_new_from_stock (GTK_STOCK_COPY, NULL);
        g_signal_connect (item_copy_link, "activate", G_CALLBACK (popup_copy_stuff), devchat_cb_data_new (data->window, g_strdup (name+5)));

        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item_open_link);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item_copy_link);
      }
      else if (name && g_str_has_prefix (name, "url::"))
      {
        found = TRUE;
        GtkWidget* item_open_link = gtk_image_menu_item_new_with_label (_("Open link in browser"));
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item_open_link), gtk_image_new_from_stock (GTK_STOCK_JUMP_TO, GTK_ICON_SIZE_MENU));
        g_signal_connect (item_open_link, "activate", G_CALLBACK (popup_open_link), devchat_cb_data_new (data->window, g_strdup (name+5)));

        GtkWidget* item_copy_link = gtk_image_menu_item_new_from_stock (GTK_STOCK_COPY, NULL);
        g_signal_connect (item_copy_link, "activate", G_CALLBACK (popup_copy_stuff), devchat_cb_data_new (data->window, g_strdup (name+5)));

        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item_open_link);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item_copy_link);
      }
      else if (name && g_str_has_prefix (name, "user-"))
      {
        username = g_regex_replace (r, name+5, -1, 0, "&nbsp;", 0, NULL);
      }
      else if (name && g_str_has_prefix (name, "lid::"))
      {
        found = TRUE;
        GtkWidget* item_copy_id = gtk_image_menu_item_new_with_label (_("Copy ID"));
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item_copy_id), gtk_image_new_from_stock (GTK_STOCK_COPY, GTK_ICON_SIZE_MENU));
        g_signal_connect (item_copy_id, "activate", G_CALLBACK (popup_copy_stuff), devchat_cb_data_new (data->window, g_strdup (name+5)));
        GtkWidget* item_cite_id = gtk_image_menu_item_new_with_label (_("Cite Post"));
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item_cite_id), gtk_image_new_from_stock (GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_MENU));

        if (!username)
        {
          GSList* tag2 = tag;

          while (tag2 && !username)
          {
            gchar* name2;
            g_object_get (tag->data, "name", &name2, NULL);

            if (name && g_str_has_prefix (name, "user-"))
              username = g_regex_replace (r, name+5, -1, 0, "&nbsp;", 0, NULL);
            tag2 = tag2->next;
          }
        }

        gchar* text = g_strconcat ("@", username, "/", (name+5), NULL);

        g_signal_connect (item_cite_id, "activate", G_CALLBACK (popup_insert_text), devchat_cb_data_new (data->window, text));

        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item_copy_id);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item_cite_id);
      }
      tag = tag->next;
    }

    if (found)
    {
      gtk_widget_show_all (menu);
      gtk_menu_attach_to_widget (GTK_MENU (menu), view, NULL);
      gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time());
    }

    return found;

    if (username)
      g_free (username);
    g_free (r);
  }
  return FALSE;
}

void popup_open_link (GtkWidget* w, DevchatCBData* data)
{
  launch_browser (w, (gchar*) data->data, data);
}

void popup_copy_stuff (GtkWidget* w, DevchatCBData* data)
{
  GtkClipboard* c = gtk_clipboard_get (GDK_NONE);
  gtk_clipboard_set_text (c, (gchar*) data->data, -1);
}

void popup_insert_text (GtkWidget* w, DevchatCBData* data)
{
  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkTextBuffer* buf;
  GtkWidget* in_view;

  if (pagenum == 0)
  {
    buf = data->window->input;
    in_view = data->window->inputwidget;
  }
  else
  {
    const gchar* target = gtk_notebook_get_menu_label_text (GTK_NOTEBOOK (data->window->notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (data->window->notebook), pagenum));
    DevchatConversation* conv = g_hash_table_lookup (data->window->conversations, target);
    buf = conv->in_buffer;
    in_view = conv->in_widget;
  }

  gtk_text_buffer_insert_at_cursor (buf, (gchar*) data->data, -1);
  gtk_widget_grab_focus (in_view);
}

gchar* current_time ()
{
  time_t rawtime = time (NULL);
  struct tm *tm_s;
  char datestring[10];

  tm_s = localtime (&rawtime);

  strftime (datestring, 10, "%H:%M:%S", tm_s);

  return g_strdup(datestring);
}

void err(gchar* message)
{
#ifdef G_OS_UNIX
  g_warning ("ERROR: %s.", message);
#else
  #ifdef G_OS_WIN32
  FILE* logfile = fopen (g_build_filename (g_getenv ("TEMP"), "dcgui_error.log", NULL), "a");
  fprintf (logfile, "%s\n", message);
  fclose (logfile);
  #endif
#endif
}

void dbg(gchar* message)
{
#ifdef G_OS_WIN32
  /*F'ing Windows doesn't open stdout/-err for GUI apps.*/
  FILE* logfile = fopen (g_build_filename (g_getenv ("TEMP"), "dcgui_debug.log", NULL), "a");
  fprintf (logfile, "%s\n", message);
  fclose (logfile);
#else
  g_printf ("%s\n", message);
#endif
}

#ifdef INGAME
gboolean get_ingame_messages (DevchatCBData* data)
{
  gchar* message_lines;
  const gchar* filename = g_build_filename (g_get_user_special_dir (G_USER_DIRECTORY_DOCUMENTS), "EGOSOFT", "X3TC", "log07642.txt", NULL);

  if (g_file_get_contents (filename, &message_lines, NULL, NULL))
  {
    g_remove (filename);
    if (debug)
    dbg (g_strdup_printf ("Received message list: %s.\n\n", message_lines));

    gchar** messages = g_strsplit (message_lines, "\n", -1);

    int i;

    for (i = 0; messages[i]; i++)
    {
      gchar** message = g_strsplit (messages[i], ";;", 4);

      if (message[0] && message[1] && message[2] && message[3])
        devchat_window_text_send (data, message[3], message[2], message[1], g_strcmp0(message[0], "1") == 0);

      g_strfreev (message);
    }

    g_strfreev (messages);
    g_free (message_lines);
  }
  else if (debug)
    dbg ("Ingame logfile could not be read.\n");

  return TRUE;
}

void ingame_update_status (DevchatCBData* data, gint status)
{
  data->window->ingame_status = status;
  ingame_flush_data (data);
}

void ingame_clear_user_list (DevchatCBData* data)
{
  if (g_strcmp0 (data->window->ingame_userlist, "") != 0)
    g_free (data->window->ingame_userlist);
  data->window->ingame_userlist = "";
  data->window->ingame_usercount = 0;
}

void ingame_clear_message_list (DevchatCBData* data)
{
  if (g_strcmp0 (data->window->ingame_messagelist, "") != 0)
    g_free (data->window->ingame_messagelist);
  data->window->ingame_messagelist = "";
  data->window->ingame_lid = 0;
}

void ingame_append_user (DevchatCBData* data, gchar* user)
{
  data->window->ingame_userlist = g_strdup_printf ("%s <t id=\"%i\">%s</t>\n", data->window->ingame_userlist, data->window->ingame_usercount, user);
  data->window->ingame_usercount++;
}

void ingame_append_message (DevchatCBData* data, gchar* author, gchar* mode, gchar* time_attr, gchar* lid, gchar* message)
{
  data->window->ingame_lid++;

  GRegex* delimiter = g_regex_new (";;", 0, 0, NULL);
  GRegex* parenthesis_a = g_regex_new ("[(]", 0, 0, NULL);
  GRegex* parenthesis_b = g_regex_new ("[)]", 0, 0, NULL);
  GRegex* ampersand = g_regex_new ("&", 0, 0, NULL);

  gchar* message_r = g_regex_replace_literal (delimiter, message, -1, 0, "; ;", 0, NULL);
  message_r = g_regex_replace_literal (parenthesis_a, message_r, -1, 0, "\\(", 0, NULL);
  message_r = g_regex_replace_literal (parenthesis_b, message_r, -1, 0, "\\)", 0, NULL);
  message_r = g_regex_replace_literal (ampersand, message_r, -1, 0, "&amp;", 0, NULL);

  gchar* author_r = g_regex_replace_literal (parenthesis_a, author, -1, 0, "\\(", 0, NULL);
  author_r = g_regex_replace_literal (parenthesis_b, author_r, -1, 0, "\\)", 0, NULL);

  data->window->ingame_messagelist = g_strdup_printf ("%s <t id=\"%i\">%s;;%s;;%s;;%s;;%s</t>\n", data->window->ingame_messagelist, data->window->ingame_lid, author_r, mode, time_attr, lid, message_r);

  g_free (message_r);
  g_free (author_r);
  g_free (delimiter);
  g_free (parenthesis_a);
  g_free (parenthesis_b);
}

void ingame_flush_data (DevchatCBData* data)
{
  if (g_strcmp0 (data->window->settings.TCFolder, "") != 0)
  {
    const gchar* filename = g_build_filename (data->window->settings.TCFolder, "t", _("7641-L044.xml"), NULL);

    gchar* file_content = g_strdup_printf ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<language id=\"44\">\n\
<page id=\"7641\">\n\
 <t id=\"0\">%s</t>\n\
 <t id=\"1\">%i</t>\n\
 <t id=\"2\">%i</t>\n\
 <t id=\"3\">%i</t>\n\
 <t id=\"4\">%i</t>\n\
 <t id=\"5\">%s</t>\n\
</page>\n\
<page id=\"7642\">\n\
%s\n\
</page>\n\
<page id=\"7643\">\n\
%s\n\
</page>\n\
</language>\n", data->window->settings.user, data->window->userlevel, data->window->ingame_status, data->window->ingame_lid, data->window->ingame_usercount, current_time(), data->window->ingame_messagelist, data->window->ingame_userlist);

    if (!g_file_set_contents (filename, file_content, -1, NULL))
      err (g_strdup_printf(_("Error I: Error writing text file %s. Check write permissions for t-folder!"), filename));
  }
}

#endif
