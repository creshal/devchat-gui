/*
 * main.c
 * Copyright (C) Samuel Vincent Creshal 2010 <creshal@arcor.de>
 *
 * devchat-gui is free software: you can redistribute it and/or modify it
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

int
main (int argc, char *argv[])
{
#ifdef DEBUG
  gchar* dbg_msg;
#endif
  if (!g_thread_supported ()) g_thread_init (NULL); /*Fix libsoup-related crash with GLib < 2.24*/
  gtk_init (&argc, &argv);
  /*TODO: Optparse */
  gchar* settingsfile;
  DevchatWindow* self = devchat_window_new ();


  settingsfile = g_build_filename(g_get_user_config_dir(),"devchat", NULL);
  if (!g_file_test (settingsfile, G_FILE_TEST_EXISTS))
  {
#ifdef DEBUG
    dbg_msg = g_strdup_printf ("Settings file not found, search path was %s.\n", settingsfile);
    dbg (dbg_msg);
    g_free (dbg_msg);
#endif
  }
  else
  {
    GKeyFile* keyfile = g_key_file_new ();
#ifdef DEBUG
    dbg("Loading settings...\n");
#endif
    if (!g_key_file_load_from_file (keyfile, settingsfile, G_KEY_FILE_NONE, NULL))
      err("Error loading settings file. Possibly insufficient rights or corrupted content.\n");
    else
    {
      gchar* g = "Devchat";
      GError* e = NULL;
      g_object_set (self, "browser", g_key_file_get_value (keyfile, g, "BROWSER", &e),NULL);
      g_object_set (self, "color_font", g_key_file_get_string (keyfile, g, "COLOR_FONT", &e),NULL);
      g_object_set (self, "color_l1", g_key_file_get_string (keyfile, g, "COLOR_L1", &e),NULL);
      g_object_set (self, "color_l3", g_key_file_get_string (keyfile, g, "COLOR_L3", &e),NULL);
      g_object_set (self, "color_l5", g_key_file_get_string (keyfile, g, "COLOR_L5", &e),NULL);
      g_object_set (self, "color_l6", g_key_file_get_string (keyfile, g, "COLOR_L6", &e),NULL);
      g_object_set (self, "color_greens", g_key_file_get_string (keyfile, g, "COLOR_GREENS", &e),NULL);
      g_object_set (self, "color_blues", g_key_file_get_string (keyfile, g, "COLOR_BLUES", &e),NULL);
      g_object_set (self, "color_time", g_key_file_get_string (keyfile, g, "COLOR_TIME", &e),NULL);
      g_object_set (self, "color_url", g_key_file_get_string (keyfile, g, "COLOR_URL", &e),NULL);
      g_object_set (self, "color_url_visited", g_key_file_get_string (keyfile, g, "COLOR_URL_VISITED", &e),NULL);
      g_object_set (self, "color_url_hover", g_key_file_get_string (keyfile, g, "COLOR_URL_HOVER", &e),NULL);
      g_object_set (self, "color_highlight", g_key_file_get_string (keyfile, g, "COLOR_HIGHLIGHT", &e),NULL);
      g_object_set (self, "user", g_key_file_get_value (keyfile, g, "USER", &e),NULL);
      g_object_set (self, "pass", g_key_file_get_value (keyfile, g, "PASS", &e),NULL);
      g_object_set (self, "notify", g_key_file_get_value (keyfile, g, "NOTIFY", &e),NULL);
      g_object_set (self, "vnotify", g_key_file_get_value (keyfile, g, "VNOTIFY", &e),NULL);
      g_object_set (self, "showid", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "SHOWID", &e),"true") == 0,NULL);
      g_object_set (self, "stealthjoin", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "STEALTHJOIN", &e),"true") == 0,NULL);
      g_object_set (self, "autojoin", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "AUTOJOIN", &e),"true") == 0,NULL);
      g_object_set (self, "showhidden", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "SHOWHIDDEN", &e),"true") == 0,NULL);
      g_object_set (self, "coloruser", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "COLORUSER", &e),"true") == 0,NULL);
      g_object_set (self, "x", g_key_file_get_integer (keyfile, g, "X", &e),NULL);
      g_object_set (self, "y", g_key_file_get_integer (keyfile, g, "Y", &e),NULL);
      if (g_key_file_has_key (keyfile, g, "HANDLEWIDTH", &e))
        g_object_set (self, "handle_width", g_key_file_get_integer (keyfile, g, "HANDLEWIDTH", &e),NULL);
      else
        g_object_set (self, "handle_width", 400, NULL);
      g_object_set (self, "width", g_key_file_get_integer (keyfile, g, "WIDTH", &e),NULL);
      g_object_set (self, "height", g_key_file_get_integer (keyfile, g, "HEIGHT", &e),NULL);
      if (g_key_file_has_key (keyfile, g, "AVATARSIZE", &e))
        self->settings.avatar_size = g_key_file_get_integer (keyfile, g, "AVATARSIZE", &e);
      else
        self->settings.avatar_size = 12;
      if (g_key_file_has_key (keyfile, g, "UPDATE_TIME", &e))
        self->settings.update_time = g_key_file_get_integer (keyfile, g, "UPDATE_TIME", &e);
      else
        self->settings.update_time = 1000;

      gchar** keywords = g_strsplit (g_key_file_get_string (keyfile, g, "KEYWORDS", &e), "|", 0);
      int i;
      for (i = 0; keywords[i] != NULL; i++)
        self->settings.keywords = g_slist_prepend (self->settings.keywords, g_strdup(keywords[i]));
      g_strfreev (keywords);

      gchar** presets = g_strsplit (g_key_file_get_string (keyfile, g, "BOILERPLATES", &e), "|", 0);
      int j;
      for (j = 0; presets[j] != NULL && j < 10; j++)
        self->settings.presets[j] = g_strdup(presets[j]);
      g_strfreev (presets);

      devchat_window_refresh_presets (self);

      if (e)
        g_error ("Errors occured while loading settings: %s.", e->message);
    #ifdef DEBUG
      else
      {
        g_print ("Settings loaded successfully.\n");
      }
    #endif
    }
    g_key_file_free (keyfile);
  }


