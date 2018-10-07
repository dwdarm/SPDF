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

#include "Document.h"

int spdf::Document::s_id = 1;

spdf::Document::Document () 
{
	m_is_loaded = false;
	m_pages = 0;
	m_type = UNKNOWN;
	m_id = Document::s_id;
	
	Document::s_id++;
}

spdf::Document::~Document ()
{
	
}

int 
spdf::Document::getId () const
{
	return m_id;
}

spdf::DocumentOutline *
spdf::Document::getOutline () const
{
	return NULL;
}

int
spdf::Document::getPages () const
{
	return m_pages;
}

spdf::DocumentPageLayout 
spdf::Document::getPageLayout () const
{
	return PAGE_LAYOUT_UNKNOWN; 
}

spdf::DocumentType
spdf::Document::getType () const
{
	return m_type;
}

spdf::UString
spdf::Document::getTitle () const
{
	return m_title;
}

bool 
spdf::Document::isEncrypted() const
{
	return false;
}

bool
spdf::Document::isLoaded () const
{
	return m_is_loaded;
}
