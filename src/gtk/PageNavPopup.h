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

#ifndef PAGENAVPOPUP_H
#define PAGENAVPOPUP_H

#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/separatormenuitem.h>

namespace spdf {
	class PageNavPopup : public Gtk::Menu {
		public:
			PageNavPopup ();
			Gtk::MenuItem &getPrevMenuItem ();
			Gtk::MenuItem &getNextMenuItem ();
			Gtk::MenuItem &getFirstMenuItem ();
			Gtk::MenuItem &getLastMenuItem ();
			Gtk::MenuItem &getZoomoutMenuItem ();
			Gtk::MenuItem &getZoominMenuItem ();
			Gtk::CheckMenuItem &getMarkMenuItem ();
			
		private:
			Gtk::MenuItem m_prev_item;
			Gtk::MenuItem m_next_item;
			Gtk::MenuItem m_first_item;
			Gtk::MenuItem m_last_item;
			Gtk::MenuItem m_zoomout_item;
			Gtk::MenuItem m_zoomin_item;
			Gtk::CheckMenuItem m_mark_item;
			
			Gtk::SeparatorMenuItem m_sep1;
			Gtk::SeparatorMenuItem m_sep2;
			
	};
}

#endif
