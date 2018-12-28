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
#include "config.h"
#include "MainApp.h"
#include "LoaderTask.h"
#include "RendererTask.h"
#include "TaskScheduler.h"
#include "Cache.h"
#include "MessageQueue.h"
#include "ConfigParser.h"
#include "Bookmark.h"
#include "TaskId.h"

#if defined(_WIN32)
#include <direct.h>
#else 
#include <sys/stat.h>
#endif

#define THREAD_NUM "thread_num"
#define CACHE_SIZE "cache_size"

#define DEFAULT_THREAD_NUM_VALUE 2
#define DEFAULT_CACHE_SIZE_VALUE 8

spdf::MainApp::MainApp (const Glib::RefPtr<Gtk::Application>& app) 
				: spdf::MainWindow (app)
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
				  
	m_page_nav_popup.getConsModeMenuItem ().signal_toggled ().connect
			(sigc::mem_fun (*this, &MainApp::on_page_mode_item_toggled));
	
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
spdf::MainApp::load (Glib::ustring &path)
{
	spdf::PageView *pageview = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	spdf::UString filename = path.data ();
	open_document (*pageview, filename);
}

void 
spdf::MainApp::switch_mode (spdf::PageView &pageview, spdf::ImageViewMode mode)
{
	pageview.getImageView ().clear ();
	pageview.getImageView ().set_mode (mode);
	pageview.getImageView ().freeze ();
	render_page (pageview);
}

void 
spdf::MainApp::render_page (spdf::PageView &pageview)
{
	static int u_render_id = 0;
	std::string key;
	std::string *udata;
	ImageQueueData qdata;
	spdf::PageViewData *data = NULL;
	bool found = false;
	
	data = (spdf::PageViewData*) pageview.getData ();
	
	// encoded = u_render_id-id-scale-index
	key = std::to_string (u_render_id++) +
			 std::to_string (data->m_document->getId ()) 
		     + "-" + std::to_string (data->m_scale)
		     + "-" + std::to_string (data->m_render_index);
	
	// encoded = id-scale-index	  	      
	udata = new std::string (std::to_string (data->m_document->getId ()) 
	                         + "-" + std::to_string (data->m_scale)
		                     + "-" + std::to_string (data->m_render_index));
		
	if (!m_image_queue.empty ()) {
		for (auto it = m_image_queue.begin (); it != m_image_queue.end (); it++) {
			if ((*it).m_pageview == &pageview) {
				(*it).m_key = key;
				found = true;
				break;
			}
		}
	} 
		
	if (!found) {
		qdata.m_pageview = &pageview;
		qdata.m_key = key;
		m_image_queue.push_back (qdata);
	}
	
	spdf::TaskScheduler::instance ()->assign (
		new spdf::RendererTask 
			(data->m_document.get (), data->m_render_index, data->m_scale, key, (void*)udata));                     
}

void 
spdf::MainApp::draw_page (spdf::PageView &pageview, spdf::Image &image)
{
	spdf::PageViewData *data = NULL;
	
	
	data = (spdf::PageViewData*) pageview.getData ();
	
	if (pageview.getImageView ().get_mode () == IMAGEVIEW_CONTINOUS) {
		if (pageview.getImageView ().last_signal ()) {
			pageview.getImageView ().push_back 
				(image.getData (), data->m_render_index, image.getWidth (), image.getHeight (), image.getStride ());
		} else {
			pageview.getImageView ().push_front 
				(image.getData (), data->m_render_index, image.getWidth (), image.getHeight (), image.getStride ());
		}
	} else {
		pageview.getImageView ().set 
			(image.getData (), data->m_render_index, image.getWidth (), image.getHeight (), image.getStride ());
	}
	
	pageview.getImageView ().refresh ();
	pageview.getImageView ().freeze (false);
}

void 
spdf::MainApp::open_document (spdf::PageView &pageview, const spdf::UString &path, const spdf::UString &upass, const spdf::UString &opass)
{
	static int u_loader_id = 0;
	std::string key;
	DocumentQueueData qdata;
	spdf::PageViewData *data = NULL;
	
	data = (spdf::PageViewData*) pageview.getData ();
	data->m_path = path;
	data->m_upass = upass;
	data->m_opass = opass;
	
	key = std::to_string (u_loader_id++);
		                     
	qdata.m_pageview = &pageview;
	qdata.m_key = key;
	m_doc_queue.push_back (qdata);

	spdf::TaskScheduler::instance ()->assign (
		new spdf::LoaderTask 
			(data->m_path, data->m_upass, data->m_opass, key));
}

