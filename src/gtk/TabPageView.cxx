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

#include "TabPageView.h"

spdf::TabPageView::TabPageView () : Gtk::Notebook ()
{
	m_new_tab_btn.set_image_from_icon_name ("list-add", Gtk::ICON_SIZE_BUTTON);
	m_new_tab_btn.set_tooltip_text ("Open a new tab");
	m_new_tab_btn.signal_clicked ().connect 
				(sigc::mem_fun (*this, &TabPageView::on_new_tab_click));
	m_new_tab_btn.show_all ();
	
	set_action_widget (&m_new_tab_btn);	
	set_scrollable ();
	set_show_border ();
	set_show_tabs ();
	show_all ();
}

spdf::PageView *
spdf::TabPageView::appendPage (const Glib::ustring &title)
{
	PageView *pageview = new PageView ();
	int index = 0;
	
	pageview->getTabHeaderView ().getLabel ().set_text (title);
	index = append_page (*pageview, pageview->getTabHeaderView ());
	
	show_all ();
	set_current_page (index);
	
	return pageview;
}

void 
spdf::TabPageView::closePage (spdf::PageView *child)
{
	if (get_n_pages	() > 1) {
		remove_page (*child);
		delete (child);
	}
	
	show_all ();
}

int 
spdf::TabPageView::getCurrentIndex () const
{
	return get_current_page ();
}

spdf::PageView *
spdf::TabPageView::getPage (int index)
{
	return (PageView*)get_nth_page (index);
}

spdf::PageView *
spdf::TabPageView::getCurrentPage ()
{
	return (PageView*)get_nth_page (get_current_page ());
}

void
spdf::TabPageView::on_new_tab_click ()
{
	appendPage ("Document");
	show_all ();
}
