/*
 * GnomeSword Bible Study Tool
 * gtkhtml_dispaly.c - display Sword modules using GtkHTML 
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gal/widgets/e-unicode.h>

#include "gui/gtkhtml_display.h"
#include "gui/gnomesword.h"

#include "main/sword.h"
#include "main/bibletext.h"
#include "main/settings.h"
#include "main/key.h"


/******************************************************************************
 * Name
 *  get_font
 *
 * Synopsis
 *   #include ".h"
 *
 *   MOD_FONT * get_font(gchar *mod_name)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static MOD_FONT *get_font(gchar * mod_name)
{
	MOD_FONT *mf;

	mf = g_new(MOD_FONT, 1);
	mf->mod_name = mod_name;
	//g_warning(mf->mod_name);
	mf->old_font = NULL;
	mf->old_gdk_font = NULL;
	mf->old_font_size = NULL;
	mf->new_font = NULL;
	mf->new_gdk_font = NULL;
	mf->new_font_size = NULL;
	mf->no_font = 0;
	get_font_info(mf);
	if (strlen(mf->old_font) < 2)
		mf->old_font = "none";
	//g_warning("mf->old_font = %s",mf->old_font);
	return mf;
}

/******************************************************************************
 * Name
 *  set_gobal_options
 *
 * Synopsis
 *   #include ".h"
 *
 *  void (TEXT_DATA * t)	
 *
 * Description
 *   set module global options on page change
 *
 * Return value
 *   void
 */

static void set_global_options(TEXT_GLOBALS * tgs)
{	
	set_text_module_global_option("Strong's Numbers",tgs->strongs);
	set_text_module_global_option("Morphological Tags",tgs->morphs);
	set_text_module_global_option("Footnotes",tgs->footnotes);
	set_text_module_global_option("Greek Accents",tgs->greekaccents);
	set_text_module_global_option("Lemmas",tgs->lemmas);
	set_text_module_global_option("Scripture Cross-references",tgs->scripturerefs);
	set_text_module_global_option("Hebrew Vowel Points",tgs->hebrewpoints);
	set_text_module_global_option("Hebrew Cantillation",tgs->hebrewcant);
	set_text_module_global_option("Headings",tgs->headings);
	set_text_module_global_option("Red letter words",tgs->words_in_red);	
	if (tgs->variants_primary)
			set_text_global_option("Textual Variants",
					       "Primary Reading");
	else if (tgs->variants_secondary)
			set_text_global_option("Textual Variants",
					       "Secondary Reading");
	else
		set_text_global_option("Textual Variants",
					       "All Readings");
}



/******************************************************************************
 * Name
 *   mark_search_words
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void mark_search_words( GString *str )	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void mark_search_words(GString * str)
{
	gchar *tmpbuf, *buf, *searchbuf;
	gint len1, len2, len3, len4;
	gchar closestr[40], openstr[40];

	/* regular expression search results         **fixme** */
	if (settings.searchType == 0) {
		return;
	}
	/* close tags */
	sprintf(closestr, "</b></font>");
	/* open tags */
	sprintf(openstr, "<font color=\"%s\"><b>",
		settings.found_color);
	/* point buf to found verse */
	buf = str->str;
	searchbuf = g_strdup(settings.searchText);

	/* if we have a muti word search go here */
	if (settings.searchType == -2) {
		char *token;
		GList *list;
		gint count = 0, i = 0;

		list = NULL;
		/* seperate the search words and add them to a glist */
		if ((token = strtok(searchbuf, " ")) != NULL) {
			list = g_list_append(list, token);
			++count;
			while ((token = strtok(NULL, " ")) != NULL) {
				list = g_list_append(list, token);
				++count;
			}
			/* if we have only one word */
		} else {
			list = g_list_append(list, searchbuf);
			count = 1;
		}
		list = g_list_first(list);
		/* find each word in the list and mark it */
		for (i = 0; i < count; i++) {
			/* set len1 to length of verse */
			len1 = strlen(buf);
			/* set len2 to length of search word */
			len2 = strlen((gchar *) list->data);
			/* find search word in verse */
			if ((tmpbuf =
			     strstr(buf,
				    (gchar *) list->data)) != NULL) {
				/* set len3 to length of tmpbuf 
				   (tmpbuf points to first occurance of 
				   search word in verse) */
				len3 = strlen(tmpbuf);
				//-- set len4 to diff between len1 and len3
				len4 = len1 - len3;
				/* add end tags first 
				   (position to add tag to is len4 + len2) */
				str =
				    g_string_insert(str, (len4 + len2),
						    closestr);
				/* then add start tags 
				   (position to add tag to is len4) */
				str =
				    g_string_insert(str, len4, openstr);
			}
			/* point buf to changed str */
			buf = str->str;
			list = g_list_next(list);
		}
		g_list_free(list);

		/* else we have a phrase and only need to mark it */
	} else {
		len1 = strlen(buf);
		len2 = strlen(searchbuf);
		tmpbuf = strstr(buf, searchbuf);
		if (tmpbuf) {
			len3 = strlen(tmpbuf);
			len4 = len1 - len3;
			/* place end tag first */
			str =
			    g_string_insert(str, (len4 + len2),
					    closestr);
			/* then place start tag */
			str = g_string_insert(str, len4, openstr);
		}
	}
	/* free searchbuf */
	g_free(searchbuf);
}

