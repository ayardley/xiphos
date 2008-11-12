/*
 * GnomeSword Bible Study Tool
 * editor_menu.h - popup menu for html editor
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __EDITOR_MENU_H_
#define __EDITOR_MENU_H_

#include <config.h>
#ifndef USE_GTKHTML38

#include <gnome.h>

#include "editor/editor.h"

void gui_new_activate(GtkMenuItem * menuitem, 
				GSHTMLEditorControlData * ecd);
void on_save_activate(GtkMenuItem * menuitem, 
				GSHTMLEditorControlData * ecd);
GtkWidget *gui_create_editor_popup(GSHTMLEditorControlData * ecd);

#endif
#endif