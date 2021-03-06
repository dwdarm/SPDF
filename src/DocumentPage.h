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

#ifndef DOCUMENTPAGE_H
#define DOCUMENTPAGE_H

#ifndef IMAGE_H
#include "Image.h"
#endif

#include "UString.h"

#include <vector>
#include <string>

namespace spdf {
	
	typedef struct {
		int index;
		int x, y;
		int width, height;
	} Rect;
	
	enum PageSelectionStyle {
		SELECTION_STYLE_GLYPH,
		SELECTION_STYLE_WORD,
		SELECTION_STYLE_LINE
	};
	
	enum PageSearchDirection {
		SEARCH_DIRECTION_NEXT,
		SEARCH_DIRECTION_PREVIOUS
	};

	class DocumentPage {
		
		public:
			DocumentPage ();
			virtual ~DocumentPage ();
			virtual Image *render (double scale) = 0;
			virtual UString searchText (Rect &rect, double scale);
			virtual Rect searchRect (Rect &rect, std::string &text, double scale, PageSearchDirection dir);
			virtual std::vector<Rect> searchRect (std::string &text, double scale);
			virtual std::vector<Rect> getSelectionRegion (PageSelectionStyle style, Rect &rect, double scale);
			virtual std::string getSelectionText (PageSelectionStyle style, Rect &rect, double scale);
			virtual double getHeight () const;
			virtual int getIndex () const;
			virtual double getWidth () const;
			
		protected:
			int m_index;
			double m_height;
			double m_width;
	};

}

#endif
