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

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/image.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/stylecontext.h>
#include <gtkmm/adjustment.h>
#include <vector>
#include <memory>

namespace spdf {
	
	typedef struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	} ImageViewColor;
	
	typedef struct {
		int x;
		int y;
		int width;
		int height;
		ImageViewColor color;
		bool fill;
	} ImageViewRect;
	
	class ImageView : public Gtk::ScrolledWindow {
		public: 
			ImageView ();
			void appendRect (ImageViewRect &rect);
			void appendRects (std::vector<ImageViewRect> &rects);
			void clearImage ();
			void drawRect (ImageViewRect &rect);
			void drawRects (std::vector<ImageViewRect> &rects);
			Gtk::EventBox &getEventBox ();
			Gtk::Image &getImage ();
			int getImageHeight () const;
			int getImageWidth () const;
			std::shared_ptr<ImageViewColor> getPixel (int x, int y);
			void refresh ();
			void setImage (const unsigned char *d, int w, int h, int row);
			void setPixel (int x, int y, ImageViewColor &color);
			
		private:
			Gtk::Image m_image;
			int m_image_width;
			int m_image_height;
			int m_image_row;
			Gtk::EventBox m_image_event_box;
			std::shared_ptr<unsigned char> m_data; 
			std::shared_ptr<unsigned char> m_cdata; 
			Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;
			Glib::RefPtr<Gtk::CssProvider> cssprovider;
			Glib::RefPtr<Gtk::Adjustment> m_vadj;
			
			void on_mvadj_val_changed ();
	};
}

#endif
