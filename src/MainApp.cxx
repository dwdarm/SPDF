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

#include <cstdlib>
#include <stdexcept> 
#include <gtkmm/treepath.h>
#include <gtkmm/hvscrollbar.h>
#include <glibmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/aboutdialog.h>
#include "MainApp.h"
#include "RendererTask.h"
#include "TaskScheduler.h"
#include "Cache.h"
#include "Message.h"
#include "MessageQueue.h"
#include "ConfigParser.h"
#include "Bookmark.h"

#if defined(_WIN32)
#include <direct.h>
#else 
#include <sys/stat.h>
#endif

#define THREAD_NUM "thread_num"
#define CACHE_SIZE "cache_size"

#define DEFAULT_THREAD_NUM_VALUE 2
#define DEFAULT_CACHE_SIZE_VALUE 8

spdf::MainApp::MainApp () : spdf::MainWindow ()
{	
	
	// configuration
	ConfigParser parser;
	
	#if defined(_WIN32)
	m_path = "config";
	_mkdir(m_path.c_str());
	m_path += "\\";
	
    #else 
    m_path = getenv("HOME");
	m_path += "/.config/spdf";
	mkdir(m_path.c_str(), 0755); 
	m_path += "/";
    #endif
	
	std::string cfg_path = m_path + "config.cfg";
	m_configs = parser.parse (cfg_path);
	if (!m_configs.size ()) {
		m_configs[THREAD_NUM] = std::to_string (DEFAULT_THREAD_NUM_VALUE);
		m_configs[CACHE_SIZE] = std::to_string (DEFAULT_CACHE_SIZE_VALUE);
		parser.save (cfg_path, m_configs);
	}
	
	// init TaskScheduler
	if (m_configs.find (THREAD_NUM) == m_configs.end ()) {
		m_configs[THREAD_NUM] = std::to_string (DEFAULT_THREAD_NUM_VALUE);
		parser.save (cfg_path, m_configs);
	}
	TaskScheduler::instance ()->init (std::stoi (m_configs[THREAD_NUM]));
	
	// init cache
	if (m_configs.find (CACHE_SIZE) == m_configs.end ()) {
		m_configs[CACHE_SIZE] = std::to_string (DEFAULT_CACHE_SIZE_VALUE);
		parser.save (cfg_path, m_configs);
	}
	spdf::Cache::instance ()->setDeleteHandler (
			[](void *data) { 
				spdf::Image *image = static_cast<spdf::Image*> (data);
				delete (image);
			}
	);
	spdf::Cache::instance ()->setMaxSize (std::stoi (m_configs[CACHE_SIZE]));
	
	// load bookmarks
	std::string mark_path = m_path + "marks.cfg";
	spdf::Bookmark::instance ()->open (mark_path);
	
	// register toolbar button signal
	m_main_toolbar.getOpenButtonItem ().signal_clicked ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_open_btn_clicked));
				 
	m_main_toolbar.getCloseButtonItem ().signal_clicked ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_close_btn_clicked));
				 
	m_main_toolbar.getPrevButtonItem ().signal_clicked ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_prev_btn_clicked));
				 
	m_main_toolbar.getNextButtonItem ().signal_clicked ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_next_btn_clicked));
				 
	m_main_toolbar.getFindButtonItem ().signal_clicked ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_find_btn_clicked));
				 
	m_main_toolbar.getZoominButtonItem ().signal_clicked ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_zoomin_btn_clicked));
				 
	m_main_toolbar.getZoomoutButtonItem ().signal_clicked ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_zoomout_btn_clicked));
				 
	m_main_toolbar.getAboutButtonItem ().signal_clicked ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_about_btn_clicked));
	
	Gtk::Entry *entry = static_cast<Gtk::Entry*>
						(m_main_toolbar.getNavEntryItem ().get_child ());			 
	entry->signal_activate ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_entry_text_changed));
				 
	Gtk::CheckButton *check = static_cast<Gtk::CheckButton*>
				           (m_main_toolbar.getMarkItem ().get_child ());
	check->signal_toggled ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_mark_btn_toggled));
				 
	m_main_toolbar.getFscreenButtonItem ().signal_clicked ().connect
				  (sigc::mem_fun (*this, &MainApp::on_fscreen_btn_clicked));
	
	// register popup menu signal
	
	m_page_nav_popup.getCopyMenuItem ().signal_activate ().connect
				  (sigc::mem_fun (*this, &MainApp::on_copy_btn_clicked));
	
	m_page_nav_popup.getFirstMenuItem ().signal_activate ().connect
				  (sigc::mem_fun (*this, &MainApp::on_first_btn_clicked));
				  
	m_page_nav_popup.getPrevMenuItem ().signal_activate ().connect
				  (sigc::mem_fun (*this, &MainApp::on_prev_btn_clicked));
				  
	m_page_nav_popup.getNextMenuItem ().signal_activate ().connect
				  (sigc::mem_fun (*this, &MainApp::on_next_btn_clicked));
				  
	m_page_nav_popup.getLastMenuItem ().signal_activate ().connect
				  (sigc::mem_fun (*this, &MainApp::on_last_btn_clicked));
				  
	m_page_nav_popup.getZoominMenuItem ().signal_activate ().connect
				  (sigc::mem_fun (*this, &MainApp::on_zoomin_btn_clicked));
				  
	m_page_nav_popup.getZoomoutMenuItem ().signal_activate ().connect
				  (sigc::mem_fun (*this, &MainApp::on_zoomout_btn_clicked));
				 
	m_tabpageview.signal_switch_page ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_tab_page_changed));
				 
	m_tabpageview.signal_page_added ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_tab_page_added));
				 
	m_findview.getEntry ().signal_activate ().connect 
		  (sigc::mem_fun (*this, &MainApp::on_find_entry_text_changed));
	
	m_selecting = false;	
	Glib::signal_timeout ().connect (sigc::mem_fun 
								 (*this, &MainApp::on_timeout_sel), 100);		 
	Glib::signal_timeout ().connect (sigc::mem_fun 
								 (*this, &MainApp::on_timeout_msg), 10);
	
	add_events (Gdk::KEY_PRESS_MASK);							 
	signal_key_press_event ().connect 
		   (sigc::mem_fun (*this, &MainApp::on_mainapp_key_press_event), false);
	
	set_title ("SPDF");
	set_size_request (800, 600);
	show_all ();
	
	m_findview.hide ();
	m_tabpageview.appendPage ("Document");
}

