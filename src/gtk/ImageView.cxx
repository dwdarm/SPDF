#include "ImageView.h"
#include <gdkmm/general.h>
#include <gdkmm/display.h>
#include <gdkmm/devicemanager.h>
#include <gdkmm/device.h>
#include <cstring>
#include <iostream>

spdf::ImageView::ImageView ()
				: Glib::ObjectBase("imageview"),
				  Gtk::Widget ()
{
	set_has_window(true);
	
	m_sspeed = 50;
	m_padding = 10;
	m_mode = IMAGEVIEW_CONTINOUS;
	m_freeze = false;
	m_image_total_height = 0;
	m_pos = 0;
	m_posx = 0;
	m_max_size = 2;
	m_last_signal = true;
	m_is_edited = false;
	
	Gtk::Widget::signal_scroll_event ().connect (sigc::mem_fun (*this, &spdf::ImageView::on_imageview_scroll_event));
}

spdf::ImageView::~ImageView ()
{
	m_image_surface.clear ();
	m_data.clear ();
}

void 
spdf::ImageView::cursor_position (int &x, int &y)
{
	Glib::RefPtr<Gdk::Display> display;
	Glib::RefPtr<Gdk::DeviceManager> devm;
	Glib::RefPtr<Gdk::Device> dev;
	Gdk::ModifierType mod;
	
	x = -1;
	y = -1;
	
	if (!m_refGdkWindow) {
		return;
	}
	
	display = m_refGdkWindow->get_display ();
	
	devm = display->get_device_manager ();
	if (!devm) {
		return;
	}
	
	dev = devm->get_client_pointer ();
	m_refGdkWindow->get_device_position (dev, x, y, mod);
}

void 
spdf::ImageView::clear ()
{
	m_image_total_height = 0;
	m_pos = 0;
	m_posx = 0;
	m_max_size = 2;
	m_last_signal = true;
	m_is_edited = false;
	
	m_image_surface.clear ();
	m_data.clear ();
	m_markers.clear ();
	
	queue_draw ();
}

void 
spdf::ImageView::clear_marker ()
{
	if (!m_data.size ()) {
		return;
	}
	
	m_markers.clear ();
	do_surface_drawing ();
	queue_draw ();
}

void 
spdf::ImageView::draw_marker (std::vector<ImageViewMarker> &markers)
{
	if (!m_data.size ()) {
		return;
	}
	
	m_markers = markers;
	do_surface_drawing ();
	queue_draw ();
}

void 
spdf::ImageView::freeze (bool f)
{
	m_freeze = f;
}

spdf::ImageViewMode 
spdf::ImageView::get_mode () const
{
	return m_mode;
}

std::vector<spdf::ImageViewRect> 
spdf::ImageView::get_image_at_region (int x1, int y1, int x2, int y2)
{
	const Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
	std::vector<spdf::ImageViewRect> rects;
	spdf::ImageViewRect rect;
	int pos = 0;
	int lim = 0;
	int temp = 0;
	bool found = false;
	
	if (!m_image_total_height) {
		return rects;
	}

	pos = m_pos + y1;
	lim = y2 - y1;
	
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		temp += (*it).m_image_height + m_padding;
		if (pos < temp) {
			rect.m_imageview_data = *it;
			
			if ((*it).m_image_width < width) {
				rect.m_x1 = x1 - (width - (*it).m_image_width)/2;
				rect.m_x2 = x2 - (width - (*it).m_image_width)/2;
				if (rect.m_x1 < 0) {
					rect.m_x1 = 0;
				}
				if (rect.m_x2 >= (*it).m_image_width) {
					rect.m_x2 = (*it).m_image_width - 1;
				}
			} else {
				rect.m_x1 = m_posx + x1;
				rect.m_x2 = m_posx + x2;
			}
			
			if (!rects.size ()) {
				if (temp > ((*it).m_image_height + m_padding)) {
					rect.m_y1 = pos - (temp - ((*it).m_image_height + m_padding));
				} else {
					rect.m_y1 = pos;
				}
				
				if ((rect.m_y1 + lim) < ((*it).m_image_height + m_padding)) {
					rect.m_y2 = rect.m_y1 + lim;
				} else {
					rect.m_y2 = (*it).m_image_height;
					lim -= (((*it).m_image_height + m_padding) - rect.m_y1);
				}
				
			} else {
				rect.m_y1 = 0;
				if (lim < ((*it).m_image_height + m_padding)) {
					rect.m_y2 = lim;
				} else {
					rect.m_y2 = (*it).m_image_height;
					lim -= ((*it).m_image_height + m_padding);
				}
			}
			
			rects.push_back (rect);
			if ((pos + lim) < temp) {
				break;
			}
			pos += (((*it).m_image_height - pos) + m_padding);
		}
	}
	
	return rects;
}

