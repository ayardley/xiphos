/*
 * GnomeSword Bible Study Tool
 * display.hh - 
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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

#include <gtk/gtk.h>
#include <swmgr.h>
#include <swdisp.h>
#include "backend/sword_main.hh"
#include "gui/utilities.h"

using namespace sword;

class GTKEntryDisp : public SWDisplay {
protected:
	GtkWidget *gtkText;
	BackEnd *be;
public:	
	GTKEntryDisp(GtkWidget *gtkText, BackEnd *be) 
					 { this->gtkText = gtkText; 
					   this->be = be;}
	virtual char Display(SWModule &imodule);
};

class GTKChapDisp : public GTKEntryDisp {
public:
	GTKChapDisp(GtkWidget *gtkText, BackEnd *be) : GTKEntryDisp(gtkText,be) {}
	virtual char Display(SWModule &imodule);
	virtual void getVerseBefore(SWModule &imodule);
	virtual void getVerseAfter(SWModule &imodule);
	MOD_FONT *mf;
	SWBuf swbuf;
};

#ifdef USE_GTKMOZEMBED
class GTKMozEntryDisp : public  SWDisplay{
protected:
	GtkWidget *gtkText;
	BackEnd *be;
public:
	GTKMozEntryDisp(GtkWidget *gtkText, BackEnd *be) {this->gtkText = gtkText;this->be = be;}
	virtual char Display(SWModule &imodule);
};

class GtkMozChapDisp : public GTKMozEntryDisp {
public:
	GtkMozChapDisp(GtkWidget *gtkText, BackEnd *be) : GTKMozEntryDisp(gtkText,be) {}
	virtual char Display(SWModule &imodule);
	virtual void getVerseBefore(SWModule &imodule);
	virtual void getVerseAfter(SWModule &imodule);
	MOD_FONT *mf;
};
#endif


class GTKTextviewChapDisp : public GTKEntryDisp {
public:
	GTKTextviewChapDisp(GtkWidget *gtkText, BackEnd *be) : GTKEntryDisp(gtkText,be) {}
	virtual char Display(SWModule &imodule);
};

class DialogEntryDisp : public SWDisplay {
protected:
	GtkWidget *gtkText;
	BackEnd *be;
public:
	DialogEntryDisp(GtkWidget *gtkText, BackEnd *be) {this->gtkText = gtkText;this->be = be;}
	virtual char Display(SWModule &imodule);
};

class DialogChapDisp : public  DialogEntryDisp {
public:
	DialogChapDisp(GtkWidget *gtkText, BackEnd *be) : DialogEntryDisp(gtkText,be) {}
	virtual char Display(SWModule &imodule);
};
class DialogTextviewChapDisp : public  DialogEntryDisp {
public:
	DialogTextviewChapDisp(GtkWidget *gtkText, BackEnd *be) : DialogEntryDisp(gtkText,be) {}
	virtual char Display(SWModule &imodule);
};
