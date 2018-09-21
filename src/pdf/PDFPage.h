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

#ifndef PDFPAGE_H
#define PDFPAGE_H

#ifndef DOCUMENTPAGE_H
#include "../DocumentPage.h"
#endif

class Page;
class TextPage;
class PDFDocument;

namespace spdf {

	class PDFPage : public DocumentPage {
		
		friend class PDFDocument;
		
		public:
			PDFPage ();
			~PDFPage ();
			Image *render (double scale);
			std::string searchText (Rect &rect, double scale);
			std::vector<Rect> searchRect (std::string &text, double scale);
			std::vector<Rect> getSelectionRegion (PageSelectionStyle style, Rect &rect, double scale);
			std::string getSelectionText (PageSelectionStyle style, Rect &rect, double scale);
			
		private:
			Page *m_poppler_page;
			TextPage *m_text_page;
	};

}

#endif
