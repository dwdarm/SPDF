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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include "PageNavPopup.h" //

#ifndef MAINTOOLBAR_H
#include "MainToolbar.h"
#endif

#ifndef TABPAGEVIEW_H
#include "TabPageView.h"
#endif

#ifndef FINDVIEW_H
#include "FindView.h"
#endif

namespace spdf {
	class MainWindow : public Gtk::Window {
		public:
			MainWindow ();
			
		protected:
			Gtk::Box m_main_box;
			MainToolbar m_main_toolbar;
			TabPageView m_tabpageview;
			FindView m_findview;
			PageNavPopup m_page_nav_popup;
	};
}

#endif
