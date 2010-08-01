/*
 * devchat_url_tag.h
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

#ifndef __DEVCHAT_URL_TAG_H__
#define __DEVCHAT_URL_TAG_H__

#include <glib.h>
#include <glib-2.0/glib-object.h>
#include <gtk/gtktexttag.h>

G_BEGIN_DECLS

#define DEVCHAT_TYPE_URL_TAG            (devchat_url_tag_get_type ())
#define DEVCHAT_URL_TAG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DEVCHAT_TYPE_URL_TAG, DevchatURLTag))
#define DEVCHAT_IS_URL_TAG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DEVCHAT_TYPE_URL_TAG))
#define DEVCHAT_URL_TAG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DEVCHAT_TYPE_URL_TAG, DevchatURLTagClass))
#define DEVCHAT_IS_URL_TAG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DEVCHAT_TYPE_URL_TAG))
#define DEVCHAT_URL_TAG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DEVCHAT_TYPE_URL_TAG, DevchatURLTagClass))

typedef struct _DevchatURLTag         DevchatURLTag;
typedef struct _DevchatURLTagClass    DevchatURLTagClass;

struct _DevchatURLTag
{
  GtkTextTag parent_instance;

  gboolean visited;
};

struct _DevchatURLTagClass
{
  GtkTextTagClass parent_class;
};

DevchatURLTag* devchat_url_tag_new (gchar* name, gchar* color);

GType devchat_url_tag_get_type (void) G_GNUC_CONST;

G_END_DECLS;

#endif /* __DEVCHAT_URL_TAG_H__ */
