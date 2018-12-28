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

#ifndef PAGEVIEW_H
#define PAGEVIEW_H

#include <gtkmm/paned.h>
#include <gtkmm/box.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/label.h>

#ifndef SIDEBARVIEW_H
#include "SidebarView.h"
#endif

#ifndef IMAGEVIEW_H
#include "ImageView.h"
#endif


#ifndef DOCUMENT_H
#include "../Document.h"
#endif

#include <memory>
#include <vector>

namespace spdf {
	
	class TabHeaderView : public Gtk::Box {
		public:
			TabHeaderView ();
			Gtk::EventBox &getButton ();
			Gtk::Label &getLabel ();
			void setTitle (Glib::ustring &title);
			
		private:
			Gtk::Label m_tab_lbl;
			Gtk::Image m_tab_close_img;
			Gtk::EventBox m_tab_close_btn;
			
	};
	
	class PageView : public Gtk::Paned {
		public:
			PageView ();
			void *getData ();
			Gtk::EventBox &getEventBox ();
			ImageView &getImageView ();
			SidebarView &getSidebarView ();
			TabHeaderView &getTabHeaderView ();
			void hideSidebarView ();
			void setData (void *data);
			void showSidebarView ();
			
			int m_id;
			static int count_id;
			
		private:
			Gtk::ScrolledWindow m_scroll;
			Gtk::EventBox m_box;
			ImageView m_imageview;
			SidebarView m_sidebarview;
			TabHeaderView m_tabheaderview;
			void *m_data;
			
		// slot:
			bool on_tab_close_btn (GdkEventButton *event);
			void on_page_realize ();
	};
}

#endif
