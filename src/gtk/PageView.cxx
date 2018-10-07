#include <gtkmm/label.h>
#include "TabPageView.h"
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

#include <pangomm/layout.h>


// TabHeaderView definition
spdf::TabHeaderView::TabHeaderView () : Gtk::Box ()
{
	m_tab_lbl.set_width_chars (12);
	m_tab_lbl.set_max_width_chars (16);	
	m_tab_lbl.set_text ("Document");
	m_tab_lbl.set_ellipsize (Pango::ELLIPSIZE_END);
	m_tab_lbl.set_tooltip_text (m_tab_lbl.get_text ());
	
	m_tab_close_img.set_from_icon_name ("window-close", Gtk::IconSize (Gtk::ICON_SIZE_SMALL_TOOLBAR));
	//m_tab_close_btn.set_icon_widget (m_tab_close_img);
	//m_tab_close_btn.set_homogeneous (false);
	m_tab_close_btn.add (m_tab_close_img);
	//m_tab_close_btn.set_expand (false);
	m_tab_close_btn.set_tooltip_text ("Close tab");
	
	pack_start (m_tab_lbl, false, false, 5);
	pack_start (m_tab_close_btn, false, false, 5);
	
	show_all ();
}

Gtk::EventBox &
spdf::TabHeaderView::getButton ()
{
	return m_tab_close_btn;
}

Gtk::Label &
spdf::TabHeaderView::getLabel ()
{
	return m_tab_lbl;
}

void 
spdf::TabHeaderView::setTitle (Glib::ustring &title)
{
	m_tab_lbl.set_text (title);
	m_tab_lbl.set_tooltip_text (m_tab_lbl.get_text ());
}

// PageView definition
int spdf::PageView::count_id = 1;

spdf::PageView::PageView () : Gtk::Paned ()
{	
	m_id = spdf::PageView::count_id;
	m_index = 0;
	m_scale = 1.0;
	
	add2 (m_imageview);
	set_wide_handle ();
	
	m_tabheaderview.getButton ().signal_button_press_event ().connect 
				   (sigc::mem_fun (*this, &PageView::on_tab_close_btn));
				   
	spdf::PageView::count_id++;
}

spdf::ImageView &
spdf::PageView::getImageView ()
{
	return m_imageview;
} 

spdf::SidebarView &
spdf::PageView::getSidebarView ()
{
	return m_sidebarview;
}

spdf::TabHeaderView &
spdf::PageView::getTabHeaderView ()
{
	return m_tabheaderview;
}

void 
spdf::PageView::hideSidebarView ()
{
	if (get_child1 ()) {
		remove (*(get_child1 ()));
	}
	
	show_all ();
}

void 
spdf::PageView::showSidebarView ()
{
	if (!get_child1 ()) {
		add1 (m_sidebarview);
	}
	
	show_all ();
}

bool
spdf::PageView::on_tab_close_btn (GdkEventButton *event)
{
	spdf::TabPageView *parent = NULL;
	
	if (event->button == 1) {
		parent = (spdf::TabPageView*)this->get_parent ();
		parent->closePage (this);
	}
	
	return true;
}
