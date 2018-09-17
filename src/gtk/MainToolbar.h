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

#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include <gtkmm/toolbar.h>
#include <gtkmm/toolbutton.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/separatortoolitem.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>

namespace spdf {
	class MainToolbar : public Gtk::Toolbar {
		public:
			MainToolbar ();
			Gtk::ToolButton &getOpenButtonItem ();
			Gtk::ToolButton &getCloseButtonItem ();
			Gtk::ToolButton &getPrevButtonItem ();
			Gtk::ToolButton &getNavEntryItem ();
			Gtk::ToolButton &getPagesLabelItem ();
			Gtk::ToolButton &getNextButtonItem ();
			Gtk::ToolItem   &getMarkItem ();
			Gtk::ToolButton &getFindButtonItem ();
			Gtk::ToolButton &getZoominButtonItem ();
			Gtk::ToolButton &getZoomoutButtonItem ();
			Gtk::ToolButton &getAboutButtonItem ();
			
		private:
			Gtk::ToolButton m_open_btn_item;
			Gtk::ToolButton m_close_btn_item;
			Gtk::ToolButton m_prev_btn_item;
			Gtk::ToolButton m_nav_ent_item;
			Gtk::ToolButton m_pages_lbl_item;
			Gtk::ToolButton m_next_btn_item;
			Gtk::ToolItem   m_mark_tool_item;
			Gtk::ToolButton m_find_btn_item;
			Gtk::ToolButton m_zoomin_btn_item;
			Gtk::ToolButton m_zoomout_btn_item;
			Gtk::ToolButton m_about_btn_item;
			
			Gtk::Entry m_nav_ent;
			Gtk::Label m_pages_lbl;
			Gtk::CheckButton m_mark_btn;
			
			Gtk::SeparatorToolItem m_sep1;
			Gtk::SeparatorToolItem m_sep2;
			Gtk::SeparatorToolItem m_sep3;
			Gtk::SeparatorToolItem m_sep4;
			Gtk::SeparatorToolItem m_sep5;
	};
}

#endif
