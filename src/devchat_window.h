/*
 * devchat_window.h
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

#ifndef __DEVCHAT_WINDOW_H__
#define __DEVCHAT_WINDOW_H__

#include "config.h"
#include "devchat_url_tag.h"

#include <glib.h>
#include <glib-2.0/glib-object.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include <libxml/xmlreader.h>
#include <libxml/parserInternals.h>

#include <libsoup-2.4/libsoup/soup.h>

#ifdef G_OS_WIN32
#include <windows.h>
#include <Shellapi.h>
#endif

#ifdef NOTIFY
  #include <libnotify/notify.h>
#endif

#define SOUP_SESSION_CALLBACK(f)  ((SoupSessionCallback) (f))

G_BEGIN_DECLS

#define DEVCHAT_TYPE_WINDOW             (devchat_window_get_type ())
#define DEVCHAT_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), DEVCHAT_TYPE_WINDOW, DevchatWindow))
#define DEVCHAT_IS_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DEVCHAT_TYPE_WINDOW))
#define DEVCHAT_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), DEVCHAT_TYPE_WINDOW, DevchatWindowClass))
#define DEVCHAT_IS_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), DEVCHAT_TYPE_WINDOW))
#define DEVCHAT_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), DEVCHAT_TYPE_WINDOW, DevchatWindowClass))
#define MAX_BUF 19

typedef struct dc_settings
{
  gchar* browser;
  gchar* color_font;
  gchar* color_l1;
  gchar* color_l3;
  gchar* color_l5;
  gchar* color_l6;
  gchar* color_greens;
  gchar* color_blues;
  gchar* color_time;
  gchar* color_url;
  gchar* color_url_visited;
  gchar* color_url_hover;
  gchar* color_highlight;
  gchar* user;
  gchar* pass;
  gchar* notify;
  gchar* vnotify;
  guint width;
  guint height;
  gint x;
  gint y;
  guint update_time;
  guint avatar_size;
  guint handle_width;
  GSList* keywords;
  gchar* presets[10];
  gboolean showid;
  gboolean stealthjoin;
  gboolean autojoin;
  gboolean showhidden;
  gboolean coloruser;
} dc_settings;

enum
{
  URL_PROFILE_EDIT = -255,
  URL_VISIT_L1 = -1,
  URL_VISIT_L3 = -3,
  URL_VISIT_L5 = -5
};


typedef struct _DevchatWindow           DevchatWindow;
typedef struct _DevchatWindowClass      DevchatWindowClass;

struct _DevchatWindow
{
  GObject parent_instance;

  dc_settings settings;
  dc_settings settings_backup;

  SoupSession* session;
  guint msg_list_getter;
  guint usr_list_getter;

  GtkAccelGroup* accelgroup;

  GtkWidget* window;
  GtkWidget* notebook;
  GtkWidget* userlist;
  GtkWidget* userlist_port;
  GtkWidget* inputbar;
  GtkWidget* level_box;
  GtkWidget* filter_ml;
  GtkWidget* filter_ul;
  GtkWidget* loginbar;
  GtkWidget* user_entry;
  GtkWidget* pass_entry;
  GtkWidget* btn_connect;
  GtkWidget* statusbar;
  GtkWidget* statuslabel;
  GtkWidget* userlabel;
  GtkWidget* outputwidget;
  GtkWidget* inputwidget;
  GtkWidget* item_connect;
  GtkWidget* item_l3;
  GtkWidget* item_l5;
  GtkWidget* item_reconnect;
  GtkWidget* item_smilies;
  GtkWidget* item_presets;
  GtkWidget* chk_raw;

  GtkTextBuffer* output;
  GtkTextBuffer* input;

  GHashTable* smilies;
  GHashTable* avatars;
  GHashTable* conversations;
  GHashTable* users;

  DevchatURLTag* hovertag;

  GSList* users_without_avatar;

  gboolean firstrun;
  gboolean no_halt_requested;

  gint lineno;
  gchar* lastid;
  guint userlevel;

  gchar* workingdir;
  gchar* avadir;
  gchar* buffer[MAX_BUF+1];
  gint buf_current;
};

struct _DevchatWindowClass
{
  GObjectClass parent_class;
};

typedef struct devchat_cb_data
{
  DevchatWindow* window;
  gpointer data;
} devchat_cb_data;

void err(gchar* message);

#ifdef DEBUG
void dbg(gchar* message);
#endif

GType devchat_window_get_type (void);

DevchatWindow* devchat_window_new (void);

void devchat_window_refresh_smilies (DevchatWindow* self);
void devchat_window_refresh_presets (DevchatWindow* self);

void devchat_window_on_mark_set_cb();
gboolean devchat_window_on_motion_cb ();
gboolean devchat_window_tab_changed_win ();
void devchat_window_btn_format ();
void devchat_window_btn_send ();
void devchat_window_close_tab ();
void devchat_window_create_tags ();

G_END_DECLS

#endif /* __DEVCHAT_WINDOW_H__ */
