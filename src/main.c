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
  GOptionEntry entries[] =
  {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &debug, _("Show verbose information"), NULL },
    { "really-verbose", 'd', 0, G_OPTION_ARG_NONE, &real_debug, _("Show even more information, mainly from HTML parser. Do not use unless explicitly stated, since it's chattier than Clippy. You have been warned."), NULL },
    { "builtin-config", 'c', 0, G_OPTION_ARG_NONE, &no_config, _("Neither load config from nor save config to disk."), NULL },
  #ifdef SPELLCHECK
    { "no-spellcheck", 's', 0, G_OPTION_ARG_NONE, &no_spellcheck, _("Disable spellchecker."), NULL },
  #endif
    { NULL }
  };

  if (!g_thread_supported ()) g_thread_init (NULL); /*Fix libsoup-related crash with GLib < 2.24*/
  gtk_init (&argc, &argv);

  GOptionContext* opt_ctxt = g_option_context_new (_(" - Gtk+ client for X-Devchat."));
  g_option_context_add_main_entries (opt_ctxt, entries, NULL);
  if (!g_option_context_parse (opt_ctxt, &argc, &argv, NULL))
  {
    err (_("Error -2: Option parsing failed!"));
    return -2;
  }

  DevchatWindow* self = devchat_window_new ();

  /*Basic transparency support for murrine.*/
  GdkColormap* m = gdk_screen_get_rgba_colormap (gdk_display_get_screen (gdk_display_get_default (), 0));
  if (m && gdk_screen_is_composited (gdk_display_get_screen (gdk_display_get_default (), 0)))
    gtk_widget_set_default_colormap (m);

  gchar* dbg_msg;

  if (!no_config)
  {
    gchar* settingsfile;
    settingsfile = g_build_filename(g_get_user_config_dir(),"devchat", NULL);
    if (!g_file_test (settingsfile, G_FILE_TEST_EXISTS))
    {
      if (debug) {
        dbg_msg = g_strdup_printf ("Settings file not found, search path was %s.\n", settingsfile);
        dbg (dbg_msg);
        g_free (dbg_msg);
      }
    }
    else
    {
      GKeyFile* keyfile = g_key_file_new ();
      if (debug) {
        dbg("Loading settings...\n");
      }
      if (!g_key_file_load_from_file (keyfile, settingsfile, G_KEY_FILE_NONE, NULL))
        err(_("Error loading settings file. Possibly insufficient rights or corrupted content.\n"));
      else
      {
        gchar* g = "Devchat";
        GError* e = NULL;

        if (g_key_file_has_key (keyfile, g, "SERVER_NAME", &e))
          self->settings.servername = g_key_file_get_string (keyfile, g, "SERVER_NAME", &e);

        g_object_set (self, "browser", g_key_file_get_value (keyfile, g, "BROWSER", &e), "color_font", g_key_file_get_string (keyfile, g, "COLOR_FONT", &e),
        "color_l1", g_key_file_get_string (keyfile, g, "COLOR_L1", &e),
        "color_l3", g_key_file_get_string (keyfile, g, "COLOR_L3", &e),
        "color_l5", g_key_file_get_string (keyfile, g, "COLOR_L5", &e),
        "color_l6", g_key_file_get_string (keyfile, g, "COLOR_L6", &e),
        "color_greens", g_key_file_get_string (keyfile, g, "COLOR_GREENS", &e),
        "color_blues", g_key_file_get_string (keyfile, g, "COLOR_BLUES", &e),
        "color_time", g_key_file_get_string (keyfile, g, "COLOR_TIME", &e),
        "color_url", g_key_file_get_string (keyfile, g, "COLOR_URL", &e),
        "color_url_visited", g_key_file_get_string (keyfile, g, "COLOR_URL_VISITED", &e),
        "color_url_hover", g_key_file_get_string (keyfile, g, "COLOR_URL_HOVER", &e),
        "color_highlight", g_key_file_get_string (keyfile, g, "COLOR_HIGHLIGHT", &e),
        "user", g_key_file_get_value (keyfile, g, "USER", &e),
        "pass", g_key_file_get_value (keyfile, g, "PASS", &e),
        "notify", g_key_file_get_value (keyfile, g, "NOTIFY", &e),
        "vnotify", g_key_file_get_value (keyfile, g, "VNOTIFY", &e),
        "showid", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "SHOWID", &e),"true") == 0,
        "stealthjoin", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "STEALTHJOIN", &e),"true") == 0,
        "showhidden", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "SHOWHIDDEN", &e),"true") == 0,
        "coloruser", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "COLORUSER", &e),"true") == 0,
        "x", g_key_file_get_integer (keyfile, g, "X", &e),
        "y", g_key_file_get_integer (keyfile, g, "Y", &e),
        "width", g_key_file_get_integer (keyfile, g, "WIDTH", &e),
        "height", g_key_file_get_integer (keyfile, g, "HEIGHT", &e),NULL);
        if (g_key_file_has_key (keyfile, g, "MAXIMIZED", &e))
          g_object_set (self, "maximized", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "MAXIMIZED", &e),"true") == 0, NULL);
        if (g_key_file_has_key (keyfile, g, "AVATARSIZE", &e))
          self->settings.avatar_size = g_key_file_get_integer (keyfile, g, "AVATARSIZE", &e);
        else
          self->settings.avatar_size = 12;
        if (g_key_file_has_key (keyfile, g, "UPDATE_TIME", &e))
          self->settings.update_time = g_key_file_get_integer (keyfile, g, "UPDATE_TIME", &e);
        else
          self->settings.update_time = 1000;
        if (g_key_file_has_key (keyfile, g, "STORE_PASS", &e))
          self->settings.store_pass = g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "STORE_PASS", &e),"true") == 0;
        else
          self->settings.store_pass = FALSE;
        if (g_key_file_has_key (keyfile, g, "COLOR_GOLDIES", &e))
          g_object_set (self, "color_goldies", g_key_file_get_string (keyfile, g, "COLOR_GOLDIES", &e),NULL);
        if (g_key_file_has_key (keyfile, g, "SHOW_TRAY", &e))
          g_object_set (self, "trayicon", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "SHOW_TRAY", &e),"true") == 0,NULL);
        if (g_key_file_has_key (keyfile, g, "JUMP_TAB", &e))
          self->settings.jumptab = g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "JUMP_TAB", &e),"true") == 0;

        gchar** keywords = g_strsplit (g_key_file_get_string (keyfile, g, "KEYWORDS", &e), "|", 0);
        int i;
        for (i = 0; keywords[i] != NULL; i++)
          self->settings.keywords = g_slist_append (self->settings.keywords, g_strdup(keywords[i]));
        g_strfreev (keywords);

        gchar** presets = g_strsplit (g_key_file_get_string (keyfile, g, "BOILERPLATES", &e), "|", 0);
        int j;
        for (j = 0; presets[j] != NULL && j < 10; j++)
          self->settings.presets[j] = g_strdup(presets[j]);
        g_strfreev (presets);

        devchat_window_refresh_presets (self);

        if (g_key_file_has_key (keyfile, g, "HANDLE_WIDTH", &e))
          g_object_set (self, "handle_width", g_key_file_get_integer (keyfile, g, "HANDLE_WIDTH", &e),NULL);
        else
          g_object_set (self, "handle_width", 400, NULL);

        if (g_key_file_has_key (keyfile, g, "COLOR_GREEN", &e))
        {
          g_object_set (self, "color_green", g_key_file_get_string (keyfile, g, "COLOR_GREEN", &e),
                        "color_red", g_key_file_get_string (keyfile, g, "COLOR_RED", &e),
                        "color_blue", g_key_file_get_string (keyfile, g, "COLOR_BLUE", &e),
                        "color_cyan", g_key_file_get_string (keyfile, g, "COLOR_CYAN", &e),
                        "color_yellow", g_key_file_get_string (keyfile, g, "COLOR_YELLOW", &e),
                        "color_magenta", g_key_file_get_string (keyfile, g, "COLOR_MAGENTA", &e), NULL);
        }

        g_object_set (self, "autojoin", g_ascii_strcasecmp (g_key_file_get_string (keyfile, g, "AUTOJOIN", &e),"true") == 0, NULL);

        if (e)
          err (g_strdup_printf (_("Errors occured while loading settings: %s."), e->message));
        else if (debug)
          dbg ("Settings loaded successfully.\n");
      }
      g_key_file_free (keyfile);
    }
  }

  self->workingdir = NULL;
  const gchar* const* dirs = g_get_system_data_dirs ();

  int i;

  for (i=0; dirs[i]; i++)
  {
    if (debug) {
      dbg_msg = g_strdup_printf ("Testing for directory %s...\n", g_build_filename (dirs[i], "pixmaps", "devchat", NULL));
      dbg (dbg_msg);
      g_free (dbg_msg);
    }
    if (g_file_test (g_build_filename (dirs[i], "pixmaps", "devchat", NULL), G_FILE_TEST_IS_DIR))
    {
      self->workingdir = g_build_filename (dirs[i], "pixmaps", "devchat", NULL);
      break;
    }
  }

  if (!self->workingdir)
  {
    if (debug) {
      dbg_msg = g_strdup_printf ("Testing for directory %s...\n", g_build_filename (g_get_user_data_dir (), "pixmaps", "devchat", NULL));
      dbg (dbg_msg);
      g_free (dbg_msg);
    }
    if (g_file_test (g_build_filename (g_get_user_data_dir (), "pixmaps", "devchat", NULL), G_FILE_TEST_IS_DIR))
    {
      self->workingdir = g_build_filename (g_get_user_data_dir (), "pixmaps", "devchat", NULL);
    }
    else
    {
      err (_("Error -1: Could not find working dir!"));
      return -1;
    }
  }


  if (debug) {
    dbg_msg = g_strdup_printf ("Working dir determined to be %s \n", self->workingdir);
    dbg (dbg_msg);
    g_free (dbg_msg);
  }

  g_hash_table_insert (self->smilies, ":keks:", g_build_filename (self->workingdir, "atomkeks.png", NULL));
  g_hash_table_insert (self->smilies, ":cube:", g_build_filename (self->workingdir, "cube.png", NULL));
  g_hash_table_insert (self->smilies, ":fp:", g_build_filename (self->workingdir, "fp1.gif", NULL));
  g_hash_table_insert (self->smilies, "8-)", g_build_filename (self->workingdir, "icon_cool.gif", NULL));
  g_hash_table_insert (self->smilies, ":wall:", g_build_filename (self->workingdir, "icon_headbang.gif", NULL));
  g_hash_table_insert (self->smilies, ":-D", g_build_filename (self->workingdir, "icon_mrgreen.gif", NULL));
  g_hash_table_insert (self->smilies, ":-|", g_build_filename (self->workingdir, "icon_neutral.gif", NULL));
  g_hash_table_insert (self->smilies, ":-S", g_build_filename (self->workingdir, "icon_oops.gif", NULL));
  g_hash_table_insert (self->smilies, ":-P", g_build_filename (self->workingdir, "icon_razz.gif", NULL));
  g_hash_table_insert (self->smilies, ":oops:", g_build_filename (self->workingdir, "icon_redface.gif", NULL));
  g_hash_table_insert (self->smilies, ":roll:", g_build_filename (self->workingdir, "icon_rolleyes.gif", NULL));
  g_hash_table_insert (self->smilies, ":-(", g_build_filename (self->workingdir, "icon_sad.gif", NULL));
  g_hash_table_insert (self->smilies, "o_O", g_build_filename (self->workingdir, "icon_sceptic.gif", NULL));
  g_hash_table_insert (self->smilies, ":-)", g_build_filename (self->workingdir, "icon_smile.gif", NULL));
  g_hash_table_insert (self->smilies, ":-o", g_build_filename (self->workingdir, "icon_surprised.gif", NULL));
  g_hash_table_insert (self->smilies, ":==", g_build_filename (self->workingdir, "icon_thumb.gif", NULL));
  g_hash_table_insert (self->smilies, ":--", g_build_filename (self->workingdir, "icon_thumb_down.gif", NULL));
  g_hash_table_insert (self->smilies, ":++", g_build_filename (self->workingdir, "icon_thumb_up.gif", NULL));
  g_hash_table_insert (self->smilies, ";-)", g_build_filename (self->workingdir, "icon_wink.gif", NULL));
  g_hash_table_insert (self->smilies, ":eg:", g_build_filename (self->workingdir, "icon_evillaugh.gif", NULL));
  g_hash_table_insert (self->smilies, ":wtf:", g_build_filename (self->workingdir, "icon_wtf.gif", NULL));
  g_hash_table_insert (self->smilies, ":ugly:", g_build_filename (self->workingdir, "ugly.gif", NULL));
  g_hash_table_insert (self->smilies, ":wub:", g_build_filename (self->workingdir, "wub.gif", NULL));
  g_hash_table_insert (self->smilies, ":giggle:", g_build_filename (self->workingdir, "giggle.gif", NULL));
  g_hash_table_insert (self->smilies, ":shocked:", g_build_filename (self->workingdir, "icon_eek.gif", NULL));
  g_hash_table_insert (self->smilies, ":confused:", g_build_filename (self->workingdir, "icon_confused.gif", NULL));
  g_hash_table_insert (self->smilies, ":mad:", g_build_filename (self->workingdir, "icon_mad.gif", NULL));
  g_hash_table_insert (self->smilies, ":cry:", g_build_filename (self->workingdir, "icon_cry.gif", NULL));
  g_hash_table_insert (self->smilies, ":evil:", g_build_filename (self->workingdir, "icon_evil.gif", NULL));
  g_hash_table_insert (self->smilies, ":twisted:", g_build_filename (self->workingdir, "icon_twisted.gif", NULL));
  g_hash_table_insert (self->smilies, ":idea:", g_build_filename (self->workingdir, "icon_idea.gif", NULL));
  g_hash_table_insert (self->smilies, ":arrow:", g_build_filename (self->workingdir, "icon_arrow.gif", NULL));
  g_hash_table_insert (self->smilies, ":think:", g_build_filename (self->workingdir, "icon_think.gif", NULL));
  g_hash_table_insert (self->smilies, ":doh:", g_build_filename (self->workingdir, "icon_doh.gif", NULL));
  g_hash_table_insert (self->smilies, ":rant:", g_build_filename (self->workingdir, "icon_rant.gif", NULL));
  g_hash_table_insert (self->smilies, ":star:", g_build_filename (self->workingdir, "icon_star.gif", NULL));
  g_hash_table_insert (self->avatars, "default", gdk_pixbuf_new_from_file_at_size (g_build_filename (self->workingdir, "default", NULL), self->settings.avatar_size, self->settings.avatar_size, NULL));

  devchat_window_refresh_smilies (self);

  gchar* icon_sizes[] = {"dcgui.png","dcgui_32.png","dcgui_48.png","dcgui_256.png"};

  GList* icons = NULL;

  int icon_cnt = 0;

  for (icon_cnt = 0;icon_cnt < 4; icon_cnt++)
  {
    icons = g_list_append (icons, gdk_pixbuf_new_from_file (g_build_filename (self->workingdir, icon_sizes[icon_cnt], NULL), NULL));
  }

  gtk_window_set_icon_list (GTK_WINDOW (self->window), icons);

  gchar* iconname = g_build_filename(self->workingdir, "dcgui.png",NULL);
  gtk_status_icon_set_from_file (GTK_STATUS_ICON (self->trayicon), iconname);
  g_free (iconname);

  self->avadir = g_build_filename (g_get_user_cache_dir(),"avatars",NULL);

  if (!g_file_test (self->avadir, G_FILE_TEST_IS_DIR))
  {
    if (g_mkdir (self->avadir,750) != 0)
    {
      err (g_strdup_printf (_("Error creating avatar directory %s."),self->avadir));
    }
  }

  gtk_main ();
  return 0;
}
