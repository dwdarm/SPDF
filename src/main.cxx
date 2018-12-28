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

#include "MainApp.h"

static void
on_activate (Glib::RefPtr<Gtk::Application>& app) 
{
	spdf::MainApp *main_app = new spdf::MainApp (app);
}

static void
on_open_file (const std::vector<Glib::RefPtr<Gio::File>> &files, const Glib::ustring &hint, Glib::RefPtr<Gtk::Application>& app)
{
	spdf::MainApp *main_app;
	Glib::ustring filename;
	
	for (auto it = files.cbegin (); it != files.cend (); it++) {
		filename = (*it)->get_path ();
		break;
	}
	
	app->activate ();
	
	main_app = (spdf::MainApp*) app->get_active_window ();
	main_app->load (filename);
}

int main (int argc, char **argv)
{
	auto app = Gtk::Application::create("org.gtkmm.spdf.base", Gio::APPLICATION_HANDLES_OPEN);
	
	app->signal_activate ().connect (sigc::bind<Glib::RefPtr<Gtk::Application>&> (sigc::ptr_fun (on_activate), app));
	app->signal_open ().connect (sigc::bind<Glib::RefPtr<Gtk::Application>&> (sigc::ptr_fun (on_open_file), app));
	
	return app->run(argc, argv);
}
