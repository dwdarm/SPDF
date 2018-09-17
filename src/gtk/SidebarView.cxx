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

#include <gtkmm/label.h>
#include "SidebarView.h"

spdf::SidebarView::SidebarView () : Gtk::Notebook ()
{
	set_tab_pos (Gtk::POS_BOTTOM);
	set_scrollable ();
	set_show_border ();
	set_show_tabs ();
	
	append_page (m_outlineview, "Outline");
	append_page (m_bookmarkview, "Bookmark");
}

spdf::BookmarkView &
spdf::SidebarView::getBookmarkView ()
{
	return m_bookmarkview;
}

spdf::OutlineView &
spdf::SidebarView::getOutlineView ()
{
	return m_outlineview;
}