void 
spdf::MainApp::open_document (Glib::ustring &path, spdf::PageView &pageview)
{
	UString filename, upass, opass;
	DocumentError err;
	
	close_document (pageview);
	
	filename = path.data ();
	pageview.m_document = std::shared_ptr<Document>
				(DocumentCreator::instance ()->openDocument 
										(filename, upass, opass, &err));
										
	if (!pageview.m_document.get ()) {
		
		Gtk::MessageDialog msg ("");
		
		// PDF file error
		if (err.type == PDF) {
			// error code from ErrorCodes.h in poppler library
			if (err.code == 1) {
				msg.set_message ("couldn't open the PDF file");
				msg.run ();
				return;
			} else if (err.code == 2) {
				msg.set_message ("couldn't read the page catalog");
				msg.run ();
				return;
			} else if (err.code == 3) {
				msg.set_message ("PDF file was damaged and couldn't be repaired");
				msg.run ();
				return;
			} else if (err.code == 4) {
				int ret;
				Gtk::Entry pass_entry;
				
				msg.set_message ("Enter Password:");
				msg.get_message_area ()->pack_start (pass_entry, 1, 1);
				msg.show_all ();
				msg.run ();
				
				opass = pass_entry.get_text ().data ();
				pageview.m_document = std::shared_ptr<Document>
				             (DocumentCreator::instance ()->openDocument 
										(filename, upass, opass, &err));
										
				if (!pageview.m_document.get ()) {
					msg.set_message ("file was encrypted and password was incorrect or not supplied");
					msg.get_message_area ()->remove (pass_entry);
					msg.run ();
					return;
				}
			} 
		
		// unknown file	
		} else {
			msg.set_message ("document file is not supported");
			msg.run ();
			return;
		}
	}
	
	load_document (pageview);
}

