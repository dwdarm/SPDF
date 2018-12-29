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

#include "FindView.h"

spdf::FindView::FindView () : Gtk::Toolbar (), m_label ("Find: ")
{
	m_label_item.add (m_label);
	m_entry_item.add (m_entry);
	m_search_btn.set_icon_name ("edit-find");
	m_prev_btn.set_icon_name ("go-previous");
	m_next_btn.set_icon_name ("go-next");
	m_close_btn.set_icon_name ("window-close");
	
	append (m_label_item);
	append (m_entry_item);
	append (m_search_btn);
	//append (m_prev_btn);
	//append (m_next_btn);
	append (m_close_btn);
	
	m_close_btn.signal_clicked ().connect (sigc::mem_fun 
						(*this, &spdf::FindView::on_close_btn_clicked));
	
	show_all ();
}

Gtk::Entry &
spdf::FindView::getEntry ()
{
	return m_entry;
}

Gtk::ToolButton &
spdf::FindView::getSearchButtonItem ()
{
	return m_search_btn;
}

void 
spdf::FindView::on_close_btn_clicked ()
{
	hide ();
}
