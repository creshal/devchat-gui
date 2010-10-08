/*
 * devchat_conversation.c
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

#include "devchat_conversation.h"

G_DEFINE_TYPE (DevchatConversation, devchat_conversation, G_TYPE_OBJECT)

DevchatConversation*
devchat_conversation_new (gboolean is_history, DevchatWindow* parent)
{
  DevchatConversation* obj = g_object_new (DEVCHAT_TYPE_CONVERSATION, NULL);

  obj->parent = parent;
  obj->scroll_to = NULL;

  if (!is_history)
    obj->child = gtk_vpaned_new ();

  DevchatCBData* parent_data = devchat_cb_data_new (parent, NULL);

  GdkColor l1;
  GdkColor font;
  gdk_color_parse (parent->settings.color_l1, &l1);
  gdk_color_parse (parent->settings.color_font, &font);


  GtkWidget* scroll_out = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll_out), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll_out), GTK_SHADOW_ETCHED_IN);
  obj->out_buffer = gtk_text_buffer_new (NULL);
  obj->out_widget = gtk_text_view_new_with_buffer (obj->out_buffer);
  g_object_set (obj->out_widget, "left-margin", 2, "right-margin", 2, NULL);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (obj->out_widget), GTK_WRAP_WORD_CHAR);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (obj->out_widget), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (obj->out_widget), FALSE);
  gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW (obj->out_widget), 2);
  gtk_widget_modify_base (obj->out_widget, GTK_STATE_NORMAL, &l1);
  gtk_widget_modify_text (obj->out_widget, GTK_STATE_NORMAL, &font);
  g_signal_connect (obj->out_buffer, "mark-set", G_CALLBACK (devchat_window_on_mark_set_cb), parent_data);
  g_signal_connect (obj->out_widget, "motion-notify-event", G_CALLBACK (devchat_window_on_motion_cb), parent_data);
  g_signal_connect (obj->out_widget, "button-press-event", G_CALLBACK (devchat_window_tab_changed_win), parent_data);
  gtk_container_add (GTK_CONTAINER (scroll_out), obj->out_widget);

  GtkWidget* search_box = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (search_box), scroll_out, TRUE, TRUE, 0);

  obj->searchbar = gtk_hbox_new (FALSE, 1);
  obj->search_entry = gtk_entry_new ();
  obj->search_button = gtk_button_new_from_stock (GTK_STOCK_FIND);
  g_signal_connect (obj->search_button, "clicked", G_CALLBACK (devchat_window_find), devchat_cb_data_new (parent, obj->search_entry));
  GtkWidget* btn_bar_close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  g_signal_connect (btn_bar_close, "clicked", G_CALLBACK (devchat_window_close_search), devchat_cb_data_new (parent, obj->searchbar));
  gtk_widget_add_accelerator (btn_bar_close, "clicked", parent->accelgroup, GDK_Escape, 0, 0);
  gtk_box_pack_start (GTK_BOX (obj->searchbar), obj->search_entry, TRUE, TRUE, 1);
  gtk_box_pack_start (GTK_BOX (obj->searchbar), obj->search_button, FALSE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX (obj->searchbar), btn_bar_close, FALSE, FALSE, 1);

  gtk_box_pack_start (GTK_BOX (search_box), obj->searchbar, FALSE, FALSE, 0);
  gtk_widget_set_no_show_all (obj->searchbar, TRUE);
  obj->search_start_set = FALSE;


  if (!is_history)
  {
    gtk_paned_pack1 (GTK_PANED (obj->child), search_box, TRUE, TRUE);
    GtkWidget* scroll_in = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll_in), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll_in), GTK_SHADOW_ETCHED_IN);
    obj->in_buffer = gtk_text_buffer_new (NULL);
    obj->in_widget = gtk_text_view_new_with_buffer (obj->in_buffer);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (obj->in_widget), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW (obj->in_widget), 2);
    gtk_widget_modify_base (obj->in_widget, GTK_STATE_NORMAL, &l1);
    gtk_widget_modify_text (obj->in_widget, GTK_STATE_NORMAL, &font);
    g_signal_connect (obj->in_widget, "button-press-event", G_CALLBACK (devchat_window_tab_changed_win), parent_data);

    gtk_container_add (GTK_CONTAINER (scroll_in), obj->in_widget);

  #ifdef SPELLCHECK
    if (!gtkspell_new_attach (GTK_TEXT_VIEW (obj->in_widget), "en_EN", NULL))
      err ("Error initialising spell checker!");
  #endif

    GtkWidget* vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_set_size_request (vbox, -1, 74);

    GtkWidget* hbox = gtk_hbox_new (FALSE, 0);
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
    gtk_widget_set_tooltip_text (btn_img, "Insert Image\nNote that you will be killed if you insert images larger than 32*32px. You have been warned.");

    DevchatCBData* format_b = devchat_cb_data_new (parent, "b");
    DevchatCBData* format_i = devchat_cb_data_new (parent, "i");
    DevchatCBData* format_u = devchat_cb_data_new (parent, "u");
    DevchatCBData* format_img = devchat_cb_data_new (parent, "img");
    DevchatCBData* format_url = devchat_cb_data_new (parent, "url");

    g_signal_connect (btn_bold, "clicked", G_CALLBACK (devchat_window_btn_format), format_b);
    g_signal_connect (btn_ital, "clicked", G_CALLBACK (devchat_window_btn_format), format_i);
    g_signal_connect (btn_line, "clicked", G_CALLBACK (devchat_window_btn_format), format_u);
    g_signal_connect (btn_url, "clicked", G_CALLBACK (devchat_window_btn_format), format_url);
    g_signal_connect (btn_img, "clicked", G_CALLBACK (devchat_window_btn_format), format_img);

    gtk_box_pack_start (GTK_BOX(hbox),btn_bold,FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX(hbox),btn_ital,FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX(hbox),btn_line,FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX(hbox),btn_url,FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX(hbox),btn_img,FALSE,FALSE,0);

    GtkWidget* color_box = gtk_combo_box_new_text ();
    g_signal_connect (color_box, "changed", G_CALLBACK (devchat_window_color_changed), parent_data);

    gtk_combo_box_append_text (GTK_COMBO_BOX (color_box), "Text color");
    gtk_combo_box_append_text (GTK_COMBO_BOX (color_box), "red");
    gtk_combo_box_append_text (GTK_COMBO_BOX (color_box), "green");
    gtk_combo_box_append_text (GTK_COMBO_BOX (color_box), "blue");
    gtk_combo_box_append_text (GTK_COMBO_BOX (color_box), "cyan");
    gtk_combo_box_append_text (GTK_COMBO_BOX (color_box), "magenta");
    gtk_combo_box_append_text (GTK_COMBO_BOX (color_box), "yellow");

    if (parent->userlevel > 5)
    {
      gtk_combo_box_append_text (GTK_COMBO_BOX(color_box), "chatname_green");
      gtk_combo_box_append_text (GTK_COMBO_BOX(color_box), "chatname_blue");
    }

    gtk_combo_box_set_active (GTK_COMBO_BOX (color_box), 0);

    gtk_box_pack_start (GTK_BOX(hbox),color_box,FALSE,FALSE,1);

    obj->btn_send = gtk_button_new_from_stock (GTK_STOCK_OK);
    g_signal_connect (obj->btn_send, "clicked", G_CALLBACK (devchat_window_btn_send), parent_data);
    gtk_widget_add_accelerator (obj->btn_send, "clicked", parent->accelgroup, GDK_Return, 0, 0);
    gtk_widget_add_accelerator (obj->btn_send, "clicked", parent->accelgroup, GDK_KP_Enter, 0, 0);

    GtkWidget* btn_quit = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    g_signal_connect (btn_quit, "clicked", G_CALLBACK (devchat_window_close_tab), parent_data);
    gtk_widget_add_accelerator(btn_quit, "clicked", parent->accelgroup, GDK_W, GDK_CONTROL_MASK, 0);

    gtk_box_pack_end (GTK_BOX(hbox),btn_quit,FALSE,FALSE,0);
    gtk_box_pack_end (GTK_BOX(hbox),gtk_vseparator_new(),FALSE,FALSE,0);
    gtk_box_pack_end (GTK_BOX(hbox),obj->btn_send,FALSE,FALSE,0);

    obj->chk_raw = gtk_check_button_new_with_label ("Raw mode");
    gtk_widget_set_tooltip_text (obj->chk_raw, "Send raw HTML text. Needed i.e. for browser-kicks and <!-- comments -->. Not recommended for daily use.");
    gtk_box_pack_end (GTK_BOX(hbox),obj->chk_raw,FALSE,FALSE,0);
    if (parent->userlevel < 6)
    {
      gtk_widget_set_no_show_all (obj->chk_raw,TRUE);
    }

    gtk_box_pack_start (GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX(vbox),scroll_in,TRUE,TRUE,0);

    gtk_paned_pack2 (GTK_PANED(obj->child),vbox,FALSE,FALSE);
    gtk_paned_set_position (GTK_PANED (obj->child), -1);

  }
  else
    obj->child = search_box;

  devchat_window_create_tags (obj->out_buffer, parent_data);

  return obj;
}

static void
devchat_conversation_init (DevchatConversation* self)
{
}

static void
devchat_conversation_class_init (DevchatConversationClass* klass)
{
}