void 
spdf::MainApp::load_document (spdf::PageView &pageview)
{
	Glib::ustring title;
	
	title = pageview.m_document->getTitle ().data ();
	pageview.getTabHeaderView ().setTitle (title);
	
	fill_outline (pageview);
	fill_bookmark (pageview);
	
	pageview.showSidebarView ();
	
	update_toolbar (pageview);
	
	draw_page (pageview);
}

void 
spdf::MainApp::close_document (spdf::PageView &pageview)
{
	Glib::ustring title = "Document";

	pageview.getTabHeaderView ().setTitle (title);
	
	pageview.getImageView ().clearImage ();
	
	pageview.m_document.reset ();
	pageview.m_index = 0;
	pageview.m_scale = 1.0;
	
	pageview.getSidebarView ().getOutlineView ().clear ();
	pageview.getSidebarView ().getBookmarkView ().clear ();
	pageview.hideSidebarView ();
	
	update_toolbar (pageview);
}

void 
spdf::MainApp::go_to_page (spdf::PageView &pageview, int index)
{
	int in = 0;
	
	if (!pageview.m_document.get ()) {
		return;
	}
	
	in = index;
	if (in == -1) {
		in = pageview.m_document->getPages () - 1;
	}
	
	if ((in >= 0) && (in < pageview.m_document->getPages ())) {
		pageview.m_index = in;
		update_toolbar (pageview);
		draw_page (pageview);
	}
}

void 
spdf::MainApp::go_to_next_page (spdf::PageView &pageview)
{
	int index = 0;
	
	if (!pageview.m_document.get ()) {
		return;
	}
	
	index = pageview.m_index + 1;
	if (index < pageview.m_document->getPages ()) {
		pageview.m_index = index;
		update_toolbar (pageview);
		draw_page (pageview);
	}
}

void 
spdf::MainApp::go_to_prev_page (spdf::PageView &pageview)
{
	int index = 0;
	
	if (!pageview.m_document.get ()) {
		return;
	}
	
	index = pageview.m_index - 1;
	if (index >= 0) {
		pageview.m_index = index;
		update_toolbar (pageview);
		draw_page (pageview);
	}
}

void 
spdf::MainApp::zoom_in_page (spdf::PageView &pageview, double f)
{
	double scale = 0.0;
	
	if (!pageview.m_document.get ()) {
		return;
	}
	
	scale = pageview.m_scale + f;
	
	pageview.m_scale = scale;
	update_toolbar (pageview);
	draw_page (pageview);
}

void 
spdf::MainApp::zoom_out_page (spdf::PageView &pageview, double f)
{
	double scale = 0.0;
	
	if (!pageview.m_document.get ()) {
		return;
	}
	
	scale = pageview.m_scale - f;
	if (scale < 0.25) {
		return;
	}
	
	pageview.m_scale = scale;
	update_toolbar (pageview);
	draw_page (pageview);
}

void 
spdf::MainApp::mark_current_page (spdf::PageView &pageview)
{
	if (!pageview.m_document.get ()) {
		return;
	}
	
	UString title = pageview.m_document->getTitle ();
	std::string temp = title.data ();
	int index = pageview.m_index;
	Gtk::CheckButton *check = static_cast<Gtk::CheckButton*>
				           (m_main_toolbar.getMarkItem ().get_child ());
	
	if (check->get_active ()) {
		if (Bookmark::instance ()->find (temp, index)) {
			return;
		}
		Bookmark::instance ()->add (temp, index);
		fill_bookmark (pageview);
	} else {
		if (!Bookmark::instance ()->find (temp, index)) {
			return;
		}
		Bookmark::instance ()->erase (temp, index);
		fill_bookmark (pageview);
	}
}

