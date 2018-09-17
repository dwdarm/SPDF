/*
 * Copyright (C) 2017, Fajar Dwi Darmanto <fajardwidarm@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef SIDEBARVIEW_H
#define SIDEBARVIEW_H

#include <gtkmm/notebook.h>

#ifndef OUTLINEVIEW_H
#include "OutlineView.h"
#endif

namespace spdf {
	
	typedef OutlineView BookmarkView;
	
	class SidebarView : public Gtk::Notebook {
		public:
			SidebarView ();
			void addBookmarkView ();
			void addOutlineView ();
			BookmarkView &getBookmarkView ();
			OutlineView &getOutlineView ();
			
		private:
			Gtk::Notebook m_notebook;
			OutlineView m_outlineview;
			BookmarkView m_bookmarkview;
	};
}

#endif
