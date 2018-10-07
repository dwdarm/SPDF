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

#include "UString.h"
#include <cstring>

static int
ustring_ucs4_to_utf8 (char *buff, const unsigned int *data, int len, int *c_len) 
{
	int p = 0;
	
	*c_len = 0;
	
	if ((!data) || (!buff) || (!len)) {
		return p;
	}
	
	for (int i = 0; i < len; i++) {
		if ((data[i] >= 0x0000) && (data[i] <= 0x007f)) {
			buff[p] = ((char)data[i]) & 0x7f;
			p++;
			(*c_len)++;
		} else if ((data[i] >= 0x0080) && (data[i] <= 0x07ff)) {
			buff[p] = (((char)(data[i] >> 6)) & 0x1f) | 0xc0;
			p++;
			buff[p] = (((char)data[i]) & 0x3f) | 0x80;
			p++;
			(*c_len)++;
		} else if ((data[i] >= 0x0800) && (data[i] <= 0xffff)) {
			buff[p] = (((char)(data[i] >> 12)) & 0x0f) | 0xe0;
			p++;
			buff[p] = (((char)(data[i] >> 6)) & 0x3f) | 0x80;
			p++;
			buff[p] = (((char)data[i]) & 0x3f) | 0x80;
			p++;
			(*c_len)++;
		} else if ((data[i] >= 0x10000) && (data[i] <= 0x10ffff)) {
			buff[p] = (((char)(data[i] >> 18)) & 0x07) | 0xf0;
			p++;
			buff[p] = (((char)(data[i] >> 12)) & 0x3f) | 0x80;
			p++;
			buff[p] = (((char)(data[i] >> 6)) & 0x3f) | 0x80;
			p++;
			buff[p] = (((char)data[i]) & 0x3f) | 0x80;
			p++;
			(*c_len)++;
		} 
	}
	
	return p;
}

static int
ustring_get_length_utf8 (const char *c, int len)
{
	int n = 0;
	
	while (n < len) {
		if ((c[n] & 0x80) == 0x00) {
			n++;
		} else if ((c[n] & 0xc0) == 0xc0) {
			n+=2;
		} else if ((c[n] & 0xe0) == 0xe0) {
			n+=3;
		} else if ((c[n] & 0xf0) == 0xf0) {
			n+=4;
		} else {
			n++;
		}
	}
	
	return n;	
}

spdf::UString::UString () 
{	
	m_bsize = 0;
	m_size = 0;
	m_capacity = 0;
	m_data = NULL;
}

spdf::UString::UString (const char *str)
{
	m_bsize = strlen (str);
	m_size = ustring_get_length_utf8 (str, m_bsize);
	m_capacity = m_bsize + 2;
	m_data = new char[m_capacity];
	
	strcpy (m_data, str);
	
	m_data[m_bsize] = 0x00;
	m_data[m_bsize+1] = 0x00;
}

spdf::UString::UString (const char *str, int size)
{
	m_bsize = size;
	m_size = ustring_get_length_utf8 (str, m_bsize);
	m_capacity = m_bsize + 2;
	m_data = new char[m_capacity];
	
	memcpy (m_data, str, m_bsize);
	
	m_data[m_bsize] = 0x00;
	m_data[m_bsize+1] = 0x00;
}

spdf::UString::UString (const unsigned int *data, int size)
{
	m_capacity = (size * 4) + 2;
	m_data = new char[m_capacity];
	m_bsize = ustring_ucs4_to_utf8 (m_data, data, size, &m_size);
	
	m_data[m_bsize] = 0x00;
	m_data[m_bsize+1] = 0x00;
}

spdf::UString::UString (const UString &obj)
{
	if (this != &obj) {
		
		m_size = obj.m_size;
		m_bsize = obj.m_bsize;
		m_capacity = obj.m_capacity;
	    m_data = NULL;
		
		if (obj.m_data) {
			m_data = new char[m_capacity];
			memcpy (m_data, obj.m_data, m_bsize);
			m_data[m_bsize] = 0x00;
			m_data[m_bsize+1] = 0x00;
		}
	}
}

spdf::UString::~UString ()
{
	if (m_data) {
		delete [] m_data;
	}
}

spdf::UString& 
spdf::UString::operator=(const spdf::UString &str)
{
	if (this != &str) {
		
		if (m_data) {
			delete [] m_data;
			m_data = NULL;
		}
		
		m_size = str.m_size;
		m_bsize = str.m_bsize;
		m_capacity = str.m_capacity;
		
		if (str.m_data) {
			m_data = new char[m_capacity];
			memcpy (m_data, str.m_data, m_bsize);
			m_data[m_bsize] = 0x00;
			m_data[m_bsize+1] = 0x00;
		}
	}
	
	return *this;
}

spdf::UString& 
spdf::UString::operator=(const char *str)
{
	if (str) {
		
		if (m_data) {
			delete [] m_data;
			m_data = NULL;
		}
		
		m_bsize = strlen (str);
		m_size = ustring_get_length_utf8 (str, m_bsize);
		m_capacity = m_bsize + 2;
		m_data = new char[m_capacity];
	
		strcpy (m_data, str);
	
		m_data[m_bsize] = 0x00;
		m_data[m_bsize+1] = 0x00;
	}
	
	return *this;
}

int 
spdf::UString::byte_size () const 
{
	return m_bsize;
}

char *
spdf::UString::data () const 
{
	return m_data;
}

int 
spdf::UString::size () const
{
	return m_size;
}