void 
spdf::MainApp::go_fullscreen ()
{
	get_window ()->fullscreen ();
}

void 
spdf::MainApp::go_unfullscreen ()
{
	get_window ()->unfullscreen ();
}

// draw a rendered page
void
spdf::MainApp::draw_page (spdf::PageView &pageview)
{
	spdf::Image *image = NULL;
	std::string key;
	
	// encoded = id-scale-index
	key = std::to_string (pageview.m_document->getId ()) 
							  + "-" + std::to_string (pageview.m_scale)
							 + "-" + std::to_string (pageview.m_index);
	pageview.getImageView ().clearImage ();
	spdf::Cache::instance ()->lock ();
	image = static_cast<spdf::Image*> (spdf::Cache::instance ()->get (key));
	if (image) {
		pageview.getImageView ().setImage (
			static_cast<const unsigned char*> (image->getData ()), image->getWidth (),
			image->getHeight (), image->getStride ());
		spdf::Cache::instance ()->unlock ();
		return;
	}
	spdf::Cache::instance ()->unlock ();
	spdf::TaskScheduler::instance ()->assign (new spdf::RendererTask 
						(pageview.m_document.get (), pageview.m_index, 
												pageview.m_scale, key));
}

// populate outline sidebar
void
spdf::MainApp::fill_outline (spdf::PageView &pageview)
{
	std::shared_ptr<DocumentOutline> outline 
								   (pageview.m_document->getOutline ());
								  
	pageview.getSidebarView ().getOutlineView ().clear ();
	
	if (!outline.get ()) {
		return;
	}
	
	Gtk::TreeIter iter =
		   pageview.getSidebarView ().getOutlineView ().getTreeIter ();
	
	walk_fill_outline (pageview.getSidebarView ().getOutlineView (),
											outline ->getRoot (), iter);
}

void 
spdf::MainApp::walk_fill_outline (spdf::OutlineView &outlineview,
			           std::vector<spdf::DocumentOutlineItem> &toc_item, 
													 Gtk::TreeIter &iter)
{
	std::vector<DocumentOutlineItem>::iterator it;
	Gtk::TreeIter parent;
	Glib::ustring title;
	
	for (it = toc_item.begin (); it != toc_item.end (); it++) {
		title = (*it).getTitle ().data ();
		parent = outlineview.append (title, (*it).getIndex (), iter);
		walk_fill_outline (outlineview, (*it).getChild (), parent);
	}
}

// populate bookmark sidebar
void
spdf::MainApp::fill_bookmark (spdf::PageView &pageview)
{
	std::vector<int> marks;
	UString title;
	Glib::ustring text;
	int index = 0;
	int start = 0;
	int end = 0;
	std::string temp;
	
	pageview.getSidebarView ().getBookmarkView ().clear ();
	title = pageview.m_document->getTitle ();
	temp = title.data ();
	marks = Bookmark::instance ()->get (temp);
	
	if (marks.empty ()) {
		return;
	}
	
	for (auto it = marks.begin (); it != marks.end (); it++) {
		text = "Page " + std::to_string (*it + 1);
		index = (*it);
		pageview.getSidebarView ().getBookmarkView ().append 
		                                              (text, index + 1);
	}
}

// update toolbar
void 
spdf::MainApp::update_toolbar (spdf::PageView &pageview)
{
	Gtk::Entry *entry = NULL;
	Gtk::Label *label = NULL;
	Gtk::CheckButton *check = NULL;
	Glib::ustring pages;
	Glib::ustring index;
	UString title;
	bool active = false;
	std::string temp;
	
	entry = dynamic_cast<Gtk::Entry*> 
					   (m_main_toolbar.getNavEntryItem ().get_child ());
	label = dynamic_cast<Gtk::Label*> 
	                 (m_main_toolbar.getPagesLabelItem ().get_child ());
	check = dynamic_cast<Gtk::CheckButton*>
				           (m_main_toolbar.getMarkItem ().get_child ());
	pages = " of 0";
	index = "0";
	
	if(pageview.m_document.get ()) {
		pages = " of " + std::to_string (pageview.m_document->getPages ());
		index = std::to_string (pageview.m_index + 1);
		title = pageview.m_document->getTitle ();
		temp = title.data ();
		active = Bookmark::instance ()->find (temp, pageview.m_index);
	}
	
	entry->set_text (index);
	label->set_text (pages);
	check->set_active (active);
}

