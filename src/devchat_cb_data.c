/*
 * devchat_cb_data.c
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

#include "devchat_cb_data.h"

G_DEFINE_TYPE (DevchatCBData, devchat_cb_data, G_TYPE_OBJECT)

DevchatCBData*
devchat_cb_data_new (DevchatWindow* window, gpointer data)
{
  DevchatCBData* obj = g_object_new (DEVCHAT_TYPE_CB_DATA, NULL);

  obj->window = window;
  obj->data = data;

  return obj;
}

static void
devchat_cb_data_init (DevchatCBData* self)
{
}

static void
devchat_cb_data_class_init (DevchatCBDataClass* klass)
{
}
