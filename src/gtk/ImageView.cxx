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

#include "ImageView.h"
#include <cstring>
#include <iostream>

const char defcssdata[] {
	"scrolledwindow { background-color:grey; }"\
	"GtkScrolledWindow { background-color:grey; }"
};

spdf::ImageView::ImageView () : Gtk::ScrolledWindow ()
{
	m_vadj = get_vadjustment ();
	
	cssprovider = Gtk::CssProvider::create ();
	if (!cssprovider->load_from_data (defcssdata)) {
		std::cout << "CSS error\n";
	}
	get_style_context ()->add_provider (cssprovider, 5);
	
	m_image_width = 0;
	m_image_height = 0;
	m_image_event_box.add (m_image);
	
	add (m_image_event_box);
	
	//m_vadj->signal_value_changed ().connect 
	//	(sigc::mem_fun (*this, &spdf::ImageView::on_mvadj_val_changed));
}

void 
spdf::ImageView::appendRect (ImageViewRect &rect) 
{
	unsigned char *data = NULL;
	int pos = 0;
	
	if (!m_data.get ()) {
		return;
	} 
	
	if (!m_cdata.get ()) {
		m_cdata = std::shared_ptr<unsigned char> 
						(new unsigned char[m_image_row*m_image_height]);
		std::memcpy (m_cdata.get (), m_data.get (), m_image_row*m_image_height);
	}
	data = m_cdata.get ();
	
	clearImage ();
	for (int y = 0; y < rect.height; y++) {
		pos = (rect.x*4) + ((rect.y+y)*(m_image_width*4));
		for (int x = 0; x < rect.width; x++) {
			data[pos] = data[pos] + rect.color.r;
			data[pos+1] = data[pos+1] + rect.color.g;
			data[pos+2] = data[pos+2] + rect.color.b;
			data[pos+3] = data[pos+3] + rect.color.a;
			pos += 4;
		}
	}
	
	m_pixbuf = Gdk::Pixbuf::create_from_data (m_cdata.get (), 
			 Gdk::COLORSPACE_RGB, true, 8, m_image_width, m_image_height, 
														   m_image_row);
	                         
	m_image.set (m_pixbuf);   
	
}

void 
spdf::ImageView::appendRects (std::vector<ImageViewRect> &rects)
{
	unsigned char *data = NULL;
	int pos = 0;
	std::vector<ImageViewRect>::iterator it;
	
	if (!m_data.get ()) {
		return;
	} 
	
	if (rects.size () == 0) {
		return;
	} 
	
	if (!m_cdata.get ()) {
		m_cdata = std::shared_ptr<unsigned char> 
						(new unsigned char[m_image_row*m_image_height]);
		std::memcpy (m_cdata.get (), m_data.get (), m_image_row*m_image_height);
	}
	data = m_cdata.get ();
	
	clearImage ();
	for (it = rects.begin (); it != rects.end (); it++) {
		for (int y = 0; y < (*it).height; y++) {
			pos = ((*it).x*4) + (((*it).y+y)*(m_image_width*4));
			for (int x = 0; x < (*it).width; x++) {
				data[pos] = data[pos] + (*it).color.r;
				data[pos+1] = data[pos+1] + (*it).color.g;
				data[pos+2] = data[pos+2] + (*it).color.b;
				data[pos+3] = data[pos+3] + (*it).color.a;
				pos += 4;
			}
		}
	}
	
	m_pixbuf = Gdk::Pixbuf::create_from_data (m_cdata.get (), 
			 Gdk::COLORSPACE_RGB, true, 8, m_image_width, m_image_height, 
														   m_image_row);
	                         
	m_image.set (m_pixbuf); 
}

void
spdf::ImageView::clearImage ()
{
	m_image.clear ();
}

void 
spdf::ImageView::drawRect (ImageViewRect &rect)
{
	unsigned char *data = NULL;
	int pos = 0;
	
	if (!m_data.get ()) {
		return;
	} 
	
	if (!m_cdata.get ()) {
		m_cdata = std::shared_ptr<unsigned char> 
						(new unsigned char[m_image_row*m_image_height]);
		std::memcpy (m_cdata.get (), m_data.get (), m_image_row*m_image_height);
	}
	data = m_cdata.get ();
	
	clearImage ();
	for (int y = 0; y < rect.height; y++) {
		pos = (rect.x*4) + ((rect.y+y)*(m_image_width*4));
		for (int x = 0; x < rect.width; x++) {
			data[pos] = rect.color.r;
			data[pos+1] = rect.color.g;
			data[pos+2] = rect.color.b;
			data[pos+3] = rect.color.a;
			pos += 4;
		}
	}
	
	m_pixbuf = Gdk::Pixbuf::create_from_data (m_cdata.get (), 
			 Gdk::COLORSPACE_RGB, true, 8, m_image_width, m_image_height, 
														   m_image_row);
	                         
	m_image.set (m_pixbuf);   
	
}