bool 
spdf::ImageView::last_signal () const
{
	return m_last_signal;
}

void
spdf::ImageView::push_front (unsigned char *d, int i, int w, int h, int r)
{
	ImageViewData data;
	
	if (m_mode == IMAGEVIEW_CONTINOUS) {
		while (m_data.size () >= m_max_size) {
			pop_back ();
		}
	}
	
	data.m_refImage = std::shared_ptr<unsigned char> 
	     (new unsigned char[r*h], [] (unsigned char *p) { delete [] p;});
	memcpy (data.m_refImage.get (), d, r*h);
	
	data.m_image_index = i;
	data.m_image_width = w;
	data.m_image_height = h;
	data.m_image_row = r;
	
	m_data.push_front (data);
	m_image_total_height += (h + m_padding);
	m_pos += (h);
	
	m_is_edited = true;
}

void
spdf::ImageView::push_back (unsigned char *d, int i, int w, int h, int r)
{
	ImageViewData data;
	
	if (m_mode == IMAGEVIEW_CONTINOUS) {
		while (m_data.size () >= m_max_size) {
			pop_front ();
		}
	}
	
	data.m_refImage = std::shared_ptr<unsigned char> 
	     (new unsigned char[r*h], [] (unsigned char *p) { delete [] p;});
	memcpy (data.m_refImage.get (), d, r*h);
	
	data.m_image_index = i;
	data.m_image_width = w;
	data.m_image_height = h;
	data.m_image_row = r;
	
	m_data.push_back (data);
	m_image_total_height += (h + m_padding);
	
	m_is_edited = true;
}

void 
spdf::ImageView::pop_front()
{
	if (!m_data.size ()) {
		return;
	}
	
	if (m_pos > (m_data[0].m_image_height)) {
		m_pos -= (m_data[0].m_image_height + m_padding);
	}
	
	m_data.pop_front ();
	m_image_total_height = 0;
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		m_image_total_height += ((*it).m_image_height + m_padding);
	}
	
	m_is_edited = true;
}

void 
spdf::ImageView::pop_back ()
{
	if (!m_data.size ()) {
		return;
	}
	
	m_data.pop_back ();
	m_image_total_height = 0;
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		m_image_total_height += ((*it).m_image_height + m_padding);
	}
	
	m_is_edited = true;
}

void 
spdf::ImageView::refresh ()
{
	if (!m_data.size ()) {
		return;
	}
	
	if (m_is_edited) {
		do_surface_drawing ();
		m_is_edited = false;
	}
	
	if (m_mode == IMAGEVIEW_CONTINOUS) {
		if (m_image_total_height <= get_allocation().get_height()) {
			m_last_signal = true;
			m_signal_runout.emit (m_data.back ().m_image_index);
			m_max_size += 1;
		} else {
			queue_draw ();
		}
	} else {
		queue_draw ();
	}
}

void 
spdf::ImageView::scroll_up ()
{
	int pos = 0;
	
	if (!m_data.size ()) {
		return;
	}
	
	if (m_image_total_height <= get_allocation().get_height()) {
		return;
	}
	
	if (m_freeze) {
		return;
	}
	
	if ((m_pos - m_sspeed) <= 0) {
		m_pos = 0;
		queue_draw ();
		if (m_mode == IMAGEVIEW_CONTINOUS) {
			m_last_signal = false;
			m_signal_offset.emit (false, m_data.front ().m_image_index);
			return;
		}
	} else {
		m_pos -= m_sspeed;
	}
	
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		if (m_pos < pos) {
			m_signal_current.emit ((*it).m_image_index);
			break;
		}
		pos += ((*it).m_image_height + m_padding);
	}
	
	refresh ();
}

void 
spdf::ImageView::scroll_down ()
{
	int pos = 0;
	
	if (!m_data.size ()) {
		return;
	}
	
	if (m_image_total_height <= get_allocation().get_height()) {
		return;
	}
	
	if (m_freeze) {
		return;
	}
	
	if ((m_pos + m_sspeed) >= (m_image_total_height - get_allocation().get_height ())) {
		m_pos = m_image_total_height - get_allocation().get_height ();
		queue_draw ();
		if (m_mode == IMAGEVIEW_CONTINOUS) {
			m_last_signal = true;
			m_signal_offset.emit (true, m_data.back ().m_image_index);
			return;
		}
	} else {
		m_pos += m_sspeed;
	}
	
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		if (m_pos < pos) {
			m_signal_current.emit ((*it).m_image_index);
			break;
		}
		pos += ((*it).m_image_height + m_padding);
	}
	
	refresh ();
}

