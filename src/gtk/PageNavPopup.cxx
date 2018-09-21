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

#include "PageNavPopup.h"

spdf::PageNavPopup::PageNavPopup () 
		: Gtk::Menu (),
	 	  m_copy_item ("Copy"),
		  m_prev_item ("Previous page"),
		  m_next_item ("Next page"),
		  m_first_item ("First page"),
		  m_last_item ("Last page"),
		  m_zoomout_item ("Zoom out"),
		  m_zoomin_item ("Zoom in"),
		  m_mark_item ("Marked")
{
	append (m_copy_item);
	append (m_sep1);
	append (m_first_item);
	append (m_prev_item);
	append (m_next_item);
	append (m_last_item);
	append (m_sep2);
	append (m_zoomout_item);
	append (m_zoomin_item);
	append (m_sep3);
	append (m_mark_item);
	
	show_all ();
}

Gtk::MenuItem &
spdf::PageNavPopup::getCopyMenuItem ()
{
	return m_copy_item;
}

Gtk::MenuItem &
spdf::PageNavPopup::getPrevMenuItem ()
{
	return m_prev_item;
}

Gtk::MenuItem &
spdf::PageNavPopup::getNextMenuItem ()
{
	return m_next_item;
}

Gtk::MenuItem &
spdf::PageNavPopup::getFirstMenuItem ()
{
	return m_first_item;
}

Gtk::MenuItem &
spdf::PageNavPopup::getLastMenuItem ()
{
	return m_last_item;
}

Gtk::MenuItem &
spdf::PageNavPopup::getZoomoutMenuItem ()
{
	return m_zoomout_item;
}

Gtk::MenuItem &
spdf::PageNavPopup::getZoominMenuItem ()
{
	return m_zoomin_item;
}

Gtk::CheckMenuItem &
spdf::PageNavPopup::getMarkMenuItem ()
{
	return m_mark_item;
}
