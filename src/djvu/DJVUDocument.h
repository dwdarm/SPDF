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

#ifndef DJVUDOCUMENT_H
#define DJVUDOCUMENT_H

#ifndef DOCUMENT_H
#include "../Document.h"
#endif

#ifndef DJVUPAGE_H
#include "DJVUPage.h"
#endif

#ifndef DDJVUAPI_H
#include <libdjvu/ddjvuapi.h>
#endif

#include <string>

namespace spdf {
	class DJVUDocument : public Document {
		public:
		    ~DJVUDocument ();
			DocumentPage *createPage (int index);
			
			static DJVUDocument *openDocument (const UString &filename);
			static int m_count;
			static ddjvu_context_t *m_context;
			
		private:
			ddjvu_document_t *m_document;
	};
}

#endif