// find text
void 
spdf::MainApp::find_text (spdf::PageView &pageview, std::string &str)
{
	static Rect rect = {0, 0, 0, 0};
	static std::string text;
	
	std::shared_ptr<DocumentPage> page;
	ImageViewRect irect;
	
	if (text != str) {
		rect = {0, 0, 0, 0};
		text = str;
	}
	
	page = std::shared_ptr<DocumentPage> 
				   (pageview.m_document->createPage (pageview.m_index));
	rect = page->searchRect (rect, text, pageview.m_scale, SEARCH_DIRECTION_NEXT);
	
	irect.color.r = 0;
	irect.color.g = 0;
	irect.color.b = 0;
	irect.color.a = 100;
	pageview.getImageView ().refresh ();
	
	irect.x = rect.x;
	irect.y = rect.y;
	irect.width = rect.width;
	irect.height = rect.height;
		
	pageview.getImageView ().clearImage ();
	pageview.getImageView ().appendRect (irect);
	
	rect.x = rect.x + rect.width;
	rect.y = rect.y + rect.height;
}

void 
spdf::MainApp::draw_selection_page (spdf::PageView &pageview)
{
	std::shared_ptr<DocumentPage> page;
	std::shared_ptr<ImageViewPointer> pointer;
	std::vector<ImageViewRect> irects;
	ImageViewRect irect;
	
	if (!pageview.m_document.get ()) {
		return;
	}
	
	pageview.getImageView ().refresh ();
	m_selected_regs.clear ();
	
	pointer = pageview.getImageView ().getPointer ();
								
	if ((pointer->x != -1) && (pointer->y != -1)) {
		m_selected_rect.width = pointer->x;
		m_selected_rect.height = pointer->y;
		
		if (((m_selected_rect.width - m_selected_rect.x) == 0) &&
			((m_selected_rect.height - m_selected_rect.y) == 0)) {
				
			m_selected_regs.clear ();
			return;
		}
		
		page = std::shared_ptr<DocumentPage> 
				   (pageview.m_document->createPage (pageview.m_index));
				   
		m_selected_regs = page->getSelectionRegion 
			 (SELECTION_STYLE_WORD, m_selected_rect, pageview.m_scale);
			 
		irect.color.r = 0;
		irect.color.g = 0;
		irect.color.b = 0;
		irect.color.a = 100;
		
		for (auto it = m_selected_regs.begin (); it != m_selected_regs.end (); it++) {
			irect.x = (*it).x;
			irect.y = (*it).y;
			irect.width = (*it).width - (*it).x;
			irect.height = (*it).height - (*it).y;
			irects.push_back (irect);
		}
		
		pageview.getImageView ().appendRects (irects);
	}
}

void
spdf::MainApp::copy_text_selection_page (spdf::PageView &pageview)
{
	std::shared_ptr<DocumentPage> page;
	Glib::ustring text;
	
	if (!pageview.m_document.get ()) {
		return;
	}
	
	page = std::shared_ptr<DocumentPage> 
				   (pageview.m_document->createPage (pageview.m_index));
	
	for (auto it = m_selected_regs.begin (); it != m_selected_regs.end (); it++) {
		text += page->searchText (*it, pageview.m_scale).data ();
		text += ' ';
	}
	
	Gtk::Clipboard::get	()->set_text (text);
}

/*
 * All slot definition 
 */

void
spdf::MainApp::on_open_btn_clicked ()
{
	Gtk::FileChooserDialog chooser (*this, "Open Document");
	chooser.add_button ("_Cancel", GTK_RESPONSE_CANCEL);
	chooser.add_button ("_Open", GTK_RESPONSE_ACCEPT);
			
	int ret = chooser.run ();
	chooser.hide ();
	
	if (ret != GTK_RESPONSE_ACCEPT) {
		return;
	}
	
	spdf::PageView *pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	Glib::ustring filename = chooser.get_filename ().data ();
	open_document (filename, *pageview);
}

