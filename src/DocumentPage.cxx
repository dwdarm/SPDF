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

#include "DocumentPage.h"

spdf::DocumentPage::DocumentPage ()
{
	m_index = -1;
	m_height = 0.0;
	m_width = 0.0;
}

spdf::DocumentPage::~DocumentPage ()
{
	
}

std::string 
spdf::DocumentPage::searchText (spdf::Rect &rect, double scale)
{
	return std::string ();
}

std::vector<spdf::Rect> 
spdf::DocumentPage::searchRect (std::string &text, double scale)
{
	return std::vector<Rect>  ();
}

std::vector<spdf::Rect> 
spdf::DocumentPage::getSelectionRegion (PageSelectionStyle style, Rect &rect, double scale)
{
	return std::vector<Rect>  ();
}

std::string 
spdf::DocumentPage::getSelectionText (PageSelectionStyle style, Rect &rect, double scale)
{
	return std::string ();
}

double
spdf::DocumentPage::getHeight () const
{
	return m_height;
}

int
spdf::DocumentPage::getIndex () const
{
	return m_index;
}

double
spdf::DocumentPage::getWidth () const
{
	return m_width;
}
