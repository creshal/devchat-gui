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

void notify_cb ();
void urlopen ();
void user_list_get();
void message_list_get();
void login_cb ();
void remote_level ();
gboolean hotkey_cb ();
void destroy (GtkWidget* widget, DevchatCBData* data);
void login (GtkWidget* widget, DevchatCBData* data);
void config_cb (GtkWidget* widget, DevchatCBData* data);
void go_forum (GtkWidget* widget, DevchatCBData* data);
void close_tab (GtkWidget* widget, DevchatCBData* data);
void reconnect (GtkWidget* widget, DevchatCBData* data);
void tab_changed (GtkWidget* widget, DevchatCBData* data);
gboolean tab_changed_win (GtkWidget* widget, DevchatCBData* data);
void on_motion (GtkWidget* widget, DevchatCBData* data);
void on_mark_set (GtkWidget* widget, DevchatCBData* data);
void level_changed (GtkWidget* widget, DevchatCBData* data);
void btn_format (GtkWidget* widget, DevchatCBData* data);
void btn_send (GtkWidget* widget, DevchatCBData* data);
void next_tab (GtkWidget* widget, DevchatCBData* data);
void prev_tab (GtkWidget* widget, DevchatCBData* data);
void show_his (GtkWidget* widget, DevchatCBData* data);
void about_cb (GtkWidget* widget, DevchatCBData* data);
void at_cb (GtkWidget* widget, DevchatCBData* data);
void pm_cb (GtkWidget* widget, DevchatCBData* data);
void user_list_clear_cb (GtkWidget* child, DevchatCBData* data);
void post_sent (SoupSession* s, SoupMessage* m, DevchatCBData* data);

void create_tags (GtkTextBuffer* buf, DevchatCBData* data);
void add_smilie_cb (gpointer key, gpointer value, DevchatCBData* data);
void ins_smilie (GtkWidget* widget, DevchatCBData* data);

gint user_lookup (gchar* a, gchar* b);
void search_ava_cb (SoupSession* s, SoupMessage* m, DevchatCBData* data);

gboolean user_list_poll (DevchatCBData* data);
gboolean message_list_poll (DevchatCBData* data);
void ce_parse (gchar* data, DevchatCBData* self, gchar* date);
void parse_message (htmlNodePtr element, DevchatCBData* self, htmlDocPtr doc, htmlParserCtxtPtr ptr);
gchar* color_lookup (gchar* color);

gchar* current_time ();


