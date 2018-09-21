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
#include <gtkmm/hvscrollbar.h>
#include <glibmm/main.h>
#include <gtkmm/box.h>
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
				  
	//m_page_nav_popup.getMarkItem ().signal_toggled ().connect
	//			  (sigc::mem_fun (*this, &MainApp::on_mark_pop_toggled));
	
				 
	m_tabpageview.signal_switch_page ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_tab_page_changed));
				 
	m_tabpageview.signal_page_added ().connect 
				 (sigc::mem_fun (*this, &MainApp::on_tab_page_added));
	
	m_selecting = false;	
	Glib::signal_timeout().connect (sigc::mem_fun 
								 (*this, &MainApp::on_timeout_sel), 100);		 
	Glib::signal_timeout().connect (sigc::mem_fun 
								 (*this, &MainApp::on_timeout_msg), 10);
	
	set_title ("SPDF");
	set_size_request (800, 600);
	show_all ();
	
	m_tabpageview.appendPage ("Document");
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
		title = (*it).getTitle ();
		parent = outlineview.append (title, (*it).getIndex (), iter);
		walk_fill_outline (outlineview, (*it).getChild (), parent);
	}
}

// populate bookmark sidebar
void
spdf::MainApp::fill_bookmark (spdf::PageView &pageview)
{
	std::vector<int> marks;
	std::string title;
	Glib::ustring text;
	int index = 0;
	int start = 0;
	int end = 0;
	
	pageview.getSidebarView ().getBookmarkView ().clear ();
	title = pageview.m_document->getTitle ();
	marks = Bookmark::instance ()->get (title);
	
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
	std::string title;
	bool active = false;
	
	entry = static_cast<Gtk::Entry*> 
					   (m_main_toolbar.getNavEntryItem ().get_child ());
	label = static_cast<Gtk::Label*> 
	                 (m_main_toolbar.getPagesLabelItem ().get_child ());
	check = static_cast<Gtk::CheckButton*>
				           (m_main_toolbar.getMarkItem ().get_child ());
	pages = " of 0";
	index = "0";
	
	if(pageview.m_document.get ()) {
		pages = " of " + std::to_string (pageview.m_document->getPages ());
		index = std::to_string (pageview.m_index + 1);
		title = pageview.m_document->getTitle ();
		active = Bookmark::instance ()->find (title, pageview.m_index);
	}
	
	entry->set_text (index);
	label->set_text (pages);
	check->set_active (active);
}

// find text
void 
spdf::MainApp::find_text (spdf::PageView &pageview, std::string &str)
{
	std::shared_ptr<DocumentPage> 
			page (pageview.m_document->createPage (pageview.m_index));
	std::vector<Rect> rects = page->searchRect (str, pageview.m_scale);
	std::vector<Rect>::iterator it;
	
	if (rects.size () == 0) {
		return;
	}
	
	std::vector<ImageViewRect> irects;
	ImageViewRect irect;
	irect.color.r = 0;
	irect.color.g = 0;
	irect.color.b = 0;
	irect.color.a = 100;
	pageview.getImageView ().refresh ();
	for (it = rects.begin (); it != rects.end (); it++) {
		irect.x = (*it).x;
		irect.y = (*it).y;
		irect.width = (*it).width;
		irect.height = (*it).height;
		irects.push_back (irect);
	}
	pageview.getImageView ().clearImage ();
	pageview.getImageView ().appendRects (irects);
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
	chooser.signal_response ().connect (sigc::bind<-1, Gtk::FileChooser&> 
		(sigc::mem_fun (*this, &MainApp::on_open_dialog_resp), chooser));	
			
	chooser.run ();
}

void
spdf::MainApp::on_close_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	Glib::ustring title = "Document";
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}

	pageview->getTabHeaderView ().setTitle (title);
	pageview->getImageView ().clearImage ();
	pageview->m_document.reset ();
	pageview->m_index = 0;
	pageview->m_scale = 1.0;
	pageview->getSidebarView ().getOutlineView ().clear ();
	pageview->hideSidebarView ();
	update_toolbar (*pageview);
}

void
spdf::MainApp::on_prev_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	int index = 0;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	index = pageview->m_index - 1;
	if (index >= 0) {
		pageview->m_index = index;
		update_toolbar (*pageview);
		draw_page (*pageview);
	}
}

void
spdf::MainApp::on_next_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	int index = 0;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	index = pageview->m_index + 1;
	if (index < pageview->m_document->getPages ()) {
		pageview->m_index = index;
		update_toolbar (*pageview);
		draw_page (*pageview);
	}
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
	
	Gtk::Dialog dialog ("Find");
	Gtk::Box fbox;
	Gtk::Label label ("Find: ");
	Gtk::Entry entry;
	fbox.pack_start (label, 0, 0, 10);
	fbox.pack_start (entry, 0, 0, 10);
	fbox.show_all ();
	dialog.get_content_area ()->pack_start (fbox, 0, 0, 10);
	dialog.add_button ("_Cancel", GTK_RESPONSE_CANCEL);
	dialog.add_button ("_Find", GTK_RESPONSE_ACCEPT);
	dialog.signal_response ().connect (sigc::bind<-1, Gtk::Entry&> 
		(sigc::mem_fun (*this, &MainApp::on_find_dialog_resp), entry));
	
	dialog.run ();
}

void 
spdf::MainApp::on_zoomin_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	pageview->m_scale += SCALE_STEP;
	draw_page (*pageview);
}