void 
spdf::MainApp::load_document (spdf::PageView &pageview)
{
	spdf::PageViewData *data = NULL;
	Glib::ustring title;
	
	data = (spdf::PageViewData*) pageview.getData ();
	title = data->m_document->getTitle ().data ();
	
	pageview.getTabHeaderView ().setTitle (title);
	fill_outline (pageview);
	fill_bookmark (pageview);
	pageview.showSidebarView ();
	update_toolbar (pageview);
	render_page (pageview);
}

void 
spdf::MainApp::close_document (spdf::PageView &pageview)
{
	spdf::PageViewData *data = NULL;
	Glib::ustring title = "Document";
	
	data = (spdf::PageViewData*) pageview.getData ();
	data->m_document.reset ();
	data->m_index = 0;
	data->m_render_index = 0;
	data->m_scale = 1.0;

	pageview.getTabHeaderView ().setTitle (title);
	pageview.getSidebarView ().getOutlineView ().clear ();
	pageview.getSidebarView ().getBookmarkView ().clear ();
	pageview.hideSidebarView ();
	update_toolbar (pageview);
	pageview.getImageView ().clear ();
}

void 
spdf::MainApp::go_to_page (spdf::PageView &pageview, int index)
{
	spdf::PageViewData *data = NULL;
	int in = 0;
	
	data = (spdf::PageViewData*) pageview.getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	in = index;
	if (in == -1) {
		in = data->m_document->getPages () - 1;
	}
	
	if ((in >= 0) && (in < data->m_document->getPages ())) {
		data->m_index = in;
		update_toolbar (pageview);
		
		data->m_render_index = data->m_index;
		pageview.getImageView ().clear ();
	    pageview.getImageView ().freeze ();
		render_page (pageview);
	}
}

void 
spdf::MainApp::go_to_next_page (spdf::PageView &pageview)
{
	spdf::PageViewData *data = NULL;
	int index = 0;
	
	data = (spdf::PageViewData*) pageview.getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	index = data->m_index + 1;
	if (index < data->m_document->getPages ()) {
		data->m_index = index;
		update_toolbar (pageview);
		
		data->m_render_index = data->m_index;
		pageview.getImageView ().clear ();
	    pageview.getImageView ().freeze ();
		render_page (pageview);
	}
}

void 
spdf::MainApp::go_to_prev_page (spdf::PageView &pageview)
{
	spdf::PageViewData *data = NULL;
	int index = 0;
	
	data = (spdf::PageViewData*) pageview.getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	index = data->m_index - 1;
	if (index >= 0) {
		data->m_index = index;
		update_toolbar (pageview);
		
		data->m_render_index = data->m_index;
		pageview.getImageView ().clear ();
	    pageview.getImageView ().freeze ();
		render_page (pageview);
	}
}

void 
spdf::MainApp::zoom_in_page (spdf::PageView &pageview, double f)
{
	spdf::PageViewData *data = NULL;
	double scale = 0.0;
	
	data = (spdf::PageViewData*) pageview.getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	scale = data->m_scale + f;
	
	data->m_scale = scale;
	update_toolbar (pageview);
	
	data->m_render_index = data->m_index;
	pageview.getImageView ().clear ();
	pageview.getImageView ().freeze ();
	render_page (pageview);
}

void 
spdf::MainApp::zoom_out_page (spdf::PageView &pageview, double f)
{
	spdf::PageViewData *data = NULL;
	double scale = 0.0;
	
	data = (spdf::PageViewData*) pageview.getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	scale = data->m_scale - f;
	if (scale < 0.25) {
		return;
	}
	
	data->m_scale = scale;
	update_toolbar (pageview);
	
	data->m_render_index = data->m_index;
	pageview.getImageView ().clear ();
	pageview.getImageView ().freeze ();
	render_page (pageview);
}

