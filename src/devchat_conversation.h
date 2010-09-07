/*
 * devchat_conversation.h
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

#ifndef __DEVCHAT_CONVERSATION_H__
#define __DEVCHAT_CONVERSATION_H__

#include "devchat_cb_data.h"

#define DEVCHAT_TYPE_CONVERSATION            (devchat_conversation_get_type ())
#define DEVCHAT_CONVERSATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DEVCHAT_TYPE_CONVERSATION, DevchatConversation))
#define DEVCHAT_IS_CONVERSATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DEVCHAT_TYPE_CONVERSATION))
#define DEVCHAT_CONVERSATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DEVCHAT_TYPE_CONVERSATION, DevchatConversationClass))
#define DEVCHAT_IS_CONVERSATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DEVCHAT_TYPE_CONVERSATION))
#define DEVCHAT_CONVERSATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DEVCHAT_TYPE_CONVERSATION, DevchatConversationClass))

typedef struct _DevchatConversation         DevchatConversation;
typedef struct _DevchatConversationClass    DevchatConversationClass;

struct _DevchatConversation
{
  GObject parent_instance;

  DevchatWindow* parent;
  GtkWidget* child;
  GtkWidget* out_widget;
  GtkTextBuffer* out_buffer;
  GtkWidget* in_widget;
  GtkTextBuffer* in_buffer;
  GtkWidget* chk_raw;
  GtkTextMark* scroll_to;
  GtkWidget* searchbar;
  GtkWidget* search_button;
  GtkWidget* search_entry;
  GtkTextIter search_start;
  gboolean search_start_set;
  GtkWidget* btn_send;
};

struct _DevchatConversationClass
{
  GObjectClass parent_class;
};

DevchatConversation* devchat_conversation_new (gboolean is_history, DevchatWindow* parent);


GType devchat_conversation_get_type (void);


#endif /* __DEVCHAT_CONVERSATION_H__ */