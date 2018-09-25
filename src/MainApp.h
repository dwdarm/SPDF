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
#include <gtkmm/filechooserdialog.h>
#include "gtk/MainWindow.h"
#include "DocumentCreator.h"

#define APP_VERSION "0.1a"

#define SCALE_STEP 0.25

class PageNavPopup;

namespace spdf {
	class MainApp : public MainWindow {
		public:
			MainApp ();
			
		private:
			std::string m_path;
			std::map<std::string, std::string> m_configs;
			PageNavPopup m_page_nav_popup;
			bool m_selecting;
			Rect m_selected_rect;
			std::vector<Rect> m_selected_regs;
			void go_fullscreen ();
			void go_unfullscreen ();
			void draw_page (spdf::PageView &pageview);
			void fill_outline (spdf::PageView &pageview);
			void fill_bookmark (spdf::PageView &pageview);
			void walk_fill_outline (spdf::OutlineView &outlineview,
						std::vector<spdf::DocumentOutlineItem> &toc_item, 
													Gtk::TreeIter &iter);
			void update_toolbar (spdf::PageView &pageview);
			void find_text (spdf::PageView &pageview, std::string &str);
			
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
			
			void on_outline_sel_changed ();
			void on_bookmark_sel_changed ();
			
			void on_find_entry_text_changed ();
			
			void on_open_dialog_resp (int id, Gtk::FileChooser &chooser);
			
			bool on_timeout_msg ();
			bool on_timeout_sel ();
			bool on_image_button_event (GdkEventButton *event);
			bool on_idle_right_click_event (GdkEventButton *event);
			bool on_mainapp_key_press_event (GdkEventKey *event);
	};
}
