/*
 * Xiphos Bible Study Tool
 * bookmarks_treeview.c - gui for bookmarks using treeview
 *
 * Copyright (C) 2003-2008 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <libxml/parser.h>
//#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gui/xiphos.h"
#include "gui/bookmarks_menu.h"
#include "gui/bookmarks_treeview.h"
#include "gui/utilities.h"
#include "gui/main_window.h"
#include "gui/dialog.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/module_dialogs.h"
#include "main/url.hh"

#ifdef USE_GTKHTML3_14_23
#include "editor/slib-editor.h"
#else
#include "editor/bonobo-editor.h"
#endif

BOOKMARK_MENU menu;

gboolean bookmarks_changed;



/******************************************************************************
 * Name
 *  parse_treeview
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void parse_treeview(xmlNodePtr parent, GtkTreeIter * tree_parent)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void parse_treeview(xmlNodePtr parent, GtkTreeIter * tree_parent)
{
	static xmlNodePtr cur_node;
	GtkTreeIter child;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;
	
	gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &child,
                                             tree_parent);
	
	do {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &child,
			   		2, &caption, 
					3, &key, 
					4, &module, 
					5, &mod_desc, 
					6, &description,
					-1);
		if(gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), 
							&child)) {
			cur_node = xml_add_folder_to_parent(parent, 
							caption);
			parse_treeview(cur_node, &child);
						     
		}
		else 
			xml_add_bookmark_to_parent(parent, 
						description,
						key,
						module,
						mod_desc);
		
		g_free(caption);
		g_free(key);
		g_free(module);	
		g_free(mod_desc);
		g_free(description);
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &child));
}


/******************************************************************************
 * Name
 *   save_iter_to_xml_removed
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_iter_to_xml_removed(GtkTreeIter * iter)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void save_iter_to_xml_removed(GtkTreeIter * iter)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	xmlAttrPtr root_attr;
	//const xmlChar *xml_filename;
	gchar *caption = NULL;
	gchar filename[256];
//	GtkTreeIter child;
	
	if (!bookmarks_changed) 
		return;
	
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   2, &caption, -1);
	sprintf(filename, "%s/bookmarks/removed/%s.xml", settings.gSwordDir,caption);
	//xml_filename = (const xmlChar *) buf;
	root_doc = xmlNewDoc((const xmlChar *) "1.0");
	
	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)
				       "SwordBookmarks");
		root_attr =
		    xmlNewProp(root_node, (const xmlChar *)"syntaxVersion",
			       (const xmlChar *) "1");
		xmlDocSetRootElement(root_doc, root_node);
	}
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   2, &caption, -1);
		if( gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model),iter)) {
			cur_node = xml_add_folder_to_parent(root_node, caption);
			parse_treeview(cur_node, iter);
		}
	g_free(caption);
	xmlSaveFormatFile(filename, root_doc,1);
	xmlFreeDoc(root_doc);
	
	bookmarks_changed = FALSE;
}

/******************************************************************************
 * Name
 *  save_treeview_to_xml_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_treeview_to_xml_bookmarks(GtkTreeIter * iter, gchar *file_buf)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void save_treeview_to_xml_bookmarks(GtkTreeIter * iter, gchar * filename)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	xmlAttrPtr root_attr;
	//const xmlChar *xml_filename;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;
	
	if (!bookmarks_changed) 
		return;
	
	//xml_filename = (const xmlChar *) filename;
	root_doc = xmlNewDoc((const xmlChar *) "1.0");
	
	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)
				       "SwordBookmarks");
		root_attr =
		    xmlNewProp(root_node, (const xmlChar *) "syntaxVersion",
			       (const xmlChar *) "1");
		xmlDocSetRootElement(root_doc, root_node);
	}
	
	do {
		gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   		2, &caption, 
					3, &key, 
					4, &module, 
					5, &mod_desc, 
					6, &description, 
					-1);
		if( gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model),iter)) {
			cur_node = xml_add_folder_to_parent(root_node, caption);
			parse_treeview(cur_node, iter);
		}
		else 		
			xml_add_bookmark_to_parent(root_node,  
						description,
						key,
						module,
						mod_desc);
		g_free(caption);
		g_free(key);
		g_free(module);	
		g_free(mod_desc);
		g_free(description);	
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model),iter));
	
	xmlSaveFormatFile(filename, root_doc,1);
	xmlFreeDoc(root_doc);
	g_free(filename);	
	bookmarks_changed = FALSE;
}


					    
/******************************************************************************
 * Name
 *  add_node_to_ctree
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   GtkCTreeNode *add_node_to_ctree(GtkCTree * ctree, 
 *			GtkCTreeNode *node, BOOKMARK_DATA * data)	
 *
 * Description
 *    actually add the GtkCTreeNode to the bookmark ctree
 *
 * Return value
 *   GtkCTreeNode
 */