void
spdf::MainApp::on_close_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}

	close_document (*pageview);
}

void
spdf::MainApp::on_prev_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	go_to_prev_page (*pageview);
}

void
spdf::MainApp::on_next_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	go_to_next_page (*pageview);
}

void 
spdf::MainApp::on_find_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	m_findview.show_all ();
}

void 
spdf::MainApp::on_zoomin_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	zoom_in_page (*pageview, SCALE_STEP);
}

void 
spdf::MainApp::on_zoomout_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	zoom_out_page (*pageview, SCALE_STEP);
}

void 
spdf::MainApp::on_fscreen_btn_clicked ()
{
	go_fullscreen ();
}

void 
spdf::MainApp::on_about_btn_clicked ()
{
	Gtk::AboutDialog dialog;
	Glib::ustring name = "Simple PDF Reader";
	Glib::ustring version = APP_VERSION;
	Glib::ustring copyright = "Copyright © 2017-2018 Fajar Dwi Darmanto";
	
	dialog.set_program_name (name);
	dialog.set_version (version);
	dialog.set_copyright (copyright);
	
	dialog.run ();
}

void 
spdf::MainApp::on_mark_btn_toggled ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	mark_current_page (*pageview);
}

void 
spdf::MainApp::on_entry_text_changed ()
{
	spdf::PageView *pageview = NULL;
	Gtk::Entry *entry = NULL;
	int index = 0;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	entry = static_cast<Gtk::Entry*> (m_main_toolbar.getNavEntryItem ().get_child ());
	
	try {
		index = std::stoi (entry->get_text ().raw ());
		if ((index > 0) && (index <= pageview->m_document->getPages ())) {
			if ((index-1) != pageview->m_index) {
				pageview->m_index = index - 1;
				update_toolbar (*pageview);
				draw_page (*pageview);
			}
		}
	} catch (const std::invalid_argument &ia) {
		std::cout << "[WARNING] " << ia.what () << "\n";
	}
}

void 
spdf::MainApp::on_first_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	go_to_page (*pageview, 0);
}

void 
spdf::MainApp::on_last_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	go_to_page (*pageview, -1);
}

void 
spdf::MainApp::on_copy_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	copy_text_selection_page (*pageview);
}

void 
spdf::MainApp::on_tab_page_added (Gtk::Widget* page, guint page_num)
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getPage (page_num);
	if (!pageview) {
		return;
	}
	
	pageview->getSidebarView ().getOutlineView ().getTreeView ().
					  signal_button_press_event().connect (sigc::mem_fun 
					  (*this, &MainApp::on_outline_sel_changed), false);
	
	pageview->getSidebarView ().getBookmarkView ().getTreeView ().
					  signal_button_press_event().connect (sigc::mem_fun 
					 (*this, &MainApp::on_bookmark_sel_changed), false);
							 
	pageview->getImageView ().getEventBox ().signal_button_press_event()
	 .connect (sigc::mem_fun (*this, &MainApp::on_image_button_event));
	  
	pageview->getImageView ().getEventBox ().signal_button_release_event()
	  .connect (sigc::mem_fun (*this, &MainApp::on_image_button_event));
}

void
spdf::MainApp::on_tab_page_changed (Gtk::Widget* page, guint page_num)
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getPage (page_num);
	if (!pageview) {
		return;
	}
	
	update_toolbar (*pageview);
}

