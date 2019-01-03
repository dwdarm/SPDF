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
 
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <deque>
#include <gtkmm/filechooserdialog.h>
#include "gtk/MainWindow.h"
#include "DocumentCreator.h"
#include "Message.h"
#include "Configuration.h"

#define SCALE_STEP 0.25

class PageNavPopup;

namespace spdf {
	
	enum PageViewMode {
		MODE_SINGLE_PAGE = 0,
		MODE_CONTINOUS_pAGE
	};
	
	typedef struct {
		spdf::ImageView *m_imageview;
		std::shared_ptr<spdf::Document> m_document;
		spdf::UString m_path;
		spdf::UString m_upass;
		spdf::UString m_opass;
		PageViewMode m_mode;
		int m_index;
		int m_render_index;
		int m_image_id;
		double m_scale;
	} PageViewData;
	
	typedef struct {
		spdf::PageView *m_pageview;
		std::string m_key;
		int m_id;
	} ImageQueueData;
	
	typedef struct {
		spdf::PageView *m_pageview;
		std::string m_key;
	} DocumentQueueData;
	
	class MainApp : public MainWindow {
		public:
			MainApp (const Glib::RefPtr<Gtk::Application>& app);
			
			/* load document's path */
			void load (Glib::ustring &path);
			
		private:
		
			/* opening document queue */
			std::deque<spdf::DocumentQueueData> m_doc_queue; 
			
			/* rendering image (document's page) queue */
			std::deque<spdf::ImageQueueData> m_image_queue;
			
			/* config and bookmark file's path */
			std::string m_path;
			
			/* loaded config file  */
			std::map<std::string, std::string> m_configs;
			spdf::ConfigData m_config;
			
			/* true if user is selecting texts, false otherwise  */
			bool m_selecting;
			
			/* sub-region of page. filled when user is selecting texts */
			int m_x1, m_y1, m_x2, m_y2;
			
			/* list of selected texts */
			std::vector<Rect> m_rects;
			
			/* load and save config */
			void load_config (std::string &path);
			void save_config ();
			
			/* save current page session */
			void save_session (spdf::PageView &pageview);
			
			/* switching page view mode */
			void switch_mode (spdf::PageView &pageview, spdf::ImageViewMode mode);
			
			/* render a page */
			void render_page (spdf::PageView &pageview);
			
			/* open a document */
			void open_document 
				(spdf::PageView &pageview, const spdf::UString &path, const spdf::UString &upass = spdf::UString (), const spdf::UString &opass = spdf::UString ());
				
			/* load a document */
			void load_document (spdf::PageView &pageview);
			
			/* close a document */
			void close_document (spdf::PageView &pageview);
			
			/* page navigation */
			void go_to_page (spdf::PageView &pageview, int index);
			void go_to_next_page (spdf::PageView &pageview);
			void go_to_prev_page (spdf::PageView &pageview);
			void zoom_in_page (spdf::PageView &pageview, double f);
			void zoom_out_page (spdf::PageView &pageview, double f);
			
			/* mark current page */
			void mark_current_page (spdf::PageView &pageview);
			
			/* fullscreen mode */
			void go_fullscreen ();
			void go_unfullscreen ();
			
			/* fill outline and bookmark */
			void fill_outline (spdf::PageView &pageview);
			void fill_bookmark (spdf::PageView &pageview);
			void update_toolbar (spdf::PageView &pageview);
			
			/* update toolbar */
			void find_text (spdf::PageView &pageview, spdf::UString &text);
			
			/* draw markers on selected texts */
			void draw_selection_page (spdf::PageView &pageview);
			void update_cursor (spdf::PageView &pageview);
			
			/* copy selected texts */
			void copy_text_selection_page (spdf::PageView &pageview);
			
			/* document error handler */
			void handle_document_error (spdf::PageView &pageview, DocumentError &err);
			
			/* translate message */
			void translate_message (spdf::Message &msg);
			void process_render_message (spdf::Message &msg);
			void process_load_message (spdf::Message &msg);
			void password_prompt (spdf::PageView &pageview);
			
		//slot:
			void on_open_btn_clicked ();
			void on_close_btn_clicked ();
			void on_next_btn_clicked ();
			void on_prev_btn_clicked ();
			void on_find_btn_clicked ();
			void on_zoomin_btn_clicked ();
			void on_zoomout_btn_clicked ();
			void on_fscreen_btn_clicked ();
			void on_about_btn_clicked ();
			void on_mark_btn_toggled ();
			void on_entry_text_changed ();
			
			void on_first_btn_clicked ();
			void on_last_btn_clicked ();
			void on_copy_btn_clicked ();
			
			void on_tab_page_changed (Gtk::Widget* page, guint page_num);
			void on_tab_page_added (Gtk::Widget* page, guint page_num);
			void on_tab_page_removed (Gtk::Widget* page, guint page_num);
			
			bool on_outline_sel_changed (GdkEventButton *event);
			bool on_bookmark_sel_changed (GdkEventButton *event);
			
			void on_find_entry_text_changed ();
			
			bool on_timeout_msg ();
			bool on_timeout_sel ();
			bool on_timeout_session ();
			bool on_image_button_event (GdkEventButton *event);
			bool on_idle_right_click_event (GdkEventButton *event);
			bool on_mainapp_key_press_event (GdkEventKey *event);
			void on_imageview_offset (bool dir, int index);
			void on_imageview_current (int index);
			void on_page_mode_item_toggled ();
	};
}
