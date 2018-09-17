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

#ifndef DJVUPAGE_H
#define DJVUPAGE_H

#ifndef DOCUMENTPAGE_H
#include "../DocumentPage.h"
#endif

#ifndef DDJVUAPI_H
#include <libdjvu/ddjvuapi.h>
#endif

namespace spdf {
	class DJVUPage : public DocumentPage {
		public:
			friend class DJVUDocument;
		
			DJVUPage ();
			~DJVUPage ();
			Image *render (double scale);
			
		private:
			ddjvu_page_t *m_page_handler;
			double m_dpi;
	};
}

#endif