#ifdef G_OS_WIN32
  self->workingdir = g_build_filename (g_getenv("PROGRAMFILES"),"Devchat","pixmaps",NULL);
#else
  #ifdef G_OS_UNIX
    if (!g_file_test (g_build_filename("/","usr","share","pixmaps","devchat",NULL), G_FILE_TEST_IS_DIR))
    {
      if (g_file_test (g_build_filename(g_get_user_data_dir (),"pixmaps","devchat",NULL), G_FILE_TEST_IS_DIR))
        self->workingdir = g_build_filename(g_get_user_data_dir (),"pixmaps","devchat",NULL);
      else
        err("Could not find working dir.\n");
    }
    else
      self->workingdir = g_build_filename("/","usr","share","pixmaps","devchat",NULL);
  #endif
#endif

#ifdef DEBUG
  dbg_msg = g_strdup_printf ("Working dir determined to be %s \n", self->workingdir);
  dbg (dbg_msg);
  g_free (dbg_msg);
#endif

  g_hash_table_insert (self->smilies, ":keks:",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "atomkeks.png", NULL),NULL));
  g_hash_table_insert (self->smilies, ":cube:",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "cube.png", NULL),NULL));
  g_hash_table_insert (self->smilies, ":fp:",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "fp1.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, "8-)",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_cool.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":wall:",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_headbang.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":-D",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_mrgreen.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":-|",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_neutral.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":-S",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_oops.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":-P",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_razz.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":oops:",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_redface.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":roll:",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_rolleyes.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":-(",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_sad.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, "o_O",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_sceptic.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":-)",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_smile.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":-o",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_surprised.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":==",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_thumb.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":--",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_thumb_down.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":++",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_thumb_up.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ";-)",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_wink.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":wtf:",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "icon_wtf.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":ugly:",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "ugly.gif", NULL),NULL));
  g_hash_table_insert (self->smilies, ":wub:",gdk_pixbuf_new_from_file (g_build_filename(self->workingdir, "wub.gif", NULL),NULL));
  g_hash_table_insert (self->avatars, "default",gdk_pixbuf_new_from_file_at_size (g_build_filename(self->workingdir, "default", NULL),self->settings.avatar_size,self->settings.avatar_size,NULL));

  devchat_window_refresh_smilies (self);

  /*FIXME: gtk_window_set_icon_list */
  gtk_window_set_icon_from_file(GTK_WINDOW(self->window), g_build_filename(self->workingdir, "dcgui.png",NULL),NULL);

  self->avadir = g_build_filename (g_get_user_cache_dir(),"avatars",NULL);

  if (!g_file_test (self->avadir, G_FILE_TEST_IS_DIR))
  {
    if (g_mkdir (self->avadir,750) != 0)
    {
      err (g_strdup_printf ("Error creating avatar directory %s.",self->avadir));
    }
  }

  gtk_main ();
  return 0;
}