void 
spdf::ImageView::scroll_left ()
{
	int pos = 0;
	int max_width = 0;
	
	if (!m_data.size ()) {
		return;
	}
	
	if ((m_posx - 20) <= 0) {
		return;
	}
	
	m_posx = m_posx - 20;
	
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		if (max_width < (*it).m_image_width) {
			max_width = (*it).m_image_width;
		}
	}
	
	if (max_width < get_allocation().get_width ()) {
		return;
	}
	
	if (m_freeze) {
		return;
	}
	
	queue_draw ();
}

void 
spdf::ImageView::scroll_right ()
{
	int pos = 0;
	int max_width = 0;
	
	if (!m_data.size ()) {
		return;
	}
	
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		if (max_width < (*it).m_image_width) {
			max_width = (*it).m_image_width;
		}
	}
	
	if (max_width < get_allocation().get_width ()) {
		return;
	}
	
	if (m_freeze) {
		return;
	}
	
	if ((m_posx + 20) >= (max_width - get_allocation().get_width ())) {
		return;
	}
	
	m_posx = m_posx + 20;
	
	queue_draw ();
}

void
spdf::ImageView::set (unsigned char *d, int i, int w, int h, int r)
{
	ImageViewData data;
	
	if (!m_data.size ()) {
		m_data.push_back (data);
	}
	
	m_data.front ().m_refImage = std::shared_ptr<unsigned char> 
	     (new unsigned char[r*h], [] (unsigned char *p) { delete [] p;});
	memcpy (m_data.front ().m_refImage.get (), d, r*h);
	
	m_data.front ().m_image_index = i;
	m_data.front ().m_image_width = w;
	m_data.front ().m_image_height = h;
	m_data.front ().m_image_row = r;
	
	m_image_total_height = (h + m_padding);
	
	m_is_edited = true;
}

void 
spdf::ImageView::set_mode (ImageViewMode mode)
{
	m_mode = mode;
}

sigc::signal<void, bool, int> 
spdf::ImageView::signal_offset ()
{
	return m_signal_offset;
}

sigc::signal<void, int> 
spdf::ImageView::signal_runout ()
{
	return m_signal_runout;
}

sigc::signal<void, int> 
spdf::ImageView::signal_current ()
{
	return m_signal_current;
}

void 
spdf::ImageView::do_surface_drawing ()
{
	Cairo::RefPtr<Cairo::Context> cr, cr2;
	Cairo::RefPtr<::Cairo::ImageSurface> image_surface;
	int max_width = 0;
	int offset = 0;
	
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		if (max_width < (*it).m_image_width) {
			max_width = (*it).m_image_width;
		}
	}
	
	m_image_surface = Cairo::ImageSurface::create (Cairo::FORMAT_ARGB32, max_width, m_image_total_height);
	cr = Cairo::Context::create (m_image_surface);
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		image_surface = Cairo::ImageSurface::create ((*it).m_refImage.get (), Cairo::FORMAT_ARGB32, (*it).m_image_width, (*it).m_image_height, (*it).m_image_row);
		if ((*it).m_image_width < max_width) {
			cr->set_source (image_surface, (max_width - (*it).m_image_width)/2, offset);
		} else {
			cr->set_source (image_surface, 0, offset);
		}
		cr->paint ();
		
		if (m_markers.size ()) {
			cr->save ();
			cr->set_source_rgba (0, 0, 0, 0.5);
			for (auto it2 = m_markers.begin (); it2 != m_markers.end (); it2++) {
				if ((*it).m_image_index == (*it2).m_index) {
					cr->rectangle ((*it2).m_x, offset + (*it2).m_y, (*it2).m_width, (*it2).m_height);
					cr->fill ();
				}
			}
			cr->restore ();
		}
		
		offset += (*it).m_image_height + m_padding;
	}
}