bool 
spdf::MainApp::on_outline_sel_changed (GdkEventButton *event)
{
	spdf::PageView *pageview = NULL;
	Gtk::TreePath path;
	Gtk::TreeIter iter;
	int index = 0;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	if (!pageview->m_document.get ()) {
		return true;
	}
	
	spdf::OutlineView &outlineview = pageview->getSidebarView ().getOutlineView ();
	
	if (!outlineview.getSize ()) {
		return true;
	}
	
	if (!outlineview.getTreeView ().get_path_at_pos (static_cast<int>(event->x),
								                     static_cast<int>(event->y),
								                     path)) 
	{
		outlineview.getTreeSelection ()->unselect_all ();
		return true;
	}
	
	outlineview.getTreeSelection ()->select (path);
	iter = outlineview.getTreeStore ()->get_iter (path);
	index = outlineview.getIndex (iter);
	pageview->m_index = index - 1;
	update_toolbar (*pageview);
	draw_page (*pageview);
	
	return false;
}

bool
spdf::MainApp::on_bookmark_sel_changed (GdkEventButton *event)
{
	spdf::PageView *pageview = NULL;
	Gtk::TreePath path;
	Gtk::TreeIter iter;
	int index = 0;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	if (!pageview->m_document.get ()) {
		return true;
	}
	
	spdf::OutlineView &bookmarkview = pageview->getSidebarView ().getBookmarkView ();
	
	if (!bookmarkview.getSize ()) {
		return true;
	}
	
	if (!bookmarkview.getTreeView ().get_path_at_pos (static_cast<int>(event->x),
								                      static_cast<int>(event->y),
								                      path)) 
	{
		bookmarkview.getTreeSelection ()->unselect_all ();
		return true;
	}
	
	bookmarkview.getTreeSelection ()->select (path);
	iter = bookmarkview.getTreeStore ()->get_iter (path);
	index = bookmarkview.getIndex (iter);
	pageview->m_index = index - 1;
	update_toolbar (*pageview);
	draw_page (*pageview);
	
	return false;
}

void 
spdf::MainApp::on_find_entry_text_changed ()
{
	spdf::PageView *pageview = NULL;
	std::string text;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	text = m_findview.getEntry ().get_text ().raw ();
	find_text (*pageview, text);
}

bool 
spdf::MainApp::on_timeout_msg ()
{	
	spdf::PageView *pageview = NULL;
	spdf::Image *image = NULL;
	
	if (MessageQueue::instance ()->isEmpty ()) {
		return true;
	}
	
	Message msg = MessageQueue::instance ()->pop ();
	if (msg.getStatus () == RENDER_ERROR) {
		std::cout << msg.getText () << "\n";
		return true;
	}
	
	int tabs = m_tabpageview.get_n_pages ();
	int num = 0;
	for (num = 0; num < tabs; num++) {
		pageview = m_tabpageview.getPage (num);
		if (pageview->m_document.get ()) {
			if (pageview->m_document->getId () == msg.getId ()) {
				break;
			}
		}
	}
	
	std::string key = std::to_string (pageview->m_document->getId ()) 
							  + "-" + std::to_string (pageview->m_scale)
							 + "-" + std::to_string (pageview->m_index);
	if (msg.getText () != key) {
		return true;
	}
							 
	spdf::Cache::instance ()->lock ();
	image = (spdf::Image*)spdf::Cache::instance ()->get (msg.getText ());
	if (image) {
		pageview->getImageView ().setImage ((unsigned char*)image->getData (), 
			image->getWidth (), image->getHeight (), image->getStride ());
	}
	spdf::Cache::instance ()->unlock ();
	
	return true;
}

bool 
spdf::MainApp::on_timeout_sel ()
{	
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	if (m_selecting) {
		draw_selection_page (*pageview);
	}
	
	return true;
}

bool 
spdf::MainApp::on_image_button_event (GdkEventButton *event)
{
	spdf::PageView *pageview = NULL;
	std::shared_ptr<ImageViewPointer> pointer;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	if (event->button == 3) {
		return on_idle_right_click_event (event);
     }

	if (event->type == GDK_BUTTON_PRESS) {
		pointer = pageview->getImageView ().getPointer ();
		if ((pointer->x == -1) && (pointer->y == -1)) {
			m_selecting = false;
			return true;
		}
		m_selected_rect.x = pointer->x;
		m_selected_rect.y = pointer->y;
		m_selecting = true;
	} else if (event->type == GDK_BUTTON_RELEASE) {
		m_selecting = false;
	}
	
	return true;
}