void 
spdf::MainApp::mark_current_page (spdf::PageView &pageview)
{
	spdf::PageViewData *data = NULL;
	UString title;
	std::string temp;
	int index;
	
	data = (spdf::PageViewData*) pageview.getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	title = data->m_document->getTitle ();
	temp = title.data ();
	index = data->m_index;
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

// populate outline sidebar
void
spdf::MainApp::fill_outline (spdf::PageView &pageview)
{
	spdf::PageViewData *data = NULL;
	
	data = (spdf::PageViewData*) pageview.getData ();
	std::shared_ptr<DocumentOutline> outline (data->m_document->getOutline ());
								  
	pageview.getSidebarView ().getOutlineView ().clear ();
	
	if (!outline.get ()) {
		return;
	}
	
	Gtk::TreeIter iter = pageview.getSidebarView ().getOutlineView ().getTreeIter ();
	
	walk_fill_outline 
		(pageview.getSidebarView ().getOutlineView (), outline ->getRoot (), iter);
}

void 
spdf::MainApp::walk_fill_outline 
	(spdf::OutlineView &outlineview, std::vector<spdf::DocumentOutlineItem> &toc_item, Gtk::TreeIter &iter)
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
	spdf::PageViewData *data = NULL;
	
	data = (spdf::PageViewData*) pageview.getData ();
	pageview.getSidebarView ().getBookmarkView ().clear ();
	title = data->m_document->getTitle ();
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
	spdf::PageViewData *data = NULL;
	
	data = (spdf::PageViewData*) pageview.getData ();
	entry = static_cast<Gtk::Entry*> 
					   (m_main_toolbar.getNavEntryItem ().get_child ());
	label = static_cast<Gtk::Label*> 
	                 (m_main_toolbar.getPagesLabelItem ().get_child ());
	check = static_cast<Gtk::CheckButton*>
				           (m_main_toolbar.getMarkItem ().get_child ());
	pages = " of 0";
	index = "0";
	
	if(data->m_document.get ()) {
		pages = " of " + std::to_string (data->m_document->getPages ());
		index = std::to_string (data->m_index + 1);
		title = data->m_document->getTitle ();
		temp = title.data ();
		active = Bookmark::instance ()->find (temp, data->m_index);
	}
	
	entry->set_text (index);
	label->set_text (pages);
	check->set_active (active);
}

// find text
void 
spdf::MainApp::find_text (spdf::PageView &pageview, spdf::ImageView &imageview, std::string &str)
{
	/*
	static Rect rect = {0, 0, 0, 0};
	static std::string text;
	
	std::shared_ptr<DocumentPage> page;
	ImageViewRect irect;
	spdf::PageViewData *data = NULL;
	
	data = (spdf::PageViewData*) pageview.getData ();
	
	if (text != str) {
		rect = {0, 0, 0, 0};
		text = str;
	}
	
	page = std::shared_ptr<DocumentPage> 
				   (data->m_document->createPage (data->m_index));
	rect = page->searchRect (rect, text, data->m_scale, SEARCH_DIRECTION_NEXT);
	
	irect.color.r = 0;
	irect.color.g = 0;
	irect.color.b = 0;
	irect.color.a = 100;
	imageview.refresh ();
	
	irect.x = rect.x;
	irect.y = rect.y;
	irect.width = rect.width;
	irect.height = rect.height;
		
	imageview.clearImage ();
	imageview.appendRect (irect);
	
	rect.x = rect.x + rect.width;
	rect.y = rect.y + rect.height;
	*/
}

void 
spdf::MainApp::draw_selection_page (spdf::PageView &pageview)
{
	spdf::PageViewData *data = NULL;
	DocumentPage *page = NULL;
	std::vector<ImageViewRect>  irects;
	std::vector<ImageViewMarker> markers;
	Rect rect;
	ImageViewMarker marker;
	
	data = (spdf::PageViewData*) pageview.getData ();
	
	if (!data->m_document.get ()) {
		return;
	}
	
	m_rects.clear ();
	pageview.getImageView ().clear_marker ();
	pageview.getImageView ().cursor_position (m_x2, m_y2);
	irects = pageview.getImageView ().get_image_at_region (m_x1, m_y1, m_x2, m_y2);
	for (auto it1 = irects.begin (); it1 != irects.end (); it1++) {
		rect.x = (*it1).m_x1;
		rect.y = (*it1).m_y1;
		rect.width = (*it1).m_x2;
		rect.height = (*it1).m_y2;
		page = data->m_document->createPage ((*it1).m_imageview_data.m_image_index);
		m_rects = page->getSelectionRegion (SELECTION_STYLE_GLYPH, rect, data->m_scale);
		if (m_rects.size ()) {
			for (auto it2 = m_rects.begin (); it2 != m_rects.end (); it2++) {
				marker.m_index = (*it1).m_imageview_data.m_image_index;
				marker.m_x = (*it2).x;
				marker.m_y = (*it2).y;
				marker.m_width = (*it2).width - (*it2).x;
				marker.m_height = (*it2).height - (*it2).y;
				markers.push_back (marker);
			}
			if (markers.size ()) {
				pageview.getImageView ().draw_marker (markers);
			}
		}
		delete (page);
		break;
	}
}