void 
spdf::ImageView::do_regular_drawing (const Cairo::RefPtr<Cairo::Context>& cr)
{
	const Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
	Cairo::RefPtr<::Cairo::Surface> surface;
	int fit_width = 0;
	int center_width = 0;
	int center_height = 0;
	int max_width = 0;
	
	cr->save();
	cr->set_source_rgba (0, 0, 0, 0);
	cr->paint ();		
	cr->restore();
	
	if (!m_image_surface) {
		return;
	}
	
	for (auto it = m_data.begin (); it != m_data.end (); it++) {
		if (max_width < (*it).m_image_width) {
			max_width = (*it).m_image_width;
		}
	}
	
	if (m_mode == IMAGEVIEW_CONTINOUS) {
		if (m_pos < 0) {
			m_pos = 0;
		} else if (m_pos >= (m_image_surface->get_height () - height)) {
			m_pos = m_image_surface->get_height () - height;
		}
		fit_width = height;
		center_width = (width - m_image_surface->get_width ())/2;
		if (center_width < 0) {
			center_width = 0;
		}
	} else {
		if (m_image_surface->get_height () > height) {
			if (m_pos < 0) {
				m_pos = 0;
			} else if (m_pos >= (m_image_surface->get_height () - height)) {
				m_pos = m_image_surface->get_height () - height;
			}
		} else {
			m_pos = 0;
		}
		fit_width = m_image_surface->get_height () - m_pos;
		center_width = (width - m_image_surface->get_width ())/2;
		if (center_width < 0) {
			center_width = 0;
		}
		center_height = (height - m_image_surface->get_height ())/2;
		if (center_height < 0) {
			center_height = 0;
		}
	}
	
	if (max_width < width) {
		m_posx = 0;
	}
	
	cr->save();
	surface = Cairo::Surface::create (m_image_surface, m_posx, m_pos, m_image_surface->get_width (), fit_width);
	cr->set_source (surface, center_width, center_height);
	cr->paint ();		
	cr->restore();
}

bool 
spdf::ImageView::on_imageview_scroll_event (GdkEventScroll* scroll_event)
{
	if (scroll_event->direction == GDK_SCROLL_UP) {
		scroll_up ();
	} else if (scroll_event->direction == GDK_SCROLL_DOWN) {
		scroll_down ();
	}
	
	return true;
}

Gtk::SizeRequestMode 
spdf::ImageView::get_request_mode_vfunc () const
{
	return Gtk::Widget::get_request_mode_vfunc();
}

void 
spdf::ImageView::get_preferred_width_vfunc (int& minimum_width, int& natural_width) const
{
	minimum_width = 0;
	natural_width = 0;
}

void 
spdf::ImageView::get_preferred_height_for_width_vfunc (int /* width */, int& minimum_height, int& natural_height) const
{
	minimum_height = 0;
	natural_height = 0;
}

void 
spdf::ImageView::get_preferred_height_vfunc (int& minimum_height, int& natural_height) const
{
	minimum_height = 0;
	natural_height = 0;
}

void 
spdf::ImageView::get_preferred_width_for_height_vfunc (int /* height */, int& minimum_width, int& natural_width) const
{
	minimum_width = 0;
	natural_width = 0;
}

void 
spdf::ImageView::on_size_allocate(Gtk::Allocation& allocation)
{
	set_allocation (allocation);
	
	if (m_refGdkWindow) {
		m_refGdkWindow->move_resize (allocation.get_x(), allocation.get_y(), allocation.get_width(), allocation.get_height());
    }
}

void 
spdf::ImageView::on_map()
{
	Gtk::Widget::on_map();
}

void 
spdf::ImageView::on_unmap()
{
	Gtk::Widget::on_unmap();
}

void 
spdf::ImageView::on_realize()
{
	set_realized();
	
	if(!m_refGdkWindow) {
		GdkWindowAttr attributes;
		memset(&attributes, 0, sizeof(attributes));

		Gtk::Allocation allocation = get_allocation();

		attributes.x = allocation.get_x();
		attributes.y = allocation.get_y();
		attributes.width = allocation.get_width();
		attributes.height = allocation.get_height();

		attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK | Gdk::SCROLL_MASK | Gdk::POINTER_MOTION_MASK;
		attributes.window_type = GDK_WINDOW_CHILD;
		attributes.wclass = GDK_INPUT_OUTPUT;

		m_refGdkWindow = Gdk::Window::create(get_parent_window(), &attributes, GDK_WA_X | GDK_WA_Y);
		set_window(m_refGdkWindow);

		m_refGdkWindow->set_user_data(gobj());
	}
}

void 
spdf::ImageView::on_unrealize()
{
	m_refGdkWindow.reset();
	Gtk::Widget::on_unrealize();
}

bool 
spdf::ImageView::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	do_regular_drawing (cr);
	
	return true;
}
