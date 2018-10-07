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
#include <cstdio>

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
spdf::DocumentCreator::openDocument (const UString &filename, 
	 const UString &user_pass, const UString &owner_pass, DocumentError *err)
{
	Document *retVal = NULL;
	FILE *f = NULL;
	char b [16];
	
	f = fopen (filename.data (), "rb");
	if (!f) {
		return retVal;
	}
	
	fread (b, 1, 16, f);
	fclose (f);
	
	// PDF Support
	if ((b[0] == '%') && (b[1] == 'P') && (b[2] == 'D') && (b[3] == 'F')) {
		retVal = spdf::PDFDocument::openDocument (filename, user_pass, owner_pass, err);
	// DJVU Support
	} else if ((b[0] == 'A') && (b[1] == 'T') && (b[2] == '&') && (b[3] == 'T')) {
		retVal = spdf::DJVUDocument::openDocument (filename);
	}
	
	return retVal;
}