bool 
spdf::MainApp::on_idle_right_click_event (GdkEventButton *event)
{
	spdf::PageView *pageview = NULL;
	bool copy = false;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	if (!pageview->m_document.get ()) {
		return true;
	}
	
	if (event->type == GDK_BUTTON_PRESS) {
		if (event->button == 3) {
			if (m_selected_regs.size ()) {
				copy = true;
			}
			m_page_nav_popup.getCopyMenuItem ().set_sensitive (copy);
			m_page_nav_popup.popup (event->button, event->time);
        }
	}
	
	return true;
}

// Event key handler
bool 
spdf::MainApp::on_mainapp_key_press_event (GdkEventKey *event)
{
	static bool show_find = false;
	spdf::PageView *pageview = NULL;
	Gtk::CheckButton *check = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	switch (event->keyval) {
		
		// Open a file
		case GDK_KEY_o:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
			
			on_open_btn_clicked ();
			return true;
			
		// Close a file
		case GDK_KEY_w:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
			
			on_close_btn_clicked ();
			return true;
			
		// Add new tab
		case GDK_KEY_t:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
			
			m_tabpageview.appendPage ("Document");
			return true;
			
		// go to next tab
		case GDK_KEY_Tab:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
			
			if (m_tabpageview.getCurrentIndex () == (m_tabpageview.get_n_pages () - 1)) {
				m_tabpageview.set_current_page (0);
			} else {
				m_tabpageview.next_page ();
			}
			
			return true;
			
		// Bookmark/Unbookmark
		case GDK_KEY_b:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
			
			check = static_cast<Gtk::CheckButton*>
				           (m_main_toolbar.getMarkItem ().get_child ());
			if (check->get_active ()) {
				check->set_active (false);
			} else {
				check->set_active (true);
			}
			
			return true;
			
		// Find a text
		case GDK_KEY_f:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
			
			if (!show_find) {
				m_findview.show_all ();
				show_find = true;
			} else {
				m_findview.hide ();
				show_find = false;
			}
			
			return true;
		
		// Scroll up 
		case GDK_KEY_Up:
			if (!pageview->m_document.get ()) {
				return false;
			}
			pageview->getImageView ().scrollUp ();
			return true;
		
		// Scroll down	
		case GDK_KEY_Down:
			if (!pageview->m_document.get ()) {
				return false;
			}
			pageview->getImageView ().scrollDown ();
			return true;
		
		// Scroll left	
		case GDK_KEY_Left:
			if (!pageview->m_document.get ()) {
				return false;
			}
			pageview->getImageView ().scrollLeft ();
			return true;
			
		// Scroll right
		case GDK_KEY_Right:
			if (!pageview->m_document.get ()) {
				return false;
			}
			pageview->getImageView ().scrollRight ();
			return true;
		
		// Previous page		
		case GDK_KEY_z:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
		
			if (!pageview->m_document.get ()) {
				return false;
			}
			on_prev_btn_clicked ();
			return true;
		
		// Next page
		case GDK_KEY_x:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
		
			if (!pageview->m_document.get ()) {
				return false;
			}
			
			on_next_btn_clicked ();
			return true;
		
		// Zoom out	
		case GDK_KEY_KP_Subtract:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
			
			if (!pageview->m_document.get ()) {
				return false;
			}
			on_zoomout_btn_clicked ();
			return true;
		
		// Zoom in		
		case GDK_KEY_KP_Add:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
		
			if (!pageview->m_document.get ()) {
				return false;
			}
			
			on_zoomin_btn_clicked ();
			return true;
		
		// Full screen	
		case GDK_KEY_F11:
			go_fullscreen ();
			return true;
		
		// Normal screen	
		case GDK_KEY_Escape:
			go_unfullscreen ();
			return true;
			
		default:
			//std::cout << event->keyval << "\n";
			break;
	}
	
	return false;
}
