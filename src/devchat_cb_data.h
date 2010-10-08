/*
 * devchat_cb_data.h
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

#ifndef __DEVCHAT_CB_DATA_H__
#define __DEVCHAT_CB_DATA_H__

#include "devchat_window.h"

#define DEVCHAT_TYPE_CB_DATA            (devchat_cb_data_get_type ())
#define DEVCHAT_CB_DATA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DEVCHAT_TYPE_CB_DATA, DevchatCBData))
#define DEVCHAT_IS_CB_DATA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DEVCHAT_TYPE_CB_DATA))
#define DEVCHAT_CB_DATA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DEVCHAT_TYPE_CB_DATA, DevchatCBDataClass))
#define DEVCHAT_IS_CB_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DEVCHAT_TYPE_CB_DATA))
#define DEVCHAT_CB_DATA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DEVCHAT_TYPE_CB_DATA, DevchatCBDataClass))

typedef struct _DevchatCBData         DevchatCBData;
typedef struct _DevchatCBDataClass    DevchatCBDataClass;

struct _DevchatCBData
{
  GObject parent_instance;

  DevchatWindow* window;
  gpointer data;
};

struct _DevchatCBDataClass
{
  GObjectClass parent_class;
};

DevchatCBData* devchat_cb_data_new (DevchatWindow* window, gpointer data);


GType devchat_cb_data_get_type (void);


#endif /* __DEVCHAT_CB_DATA_H__ */
