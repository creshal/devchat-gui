/*
 * devchat_url_tag.c
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

#include "devchat_url_tag.h"

static void devchat_url_tag_set_property (GObject* object, guint id, const GValue* value, GParamSpec* pspec);
static void devchat_url_tag_get_property (GObject* object, guint id, GValue* value, GParamSpec* pspec);

G_DEFINE_TYPE (DevchatURLTag, devchat_url_tag, GTK_TYPE_TEXT_TAG)

DevchatURLTag* devchat_url_tag_new (gchar* name, gchar* color)
{
  return g_object_new (DEVCHAT_TYPE_URL_TAG, "name", name, "foreground", color, "underline", PANGO_UNDERLINE_SINGLE, NULL);
}

static void
devchat_url_tag_init (DevchatURLTag* self)
{

  self->visited = FALSE;
}

static void
devchat_url_tag_class_init (DevchatURLTagClass* klass)
{
  GObjectClass* gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = devchat_url_tag_set_property;
  gobject_class->get_property = devchat_url_tag_get_property;

  g_object_class_install_property (gobject_class, 1, g_param_spec_boolean
                                                     ( "visited", "Visited",
                                                       "Indicates whether the URL was visited.", FALSE,
                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)
                                                     ));

}

static void devchat_url_tag_set_property (GObject* object, guint id, const GValue* value, GParamSpec* pspec)
{
  if (id == 1)
  {
    DevchatURLTag* tag = DEVCHAT_URL_TAG (object);
    tag->visited = g_value_get_boolean (value);
  }
}

static void devchat_url_tag_get_property (GObject* object, guint id, GValue* value, GParamSpec* pspec)
{
  if (id == 1)
  {
    DevchatURLTag* tag = DEVCHAT_URL_TAG (object);
    g_value_set_boolean (value, tag->visited);
  }
}