void
spdf::MainApp::copy_text_selection_page (spdf::PageView &pageview)
{
	std::shared_ptr<DocumentPage> page;
	Glib::ustring text;
	spdf::PageViewData *data = NULL;
	
	data = (spdf::PageViewData*) pageview.getData ();
	
	if (!data->m_document.get ()) {
		return;
	}
	
	for (auto it = m_rects.begin (); it != m_rects.end (); it++) {
		page = std::shared_ptr<DocumentPage> 
						 (data->m_document->createPage ((*it).index));
		text += page->searchText (*it, data->m_scale).data ();
		text += ' ';
	}
	
	Gtk::Clipboard::get	()->set_text (text);
}

void 
spdf::MainApp::handle_document_error (spdf::PageView &pageview, spdf::DocumentError &err)
{								
	Gtk::MessageDialog msg ("");
	spdf::PageViewData *data = NULL;
	
	data = (spdf::PageViewData*) pageview.getData ();
		
	// PDF file error
	if (err.type == PDF) {
		// error code from ErrorCodes.h in poppler library
		if (err.code == 4) {
			Gtk::Entry pass_entry;
				
			msg.set_message ("Enter Password:");
			msg.get_message_area ()->pack_start (pass_entry, 1, 1);
			msg.show_all ();
			msg.run ();
				
			data->m_opass = pass_entry.get_text ().data ();
			open_document (pageview, data->m_path, data->m_upass, data->m_opass);
										
		} else {
			msg.set_message (err.msg.data ());
			msg.run ();
			return;
		}
		
	// unknown file	
	} else {
		msg.set_message ("document file is not supported");
		msg.run ();
		return;
	}
}

void 
spdf::MainApp::translate_message (spdf::Message &msg)
{
	switch (msg.getId ()) {
		case TASK_ID_LOADER:
			process_load_message (msg);
			break;
		case TASK_ID_RENDERER:
			process_render_message (msg);
			break;
		default:
			break;
	}
}

void 
spdf::MainApp::process_render_message (spdf::Message &msg)
{
	spdf::Image *image = NULL;
	spdf::ImageQueueData qdata;
	std::string *key = NULL;
	bool found = false;
	
	if (msg.getStatus () == RENDER_ERROR) {
		std::cout << msg.getText () << "\n";
		return;
	}
	
	for (auto it = m_image_queue.begin (); it != m_image_queue.end (); it++) {
		if ((*it).m_key == msg.getText ()) {
			qdata = m_image_queue[it-m_image_queue.begin ()];
			m_image_queue.erase (it);
			found = true;
			break;
		}
	}
	
	if (!found) {
		return;
	}
	
	key = (std::string*) msg.getUserData ();				 
	spdf::Cache::instance ()->lock ();
	image = (spdf::Image*)spdf::Cache::instance ()->get (*key);
	if (image) {
	    draw_page (*(qdata.m_pageview), *image);
	}
	spdf::Cache::instance ()->unlock ();
	delete (key);
	
	return;
}