G_DEFINE_TYPE (DevchatWindow, devchat_window, G_TYPE_OBJECT);

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

  self->smilies = g_hash_table_new (g_str_hash, g_str_equal);
  self->avatars = g_hash_table_new (g_str_hash, g_str_equal);
  self->settings.browser = g_strdup("<native>");
  self->settings.color_font = g_strdup("#eeeeec");
  self->settings.color_l1 = g_strdup("#2e3436");
  self->settings.color_l3 = g_strdup("#543535");
  self->settings.color_l5 = g_strdup("#354254");
  self->settings.color_l6 = g_strdup("#45513a");
  self->settings.color_greens = g_strdup("#8ae234");
  self->settings.color_blues = g_strdup("#729fcf");
  self->settings.color_time = g_strdup("#babdb6");
  self->settings.color_url = g_strdup("#fce94f");
  self->settings.color_url_visited = g_strdup("#fcaf3e");
  self->settings.color_url_hover = g_strdup("#e9b96e");
  self->settings.color_highlight = g_strdup("#ef2929");
  self->settings.user = g_strdup(g_get_user_name());
  self->settings.pass = g_strdup("hidden");
  self->settings.showid = FALSE;
  self->settings.stealthjoin = FALSE;
  self->settings.autojoin = FALSE;
  self->settings.showhidden = FALSE;
  self->settings.coloruser = TRUE;
  self->settings.notify = g_strdup("<native>");
  self->settings.vnotify = g_strdup("<native>");
  self->settings.width = 600;
  self->settings.height = 400;
  self->settings.x = 0;
  self->settings.y = 0;
  self->settings.avatar_size = 12;
  self->settings.update_time = 1000; /*Time between update requests in ms.*/
  self->settings.keywords = NULL; /*GSList*/
  self->settings.presets = NULL;
  self->firstrun = TRUE;
  self->hovertag = NULL;

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
  g_signal_connect(self->window, "destroy", G_CALLBACK(destroy),&self_data);
  g_signal_connect(self->window, "focus-in-event", G_CALLBACK(tab_changed_win),self_data);


  GtkMenuItem* menu_main = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Main"));
  GtkMenuItem* menu_edit = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Edit"));
  GtkMenuItem* menu_insert = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Insert"));
  GtkMenuItem* menu_view = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Go"));
  GtkMenuItem* menu_about = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic("_Help"));

  self->item_connect = gtk_image_menu_item_new_from_stock(GTK_STOCK_CONNECT,self->accelgroup);
  g_signal_connect (self->item_connect, "activate", G_CALLBACK (login),self_data);

  GtkWidget* item_prefs = gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES,self->accelgroup);
  g_signal_connect (item_prefs, "activate", G_CALLBACK (config_cb),self_data);
  gtk_widget_add_accelerator(item_prefs, "activate", self->accelgroup, GDK_P, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* edit_profile = devchat_cb_data_new (self, GINT_TO_POINTER (URL_PROFILE_EDIT));

  GtkWidget* item_profile = gtk_image_menu_item_new_with_label ("Edit profile...");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_profile), gtk_image_new_from_icon_name("system-users",GTK_ICON_SIZE_MENU));
  g_signal_connect (item_profile, "activate", G_CALLBACK (go_forum), edit_profile);

  DevchatCBData* view_devnet = devchat_cb_data_new (self, GINT_TO_POINTER (URL_VISIT_L3));

  self->item_l3 = gtk_menu_item_new_with_label ("Open DevNet fora...");
  g_signal_connect (self->item_l3, "activate", G_CALLBACK (go_forum), view_devnet);
  gtk_widget_set_no_show_all(self->item_l3,TRUE);

  DevchatCBData* view_beta = devchat_cb_data_new (self, GINT_TO_POINTER (URL_VISIT_L5));

  self->item_l5 = gtk_menu_item_new_with_label ("Open betatest fora...");
  g_signal_connect (self->item_l5, "activate", G_CALLBACK (go_forum), view_beta);
  gtk_widget_set_no_show_all(self->item_l5,TRUE);

  DevchatCBData* view_forum = devchat_cb_data_new (self, GINT_TO_POINTER (URL_VISIT_L1));

  GtkWidget* item_forum = gtk_menu_item_new_with_label ("Open forum...");
  g_signal_connect (item_forum, "activate", G_CALLBACK (go_forum), view_forum);

  GtkWidget* item_tabclose = gtk_image_menu_item_new_with_label ("Close tab");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_tabclose), gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_tabclose, "activate", G_CALLBACK (close_tab), self_data);
  gtk_widget_add_accelerator(item_tabclose, "activate", self->accelgroup, GDK_W, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  self->item_reconnect = gtk_image_menu_item_new_with_label ("Reconnect");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (self->item_reconnect), gtk_image_new_from_stock(GTK_STOCK_REFRESH,GTK_ICON_SIZE_MENU));
  g_signal_connect (self->item_reconnect, "activate", G_CALLBACK (reconnect), self_data);
  gtk_widget_set_no_show_all(self->item_reconnect,TRUE);
  gtk_widget_add_accelerator(self->item_reconnect, "activate", self->accelgroup, GDK_R, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_disconnect = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT,self->accelgroup);
  g_signal_connect (item_disconnect, "activate", G_CALLBACK (destroy), self_data);
  gtk_widget_add_accelerator(item_disconnect, "activate", self->accelgroup, GDK_Q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_tab_next = gtk_image_menu_item_new_with_label ("Next Tab");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_tab_next), gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_tab_next, "activate", G_CALLBACK (next_tab), self_data);
  gtk_widget_add_accelerator(item_tab_next, "activate", self->accelgroup, GDK_Page_Down, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(item_tab_next, "activate", self->accelgroup, GDK_Tab, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_tab_prev = gtk_image_menu_item_new_with_label ("Previous Tab");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_tab_prev), gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_tab_prev, "activate", G_CALLBACK (prev_tab), self_data);
  gtk_widget_add_accelerator(item_tab_prev, "activate", self->accelgroup, GDK_Page_Up, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* item_his = gtk_image_menu_item_new_with_label ("Show History...");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_his), gtk_image_new_from_icon_name("appointment-new",GTK_ICON_SIZE_MENU));
  g_signal_connect (item_his, "activate", G_CALLBACK (show_his), self_data);

  GtkWidget* item_about = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT,self->accelgroup);
  g_signal_connect (item_about, "activate", G_CALLBACK (about_cb), self_data);

  DevchatCBData* format_b = devchat_cb_data_new (self, "b");

  GtkWidget* item_bold = gtk_image_menu_item_new_from_stock (GTK_STOCK_BOLD,self->accelgroup);
  g_signal_connect (item_bold, "activate", G_CALLBACK (btn_format), format_b);
  gtk_widget_add_accelerator(item_bold, "activate", self->accelgroup, GDK_B, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_i = devchat_cb_data_new (self, "i");

  GtkWidget* item_italic = gtk_image_menu_item_new_from_stock (GTK_STOCK_ITALIC,self->accelgroup);
  g_signal_connect (item_italic, "activate", G_CALLBACK (btn_format), format_i);
  gtk_widget_add_accelerator(item_italic, "activate", self->accelgroup, GDK_I, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_u = devchat_cb_data_new (self, "u");

  GtkWidget* item_line = gtk_image_menu_item_new_from_stock (GTK_STOCK_UNDERLINE,self->accelgroup);
  g_signal_connect (item_line, "activate", G_CALLBACK (btn_format), format_u);
  gtk_widget_add_accelerator(item_line, "activate", self->accelgroup, GDK_U, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_img = devchat_cb_data_new (self, "img");

  GtkWidget* item_pict = gtk_image_menu_item_new_with_mnemonic ("I_mage");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_pict), gtk_image_new_from_icon_name("image-x-generic",GTK_ICON_SIZE_MENU));
  g_signal_connect (item_pict, "activate", G_CALLBACK (btn_format), format_img);
  gtk_widget_add_accelerator(item_pict, "activate", self->accelgroup, GDK_M, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  DevchatCBData* format_url = devchat_cb_data_new (self, "url");

  GtkWidget* item_link = gtk_image_menu_item_new_with_mnemonic ("_Link");
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (item_link), gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,GTK_ICON_SIZE_MENU));
  g_signal_connect (item_link, "activate", G_CALLBACK (btn_format), format_url);
  gtk_widget_add_accelerator(item_link, "activate", self->accelgroup, GDK_L, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  self->item_smilies = gtk_menu_item_new_with_mnemonic ("_Smilies...");
  GtkWidget* item_presets = gtk_menu_item_new_with_mnemonic ("_Preset texts...");
  /*TODO: Preset-Menü füllen.*/

  GtkMenuShell* main_sub = GTK_MENU_SHELL(gtk_menu_new());
  gtk_menu_shell_append(main_sub, self->item_connect);
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
  gtk_menu_shell_append(insert_sub, item_presets);
  gtk_menu_item_set_submenu(menu_insert,GTK_WIDGET(insert_sub));

  GtkMenuShell* edit_sub = GTK_MENU_SHELL(gtk_menu_new());
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

  self->statusbar = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(self->statusbar),TRUE);
  self->statuslabel = gtk_label_new ("Not connected.");
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
  gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(self->outputwidget), 2);
  g_signal_connect (self->output, "mark-set", G_CALLBACK(on_mark_set),self);
  g_signal_connect (self->outputwidget, "motion-notify-event", G_CALLBACK(on_motion),self_data);

  GtkWidget* scroller1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller1),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroller1),GTK_SHADOW_ETCHED_IN);
  gtk_container_add(GTK_CONTAINER(scroller1),self->outputwidget);
  gtk_paned_pack1 (GTK_PANED(hpaned1), scroller1, TRUE,TRUE);

  GtkWidget* scroller2 = gtk_scrolled_window_new (NULL, NULL);
  self->userlist = gtk_vbox_new (FALSE,1);
  gtk_widget_set_size_request (self->userlist, 180, -1);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(scroller2), self->userlist);

  if (self->settings.coloruser == TRUE)
  {
    gtk_widget_modify_bg (gtk_bin_get_child(GTK_BIN(scroller2)), GTK_STATE_NORMAL, &l1);
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
  gtk_box_pack_start(GTK_BOX(self->inputbar),self->level_box,FALSE,FALSE,0);
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
  gtk_button_set_image (GTK_BUTTON (btn_img), gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,GTK_ICON_SIZE_SMALL_TOOLBAR));
  gtk_widget_set_tooltip_text (btn_img, "Insert Image\nNote that you will be killed if you insert images larger than 32*32px. You have been warned.");

  g_signal_connect (btn_bold, "clicked", G_CALLBACK (btn_format), format_b);
  g_signal_connect (btn_ital, "clicked", G_CALLBACK (btn_format), format_i);
  g_signal_connect (btn_line, "clicked", G_CALLBACK (btn_format), format_u);
  g_signal_connect (btn_url, "clicked", G_CALLBACK (btn_format), format_url);
  g_signal_connect (btn_img, "clicked", G_CALLBACK (btn_format), format_img);

  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_bold,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_ital,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_line,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_url,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX(self->inputbar),btn_img,FALSE,FALSE,0);

  GtkWidget* btn_ok = gtk_button_new_from_stock(GTK_STOCK_OK);
  g_signal_connect (btn_ok, "clicked", G_CALLBACK (btn_send),self_data);
  gtk_widget_add_accelerator(btn_ok, "clicked", self->accelgroup, GDK_Return, 0, 0);

  GtkWidget* btn_quit = gtk_button_new_from_stock(GTK_STOCK_QUIT);
  g_signal_connect (btn_quit, "clicked", G_CALLBACK (destroy),self_data);
  gtk_widget_add_accelerator(btn_quit, "clicked", self->accelgroup, GDK_Q, GDK_CONTROL_MASK, 0);

  gtk_box_pack_end (GTK_BOX(self->inputbar),btn_quit,FALSE,FALSE,0);
  gtk_box_pack_end (GTK_BOX(self->inputbar),gtk_vseparator_new(),FALSE,FALSE,0);
  gtk_box_pack_end (GTK_BOX(self->inputbar),btn_ok,FALSE,FALSE,0);

  gtk_box_pack_start (GTK_BOX(vbox2),self->inputbar,FALSE,FALSE,0);


  GtkWidget* scroller3 = gtk_scrolled_window_new (NULL,NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroller3),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroller3),GTK_SHADOW_ETCHED_IN);

  self->inputwidget = gtk_text_view_new ();
  self->input = gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->inputwidget));
  gtk_widget_modify_base (self->inputwidget, GTK_STATE_NORMAL, &l1);
  gtk_widget_modify_text (self->inputwidget, GTK_STATE_NORMAL, &font);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(self->inputwidget), GTK_WRAP_WORD_CHAR);
  gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(self->inputwidget), 2);
  gtk_container_add(GTK_CONTAINER(scroller3),self->inputwidget);

  gtk_box_pack_start (GTK_BOX(vbox2),scroller3,TRUE,TRUE,0);

  create_tags (self->output, self_data);

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

  gtk_box_pack_start (GTK_BOX(hbox2),self->user_entry,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX(hbox2),self->pass_entry,TRUE,TRUE,0);
  gtk_box_pack_start (GTK_BOX(hbox2),self->btn_connect,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (self->loginbar), hbox2,FALSE,FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox2), self->loginbar,FALSE,FALSE,0);

  GtkWidget* vpaned = gtk_vpaned_new ();
  gtk_paned_pack1 (GTK_PANED(vpaned),hpaned1,TRUE,TRUE);
  gtk_paned_pack2 (GTK_PANED(vpaned),vbox2,FALSE,FALSE);

  gtk_notebook_append_page_menu(GTK_NOTEBOOK(self->notebook),vpaned,gtk_label_new("X-DEVCHAT"),gtk_label_new("X-DEVCHAT"));

  gtk_widget_show_all (self->window);
  gtk_widget_hide_all (self->inputbar);

  gtk_window_resize (GTK_WINDOW (self->window), self->settings.width,self->settings.height);
  gtk_paned_set_position( GTK_PANED(vpaned),-1);

  gtk_widget_grab_focus (self->user_entry);

