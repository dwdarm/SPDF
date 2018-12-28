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

#include <glibmm/ustring.h>
#include "MainWindow.h"

spdf::MainWindow::MainWindow (const Glib::RefPtr<Gtk::Application>& app) 
				: Gtk::ApplicationWindow (app)
{
	m_main_box.set_orientation (Gtk::ORIENTATION_VERTICAL);
	
	m_main_box.pack_start (m_main_toolbar, 0, 0);
	m_main_box.pack_start (m_tabpageview, 1, 1);
	m_main_box.pack_start (m_findview, 0, 0);
	
	add (m_main_box);
}
