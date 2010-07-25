/*
 * devchat_html_attr.h
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

#ifndef __DEVCHAT_HTML_ATTR_H__
#define __DEVCHAT_HTML_ATTR_H__

#include <glib.h>
#include <glib-2.0/glib-object.h>

#define DEVCHAT_TYPE_HTML_ATTR            (devchat_html_attr_get_type ())
#define DEVCHAT_HTML_ATTR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DEVCHAT_TYPE_HTML_ATTR, DevchatHTMLAttr))
#define DEVCHAT_IS_HTML_ATTR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DEVCHAT_TYPE_HTML_ATTR))
#define DEVCHAT_HTML_ATTR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DEVCHAT_TYPE_HTML_ATTR, DevchatHTMLAttrClass))
#define DEVCHAT_IS_HTML_ATTR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DEVCHAT_TYPE_HTML_ATTR))
#define DEVCHAT_HTML_ATTR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DEVCHAT_TYPE_HTML_ATTR, DevchatHTMLAttrClass))

typedef struct _DevchatHTMLAttr         DevchatHTMLAttr;
typedef struct _DevchatHTMLAttrClass    DevchatHTMLAttrClass;

struct _DevchatHTMLAttr
{
  GObject parent_instance;

  gchar* name;
  gchar* value;
};

struct _DevchatHTMLAttrClass
{
  GObjectClass parent_class;
};

DevchatHTMLAttr* devchat_html_attr_new ();


GType devchat_html_attr_get_type (void);


#endif /* __DEVCHAT_HTML_ATTR_H__ */