#ifdef NOTIFY
  notify_init(APPNAME);
#endif

  self->users_without_avatar = NULL;
  self->firstrun = TRUE;
  self->no_halt_requested = TRUE;
  self->lastid = g_strdup("1");
#ifdef DEBUG
  dbg("Initalising libsoup...");
#endif
  self->session = soup_session_async_new ();
  soup_session_add_feature (self->session, SOUP_SESSION_FEATURE(soup_cookie_jar_new()));
}

static void
devchat_window_class_init (DevchatWindowClass* klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
}

void destroy (GtkWidget* widget, DevchatCBData* data)
{
  data->window->no_halt_requested = FALSE;
#ifdef NOTIFY
  notify_uninit ();
#endif
  if (!(data->window->firstrun))
  {
    soup_session_abort (data->window->session);
  }
  gtk_main_quit ();
}

void login (GtkWidget* widget, DevchatCBData* data)
{
  gtk_widget_set_sensitive(data->window->btn_connect,FALSE);
  gtk_widget_set_sensitive(data->window->user_entry,FALSE);
  gtk_widget_set_sensitive(data->window->pass_entry,FALSE);
#ifdef DEBUG
  dbg ("Logging in...");
#endif
  gtk_label_set_text (GTK_LABEL (data->window->statuslabel), "Logging in...");
  data->window->settings.user = g_strdup (gtk_entry_get_text(GTK_ENTRY(data->window->user_entry)));
  data->window->settings.pass = g_strdup (gtk_entry_get_text(GTK_ENTRY(data->window->pass_entry)));
  SoupMessage* loginparams = soup_form_request_new("POST", "http://forum.egosoft.com/login.php","username",data->window->settings.user,"password",data->window->settings.pass,"autologin","on","redirect","","webroot","0","login","Log in",NULL);
  soup_session_queue_message (data->window->session, loginparams, SOUP_SESSION_CALLBACK (login_cb), data);
}