static void add_item_to_tree(GtkTreeIter *iter,GtkTreeIter *parent, 
							BOOKMARK_DATA * data)
{
	gtk_tree_store_append(GTK_TREE_STORE(model), iter, parent);
	
		gtk_tree_store_set(GTK_TREE_STORE(model), iter, 
			   COL_OPEN_PIXBUF, data->opened,
			   COL_CLOSED_PIXBUF, data->closed,
			   COL_CAPTION, data->caption, 
			   COL_KEY, data->key,
			   COL_MODULE, data->module,
			   COL_MODULE_DESC, data->module_desc,
			   COL_DESCRIPTION, data->description,
			   -1);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void bibletime_bookmarks_activate(GtkMenuItem * menuitem,
				  gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeIter parent;
	GString *str;
	const xmlChar *file;
	//gchar *text[3];

	if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model),&parent))
		return;
	
	str = g_string_new(settings.swbmDir);
	g_string_sprintf(str, "%s/%s", settings.homedir,
			 ".kde/share/apps/bibletime/bookmarks.xml");
	
	file = (const xmlChar *) str->str;
	
	gtk_tree_store_append(GTK_TREE_STORE(model), &iter,
			      &parent);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 
			   COL_OPEN_PIXBUF, bm_pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, bm_pixbufs->pixbuf_closed,
			   COL_CAPTION, "BibleTime", 
			   COL_KEY, NULL,
			   COL_MODULE, NULL,
			   -1);	
	gui_parse_bookmarks(bookmark_tree, file, &iter);
	g_string_free(str, TRUE);
}


/******************************************************************************
 * Name
 *   on_allow_reordering_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_allow_reordering_activate(GtkMenuItem * menuitem,
 *				  gpointer user_data)
 *
 * Description
 *   allow reordering of bookmarks
 *
 * Return value
 *   void
 */

static void on_allow_reordering_activate(GtkMenuItem * menuitem,
				  gpointer user_data)
{
	 gtk_tree_view_set_reorderable(bookmark_tree,
                           GTK_CHECK_MENU_ITEM(menuitem)->active);
}

/******************************************************************************
 * Name
 *   on_dialog_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_dialog_activate(GtkMenuItem * menuitem,
 *				  gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_dialog_activate(GtkMenuItem * menuitem, gpointer user_data)
{	
	GtkTreeIter selected;
	gchar *key = NULL;
	gchar *module = NULL;	
	gchar *url = NULL;
	
	GtkTreeSelection* selection= gtk_tree_view_get_selection(bookmark_tree);
	
	use_dialog = TRUE; 
	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model), 
					&selected,
					3, &key, 
					4, &module, 
					-1);
#ifdef USE_GTKHTML38
		if(module && (main_get_mod_type(module) == PERCOM_TYPE)) {			
			editor_create_new(module,key,TRUE);			
			use_dialog = FALSE;
			g_free(module);
			g_free(key);
			return;
		}
#endif
		url = g_strdup_printf("xiphos.url?action=showBookmark&"
					"type=%s&value=%s&module=%s",
					"newDialog",
					main_url_encode(key), 
					main_url_encode(module));
		main_url_handler(url, TRUE);
		g_free(url);
	}
	use_dialog = FALSE;
	g_free(module);
	g_free(key);
}


/******************************************************************************
 * Name
 *   on_edit_item_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_edit_item_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   edit bookmark
 *
 * Return value
 *   void
 */

