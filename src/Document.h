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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#ifndef DOCUMENTOUTLINE_H
#include "DocumentOutline.h"
#endif

#ifndef DOCUMENTPAGE_H
#include "DocumentPage.h"
#endif

#ifndef USTRING_H
#include "UString.h"
#endif

#include <string>

namespace spdf {
	
	enum DocumentType {
		UNKNOWN = 0,
		PDF ,
		DJVU
	};
	
	enum DocumentPageLayout {
		PAGE_LAYOUT_UNKNOWN = 0,
		PAGE_LAYOUT_SINGLE_PAGE,
		PAGE_LAYOUT_ONE_COLUMN,
		PAGE_LAYOUT_TWO_COLUMN_LEFT,
		PAGE_LAYOUT_TWO_COLUMN_RIGHT,
		PAGE_LAYOUT_TWO_PAGE_LEFT,
		PAGE_LAYOUT_TWO_PAGE_RIGHT,
		PAGE_LAYOUT_NULL
	};
	
	class DocumentError {
		public:
			int code;
			DocumentType type;
			UString msg;
	};
	
	class Document {
		public:
			Document ();
			virtual ~Document ();
			virtual DocumentPage *createPage (int index) = 0;
			int getId () const;
			virtual DocumentOutline *getOutline () const;
			virtual int getPages () const;
			virtual DocumentPageLayout getPageLayout () const;
			virtual DocumentType getType () const;
			virtual UString getTitle () const;
			virtual bool isEncrypted() const;
			virtual bool isLoaded () const;
			
		protected:
			UString m_title;
			bool m_is_loaded;
			int m_pages;
			DocumentType m_type;
			int m_id;
			
			static int s_id;
	};

}

#endif