void login_cb (SoupSession* session, SoupMessage* msg, DevchatCBData* data)
{
#ifdef DEBUG
  dbg ("Got login response from server.");
#endif
  if (g_strrstr(msg->response_body->data,"invalid password"))
  {
    err ("Login failed.");
    gtk_label_set_text (GTK_LABEL (data->window->statuslabel), "Login failed.");
    gtk_widget_set_sensitive(data->window->btn_connect,TRUE);
    gtk_widget_set_sensitive(data->window->user_entry,TRUE);
    gtk_widget_set_sensitive(data->window->pass_entry,TRUE);
  }
  else
  {
  #ifdef DEBUG
    dbg ("Login successful.");
  #endif
    gtk_label_set_text (GTK_LABEL (data->window->statuslabel), "Login successful! Determining user level...");
    SoupMessage* step2 = soup_message_new("GET","http://www.egosoft.com");
  #ifdef DEBUG
    dbg ("Trying to determine userlevel...");
  #endif
    soup_session_queue_message (data->window->session, step2, SOUP_SESSION_CALLBACK(remote_level), data);
  }
}

void remote_level (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  if (g_strrstr (m->response_body->data,"User-Level: 5"))
  {
    data->window->userlevel = 5;
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 1");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 3");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 5");
    gtk_combo_box_set_active ( GTK_COMBO_BOX(data->window->level_box), 0);
    gtk_widget_show (data->window->level_box);
    gtk_widget_show (data->window->item_l3);
    gtk_widget_show (data->window->item_l5);
  }
  else if (g_strrstr (m->response_body->data,"User-Level: 3"))
  {
    data->window->userlevel = 3;
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 1");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 3");
    gtk_combo_box_set_active ( GTK_COMBO_BOX(data->window->level_box), 0);
    gtk_widget_show (data->window->level_box);
    gtk_widget_show (data->window->item_l3);
  }
  else if (g_strrstr (m->response_body->data,"User-Level: "))
  {
    data->window->userlevel = 6;
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 1");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 3");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 5");
    gtk_combo_box_append_text ( GTK_COMBO_BOX(data->window->level_box), "Level 6");
    gtk_combo_box_set_active ( GTK_COMBO_BOX(data->window->level_box), 0);
    gtk_widget_show (data->window->level_box);
    gtk_widget_show (data->window->item_l3);
    gtk_widget_show (data->window->item_l5);
  }
  else
  {
    data->window->userlevel = 1;
  }
#ifdef DEBUG
  gchar* dbg_msg = g_strdup_printf("Determined userlevel to be %i.", data->window->userlevel);
  dbg (dbg_msg);
  g_free (dbg_msg);
#endif

  g_signal_connect(data->window->window, "key-press-event", G_CALLBACK (hotkey_cb), data);
  gtk_widget_grab_focus(data->window->inputwidget);
  gtk_widget_hide_all (data->window->loginbar);
  gtk_widget_show_all (data->window->inputbar);
  gtk_widget_hide (data->window->item_connect);
  gtk_widget_show (data->window->item_reconnect);
#ifdef DEBUG
  dbg ("Starting requests...");
#endif
  gtk_label_set_text (GTK_LABEL (data->window->statuslabel), "Waiting for messages...");
  g_timeout_add ((data->window->settings.update_time * 2), (GSourceFunc) user_list_poll, data);
  g_timeout_add (data->window->settings.update_time, (GSourceFunc) message_list_poll, data);
}

gboolean
user_list_poll (DevchatCBData* data)
{
  if (data->window->no_halt_requested)
  {
  #ifdef DEBUG
    dbg ("Starting user list poll...");
  #endif
    SoupMessage* listusers = soup_message_new ("GET","http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?users=1");
    soup_session_queue_message (data->window->session, listusers, SOUP_SESSION_CALLBACK (user_list_get), data);
  }
  return FALSE;
}

gboolean
message_list_poll (DevchatCBData* data)
{
  if (data->window->no_halt_requested)
  {
  #ifdef DEBUG
    dbg ("Starting message list poll...");
  #endif
    SoupMessage* listmessages = soup_message_new ("GET",g_strdup_printf("http://www.egosoft.com/x/questsdk/devchat/obj/request.obj?lid=%s",data->window->lastid));
    soup_session_queue_message (data->window->session, listmessages, SOUP_SESSION_CALLBACK (message_list_get), data);
  }
  return FALSE;
}

void user_list_clear_cb (GtkWidget* child, DevchatCBData* data)
{
  gtk_container_remove (GTK_CONTAINER (data->window->userlist), child);
}

