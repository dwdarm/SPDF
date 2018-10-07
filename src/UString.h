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
 
#ifndef USTRING_H
#define USTRING_H

#include <string>

namespace spdf {
	class UString {
		public:
			// Constructor
			UString (); 
			UString (const char *str); 
			UString (const char *str, int size); 
			UString (const unsigned int *data, int size); 
			UString (const UString &obj); 
			
			// Destructor
			~UString (); 
			
			UString& operator=(const UString &str); 
			UString& operator=(const char *str); 
			
			//void append (const UString &str);
			//void append (const unsigned int *u, int s);
			//void append (unsigned int u);
			//void append (const char *c);
			//void append (char c);
			
			int byte_size () const; 
			char *data () const; 
			int size () const; 
			
		private:
			char *m_data;
			int m_size;
			int m_bsize;
			int m_capacity;
	};
}

#endif
