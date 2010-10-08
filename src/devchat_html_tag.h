/*
 * devchat_html_tag.h
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

#ifndef __DEVCHAT_HTML_TAG_H__
#define __DEVCHAT_HTML_TAG_H__

#include <glib.h>
#include <gtk/gtk.h>
#include <glib-2.0/glib-object.h>

#define DEVCHAT_TYPE_HTML_TAG            (devchat_html_tag_get_type ())
#define DEVCHAT_HTML_TAG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DEVCHAT_TYPE_HTML_TAG, DevchatHTMLTag))
#define DEVCHAT_IS_HTML_TAG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DEVCHAT_TYPE_HTML_TAG))
#define DEVCHAT_HTML_TAG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DEVCHAT_TYPE_HTML_TAG, DevchatHTMLTagClass))
#define DEVCHAT_IS_HTML_TAG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DEVCHAT_TYPE_HTML_TAG))
#define DEVCHAT_HTML_TAG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DEVCHAT_TYPE_HTML_TAG, DevchatHTMLTagClass))

typedef struct _DevchatHTMLTag         DevchatHTMLTag;
typedef struct _DevchatHTMLTagClass    DevchatHTMLTagClass;

struct _DevchatHTMLTag
{
  GObject parent_instance;

  gchar* name;
  GtkTextMark* start_mark;
  GSList* attrs;
};

struct _DevchatHTMLTagClass
{
  GObjectClass parent_class;
};

DevchatHTMLTag* devchat_html_tag_new ();


GType devchat_html_tag_get_type (void);


#endif /* __DEVCHAT_HTML_TAG_H__ */