void user_list_get (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  g_timeout_add ((data->window->settings.update_time * 2), (GSourceFunc) user_list_poll, data);

  gchar* userlist = g_strdup (m->response_body->data);
  if (userlist)
  {
  #ifdef DEBUG
    dbg ("Got non-empty userlist.");
  #endif
    gchar* dbg_msg = g_strdup_printf("Last Update: %s",current_time());
    gtk_label_set_text (GTK_LABEL (data->window->statuslabel), dbg_msg);
    g_free (dbg_msg);

    xmlTextReaderPtr userparser = xmlReaderForMemory (userlist,strlen(userlist),"",NULL,(XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_NONET));
    xmlParserCtxtPtr ctxt = xmlCreateDocParserCtxt (userlist);

    GdkColor l1;
    gdk_color_parse (data->window->settings.color_l1, &l1);

    gtk_container_foreach (GTK_CONTAINER (data->window->userlist), (GtkCallback) user_list_clear_cb, data);

    /*TODO: Sizegroup for all Avatar buttons (to enforce fixed width).*/

    guint usercount = 0;
    while (xmlTextReaderRead (userparser) > 0)
    {
      gchar* node = xmlTextReaderLocalName(userparser);

      if (node && (g_strcmp0 (node,"cu") == 0))
      {
        usercount++;
        gchar* name = xmlTextReaderGetAttribute(userparser,"n");
        gchar* uid = xmlTextReaderGetAttribute(userparser,"uid");
        gchar* level = xmlTextReaderGetAttribute(userparser,"l");
        gchar* status = xmlTextReaderGetAttribute(userparser,"s");

        /*TODO: Hashtable für n->uid-Zuordnung füllen, falls nötig. */

        if ((g_strcmp0("Away: STEALTH",status) != 0) || (data->window->settings.showhidden))
        {

          if (!g_slist_find_custom (data->window->users_without_avatar,uid, (GCompareFunc) user_lookup) && !g_hash_table_lookup (data->window->avatars, uid))
          {
            gchar* ava_filename = g_build_filename (data->window->avadir,uid,NULL);

          #ifdef DEBUG
            dbg_msg = g_strdup_printf ("Searching for avatar %s...", ava_filename);
            dbg (dbg_msg);
            g_free (dbg_msg);
          #endif

            if (!g_file_test (ava_filename, G_FILE_TEST_EXISTS))
            {
            #ifdef DEBUG
              dbg_msg = g_strdup_printf ("Avatar %s not found. Searching...",uid);
              dbg (dbg_msg);
              g_free (dbg_msg);
            #endif

              SoupMessage* ava_get = soup_message_new ("GET",g_strdup_printf("http://forum.egosoft.com/profile.php?mode=viewprofile&u=%s",uid));
              soup_session_queue_message (data->window->session, ava_get, SOUP_SESSION_CALLBACK (search_ava_cb), devchat_cb_data_new (data->window, g_strdup(uid)));
            #ifdef DEBUG
              dbg ("Search request queued, will be executed when idling.");
            #endif
              data->window->users_without_avatar = g_slist_prepend (data->window->users_without_avatar,g_strdup(uid));
            }
            else
            {
            #ifdef DEBUG
              dbg_msg = g_strdup_printf ("Found avatar for %s",name);
              dbg (dbg_msg);
              g_free (dbg_msg);
            #endif

              g_hash_table_insert (data->window->avatars, g_strdup(uid), gdk_pixbuf_new_from_file_at_size (ava_filename,data->window->settings.avatar_size,data->window->settings.avatar_size,NULL));
            }
            g_free (ava_filename);
          }
        #ifdef DEBUG
          dbg_msg = g_strdup_printf("Adding user %s.",name);
          dbg (dbg_msg);
          g_free (dbg_msg);
        #endif

          GtkWidget* label = gtk_label_new(NULL);
          GtkWidget* container = gtk_hbox_new(FALSE,0);
          GtkWidget* at_btn = gtk_button_new();
          GtkWidget* profile_btn = gtk_button_new();
          GtkWidget* pm_btn = gtk_button_new_with_label("PM");

          gulong real_level = strtoll(g_strndup(level,1),NULL,10);
          gchar* color;
          gchar* style;
          gchar* strike;


          gtk_button_set_relief (GTK_BUTTON (profile_btn), GTK_RELIEF_NONE);

          gchar* at_text = g_strdup_printf ("View the forum profile of %s.",name);
          gtk_widget_set_tooltip_text (at_btn, at_text);
          g_free (at_text);

          GdkPixbuf* ava = (GdkPixbuf*) g_hash_table_lookup (data->window->avatars, uid);

          if (!(ava))
            ava = (GdkPixbuf*) g_hash_table_lookup (data->window->avatars, "default");

          gtk_button_set_image (GTK_BUTTON (profile_btn), gtk_image_new_from_pixbuf (ava));

          if (real_level > 5)
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
              status_d = g_strdup ("Do NOT disturb.");
            }
            else
            {
              status_d = xmlStringDecodeEntities (ctxt, status, XML_SUBSTITUTE_BOTH, 0,0,0);
              strike = "false";
            }
            gtk_widget_set_tooltip_text(at_btn, status_d);
            g_free (status_d);
          }
          else
          {
            style = "normal";
            strike = "false";
            gchar* at_text = g_strdup_printf ("Poke %s",name);
            gtk_widget_set_tooltip_text(at_btn, at_text);
            g_free (at_text);
          }
          gchar* markup = g_markup_printf_escaped ("<span foreground='%s' style='%s' strikethrough='%s'>%s</span> <span foreground='%s'>(%s)</span>",color,style,strike,name,data->window->settings.color_font,level);
          gtk_label_set_markup (GTK_LABEL (label),markup);
          g_free (markup);

          g_signal_connect (at_btn, "clicked", G_CALLBACK (at_cb), devchat_cb_data_new (data->window,g_strdup(name)));
          gtk_container_add (GTK_CONTAINER (at_btn), label);
          gtk_button_set_relief (GTK_BUTTON(at_btn), GTK_RELIEF_NONE);

          g_signal_connect (pm_btn, "clicked", G_CALLBACK (pm_cb), devchat_cb_data_new (data->window,g_strdup(name)));

          gchar* pm_text = g_strdup_printf ("Open a conversation with %s.",name);
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

        }
        g_free (uid);
        g_free (name);
        g_free (level);
        g_free (status);
        g_free (node);
      }
    }

    gchar* ul_text = g_strdup_printf ("%i user(s) online", usercount);
    gtk_label_set_text (GTK_LABEL (data->window->userlabel), ul_text);
    g_free (ul_text);

    gtk_widget_show_all (data->window->userlist);
    xmlFreeTextReader (userparser);
    xmlFreeParserCtxt (ctxt);
    g_free (userlist);
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
  #ifdef DEBUG
    gchar* dbg_msg = g_strdup_printf ("Got the forum profile of %s. Now searching for the avatar...",data->data);
    dbg (dbg_msg);
    g_free (dbg_msg);
  #endif

    GRegex* regex = g_regex_new ("<img src=\"http:\\/\\/.*\\.(jpg|png|gif)", G_REGEX_UNGREEDY, 0, NULL);
    gchar** profile_lines = g_strsplit (profile, "\n",-1);

    gchar* line;
    guint i;
    gchar* ava_url;

    for (i = 0; profile_lines[i] != NULL && !found; i++)
    {
      line = profile_lines[i];
      GMatchInfo* result;
      if (g_regex_match (regex, line, 0, &result))
      {
        gchar* match = g_match_info_fetch(result,0);

        if (!g_str_has_prefix(match,"<img src=\"http://www.egosoft.com/") && !g_str_has_prefix(match,"<img src=\"http://stats.big-boards.com/"))
        {
        #ifdef DEBUG
          dbg_msg = g_strdup_printf ("Found something remotely resembling an avatar: %s", match+10);
          dbg (dbg_msg);
          g_free (dbg_msg);
        #endif
          found = TRUE;
          ava_url = g_strdup (match+10);
        }
        g_free (match);
      }
      g_match_info_free (result);
    }

    if(found)
    {
    #ifdef DEBUG
      dbg_msg = g_strdup_printf ("Now commencing avatar write: %s", ava_url);
      dbg (dbg_msg);
      g_free (dbg_msg);
    #endif
      data->window->users_without_avatar = g_slist_delete_link (data->window->users_without_avatar,
        g_slist_find_custom (data->window->users_without_avatar,data->data, (GCompareFunc) user_lookup));
      SoupMessage* a_m = soup_message_new ("GET", ava_url);
      if (soup_session_send_message (s, a_m) == 200)
      {
        /*XXX: Workaround for imageshack being TOO MOTHERFUCKING RETARDED to return a 404.*/
        if (g_strcmp0 ("404", a_m->response_body->data) != 0)
        {
          GError* err = NULL;
          gchar* filename = g_build_filename (data->window->avadir,data->data,NULL);
          if (!g_file_set_contents (filename, a_m->response_body->data, a_m->response_body->length, &err))
          {
            g_warning (err->message);
            g_error_free (err);
          }
          g_free (filename);
        }
      }
    #ifdef DEBUG
      else
      {
        dbg ("Error downloading avatar!");
      }
    #endif
    }

    g_regex_unref (regex);
    g_strfreev (profile_lines);

    g_free (profile);

  #ifdef DEBUG
    dbg ("Avatar search done.");
  #endif
  }
}

