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

#ifndef TABPAGEVIEW_H
#define TABPAGEVIEW_H

#include <gtkmm/notebook.h>

#ifndef PAGEVIEW_H
#include "PageView.h"
#endif

namespace spdf {
	class TabPageView : public Gtk::Notebook {
		public:
			TabPageView ();
			PageView *appendPage (const Glib::ustring &title);
			void closePage (PageView *child);
			int getCurrentIndex () const;
			PageView *getCurrentPage ();
			PageView *getPage (int index);
			
		private:
			Gtk::Button m_new_tab_btn;
			
		// slot:
			void on_new_tab_click ();
	};
}

#endif
