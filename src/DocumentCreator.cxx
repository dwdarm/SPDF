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
#include "pdf/PDFDocument.h"
#endif

#ifndef DJVUDOCUMENT_H
#include "djvu/DJVUDocument.h"
#endif

#include "DocumentCreator.h"

spdf::DocumentCreator *spdf::DocumentCreator::m_instance = NULL;

spdf::DocumentCreator *
spdf::DocumentCreator::instance ()
{
	if (!m_instance) {
		m_instance = new spdf::DocumentCreator ();
	}
	
	return m_instance;
}

spdf::Document *
spdf::DocumentCreator::openDocument (const std::string &filename, 
			const std::string &user_pass, const std::string &owner_pass)
{
	Document *retVal = NULL;
	
	// PDF Support
	if (std::string (filename, filename.size ()-3, 3) == "pdf") {
		retVal = spdf::PDFDocument::openDocument (filename);
	// DJVU Support
	} else if (std::string (filename, filename.size ()-4, 4) == "djvu") {
		retVal = spdf::DJVUDocument::openDocument (filename);
	}
	
	return retVal;
}