void 
spdf::MainApp::on_zoomout_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	pageview->m_scale -= SCALE_STEP;
	draw_page (*pageview);
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
	Gtk::CheckButton *check = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	std::string title = pageview->m_document->getTitle ();
	int index = pageview->m_index;
	check = static_cast<Gtk::CheckButton*>
				           (m_main_toolbar.getMarkItem ().get_child ());
	
	if (check->get_active ()) {
		if (Bookmark::instance ()->find (title, index)) {
			return;
		}
		Bookmark::instance ()->add (title, index);
		fill_bookmark (*pageview);
	} else {
		if (!Bookmark::instance ()->find (title, index)) {
			return;
		}
		Bookmark::instance ()->erase (title, index);
		fill_bookmark (*pageview);
	}
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
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	pageview->m_index = 0;
	update_toolbar (*pageview);
	draw_page (*pageview);
}

void 
spdf::MainApp::on_last_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	pageview->m_index = pageview->m_document->getPages () - 1;
	update_toolbar (*pageview);
	draw_page (*pageview);
}

void 
spdf::MainApp::on_copy_btn_clicked ()
{
	spdf::PageView *pageview = NULL;
	spdf::DocumentPage *page = NULL;
	std::string text;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	page = pageview->m_document->createPage (pageview->m_index);
	
	for (auto it = m_selected_regs.begin (); it != m_selected_regs.end (); it++) {
		text += page->searchText (*it, pageview->m_scale);
	}
	
	Gtk::Clipboard::get	()->set_text (text.data ());
	
	delete (page);
}

void 
spdf::MainApp::on_tab_page_added (Gtk::Widget* page, guint page_num)
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getPage (page_num);
	if (!pageview) {
		return;
	}
	
	Glib::RefPtr<Gtk::TreeSelection> &sel1 = pageview->getSidebarView ()
								 .getOutlineView ().getTreeSelection ();
					 
	sel1->signal_changed ().connect (sigc::mem_fun 
							 (*this, &MainApp::on_outline_sel_changed));
							 
	Glib::RefPtr<Gtk::TreeSelection> &sel2 = pageview->getSidebarView ()
								 .getBookmarkView ().getTreeSelection ();
					 
	sel2->signal_changed ().connect (sigc::mem_fun 
							 (*this, &MainApp::on_bookmark_sel_changed));
							 
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

void 
spdf::MainApp::on_outline_sel_changed ()
{
	spdf::PageView *pageview = NULL;
	Gtk::TreeIter iter;
	int index = 0;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	spdf::OutlineView &outlineview = pageview->getSidebarView ().getOutlineView ();
	
	if (!outlineview.getSize ()) {
		return;
	}
	
	iter = outlineview.getTreeSelection ()->get_selected ();
	index = outlineview.getIndex (iter);
	pageview->m_index = index - 1;
	update_toolbar (*pageview);
	draw_page (*pageview);
}

void 
spdf::MainApp::on_bookmark_sel_changed ()
{
	spdf::PageView *pageview = NULL;
	Gtk::TreeIter iter;
	int index = 0;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	if (!pageview->m_document.get ()) {
		return;
	}
	
	spdf::OutlineView &bookmarkview = pageview->getSidebarView ().getBookmarkView ();
	
	if (!bookmarkview.getSize ()) {
		return;
	}
	
	iter = bookmarkview.getTreeSelection ()->get_selected ();
	index = bookmarkview.getIndex (iter);
	
	pageview->m_index = index - 1;
	update_toolbar (*pageview);
	draw_page (*pageview);
}

void
spdf::MainApp::on_open_dialog_resp (int id, Gtk::FileChooser &chooser)
{	
	spdf::PageView *pageview = NULL;
	std::string filename;
	Glib::ustring ufilename;
	
	if (id != GTK_RESPONSE_ACCEPT) {
		return;
	}
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	pageview->m_document.reset ();
	pageview->m_index = 0;
	pageview->m_scale = 1.0;
	pageview->getSidebarView ().getOutlineView ().clear ();
	pageview->getSidebarView ().getBookmarkView ().clear ();
	
	filename = chooser.get_filename ();
	pageview->m_document = std::shared_ptr<Document>
				(DocumentCreator::instance ()->openDocument (filename));
	if (!pageview->m_document.get ()) {
		std::cout << "[ERROR] Could not open document!!!\n";
		return;
	}
	
	ufilename = pageview->m_document->getTitle ();
	pageview->getTabHeaderView ().setTitle (ufilename);
	fill_outline (*pageview);
	fill_bookmark (*pageview);
	pageview->showSidebarView ();
	update_toolbar (*pageview);
	draw_page (*pageview);
}

void 
spdf::MainApp::on_find_dialog_resp (int id, Gtk::Entry &entry)
{
	spdf::PageView *pageview = NULL;
	std::string text;
	
	if (id != GTK_RESPONSE_ACCEPT) {
		return;
	}
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	text = entry.get_text ().raw ();
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
	spdf::DocumentPage *page = NULL;
	
	if (m_selecting) {
		pageview = m_tabpageview.getCurrentPage ();
		pageview->getImageView ().refresh ();
		m_selected_regs.clear ();
		std::shared_ptr<ImageViewPointer> pointer = 
								pageview->getImageView ().getPointer ();
		if ((pointer->x != -1) && (pointer->y != -1)) {
			m_selected_rect.width = pointer->x;
			m_selected_rect.height = pointer->y;
			page = pageview->m_document->createPage (pageview->m_index);
			m_selected_regs = page->getSelectionRegion 
			 (SELECTION_STYLE_WORD, m_selected_rect, pageview->m_scale);
			 
			std::vector<ImageViewRect> irects;
			ImageViewRect irect;
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
			pageview->getImageView ().appendRects (irects);
			delete (page);
	    }
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
