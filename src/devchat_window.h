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

#include <glib-2.0/glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include <libxml/xmlreader.h>
#include <libxml/HTMLparser.h>

#include <libsoup-2.4/libsoup/soup.h>

#ifdef NOTIFY
  #include <libnotify/notify.h>
#endif

#ifdef AUDIO
  #ifdef G_OS_UNIX
    #include <alsa/asoundlib.h>
  #else
    #ifdef G_OS_WIN32
    /*TODO*/
    #endif
  #endif
#endif

#define SOUP_SESSION_CALLBACK(f)  ((SoupSessionCallback) (f))

G_BEGIN_DECLS

#define DEVCHAT_TYPE_WINDOW             (devchat_window_get_type ())
#define DEVCHAT_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), DEVCHAT_TYPE_WINDOW, DevchatWindow))
#define DEVCHAT_IS_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DEVCHAT_TYPE_WINDOW))
#define DEVCHAT_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), DEVCHAT_TYPE_WINDOW, DevchatWindowClass))
#define DEVCHAT_IS_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), DEVCHAT_TYPE_WINDOW))
#define DEVCHAT_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), DEVCHAT_TYPE_WINDOW, DevchatWindowClass))

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
  guint x;
  guint y;
  guint update_time;
  guint avatar_size;
  GSList* keywords;
  GSList* presets;
  gboolean showid;
  gboolean stealthjoin;
  gboolean autojoin;
  gboolean showhidden;
  gboolean coloruser;
} dc_settings;

enum
{
  URL_PROFILE_EDIT,
  URL_VISIT_L1,
  URL_VISIT_L3 = 3,
  URL_VISIT_L5 = 5
};


typedef struct _DevchatWindow           DevchatWindow;
typedef struct _DevchatWindowClass      DevchatWindowClass;

struct _DevchatWindow
{
  GObject parent_instance;

  /*< public >*/

  dc_settings settings;
  dc_settings settings_backup;

  SoupSession* session;

  GtkAccelGroup* accelgroup;

  GtkWidget* window;
  GtkWidget* notebook;
  GtkWidget* userlist;
  GtkWidget* userlist_port;
  GtkWidget* inputbar;
  GtkWidget* level_box;
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
  GtkWidget* item_texts;
  GtkWidget* item_smilies;

  GtkTextBuffer* output;
  GtkTextBuffer* input;

  GHashTable* smilies;
  GHashTable* avatars;
  GHashTable* conversations;
  GHashTable* old_conversations;
  GHashTable* users;

  GtkTextTag* hovertag;

  GSList* users_without_avatar;

  gboolean firstrun;
  gboolean no_halt_requested;

  gint lineno;
  gchar* lastid;
  guint userlevel;

  guchar* workingdir;
  guchar* avadir;

  /*< private >*/
};

struct _DevchatWindowClass
{
  GObjectClass parent_class;

  /* class members */
};

typedef struct devchat_cb_data
{
  DevchatWindow* window;
  gpointer data;
} devchat_cb_data;

static void err(gchar* message)
{
  g_warning (message);
}

static void dbg(gchar* message)
{
#ifdef DEBUG
  g_print (message);
  g_print ("\n");
#endif
}

GType devchat_window_get_type (void);

DevchatWindow* devchat_window_new (void);

void devchat_window_refresh_smilies (DevchatWindow* self);

G_END_DECLS

#endif /* __DEVCHAT_WINDOW_H__ */