/******************************************************************************
 * Name
 *   entry_display
 *
 * Synopsis
 *   #include "gui/gtkhtml_display.h"
 *
 *   void entry_display(GtkWidget * html_widget, gchar * mod_name, 
 *					      gchar * text, gchar *key)
 *
 * Description
 *   display Sword modules a verse (entry) at a time
 *
 * Return value
 *   void
 */

void entry_display(GtkWidget * html_widget, gchar * mod_name,
		   gchar * text, gchar * key, gboolean show_key)
{
	gchar tmpBuf[500];
	gchar *use_font;
	gchar *use_font_size = NULL;
	gchar *utf8str;
	gint utf8len;
	GString *str;
	gboolean use_gtkhtml_font = FALSE;
	gboolean was_editable = FALSE;
	MOD_FONT *mf;
	GtkHTMLStreamStatus status1 = 0;
	GtkHTML *html;
	GtkHTMLStream *htmlstream;

	mf = get_font(mod_name);

	use_font = g_strdup(mf->old_font);
	//g_warning("use_font = %s",use_font);
	if (use_font) {
		if (!strncmp(use_font, "none", 4))
			use_gtkhtml_font = TRUE;
		else
			use_gtkhtml_font = FALSE;

	} else {
		use_gtkhtml_font = TRUE;

	}

	if ((mf->old_font_size[0] == '-')
	    || (mf->old_font_size[0] == '+'))
		use_font_size = g_strdup(mf->old_font_size);
	else
		use_font_size = g_strdup("+1");

	/* setup gtkhtml widget */
	html = GTK_HTML(html_widget);
	was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");


	sprintf(tmpBuf,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);
	utf8str = e_utf8_from_gtk_string(html_widget, tmpBuf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	/* show key in html widget  */
	if (show_key) {
		str = g_string_new("");
		if ((settings.displaySearchResults)) {
			g_string_sprintf(str,
					 "<A HREF=\"version=%s passage=%s\">"
					 "<FONT COLOR=\"%s\">[%s] %s </font></A>",
					 mod_name,
					 key,
					 settings.bible_verse_num_color,
					 mod_name, key);
		}

		else {
			g_string_sprintf(str,
					 "<A HREF=\"[%s] %s\">"
					 "<FONT COLOR=\"%s\">[%s]</A></font>[%s] ",
					 mod_name,
					 get_module_description
					 (mod_name),
					 settings.bible_verse_num_color,
					 mod_name, key);
		}

		utf8str = e_utf8_from_gtk_string(html_widget, str->str);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
		g_string_free(str, 0);
	}


	if (use_gtkhtml_font)
		sprintf(tmpBuf, "<font size=\"%s\">", use_font_size);
	else
		sprintf(tmpBuf, "<font face=\"%s\" size=\"%s\">",
			use_font, use_font_size);

	utf8str = e_utf8_from_gtk_string(html_widget, tmpBuf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	if (settings.displaySearchResults) {
		str = g_string_new(text);
		mark_search_words(str);
		utf8str = str->str;
	} else {
		str = g_string_new(text);
		utfvoid chapter_display_mozilla(GtkWidget * html_widget, gchar * mod_name,
		     TEXT_GLOBALS * tgs, gchar * key,
		     gboolean use_globals)8str = str->str;
	}
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	sprintf(tmpBuf, " %s", "</font></body></html>");
	utf8str = e_utf8_from_gtk_string(html_widget, tmpBuf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}
	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	if (use_font_size)
		free(use_font_size);
	if (use_font)
		free(use_font);
	g_free(mf);
	g_string_free(str, TRUE);
}

/******************************************************************************
 * Name
 *   chapter_display
 *
 * Synopsis
 *   #include "gui/gtkhtml_display.h"
 *
 *   void chapter_display(GtkWidget * html_widget, gchar * module_name, 
 *					      		 gchar *key)
 *
 * Description
 *   display Sword Bible texts a chapter at a time
 *
 * Return valuevoid chapter_display_mozilla(GtkWidget * html_widget, gchar * mod_name,
		     TEXT_GLOBALS * tgs, gchar * key,
		     gboolean use_globals)
 *   void
 */ 

void chapter_display(GtkWidget * html_widget, gchar * mod_name,
		     TEXT_GLOBALS * tgs, gchar * key,
		     gboolean use_globals)
{
	gchar
	    * utf8str,
	    *bgColor,
	    *textColor,
	    buf[500],
	    *tmpkey, tmpbuf[256], *use_font, *use_font_size;
	gchar *text_str = NULL;
	gchar *paragraphMark;
	gint count;
	gboolean was_editable = FALSE;
	gboolean newparagraph = FALSE;
	gboolean use_gtkhtml_font = FALSE;
	GString *str;
	gint utf8len, cur_verse, cur_chapter, i = 1;
	const char *cur_book;
	MOD_FONT *mf;

	GtkHTML *html = GTK_HTML(html_widget);
	GtkHTMLStreamStatus status1 = 0;
	GtkHTMLStream *htmlstream;

	was_editable = gtk_html_get_editable(html);
	tmpkey = get_valid_key(key);

	paragraphMark = "&para;";

	bgColor = "#f1f1f1";
	cur_verse = get_verse_from_key(tmpkey);
	cur_chapter = get_chapter_from_key(tmpkey);
	cur_book = get_book_from_key(tmpkey);

	

	mf = get_font(mod_name);

	use_font = g_strdup(mf->old_font);
	//g_warning("use_font = %s",use_font);
	if (use_font) {
		if (!strncmp(use_font, "none", 4))
			use_gtkhtml_font = TRUE;
		else
			use_gtkhtml_font = FALSE;

	} else {
		use_gtkhtml_font = TRUE;

	}

	if ((mf->old_font_size[0] == '-')
	    || (mf->old_font_size[0] == '+'))
		use_font_size = g_strdup(mf->old_font_size);
	else
		use_font_size = g_strdup("+1");

	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");

	/*
	 * set global options for current module 
	 */
	if (use_globals)
		set_global_options(tgs);

	sprintf(buf,
		HTML_START
		"<body dir=\"rtl\" bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);
	
	str = g_string_new(buf);
	tmpkey = get_valid_key(key);
	count  = verse_count(tmpkey);
	for (i = 1 ;i <= count; i++) {
		sprintf(tmpbuf, "%s %d:%d", cur_book, cur_chapter, i);
		g_free(tmpkey);
		tmpkey = get_valid_key(tmpbuf);

		if (i == cur_verse)
			textColor = settings.currentverse_color;
		else
			textColor = settings.bible_text_color;

		sprintf(buf,
			"&nbsp; <A HREF=\"*%s\" NAME=\"%d\">"
			"<font size=\"%s\" color=\"%s\">%d</font></A> ",
			tmpkey,
			i,
			settings.verse_num_font_size,
			settings.bible_verse_num_color, i);
		
		str = g_string_append(str,buf);
		
		if (use_gtkhtml_font) {
			sprintf(tmpbuf,
				"<font size=\"%s\" color=\"%s\">",
				use_font_size, textColor);
		} else {
			sprintf(tmpbuf,
			  "<font face=\"%s\" size=\"%s\" color=\"%s\">",
				use_font, use_font_size, textColor);
		}
		str = g_string_append(str,tmpbuf);
		
		
		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			strcpy(tmpbuf, paragraphMark);
			str = g_string_append(str,tmpbuf);
		}

		/* get module text and prepare to display it */
		text_str = get_module_text(0, mod_name, tmpkey);
		
		str = g_string_append(str, text_str);
		/*
		if (settings.displaySearchResults)
			mark_search_words(str);
		utf8str = str->str;
		*/

		if (settings.versestyle) {
			if ((strstr(text_str, "<BR>") == NULL)
			    && (strstr(text_str, "<!P>") == NULL)) {
				sprintf(tmpbuf, " %s", "</font><br>");
			} else {
				sprintf(tmpbuf, " %s", "</font>");
			}
			if (strstr(text_str, "<!P>") == NULL) {
				newparagraph = FALSE;
			} else {
				newparagraph = TRUE;
			}
		}

		else {
			if (strstr(text_str, "<!P>") == NULL)
				sprintf(tmpbuf, " %s", "</font>");
			else
		
char GTKChapDisp::Display(SWModule &imodule) {
	gchar tmpbuf[255];
	gchar *body;
	gchar *utf8str;
	gchar *bgColor;/*
	 * set global options for current module 
	 */
	if (use_globals)
		set_global_options(tgs);
	gchar *direction;
	gchar *textColor;
	gchar buf[500];
	gchar *tmpkey;
	gchar *use_font; 
	gchar *use_font_size; 
	const gchar *text_str;
	gchar *paragraphMark;
	gint count;
	gboolean was_editable = FALSE;
	gboolean newparagraph = FALSE;
	gboolean use_gtkhtml_font = FALSE;
	bool r2l = FALSE;
	GString *str;
	gint utf8len, cur_verse, cur_chapter, i = 1;

	
	tmpkey = get_valid_key(key);
	paragraphMark = "&para;";

	
	use_font_size = g_strdup("+1");
	
	bgColor = "#f1f1f1";
	cur_verse = get_verse_from_key(tmpkey);
	cur_chapter = get_chapter_from_key(tmpkey);
	cur_book = get_book_from_key(tmpkey);

	

	mf = get_font(mod_name);

	use_font = g_strdup(mf->old_font);
	//g_warning("use_font = %s",use_font);
	if (use_font) {
		if (!strncmp(use_font, "none", 4))
			use_gtkhtml_font = TRUE;
		else
			use_gtkhtml_font = FALSE;

	} else {
		use_gtkhtml_font = TRUE;

	}

	if ((mf->old_font_size[0] == '-')
	    || (mf->old_font_size[0] == '+'))
		use_font_size = g_strdup(mf->old_font_size);
	else
		use_font_size = g_strdup("+1");
	/*
	 * set global options for current module 
	 */
	if (use_globals)
		set_global_options(tgs);
	
	str = g_string_new(""); 
	
	
	gtk_moz_embed_open_stream((GtkMozEmbed *)gtkText, "file:///gtk-embedding.html", "text/html");
	
	
	
	g_string_sprintf(str,"%s %s",HTML_START,body);
		
	
	count  = verse_count(tmpkey);
	for (i = 1 ;i <= count; i++) {	
		
		
		
		if (key->Verse() == curVerse)
			textColor = "#339766";
		else
			textColor = "#000000";

		sprintf(buf,
			"&nbsp; <A HREF=\"*%s %d:%d\" NAME=\"%d\">"
			"<font color=\"#000DCF\">%d</font></A> ",
			cur_book,
			key->Chapter(),
			key->Verse(),
			key->Verse(),
			key->Verse());
		str = g_string_append(str, buf);

		if (newparagraph) {
			newparagraph = FALSE;
			sprintf(buf,"%s",
				paragraphMark);
			str = g_string_append(str, buf);			
		}
		if(r2l) {
			sprintf(buf,
				"<font size=\"%s\" face=\"ezra sil\" color=\"%s\">",
				use_font_size, textColor);
		}
		else {
			sprintf(buf,
				"<font size=\"%s\" color=\"%s\">",
				use_font_size, textColor);
		}
		str = g_string_append(str, buf);
		
		text_str = (const char*)imodule;
		str = g_string_append(str,(const char*)imodule);
	
		if ((strstr(text_str, "<BR>") == NULL)
		    && (strstr(text_str, "<!P>") == NULL)) {
			sprintf(tmpbuf, " %s", "</font><br>");
		} else {
			sprintf(tmpbuf, " %s", "</font>");
		}
		if (strstr(text_str, "<!P>") == NULL) {
			newparagraph = FALSE;
		} else {
			newparagraph = TRUE;
		}
			
		g_string_append(str, tmpbuf);		
	}
	
	sprintf(buf, "%s", "</body></html>");
	g_string_append(str, buf);
	utf8str = e_utf8_from_gtk_string(gtkText, str->str);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_moz_embed_append_data((GtkMozEmbed *)gtkText,
			utf8str,utf8len);
	}
	sprintf(buf, "#%d", curVerse);
	//gtk_moz_embed_load_url((GtkMozEmbed *)gtkText, buf);
	gtk_moz_embed_close_stream((GtkMozEmbed *)gtkText);
	
	g_string_free(str, TRUE);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
}		sprintf(tmpbuf, " %s", "</font><p>");
		}
		free(text_str);
		str = g_string_append(str,tmpbuf);
	}

	sprintf(buf, "%s", "</body></html>");
	str = g_string_append(str,buf);
	utf8str = e_utf8_from_gtk_string(html_widget, str->str);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	sprintf(buf, "%d", cur_verse);
	gtk_html_jump_to_anchor(html, buf);
	
	g_string_free(str, TRUE);
	if (use_font_size)
		g_free(use_font_size);
	if (use_font)
		g_free(use_font);
	g_free(mf);
	g_free(tmpkey);
}



void chapter_display_mozilla(GtkWidget * html_widget, gchar * mod_name,
		     TEXT_GLOBALS * tgs, gchar * key,
		     gboolean use_globals)
{
	gchar tmpbuf[255];
	gchar *body;
	gchar *utf8str;
	gchar *bgColor;
	gchar *direction;
	gchar *textColor;
	gchar buf[500];
	gchar *tmpkey;
	gchar *use_font; 
	gchar *use_font_size; 
	const gchar *text_str;
	gchar *paragraphMark;
	gint count;void chapter_display_mozilla(GtkWidget * html_widget, gchar * mod_name,
		     TEXT_GLOBALS * tgs, gchar * key,
		     gboolean use_globals)
	gboolean was_editable = FALSE;
	gboolean newparagraph = FALSE;
	gboolean use_gtkhtml_font = FALSE;
	bool r2l = FALSE;
	GString *str;
	gint utf8len, cur_verse, cur_chapter, i = 1;

	paragraphMark = "&para;";

	bgColor = "#f1f1f1";
	use_font_size = g_strdup("+1");
	
	str = g_string_new(""); 
	
	gtk_moz_embed_open_stream((GtkMozEmbed *)gtkText, "file:///gtk-embedding.html", "text/html");
	
	direction = (char*)imodule.getConfigEntry("Direction");
	
	if(direction) {
		if(!strcmp(direction,"RtoL")) {
			r2l = true;
			
		}
	}
	
	const char *cur_book;
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	cur_book = key->books[key->Testament() - 1][key->Book() - 1].name;
	
	if(r2l) {
		body = "<body dir=\"rtl\">";
	}
	else {
		body = "<body>";
		
	}
	g_string_sprintf(str,"%s %s",HTML_START,body);
	
	for (key->Verse(1); (key->Book() == curBook && key->Chapter() == curChapter && !imodule.Error()); imodule++) {
		sprintf(tmpbuf, "%s %d:%d", cur_book, key->Chapter(), key->Verse());
		//g_warning(tmpbuf);
		if (key->Verse() == curVerse)
			textColor = "#339766";
		else
			textColor = "#000000";

		sprintf(buf,
			"&nbsp; <A HREF=\"*%s %d:%d\" NAME=\"%d\">"
			"<font color=\"#000DCF\">%d</font></A> ",
			cur_book,
			key->Chapter(),
			key->Verse(),
			key->Verse(),
			key->Verse());
		str = g_string_append(str, buf);

		if (newparagraph) {
			newparagraph = FALSE;
			sprintf(buf,"%s",
				paragraphMark);
			str = g_string_append(str, buf);			
		}
		if(r2l) {
			sprintf(buf,
				"<font size=\"%s\" face=\"ezra sil\" color=\"%s\">",
				use_font_size, textColor);
		}
		else {
			sprintf(buf,
				"<font size=\"%s\" color=\"%s\">",
				use_font_size, textColor);
		}
		str = g_string_append(str, buf);
		
		text_str = (const char*)imodule;
		str = g_string_append(str,(const char*)imodule);
	
		if ((strstr(text_str, "<BR>") == NULL)
		    && (strstr(text_str, "<!P>") == NULL)) {
			sprintf(tmpbuf, " %s", "</font><br>");
		} else {
			sprintf(tmpbuf, " %s", "</font>");
		}
		if (strstr(text_str, "<!P>") == NULL) {
			newparagraph = FALSE;
		} else {
			newparagraph = TRUE;
		}
			
		g_string_append(str, tmpbuf);		
	}
	
	sprintf(buf, "%s", "</body></html>");
	g_string_append(str, buf);
	utf8str = e_utf8_from_gtk_string(gtkText, str->str);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_moz_embed_append_data((GtkMozEmbed *)gtkText,
			utf8str,utf8len);
	}
	sprintf(buf, "#%d", curVerse);
	//gtk_moz_embed_load_url((GtkMozEmbed *)gtkText, buf);
	gtk_moz_embed_close_stream((GtkMozEmbed *)gtkText);
	
	g_string_free(str, TRUE);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/gtkhtml_display.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_module_is_locked_display(GtkWidget * html_widget,
				  gchar * mod_name, gchar * cipher_key)
{
	GtkHTML *html = GTK_HTML(html_widget);
	GtkHTMLStreamStatus status1 = 0;
	GtkHTMLStream *htmlstream;
	gchar buf[500], *utf8str;
	gint utf8len;


	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");

	sprintf(buf, "%s", HTML_START "<body><br>");
	utf8str = e_utf8_from_gtk_string(widgets.html_interlinear, buf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	if (!cipher_key) {
		sprintf(buf, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name, "</b>", _("module is locked."));
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	else {
		sprintf(buf, "%s %s %s %s %s",
			_("The"),
			"<b>",
			mod_name,
			"</b>", _("module has been unlocked."));
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}




	if (!cipher_key) {
		sprintf(buf, "%s %s %s%s%s %s %s %s",
			"<br><br>",
			_("If you have the cipher key you can"),
			"<a href=\"U",
			mod_name,
			"\">",
			_("click here"),
			" </a>", _("to unlock the module"));
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}

		sprintf(buf, "%s%s",
			"<br><br>",
			_
			("You will have to restart GnomeSword after you unlock it."));

		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	else {
		sprintf(buf, "%s%s",
			"<br><br>",
			_("You need to restart GnomeSword to view it"));
		utf8str =
		    e_utf8_from_gtk_string(widgets.html_interlinear,
					   buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}


	strcpy(buf, "</body></html>");
	utf8str = e_utf8_from_gtk_string(html_widget, buf);
	utf8len = strlen(utf8str);
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);

}

/******   end of file   ******/
