#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <gtkmm/widget.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/adjustment.h>
#include <sigc++/sigc++.h>
#include <memory>
#include <deque>

namespace spdf {
	
	enum ImageViewMode {
		IMAGEVIEW_SINGLE = 0,
		IMAGEVIEW_CONTINOUS
	};
	
	typedef struct {
		std::shared_ptr<unsigned char> m_refImage;
		int m_image_id;
		int m_image_index;
		int m_image_width;
		int m_image_height;
		int m_image_row;
	} ImageViewData;
	
	typedef struct {
		int m_index;
		int m_x;
		int m_y;
		int m_width;
		int m_height;
	} ImageViewMarker;
	
	typedef struct {
		ImageViewData m_imageview_data;
		int m_x1;
		int m_y1;
		int m_x2;
		int m_y2;
	} ImageViewRect;

	class ImageView : public Gtk::Widget {
		
		public:
			ImageView ();
			virtual ~ImageView ();
			void cursor_position (int &x, int &y);
			void clear ();
			void clear_marker ();
			void draw_marker (std::vector<ImageViewMarker> &markers);
			void freeze (bool f = true);
			ImageViewMode get_mode () const;
			std::vector<ImageViewRect> get_image_at_region (int x1, int y1, int x2, int y2);
			std::vector<int> get_image_indexs ();
			bool last_signal () const;
			int max_size () const;
			int push_front(unsigned char *d, int i, int w, int h, int r);
			int push_back (unsigned char *d, int i, int w, int h, int r);
			void pop_front();
			void pop_back ();
			void refresh ();
			bool runout () const;
			void inc_max_size ();
			void scroll_up ();
			void scroll_down ();
			void scroll_left ();
			void scroll_right ();
			void set (unsigned char *d, int i, int w, int h, int r);
			void set (unsigned char *d, int id);
			void set_mode (ImageViewMode mode);
			
			sigc::signal<void, bool, int> signal_offset ();
			sigc::signal<void, int> signal_current ();
			
		protected:
			void do_surface_drawing ();
			void do_regular_drawing (const Cairo::RefPtr<Cairo::Context>& cr);
			
			Glib::RefPtr<Gdk::Window> m_refGdkWindow;
			Cairo::RefPtr<::Cairo::ImageSurface> m_image_surface;
			std::deque<ImageViewData> m_data;
			std::vector<ImageViewMarker> m_markers;
			int m_padding;
			int m_sspeed;
			int m_pos;
			int m_posx;
			int m_image_total_height;
			bool m_freeze;
			ImageViewMode m_mode;
			int m_max_size;
			bool m_last_signal;
			bool m_is_edited;
			int m_id;
			
			sigc::signal<void, bool, int> m_signal_offset;
			sigc::signal<void, int> m_signal_current;
			
			bool on_imageview_scroll_event (GdkEventScroll* scroll_event);
			
			Gtk::SizeRequestMode get_request_mode_vfunc() const override;
			void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
			void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const  override;
			void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
			void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
			void on_size_allocate(Gtk::Allocation& allocation) override;
			void on_map() override;
			void on_unmap() override;
			void on_realize() override;
			void on_unrealize() override;
			bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
	};
}

#endif