static void on_edit_item_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GS_DIALOG *info;
	BOOKMARK_DATA * data;	
	gint test;
	GtkTreeSelection* selection;
	GtkTreeIter selected;
//	GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;
	gboolean is_leaf;
	GString *str;
	
	str = g_string_new(NULL);
	g_string_printf(str,"<span weight=\"bold\">%s</span>",_("Edit"));
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;	
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
					2, &caption, 
					3, &key, 
					4, &module, 
					5, &mod_desc, 
					6, &description,
					-1);
	
	info = gui_new_dialog();
	info->title = N_("Bookmark");
	info->label_top = str->str;
	if(gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)) {
		info->label1 = N_("Folder name: ");
		is_leaf = FALSE;		
	} else {
		info->label1 = N_("Bookmark name: ");
		info->text2 = g_strdup(key);
		info->text3 = g_strdup(module);
		info->label2 = N_("Verse: ");
		info->label3 = N_("Module: ");
		is_leaf = TRUE;		
	}
		
	info->text1 = g_strdup(caption);
	info->ok = TRUE;
	info->cancel = TRUE;
		
	
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		data = g_new(BOOKMARK_DATA,1);
		data->caption = info->text1;
		data->key = NULL; 		
		data->module = NULL; 
		data->module_desc = NULL;
		data->description = NULL;
		if(is_leaf) {
			data->opened = bm_pixbufs->pixbuf_helpdoc;
			data->closed = NULL;	
			data->key = info->text2; 		
			data->module = info->text3; 
			data->module_desc = main_get_module_description(info->text3);
			if((strlen(description) > 1) || (strcmp(caption,info->text1))) {
				data->description = info->text1;
			}
			else 
				data->description = NULL;
			data->is_leaf = TRUE;
		} else {				
			data->opened = bm_pixbufs->pixbuf_opened;
			data->closed = bm_pixbufs->pixbuf_closed;
			data->is_leaf = FALSE;
		}
			
		gtk_tree_store_set(GTK_TREE_STORE(model), &selected, 
			   COL_OPEN_PIXBUF, data->opened,
			   COL_CLOSED_PIXBUF, data->closed,
			   COL_CAPTION, data->caption, 
			   COL_KEY, data->key,
			   COL_MODULE, data->module,
			   COL_MODULE_DESC, data->module_desc,
			   COL_DESCRIPTION, data->description,
			   -1);	
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
		g_free(data);
	}
	g_free(info->text1);	// we used g_strdup() 
	if(info->text2) g_free(info->text2);
	if(info->text3) g_free(info->text3);
	g_free(info);
	g_free(caption);
	g_free(key);
	g_free(module);
	g_free(mod_desc);
	g_free(description);
	g_string_free(str,TRUE);
}


/******************************************************************************
 * Name
 *   on_remove_folder_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_remove_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   remove folder - and save it
 *
 * Return value
 *   void
 */

static void on_remove_folder_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	GtkTreeSelection* selection;
	GtkTreeIter selected;
//	GtkTreeIter iter;
	gchar buf[256];
	gchar *caption = NULL;
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption, -1);

	sprintf(buf, "%s/removed/%s.xml", settings.swbmDir,
		caption);
	GS_warning((buf));

	bookmarks_changed = TRUE;
	save_iter_to_xml_removed(&selected);
	gtk_tree_store_remove(GTK_TREE_STORE(model), &selected);
	bookmarks_changed = TRUE;
	gui_save_bookmarks(NULL, NULL);
	g_free(caption);
}


/******************************************************************************
 * Name
 *   restore_ok
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *   
 *   void restore_ok(GtkButton *button, gpointer data);
 *
 * Description
 *   OK button for restore fileselection was clicked.
 *
 * Return value
 *   void
 */

