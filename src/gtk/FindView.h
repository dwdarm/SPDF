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

#ifndef FINDVIEW_H
#define FINDVIEW_H

#include <gtkmm/toolbar.h>
#include <gtkmm/toolbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>

namespace spdf {
	
	class FindView : public Gtk::Toolbar {
		public: 
			FindView ();
			Gtk::Entry &getEntry ();
			
		private:
			Gtk::ToolItem m_label_item;
			Gtk::ToolItem m_entry_item;
			Gtk::ToolButton m_prev_btn;
			Gtk::ToolButton m_next_btn;
			Gtk::ToolButton m_close_btn;
			
			Gtk::Label m_label;
			Gtk::Entry m_entry;
			
			void on_close_btn_clicked ();
	};
}

#endif
