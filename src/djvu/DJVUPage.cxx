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

#include "DJVUPage.h"
#include <cstring>

spdf::DJVUPage::DJVUPage () : spdf::DocumentPage ()
{
	m_page_handler = NULL;
	m_dpi = 0.0;
}

spdf::DJVUPage::~DJVUPage ()
{
	ddjvu_page_release (m_page_handler);
}

spdf::Image *
spdf::DJVUPage::render (double scale)
{
	spdf::Image *retVal = NULL;
	ddjvu_format_t *format = NULL;
	ddjvu_rect_t pagerect;
	ddjvu_rect_t rendrect;
	double width = 0.0;
	double height = 0.0;
	int res = 1;
	unsigned int mask[] = {0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000};
	
	if (m_dpi == 0) {
		return retVal;
	}
	
	width = ((scale*72.0)/m_dpi) * m_width;
	height = ((scale*72.0)/m_dpi) * m_height;
	
	format = ddjvu_format_create(DDJVU_FORMAT_RGBMASK32, 4, mask);
	ddjvu_format_set_row_order (format, 1);
	ddjvu_format_set_y_direction (format, 1);
	
	pagerect.x = 0;
	pagerect.y = 0;
	pagerect.w = (int)width;
	pagerect.h = (int)height;
	
	rendrect.x = pagerect.x;
	rendrect.y = pagerect.y;
	rendrect.w = pagerect.w;
	rendrect.h = pagerect.h;
	
	retVal = new spdf::Image ((int)width, (int)height, 4);
	res = ddjvu_page_render(m_page_handler, DDJVU_RENDER_COLOR, &pagerect, 
								&rendrect, format, retVal->getStride (),
											(char*) retVal->getData ());	
											 					 
	if (res == 0) {
		delete (retVal);
		retVal = NULL;
	} 
								 
	return retVal;
}