static void restore_ok(GtkButton * button, GtkWidget * filesel)
{
	const xmlChar *file;
	//gchar buf[256];
	
	file = (const xmlChar *)
	    gtk_file_selection_get_filename(GTK_FILE_SELECTION
					    (filesel));	
	gui_load_removed(file);
	
	bookmarks_changed = TRUE;
	gui_save_bookmarks(NULL, NULL);
	gtk_widget_destroy(filesel);
}


/******************************************************************************
 * Name
 *   restore_cancel
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *   
 *   void restore_cancel(GtkButton *button, gpointer data);
 *
 * Description
 *   Cancel button for restore fileselection was clicked.
 *
 * Return value
 *   void
 */

static void restore_cancel(GtkButton * button, GtkWidget * dlg)
{
	gtk_widget_destroy(dlg);
}


/******************************************************************************
 * Name
 *   on_restore_folder_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_restore_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   restore a saved bookmark folder
 *
 * Return value
 *   void
 */

static void on_restore_folder_activate(GtkMenuItem * menuitem,
				gpointer user_data)
{
	GtkWidget *fileselection;
	GtkWidget *ok;
	GtkWidget *cancel;
	gchar buf[256];

	fileselection =
	    gtk_file_selection_new(_("Restore Bookmark Folder"));
	gtk_container_set_border_width(GTK_CONTAINER(fileselection),
				       10);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION
					       (fileselection));

	ok = GTK_FILE_SELECTION(fileselection)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselection), "ok", ok);
	gtk_widget_show(ok);
	GTK_WIDGET_SET_FLAGS(ok, GTK_CAN_DEFAULT);

	cancel = GTK_FILE_SELECTION(fileselection)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselection), "cancel",
			    cancel);
	gtk_widget_show(cancel);
	GTK_WIDGET_SET_FLAGS(cancel, GTK_CAN_DEFAULT);

	sprintf(buf, "%s/removed/*.xml", settings.swbmDir);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION
					(fileselection), buf);

	gtk_widget_show(fileselection);

	g_signal_connect(GTK_OBJECT(ok), "clicked",
			   GTK_SIGNAL_FUNC(restore_ok),
			   (GtkWidget *) fileselection);
	g_signal_connect(GTK_OBJECT(cancel), "clicked",
			   GTK_SIGNAL_FUNC(restore_cancel),
			   (GtkWidget *) fileselection);

}


/******************************************************************************
 * Name
 *   on_delete_item_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_delete_item_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   delete bookmark - if a group delete all in the group
 *
 * Return value
 *   void
 */

static void on_delete_item_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *name_string;
	GtkTreeSelection* selection;
	GtkTreeIter selected;
//	GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *str;
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption, 3, &key, 4, &module, -1);
	name_string = caption;

	if(gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)) {
		str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s %s",
				      _("Remove the selected folder"),
				      name_string,
				      _("(and all its contents)?"));
	} else {
		str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
				      _("Remove the selected bookmark"),
				      name_string);
	}

	if (gui_yes_no_dialog(str, GTK_STOCK_DIALOG_WARNING)) {
		gtk_tree_store_remove(GTK_TREE_STORE(model), &selected);
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
	}
	g_free(caption);
	g_free(key);
	g_free(module);
	g_free(str);
}


/******************************************************************************
 * Name
 *   gui_save_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void gui_save_bookmarks(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   save bookmark tree 
 *
 * Return value
 *   void
 */

void gui_save_bookmarks(GtkMenuItem * menuitem, gpointer user_data)
{
	
	GtkTreeIter root;
	GtkTreeIter first_child;
	gchar buf[256];
	
	if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &root))
		return;
	if(!gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &first_child,
                                             &root))
		return;

	sprintf(buf, "%s/bookmarks/bookmarks.xml", settings.gSwordDir);
	save_treeview_to_xml_bookmarks(&first_child, g_strdup(buf));
}