void 
spdf::MainApp::process_load_message (spdf::Message &msg)
{
	spdf::DocumentQueueData qdata;
	spdf::PageView *pageview = NULL;
	spdf::PageViewData *data = NULL;
	bool found = false;
	
	for (auto it = m_doc_queue.begin (); it != m_doc_queue.end (); it++) {
		if ((*it).m_key == msg.getText ()) {
			qdata = m_doc_queue[it-m_doc_queue.begin ()];
			m_doc_queue.erase (it);
			found = true;
			break;
		}
	}
	
	if (!found) {
		return;
	}
	
	pageview = qdata.m_pageview;
	
	if (msg.getStatus () == LOAD_ERROR) {
		handle_document_error (*pageview, *((spdf::DocumentError*)msg.getUserData ()));
		delete ((spdf::DocumentError*)msg.getUserData ());
		return;
	}
	
	data = (spdf::PageViewData*)pageview->getData ();
	data->m_document = std::shared_ptr<spdf::Document> ((spdf::Document*)msg.getUserData ());
	data->m_index = 0;
	data->m_scale = 1.0;
	
	load_document (*pageview);
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
	
	spdf::UString path = chooser.get_filename ().data ();
	open_document (*pageview, path);
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
	spdf::PageViewData *data = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	if (!data->m_document.get ()) {
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
	Glib::ustring version = std::to_string (SPDF_VERSION_MAJOR) + "." + std::to_string (SPDF_VERSION_MINOR);
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
	spdf::PageViewData *data = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	entry = static_cast<Gtk::Entry*> (m_main_toolbar.getNavEntryItem ().get_child ());
	
	try {
		index = std::stoi (entry->get_text ().raw ());
		go_to_page (*pageview, index);
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
	spdf::PageViewData *data = NULL;
	spdf::ImageView *imageview = NULL;
	
	pageview = m_tabpageview.getPage (page_num);
	if (!pageview) {
		return;
	}
	
	data = new spdf::PageViewData;
	data->m_render_index = 0;
	data->m_scale = 1.0;
	pageview->setData ((void*)data);
	
	pageview->getSidebarView ().getOutlineView ().getTreeView ().
					  signal_button_press_event().connect (sigc::mem_fun 
					  (*this, &MainApp::on_outline_sel_changed), false);
	
	pageview->getSidebarView ().getBookmarkView ().getTreeView ().
					  signal_button_press_event().connect (sigc::mem_fun 
					 (*this, &MainApp::on_bookmark_sel_changed), false);
					 
	pageview->getImageView ().signal_offset ().connect 
	              (sigc::mem_fun (*this, &MainApp::on_imageview_offset));
	pageview->getImageView ().signal_runout ().connect 
	              (sigc::mem_fun (*this, &MainApp::on_imageview_runout));
	pageview->getImageView ().signal_current ().connect 
	             (sigc::mem_fun (*this, &MainApp::on_imageview_current));
							 
	pageview->getEventBox ().signal_button_press_event()
	   .connect (sigc::mem_fun (*this, &MainApp::on_image_button_event));
	  
	pageview->getEventBox ().signal_button_release_event()
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
	spdf::PageViewData *data = NULL;
	Gtk::TreePath path;
	Gtk::TreeIter iter;
	int index = 0;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	if (!data->m_document.get ()) {
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
	go_to_page (*pageview, index);
	
	return false;
}

bool
spdf::MainApp::on_bookmark_sel_changed (GdkEventButton *event)
{
	spdf::PageView *pageview = NULL;
	spdf::PageViewData *data = NULL;
	Gtk::TreePath path;
	Gtk::TreeIter iter;
	int index = 0;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	if (!data->m_document.get ()) {
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
	go_to_page (*pageview, index-1);
	
	return false;
}

void 
spdf::MainApp::on_find_entry_text_changed ()
{
	spdf::PageView *pageview = NULL;
	spdf::PageViewData *data = NULL;
	std::string text;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	text = m_findview.getEntry ().get_text ().raw ();
	//find_text (*pageview, text);
}

bool 
spdf::MainApp::on_timeout_msg ()
{	
	Message msg;
	
	if (MessageQueue::instance ()->isEmpty ()) {
		return true;
	}
	
	msg = MessageQueue::instance ()->pop ();
	translate_message (msg);
	
	return true;
}

bool 
spdf::MainApp::on_timeout_sel ()
{	
	spdf::PageView *pageview = NULL;
	
	if (m_selecting) {
		pageview = m_tabpageview.getCurrentPage ();
		if (!pageview) {
			return true;
		}
		draw_selection_page (*pageview);
	}
	
	return true;
}

bool 
spdf::MainApp::on_image_button_event (GdkEventButton *event)
{
	if (event->button == 3) {
		return on_idle_right_click_event (event);
    }
    
    if (event->type == GDK_BUTTON_PRESS) {
		m_x1 = event->x;
		m_y1 = event->y;
		m_selecting = true;
	} else if (event->type == GDK_BUTTON_RELEASE) {
		m_selecting = false;
		m_x1 = 0;
		m_y1 = 0;
		m_x2 = 0;
		m_y2 = 0;
	}
    
	return true;
}

bool 
spdf::MainApp::on_idle_right_click_event (GdkEventButton *event)
{
	spdf::PageView *pageview = NULL;
	spdf::PageViewData *data = NULL;
	bool copy = false;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	
	if (!data->m_document.get ()) {
		return true;
	}
	
	if (event->type == GDK_BUTTON_PRESS) {
		if (event->button == 3) {
			if (m_rects.size ()) {
				copy = true;
			}
			
			if (pageview->getImageView ().get_mode () == IMAGEVIEW_SINGLE) {
				m_page_nav_popup.getConsModeMenuItem ().set_active (false);
			} else {
				m_page_nav_popup.getConsModeMenuItem ().set_active (true);
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
	spdf::PageViewData *data = NULL;
	Gtk::CheckButton *check = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return true;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	
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
			if (!data->m_document.get ()) {
				return false;
			}
			pageview->getImageView ().scroll_up ();
			return true;
		
		// Scroll down	
		case GDK_KEY_Down:
			if (!data->m_document.get ()) {
				return false;
			}
			pageview->getImageView ().scroll_down ();
			return true;
		
		// Scroll left	
		case GDK_KEY_Left:
			if (!data->m_document.get ()) {
				return false;
			}
			pageview->getImageView ().scroll_left ();
			return true;
			
		// Scroll right
		case GDK_KEY_Right:
			if (!data->m_document.get ()) {
				return false;
			}
			pageview->getImageView ().scroll_right ();
			return true;
		
		// Previous page		
		case GDK_KEY_z:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
		
			//if (!pageview->m_document.get ()) {
			//	return false;
			//}
			on_prev_btn_clicked ();
			return true;
		
		// Next page
		case GDK_KEY_x:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
		
			//if (!pageview->m_document.get ()) {
			//	return false;
			//}
			
			on_next_btn_clicked ();
			return true;
		
		// Zoom out	
		case GDK_KEY_KP_Subtract:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
			
			//if (!pageview->m_document.get ()) {
			//	return false;
			//}
			//on_zoomout_btn_clicked ();
			return true;
		
		// Zoom in		
		case GDK_KEY_KP_Add:
			if ((event->state &
				(GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 
														GDK_CONTROL_MASK) {
				return false;											
			}
		
			//if (!pageview->m_document.get ()) {
			//	return false;
			//}
			
			//on_zoomin_btn_clicked ();
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

void 
spdf::MainApp::on_imageview_offset (bool dir, int index)
{
	spdf::PageView *pageview = NULL;
	spdf::PageViewData *data = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	if (!dir) {
		if (index == 0) {
			return;
		}
		data->m_render_index = index - 1;
	} else {
		if (index == (data->m_document->getPages () -1)) {
			return;
		}
		data->m_render_index = index + 1;
	}
	
	pageview->getImageView ().freeze ();
	render_page (*pageview);
}

void 
spdf::MainApp::on_imageview_runout (int index)
{
	spdf::PageView *pageview = NULL;
	spdf::PageViewData *data = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	if (index == (data->m_document->getPages () -1)) {
			return;
	}
	
    data->m_render_index = index + 1;
	pageview->getImageView ().freeze ();
	render_page (*pageview);
}

void 
spdf::MainApp::on_imageview_current (int index)
{
	spdf::PageView *pageview = NULL;
	spdf::PageViewData *data = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	data->m_index = index;
	update_toolbar (*pageview);
}

void 
spdf::MainApp::on_page_mode_item_toggled ()
{
	spdf::PageView *pageview = NULL;
	spdf::PageViewData *data = NULL;
	
	pageview = m_tabpageview.getCurrentPage ();
	if (!pageview) {
		return;
	}
	
	data = (spdf::PageViewData*) pageview->getData ();
	if (!data->m_document.get ()) {
		return;
	}
	
	if (m_page_nav_popup.getConsModeMenuItem ().get_active ()) {
		if (pageview->getImageView ().get_mode () == IMAGEVIEW_CONTINOUS) {
			return;
		}
		switch_mode (*pageview, IMAGEVIEW_CONTINOUS);
	} else {
		if (pageview->getImageView ().get_mode () == IMAGEVIEW_SINGLE) {
			return;
		}
		switch_mode (*pageview, IMAGEVIEW_SINGLE);
	} 
}
