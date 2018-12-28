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


#include "Image.h"
#include <iostream>

spdf::Image::Image ()
{
	m_height = 0;
	m_width = 0;
	m_stride = 0;
	m_data = NULL;
}

spdf::Image::Image (int width, int height, int bpp)
{
	m_width = width;
	m_height = height;
	m_stride = width * bpp;
	m_data = new unsigned char[m_stride*height];
}

//destructor
spdf::Image::~Image ()
{
	if (m_data) {
		delete[] m_data;
	}
}

void
spdf::Image::convertToARGB32 ()
{
	char temp;
	for (int i=0; i<(m_stride*m_height); i+=4) {
		temp = m_data[i];
		m_data[i] = m_data[i+2];
		m_data[i+2] = temp;
	}
}

int
spdf::Image::getWidth () const
{
	return m_width;	
}

int
spdf::Image::getHeight () const
{
	return m_height;	
}

double
spdf::Image::getScale () const
{
	return m_scale;
}

int
spdf::Image::getStride () const
{
	return m_stride;	
}

const unsigned char *
spdf::Image::getData () const
{
	return m_data;
}

unsigned char *
spdf::Image::getData ()
{
	return m_data;
}

void 
spdf::Image::setData (unsigned char *data, int width, int height, int bpp, size_t size)
{	
	if (data != NULL) {
		m_data = data;
		m_height = height;
		m_width = width;
		m_stride = width * bpp;
	}
}

void 
spdf::Image::setDataCopy (const unsigned char *data, int width, int height, int bpp, size_t size)
{
	if (data != NULL) {
		m_data = new unsigned char[size];
		std::memcpy (m_data, data, size);
		
		m_height = height;
		m_width = width;
		m_stride = width * bpp;
	}
}