/******************************************************************************
 * Name
 *   gui_save_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void gui_save_bookmarks(void)
 *
 * Description
 *   save bookmark tree 
 *
 * Return value
 *   void
 */

void gui_save_bookmarks_treeview(void)
{
	gui_save_bookmarks(NULL, NULL);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   
 *
 * Description
 *   expand the bookmark tree
 *
 * Return value
 *   void
 */

static void on_expand_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gtk_tree_view_expand_all(bookmark_tree);
}


/******************************************************************************
 * Name
 *  on_collapse_activate 
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_collapse_activate(GtkMenuItem * menuitem,
 *				 gpointer user_data)
 *
 * Description
 *   collapse the bookmark tree
 *
 * Return value
 *   void
 */

static void on_collapse_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{
 	gtk_tree_view_collapse_all(bookmark_tree);	
}


/******************************************************************************
 * Name
 *  on_add_bookmark_activate 
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_add_bookmark_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)
 *
 * Description
 *   add bookmark (current mod and key of active window (text, comm, or dict)
 *   to root node chosen by user
 *
 * Return value
 *   void
 */

static void on_add_bookmark_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
{	
	GtkTreeIter selected;
	GtkTreeIter iter;
//	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *mod_name = NULL;
	gint test;
	BOOKMARK_DATA * data;
	GS_DIALOG *info;
	gchar buf[256];
	GString *str = g_string_new(NULL);
	
	
	if (!gtk_tree_selection_get_selected(current_selection,NULL,&selected))
		return;

	mod_name = main_get_active_pane_module();
	key = main_get_active_pane_key();		
	data = g_new(BOOKMARK_DATA,1);
	info = gui_new_dialog();
	info->title = N_("Bookmark");
	g_string_printf(str, "<span weight=\"bold\">%s</span>", _("Add"));
	info->label_top = str->str;
	sprintf(buf, "%s, %s", key, mod_name);
	info->text1 = g_strdup(buf);
	info->text2 = g_strdup(key);
	info->text3 = g_strdup(mod_name);
	info->label1 = N_("Label: ");
	info->label2 = N_("Verse: ");
	info->label3 = N_("Module: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		data->caption = info->text1;
		data->key = info->text2; 
		data->module = info->text3; 
		data->module_desc = main_get_module_description(info->text3);
		if(!strcmp(data->caption,buf))
			data->description = NULL;
		else
			data->description = info->text1;
		data->is_leaf = TRUE;
		data->opened = bm_pixbufs->pixbuf_helpdoc;
		data->closed = NULL;			
		add_item_to_tree(&iter,&selected, data);			
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
	}
	g_free(info->text1);	/* we used g_strdup() */
	g_free(info->text2);
	g_free(info->text3);
	g_free(info);
	g_string_free(str,TRUE);
}

/******************************************************************************
 * Name
 *  on_insert_bookmark_activate 
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_insert_bookmark_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_insert_bookmark_activate(GtkMenuItem * menuitem,
					gpointer user_data)
{
	on_add_bookmark_activate(menuitem,NULL);
}


/******************************************************************************
 * Name
 *   on_new_subgroup_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_new_subgroup_activate(GtkMenuItem * menuitem,
 *			      gpointer user_data)
 *
 * Description
 *   add new sub group to selected group
 *
 * Return value
 *   void
 */

static void on_new_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkTreeIter selected;
	GtkTreeIter iter;