void message_list_get (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{
  gchar* msglist = g_strdup (m->response_body->data);
  if (msglist)
  {
    ce_parse (msglist, data, "");
  #ifdef DEBUG
    dbg ("Parsing done.");
  #endif
  }
  g_timeout_add (data->window->settings.update_time, (GSourceFunc) message_list_poll, data);
}

void ce_parse (gchar* msglist, DevchatCBData* self, gchar* date)
{
  if (g_strcmp0 (date, "") == 0)
  {
    gchar* labeltext = g_strdup_printf("Last Update: %s",current_time());
    gtk_label_set_text (GTK_LABEL (self->window->statuslabel), labeltext);
    g_free (labeltext);

    GtkTextIter old_end;
    gtk_text_buffer_get_end_iter (self->window->output, &old_end);
    GtkTextMark* scroll_to = gtk_text_mark_new ("scrollTo", FALSE);
    gtk_text_buffer_add_mark (self->window->output, scroll_to, &old_end);

    xmlTextReaderPtr msgparser = xmlReaderForMemory (msglist, strlen (msglist), "", NULL, (XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_NONET));

    while (xmlTextReaderRead (msgparser) == 1)
    {
      gchar* node = xmlTextReaderLocalName(msgparser);

      if (node && (g_strcmp0 (node,"ce") == 0))
      {
      #ifdef DEBUG
        dbg ("Processing message node...");
      #endif
        gchar* name = xmlTextReaderGetAttribute(msgparser,"a");
        gchar* mode = xmlTextReaderGetAttribute(msgparser,"u");
        gchar* time = xmlTextReaderGetAttribute(msgparser,"t");
        gchar* lid = xmlTextReaderGetAttribute(msgparser,"i");
        gchar* message = xmlTextReaderGetAttribute(msgparser,"m");

        g_free (self->window->lastid);
        self->window->lastid = g_strdup (lid);

      #ifdef DEBUG
        gchar* dbg_msg = g_strdup_printf ("Message parameters: username %s, mode %s, time %s, lid %s, message %s.\n", name, mode, time, lid, message);
        dbg (dbg_msg);
        g_free (dbg_msg);
      #endif

        gint show_name = g_strcmp0 ("0", g_strndup(mode,1));
        gulong user_level = strtoll (g_strndup(mode+1,1),NULL,10);
        gulong msg_level = strtoll (mode+2,NULL,10);

        GtkTextIter end;
        GtkTextTagTable* table = gtk_text_buffer_get_tag_table (self->window->output);
        gtk_text_buffer_get_end_iter (self->window->output, &end);

        GtkTextMark* old_start = gtk_text_mark_new (name, TRUE);
        gtk_text_buffer_add_mark (self->window->output, old_start, &end);

        gchar* tagname = g_strconcat ("user-", name, NULL);

        if (!gtk_text_tag_table_lookup (table, tagname))
          gtk_text_buffer_create_tag (self->window->output, tagname, NULL);


        gtk_text_buffer_get_end_iter (self->window->output, &end);
        gchar* time_t = g_strdup_printf ("\n%s", time);
        gtk_text_buffer_insert_with_tags (self->window->output, &end, time_t, -1, gtk_text_tag_table_lookup (table, "time"), NULL);
        g_free (time_t);

        gchar* name_color_tag = "peasant";
        if (user_level > 5)
          name_color_tag = "greenie";

        if (show_name != 0)
        {
          gtk_text_buffer_get_end_iter (self->window->output, &end);
          gchar* name_t = g_strdup_printf (" %s: ",name);

          gtk_text_buffer_insert_with_tags (self->window->output, &end, name_t, -1, gtk_text_tag_table_lookup (table, name_color_tag), NULL);
          g_free (name_t);
        }

        /*XXX: TODO: Check for keyword match and/or kick. Pass the current output buffer as argument.*/
        gtk_text_buffer_get_end_iter (self->window->output, &end);
        gchar* message_t = g_strdup_printf ("<html><body>%s</body></html>", message);
        htmlDocPtr doc = htmlParseDoc (message_t, "utf8");
        htmlParserCtxtPtr ptr = htmlCreateMemoryParserCtxt (message_t, strlen(message_t));
        htmlNodePtr root = xmlDocGetRootElement(doc);
        parse_message (root, devchat_cb_data_new (self->window, self->window->output), doc, ptr);
        xmlFreeDoc (doc);
        xmlFreeParserCtxt (ptr);
        g_free (message_t);

        GtkTextIter start;
        gtk_text_buffer_get_iter_at_mark (self->window->output, &start, old_start);
        gtk_text_buffer_get_end_iter (self->window->output, &end);

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
          case 1: taglevel = g_strdup ("l1"); break;
          case 3: taglevel = g_strdup ("l3"); break;
          case 5: taglevel = g_strdup ("l5"); break;
          default: taglevel = g_strdup ("l6"); break;
        }

        gtk_text_buffer_apply_tag_by_name (self->window->output, taglevel, &start, &end);
        if (tagulevel)
          gtk_text_buffer_apply_tag_by_name (self->window->output, tagulevel, &start, &end);
        gtk_text_buffer_apply_tag_by_name (self->window->output, tagname, &start, &end);

        g_free (taglevel);
        g_free (tagulevel);
        gtk_text_buffer_delete_mark (self->window->output, old_start);
        g_free (name);
        g_free (tagname);
        g_free (mode);
        g_free (time);
        g_free (lid);
        g_free (message);
      }

      g_free (node);
    }
  #ifdef DEBUG
    dbg ("Message list parsed.");
  #endif

    /*TODO: Nur scrollen, wenn firstrun oder User nicht manuell hochgescrollt hat!*/
    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (self->window->outputwidget), scroll_to);
    gtk_text_buffer_delete_mark (self->window->output, scroll_to);

    self->window->firstrun = FALSE;
    xmlFreeTextReader (msgparser);
  }
  else
  {
    //dbg ("Creating history window...");
    /* TODO */
  }
  g_free (msglist);
}

