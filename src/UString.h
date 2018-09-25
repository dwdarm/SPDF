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
			UString ();
			UString (const char *str);
			UString (const char *str, int size);
			UString (unsigned int *data, int size);
			UString (const UString &obj);
			
			~UString ();
			
			UString& operator=(const UString &str);
			UString& operator=(const char *str);
			
			int byte_size () const;
			char *data () const;
			int size () const;
			
		private:
			char *m_data;
			int m_size;
			int m_bsize;
	};
}

#endif
