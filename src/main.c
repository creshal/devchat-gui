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
  gtk_init (&argc, &argv);
  /*TODO: Optparse */
  gchar* settingsfile;
  DevchatWindow* self = devchat_window_new ();


  settingsfile = g_build_filename(g_get_user_config_dir(),"devchat", NULL);
  if (!g_file_test (settingsfile, G_FILE_TEST_EXISTS))
    dbg("Settings file not found.\n");
  else
  {
    GKeyFile* keyfile = g_key_file_new ();
    dbg("Loading settings...\n");
    if (!g_key_file_load_from_file (keyfile, settingsfile, G_KEY_FILE_NONE, NULL))
      err("Error loading settings file. Possibly insufficient rights or corrupted content.\n");
    else
    {
      /*TODO Einstellungen lesen. */
    }
    g_key_file_free (keyfile);
  }


#ifdef G_OS_WIN32
  #ifdef FOLDERID_ProgramFilesX86
    self->workingdir = g_build_filename (FOLDERID_ProgramFilesX86,"Devchat","pixmaps",NULL);
  #else
    #ifdef CSIDL_PROGRAM_FILES
      self->workingdir = g_build_filename (CSIDL_PROGRAM_FILES,"Devchat","pixmaps",NULL);
    #else
      err("Could not find %PROGRAMFILES%\n");
    #endif
  #endif
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

  dbg( g_strdup_printf ("Working dir determined to be %s \n",self->workingdir) );

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

  /*TODO: Restliche Avatare ergänzen.*/

  gtk_main ();
  return 0;
}