void create_tags (GtkTextBuffer* buf, DevchatCBData* data)
{
  gtk_text_buffer_create_tag (buf, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_buffer_create_tag (buf, "italic", "style", PANGO_STYLE_ITALIC, NULL);
  gtk_text_buffer_create_tag (buf, "underline", "underline", PANGO_UNDERLINE_SINGLE, NULL);
  gtk_text_buffer_create_tag (buf, "time", "foreground", data->window->settings.color_time, NULL);
  gtk_text_buffer_create_tag (buf, "peasant", "foreground", data->window->settings.color_blues, NULL);
  gtk_text_buffer_create_tag (buf, "greenie", "foreground", data->window->settings.color_greens, NULL);
  gtk_text_buffer_create_tag (buf, "l1", "background", data->window->settings.color_l1, NULL);
  gtk_text_buffer_create_tag (buf, "l3", "background", data->window->settings.color_l3, NULL);
  gtk_text_buffer_create_tag (buf, "l5", "background", data->window->settings.color_l5, NULL);
  gtk_text_buffer_create_tag (buf, "l6", "background", data->window->settings.color_l6, NULL);
  gtk_text_buffer_create_tag (buf, "ul1", NULL);
  gtk_text_buffer_create_tag (buf, "ul3", NULL);
  gtk_text_buffer_create_tag (buf, "ul5", NULL);
  gtk_text_buffer_create_tag (buf, "ul6", NULL);
}

void parse_message (htmlNodePtr element, DevchatCBData* data, htmlDocPtr doc, htmlParserCtxtPtr ptr)
{
  /*XXX: visited-Attribut von URL-Tags per g_object_set/get_data*/
  htmlNodePtr node;
  GtkTextTagTable* table = gtk_text_buffer_get_tag_table (data->data);

  for (node = element; node != NULL; node = node->next)
  {
    if (node->type == XML_ELEMENT_NODE)
    {
      GtkTextIter end;
      gtk_text_buffer_get_end_iter (data->data, &end);
      GtkTextMark* old_start = gtk_text_mark_new (NULL, TRUE);
      gtk_text_buffer_add_mark (data->data, old_start, &end);

      GSList* smilie_list = NULL;

      gchar* tag = NULL;

      if (g_ascii_strcasecmp (node->name, "img") == 0)
      {
        gchar* alt = NULL;
        gchar* src = NULL;
        /*TODO: Search for alt tag first, if exists, replace by local smilie. Else, check whether downloaded, if yes,
                load, else download and load. */
        xmlAttrPtr attr;
        for (attr = node->properties; attr != NULL; attr = attr->next)
        {
          if (g_ascii_strcasecmp (attr->name, "src") == 0)
          {
            src = attr->children->content;

          #ifdef DEBUG
            gchar* dbg_msg = g_strdup_printf ("Found image source: %s", src);
            dbg (dbg_msg);
            g_free (dbg_msg);
          #endif
          }
          else if (g_ascii_strcasecmp (attr->name, "alt") == 0)
          {
            alt = g_strstrip (g_strdup (attr->children->content));

          #ifdef DEBUG
            gchar* dbg_msg = g_strdup_printf ("Found image description: %s", alt);
            dbg (dbg_msg);
            g_free (dbg_msg);
          #endif
          }
        }

        /*TODO: Lookup alt in avatar table, add avatar to buffer / dl image and add that.*/
        if (alt)
        {
          GdkPixbuf* smilie = g_hash_table_lookup (data->window->smilies, alt);

          if (smilie)
          {
            smilie_list = g_slist_prepend (smilie_list, smilie);
            g_printf ("Added smilie %s\n", alt);
          }
        }
      }
      else if (g_ascii_strcasecmp (node->name, "br") == 0)
      {
        /*TODO*/
      }
      else if (g_ascii_strcasecmp (node->name, "b") == 0)
      {
        tag = "bold";
      }
      else if (g_ascii_strcasecmp (node->name, "i") == 0)
      {
        tag = "italic";
      }
      else if (g_ascii_strcasecmp (node->name, "u") == 0)
      {
        tag = "underline";
      }
      else if (g_ascii_strcasecmp (node->name, "font") == 0)
      {
        gchar* colorname;

        xmlAttrPtr attr;
        for (attr = node->properties; attr != NULL; attr = attr->next)
        {
          if (g_ascii_strcasecmp (attr->name, "color") == 0)
            colorname = g_strndup (attr->children->content,7);
        }

        if (colorname)
        {
          colorname = color_lookup (colorname);
          tag = g_strconcat ("color-", colorname, NULL);

          if (!gtk_text_tag_table_lookup (table, tag))
            gtk_text_buffer_create_tag (data->data, tag, "foreground", colorname, NULL);
          g_free (colorname);
        }

      }
      else if (g_ascii_strcasecmp (node->name, "span") == 0)
      {
        gchar* style;

        xmlAttrPtr attr;
        for (attr = node->properties; attr != NULL; attr = attr->next)
        {
          if (g_ascii_strcasecmp (attr->name, "class") == 0)
          {
            style = g_strdup (attr->children->content);

          #ifdef DEBUG
            gchar* dbg_msg = g_strdup_printf ("Found span style: %s", style);
            dbg (dbg_msg);
            g_free (dbg_msg);
          #endif
          }
        }

        if (style)
        {
          if (g_strcmp0 (style, "chatname_green") == 0)
          {
            tag = g_strdup ("greenie");
          }
          g_free (style);
        }

      }
      else if (g_ascii_strcasecmp (node->name, "a") == 0)
      {
        /* TODO: Build URL. */
      }

      /* Find a way to get the position of the child elements inside the content and insert them in that position. */
      parse_message (node->children, data, doc, ptr);

      gchar* content = xmlNodeListGetString (doc, node->children, 1);
      if (content)
      {
        GtkTextIter cur_end;
        gtk_text_buffer_get_end_iter (data->data, &cur_end);
        if (tag)
        {
          gtk_text_buffer_insert_with_tags_by_name (data->data, &cur_end, content, -1, tag, NULL);
        }
        else
          gtk_text_buffer_insert (data->data, &cur_end, content, -1);
        g_free (content);
      }

      if (smilie_list)
      {
        smilie_list = g_slist_reverse (smilie_list);

        for (smilie_list; smilie_list != NULL; smilie_list = g_slist_next (smilie_list))
        {
          GtkTextIter pos;
          gtk_text_buffer_get_end_iter (data->data, &pos);
          g_print ("Inserting smilie...");

          gtk_text_buffer_insert_pixbuf (data->data, &pos, smilie_list->data);
        }

        //g_slist_free (smilie_list);
      }



    }
  }
}

gboolean hotkey_cb (GtkWidget* w, DevchatCBData* data)
{
  /* TODO */
  return FALSE;
}

gchar* color_lookup (gchar* color)
{
  /*TODO: Substitute colors based on current theme.*/
  return color;
}

void config_cb(GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void go_forum(GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void close_tab(GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void reconnect(GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void tab_changed(GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

gboolean tab_changed_win(GtkWidget* widget, DevchatCBData* data)
{
  return FALSE;
}

void on_motion(GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void on_mark_set(GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void level_changed (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void btn_send (GtkWidget* widget, DevchatCBData* data)
{
#ifdef DEBUG
  dbg ("Sending message...");
#endif
  gint pagenum = gtk_notebook_get_current_page (GTK_NOTEBOOK (data->window->notebook));
  GtkTextBuffer* buf;
  gchar* text;
  GtkTextIter start;
  GtkTextIter end;

  if (pagenum == 0)
  {
  #ifdef DEBUG
    dbg ("Sending message to main channel.");
  #endif
    buf = data->window->input;
    gtk_text_buffer_get_start_iter (buf, &start);
    gtk_text_buffer_get_end_iter (buf, &end);
    text = g_strdup (gtk_text_buffer_get_text (buf, &start, &end, FALSE));
  }
  else
  {
  #ifdef DEBUG
    dbg ("Sending PM.");
  #endif
  }
  text = g_strstrip (text);

  if (g_strcmp0("",text) != 0)
  {
    /*TODO: Linebuffer füllen.*/
    gtk_text_buffer_set_text (buf, "", 0);
    unsigned char enc_text[strlen(text)*10];
    int il,ol;
    ol = strlen(text)*7;
    il = strlen(text);
    if (htmlEncodeEntities (enc_text, &ol, text, &il, 0) < 0)
      g_error ("Encoding failed!");
    enc_text[ol] = 0;

    gint level = gtk_combo_box_get_active (GTK_COMBO_BOX (data->window->level_box));
    gchar* sendlevel;

    switch (level)
    {
      case -1:
      case 0: sendlevel = "1"; break;
      case 1: sendlevel = "3"; break;
      case 2: sendlevel = "5"; break;
      default: sendlevel = "6"; break;
    }

    SoupMessage* post = soup_form_request_new("GET", "http://www.egosoft.com/x/questsdk/devchat/obj/request.obj","cmd",
      "post","chatlevel",sendlevel,"textinput", enc_text, NULL);
    soup_session_queue_message (data->window->session, post, SOUP_SESSION_CALLBACK (post_sent), data);

    g_free (text);
  }
}

void post_sent (SoupSession* s, SoupMessage* m, DevchatCBData* data)
{}

void btn_format (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void next_tab (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}
void prev_tab (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}
void show_his (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}
void about_cb (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void at_cb (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void pm_cb (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void devchat_window_refresh_smilies (DevchatWindow* self)
{
  GtkWidget* smilie_sub = gtk_menu_new();
  g_hash_table_foreach (self->smilies, (GHFunc) add_smilie_cb, devchat_cb_data_new (self, smilie_sub));
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (self->item_smilies), smilie_sub);
  gtk_widget_show_all (self->item_smilies);
}

void add_smilie_cb (gpointer key, gpointer value, DevchatCBData* data)
{
  gchar* name = (gchar*) key;
  GdkPixbuf* icon = (GdkPixbuf*) value;

#ifdef DEBUG
  dbg (g_strdup_printf ("Adding smilie %s.", name));
#endif

  GtkWidget* item = gtk_image_menu_item_new_with_label (name);
  gtk_image_menu_item_set_image ( GTK_IMAGE_MENU_ITEM (item), gtk_image_new_from_pixbuf (icon));

  g_signal_connect (item, "activate", G_CALLBACK (ins_smilie), devchat_cb_data_new (data->window, name));
  gtk_menu_shell_append (data->data, item);
}

void ins_smilie (GtkWidget* widget, DevchatCBData* data)
{
  /*TODO*/
}

void notify(gchar* title, gchar* body, GdkPixbuf* icon, DevchatCBData* data)
{
  if (g_strcmp0(data->window->settings.vnotify,"<native>") == 0)
  {
#ifdef NOTIFY
    NotifyNotification* note = notify_notification_new(title,body,NULL,NULL);
    notify_notification_set_icon_from_pixbuf(note,icon);
    notify_notification_add_action(note, "0","Show",NOTIFY_ACTION_CALLBACK (notify_cb),data,NULL);
#else
    err("libnotify support disabled at compile time.");
    data->window->settings.vnotify = g_strdup("<none>");
#endif
  }
  else if (g_strcmp0(data->window->settings.vnotify,"<none>") != 0)
  {
    if (!g_spawn_async (NULL, (gchar**) g_strdup(data->window->settings.vnotify),NULL,G_SPAWN_SEARCH_PATH,NULL,NULL,NULL,NULL))
    {
      err("Failed to launch visual notification process.");
      data->window->settings.vnotify = g_strdup("<none>");
    }
  }

  if (g_strcmp0(data->window->settings.notify,"<native>") == 0)
  {
#ifdef G_OS_UNIX
  /*TODO: ALSA*/
#else
  #ifdef G_OS_WIN32
  /*TODO: DSound*/
  #endif
#endif
  }
  else if (g_strcmp0(data->window->settings.notify,"<none>") != 0)
  {
    if (!g_spawn_async (NULL, (gchar**) g_strdup(data->window->settings.notify),NULL,G_SPAWN_SEARCH_PATH,NULL,NULL,NULL,NULL))
    {
      err("Failed to launch audio notification process.");
      data->window->settings.notify = g_strdup("<none>");
    }
  }
}

void notify_cb(NotifyNotification* note, gchar* action, DevchatCBData* data)
{
  gtk_window_present(GTK_WINDOW(data->window->window));
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
