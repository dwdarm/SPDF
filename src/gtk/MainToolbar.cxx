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

#include "MainToolbar.h"

spdf::MainToolbar::MainToolbar () 
				  : Gtk::Toolbar (),
					m_mark_btn ("Bookmark")
{
	m_open_btn_item.set_icon_name ("document-open");
	m_open_btn_item.set_tooltip_text ("Open an existing document");
	append (m_open_btn_item);
	
	m_close_btn_item.set_icon_name ("window-close");
	m_close_btn_item.set_tooltip_text ("Close the current document");
	append (m_close_btn_item);
	
	append (m_sep1);
	
	m_prev_btn_item.set_icon_name ("go-previous");
	m_prev_btn_item.set_tooltip_text ("Previouse page");
	append (m_prev_btn_item);
	
	m_nav_ent.set_width_chars (4);
	m_nav_ent_item.remove ();
	m_nav_ent_item.add (m_nav_ent);
	append (m_nav_ent_item);
	
	m_pages_lbl.set_text ("of 0");
	m_pages_lbl_item.remove ();
	m_pages_lbl_item.add (m_pages_lbl);
	append (m_pages_lbl_item);
	
	m_next_btn_item.set_icon_name ("go-next");
	m_next_btn_item.set_tooltip_text ("Next page");
	append (m_next_btn_item);
	
	append (m_sep2);
	
	m_mark_tool_item.add (m_mark_btn);
	append (m_mark_tool_item);
	
	append (m_sep3);
	
	m_find_btn_item.set_icon_name ("edit-find");
	//m_find_btn_item.set_tooltip_text ("Next page");
	append (m_find_btn_item);
	
	append (m_sep4);
	
	m_zoomin_btn_item.set_icon_name ("zoom-in");
	m_zoomin_btn_item.set_tooltip_text ("Zoom in");
	append (m_zoomin_btn_item);
	
	m_zoomout_btn_item.set_icon_name ("zoom-out");
	m_zoomout_btn_item.set_tooltip_text ("Zoom out");
	append (m_zoomout_btn_item);
	
	m_fscreen_btn_item.set_icon_name ("view-fullscreen");
	m_fscreen_btn_item.set_tooltip_text ("Full screen");
	append (m_fscreen_btn_item);
	
	append (m_sep5);
	
	m_about_btn_item.set_icon_name ("help-about");
	m_about_btn_item.set_tooltip_text ("About");
	append (m_about_btn_item);
}

Gtk::ToolButton &
spdf::MainToolbar::getOpenButtonItem ()
{
	return m_open_btn_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getCloseButtonItem ()
{
	return m_close_btn_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getPrevButtonItem ()
{
	return m_prev_btn_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getNavEntryItem ()
{
	return m_nav_ent_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getPagesLabelItem ()
{
	return m_pages_lbl_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getNextButtonItem ()
{
	return m_next_btn_item;
}

Gtk::ToolItem &
spdf::MainToolbar::getMarkItem ()
{
	return m_mark_tool_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getFindButtonItem ()
{
	return m_find_btn_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getZoominButtonItem ()
{
	return m_zoomin_btn_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getZoomoutButtonItem ()
{
	return m_zoomout_btn_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getFscreenButtonItem ()
{
	return m_fscreen_btn_item;
}

Gtk::ToolButton &
spdf::MainToolbar::getAboutButtonItem ()
{
	return m_about_btn_item;
}