//	gchar *caption = NULL;
//	gchar *key = NULL;
//	gchar *module = NULL;
	char *t, *buf;
	gint test;
	GS_DIALOG *info;
	BOOKMARK_DATA * data;
	GString *str;
	
	if(!gtk_tree_selection_get_selected(current_selection, NULL, &selected)) 
		return;
	
	t = "/|><.'`\"";
	str = g_string_new("");
	info = gui_new_dialog();
	//info->stock_icon = GTK_STOCK_OPEN;
	info->title = _("Bookmark");
	g_string_printf(str, "<span weight=\"bold\">%s</span>",
			_("Enter Folder Name"));
	info->label_top = str->str;
	info->text1 = g_strdup(_("Folder Name"));
	info->label1 = _("Folder: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	
	data = g_new(BOOKMARK_DATA,1);
	/*** open dialog to get name for new folder ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {	
		buf = g_strdelimit(info->text1, t, ' ');
		data->caption = g_strdup(buf);
		data->key = NULL; 
		data->module = NULL; 
		data->module_desc = NULL;
		data->description = NULL;
		data->is_leaf = FALSE;
		data->opened = bm_pixbufs->pixbuf_opened;
		data->closed = bm_pixbufs->pixbuf_closed;			
		add_item_to_tree(&iter,&selected, data);
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
		g_free(data->caption);
	}
	g_free(data);
	g_free(info->text1);
	g_free(info);
	g_string_free(str,TRUE);
}

/******************************************************************************
 * Name
 *   on_open_in_tab_activate
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_open_in_tab_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void on_open_in_tab_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkTreeSelection* selection;
	GtkTreeIter selected;
//	GtkTreeIter iter;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *url = NULL;
	
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if (!gtk_tree_selection_get_selected(selection,NULL,&selected))
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   3, &key, 
				   4, &module, 
			           -1);	
	url = g_strdup_printf("xiphos.url?action=showBookmark&"
					"type=%s&value=%s&module=%s",
					"newTab",
					main_url_encode(key), 
					main_url_encode(module));
	main_url_handler(url,TRUE);
	g_free(key);
	g_free(module);
	g_free(url);
}


/******************************************************************************
 * Name
 *   pmBookmarkTree_uiinfo
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   GnomeUIInfo pmBookmarkTree_uiinfo[] 
 *
 * Description
 *  gnome menu structure for bookmarks popup menu
 *  !! CHANGING MENU STRUCTURE DON'T FORGET ALSO CHANGE create_bookmark_menu !!
 *
 */

static GnomeUIInfo rr_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Remove Folder"),
	 N_("Remove folder and save it"),
	 on_remove_folder_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Restore Folder"),
	 N_("Restore saved folder"),
	 on_restore_folder_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_PASTE,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo pmBookmarkTree_uiinfo[] = {
	{ /* 0 */
	 GNOME_APP_UI_ITEM, N_("Open in a new tab"),
	 N_("Open this bookmark in a new tab"),
	 (gpointer) on_open_in_tab_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_FILENAME, NULL /* init'd in menu creation */,
	 0, (GdkModifierType) 0, NULL},
	{ /* 1 */
	 GNOME_APP_UI_ITEM, N_("Open in a dialog"),
	 N_("Open this bookmark in a dialog"),
	 (gpointer) on_dialog_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_FILENAME, NULL /* init'd in menu creation */,
	 0, (GdkModifierType) 0, NULL},
	{ /* 2 */
	 GNOME_APP_UI_ITEM, N_("New Folder"),
	 N_("Add new folder to selected folder"),
	 (gpointer) on_new_folder_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_NEW,
	 0, (GdkModifierType) 0, NULL},
	{ /* 3 */
	 GNOME_APP_UI_ITEM, N_("Insert Bookmark"),
	 N_("Insert new bookmark here"),
	 (gpointer) on_insert_bookmark_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_NEW,
	 0, (GdkModifierType) 0, NULL},
	{ /* 4 */
	 GNOME_APP_UI_ITEM, N_("_Edit Item"),
	 N_("Edit bookmark item"),
	 (gpointer) on_edit_item_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_PROPERTIES,
	 0, (GdkModifierType) 0, NULL},
	{ /* 5 */
	 GNOME_APP_UI_ITEM, N_("Delete Item"),
	 N_("Delete item"),
	 (gpointer) on_delete_item_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_TRASH,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR, /* 6 */
	{ /* 7 */
	 GNOME_APP_UI_ITEM, N_("Expand All"),
	 N_("Expand all Bookmarks groups"),
	 (gpointer) on_expand_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_GOTO_BOTTOM,
	 0, (GdkModifierType) 0, NULL},
	{ /* 8 */
	 GNOME_APP_UI_ITEM, N_("Collapse All"),
	 N_("Collapse all Bookmarks groups"),
	 (gpointer) on_collapse_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_GOTO_TOP,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR, /* 9 */
	{ /* 10 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Allow Reordering"),
	 N_
	 ("Allow items to be moved from one folder to another"),
	 (gpointer) on_allow_reordering_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR, /* 11 */
	{ /* 12 */
	 GNOME_APP_UI_ITEM, N_("Import Bibletime Bookmarks"),
	 N_("Load Bookmarks from Bibletime"),
	 (gpointer) bibletime_bookmarks_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 13 */
	 GNOME_APP_UI_SUBTREE, N_("Remove - Restore"),
	 NULL,
	 rr_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_GOTO_TOP,
	 0, 0, NULL},
	GNOMEUIINFO_END
};


/******************************************************************************
 * Name
 *   create_bookmark_menu
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void create_bookmark_menu(void)
 *
 * Description
 *   create bookmark tree popup menu
 *  !! CHANGING MENU STRUCTURE DON'T FORGET ALSO CHANGE create_bookmark_menu !!
 *
 * Return value
 *   void
 */

void gui_create_bookmark_menu(void)
{
	/*
	 * this is total magic.  set up menu before using it.
	 * indices are direct from GnomeUIInfo above.
	 */
	if (!pmBookmarkTree_uiinfo[0].pixmap_info)
		pmBookmarkTree_uiinfo[0].pixmap_info =
		    image_locator("new_tab_button.png");
	if (!pmBookmarkTree_uiinfo[1].pixmap_info)
		pmBookmarkTree_uiinfo[1].pixmap_info =
		    image_locator("dlg-un.png");
	/* end magic */

	menu.menu = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(menu.menu),
			    "menu.menu", menu.menu);
	gnome_app_fill_menu(GTK_MENU_SHELL(menu.menu),
			    pmBookmarkTree_uiinfo, NULL, FALSE, 0);

	menu.in_tab = pmBookmarkTree_uiinfo[0].widget;
	menu.in_dialog = pmBookmarkTree_uiinfo[1].widget;
	menu.new = pmBookmarkTree_uiinfo[2].widget;
	menu.insert = pmBookmarkTree_uiinfo[3].widget;
	menu.edit = pmBookmarkTree_uiinfo[4].widget;
	menu.delete = pmBookmarkTree_uiinfo[5].widget;

	menu.reorder = pmBookmarkTree_uiinfo[10].widget;

	menu.bibletime = pmBookmarkTree_uiinfo[12].widget;
	menu.rr_submenu = pmBookmarkTree_uiinfo[13].widget;

	menu.remove = rr_menu_uiinfo[0].widget;
	menu.restore = rr_menu_uiinfo[2].widget;

	gtk_widget_set_sensitive(menu.in_tab, FALSE);
	gtk_widget_set_sensitive(menu.in_dialog, FALSE);
	gtk_widget_set_sensitive(menu.new, FALSE);
	gtk_widget_set_sensitive(menu.insert, FALSE);
	gtk_widget_set_sensitive(menu.edit, FALSE);
	gtk_widget_set_sensitive(menu.delete, FALSE);
	gtk_widget_set_sensitive(menu.bibletime,
				 settings.have_bibletime);

	gtk_widget_set_sensitive(menu.rr_submenu, FALSE);
	gtk_widget_set_sensitive(menu.remove, TRUE);
	gtk_widget_set_sensitive(menu.restore, TRUE);

	gtk_widget_hide(pmBookmarkTree_uiinfo[13].widget);
	/* don't know what this is -- turn it off for now */
	
	/*gnome_app_install_menu_hints(GNOME_APP(widgets.app),
				     pmBookmarkTree_uiinfo);*/
				    
}