void 
spdf::ImageView::drawRects (std::vector<ImageViewRect> &rects)
{
	unsigned char *data = NULL;
	int pos = 0;
	std::vector<ImageViewRect>::iterator it;
	
	if (!m_data.get ()) {
		return;
	} 
	
	if (rects.size () == 0) {
		return;
	} 
	
	if (!m_cdata.get ()) {
		m_cdata = std::shared_ptr<unsigned char> 
						(new unsigned char[m_image_row*m_image_height]);
		std::memcpy (m_cdata.get (), m_data.get (), m_image_row*m_image_height);
	}
	data = m_cdata.get ();
	
	clearImage ();
	for (it = rects.begin (); it != rects.end (); it++) {
		for (int y = 0; y < (*it).height; y++) {
			pos = ((*it).x*4) + (((*it).y+y)*(m_image_width*4));
			for (int x = 0; x < (*it).width; x++) {
				data[pos] = (*it).color.r;
				data[pos+1] = (*it).color.g;
				data[pos+2] = (*it).color.b;
				data[pos+3] = (*it).color.a;
				pos += 4;
			}
		}
	}
	
	m_pixbuf = Gdk::Pixbuf::create_from_data (m_cdata.get (), 
			 Gdk::COLORSPACE_RGB, true, 8, m_image_width, m_image_height, 
														   m_image_row);
	                         
	m_image.set (m_pixbuf); 
}

Gtk::EventBox &
spdf::ImageView::getEventBox ()
{
	return m_image_event_box;
}

Gtk::Image &
spdf::ImageView::getImage ()
{
	return m_image;
}

int 
spdf::ImageView::getImageHeight () const
{
	return m_image_height;
}

int 
spdf::ImageView::getImageWidth () const
{
	return m_image_width;
}

std::shared_ptr<spdf::ImageViewColor> 
spdf::ImageView::getPixel (int x, int y)
{
	ImageViewColor *c = NULL;
	unsigned char *data = NULL;
	int pos = 0;
	
	if (!m_data.get ()) {
		return std::shared_ptr<ImageViewColor> (c);
	}
	
	data = m_cdata.get ();
	if (!data) {
		data = m_data.get ();
	}
	
	pos = (x*4) + y*(m_image_width*4);
	c = new ImageViewColor;
	c->r = data[pos];
	c->g = data[pos+1];
	c->b = data[pos+2];
	c->a = data[pos+3];
	
	return std::shared_ptr<ImageViewColor> (c);
}

void
spdf::ImageView::refresh ()
{
	if (!m_data.get ()) {
		return;
	}
	
	clearImage ();
	m_cdata.reset (); 
	
	m_pixbuf = Gdk::Pixbuf::create_from_data (m_data.get (), 
			 Gdk::COLORSPACE_RGB, true, 8, m_image_width, m_image_height, 
														   m_image_row);
	                         
	m_image.set (m_pixbuf);       
}

void 
spdf::ImageView::setImage (const unsigned char *d, int w, int h, int row)
{
	clearImage ();
	m_cdata.reset (); 
	
	m_data = std::shared_ptr<unsigned char> (new unsigned char[row*h]);
	std::memcpy (m_data.get (), d, row*h);
	
	m_pixbuf = Gdk::Pixbuf::create_from_data 
	            (m_data.get (), Gdk::COLORSPACE_RGB, true, 8, w, h, row);
	
	m_image_width = w;
	m_image_height = h;
	m_image_row = row;
	m_image.set (m_pixbuf);
}

void 
spdf::ImageView::setPixel (int x, int y, ImageViewColor &color)
{
	unsigned char *data = NULL;
	int pos = 0;
	
	if (!m_data.get ()) {
		return;
	} 
	
	if (!m_cdata.get ()) {
		m_cdata = std::shared_ptr<unsigned char> 
						(new unsigned char[m_image_row*m_image_height]);
		std::memcpy (m_cdata.get (), m_data.get (), m_image_row*m_image_height);
	}
	data = m_cdata.get ();
	
	clearImage ();
	pos = (x*4) + y*(m_image_width*4);
	data[pos] = color.r;
	data[pos+1] = color.g;
	data[pos+2] = color.b;
	data[pos+3] = color.a;
	
	m_pixbuf = Gdk::Pixbuf::create_from_data (m_cdata.get (), 
			 Gdk::COLORSPACE_RGB, true, 8, m_image_width, m_image_height, 
														   m_image_row);
	                         
	m_image.set (m_pixbuf);   
	
}

void 
spdf::ImageView::on_mvadj_val_changed ()
{
	std::cout << "value changed\n";
	std::cout << m_vadj->get_value () << "(" << m_vadj->get_lower () << " to " << m_vadj->get_upper () << ")"<< "\n";
	std::cout << "step = " << m_vadj->get_step_increment () << "\n";  
	std::cout << "pstep = " << m_vadj->get_page_increment () << "\n"; 
	std::cout << "psize = " << m_vadj->get_page_size () << "\n"; 
	std::cout << "wwidth = " << get_window ()->get_width () << "\n"; 
	std::cout << "wheight = " << get_window ()->get_height () << "\n"; 
	std::cout << "w = " << m_image_width << " " << "h = " << m_image_height << "\n";  
}
