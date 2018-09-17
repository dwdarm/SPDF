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

#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#ifndef DOCUMENT_H
#include "../Document.h"
#endif

#ifndef PDFPAGE_H
#include "PDFPage.h"
#endif

#include <string>

class PDFDoc;

namespace spdf {
	
	class PDFDocument : public Document {
		
		public:
			~PDFDocument ();
			DocumentPage *createPage (int index);
			std::string getTitle () const;
			DocumentOutline *getOutline () const;
			
			static PDFDocument *openDocument (const std::string &filename, 
						  const std::string &user_pass = std::string (), 
						const std::string &owner_pass = std::string ());
			
		private:
			PDFDoc *m_poppler_document;
	};
	
}

#endif
