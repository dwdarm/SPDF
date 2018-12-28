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
 
#ifndef IMAGE_H
#define IMAGE_H

#include <cstring>
#include <cstdlib>

namespace spdf {
	
	enum ImageColorType {
		COLOR_TYPE_RGBA32 = 0,
		COLOR_TYPE_BGRA32,
		COLOR_TYPE_RGB24,
		COLOR_TYPE_BGR24
	};
	
	class Image {
		friend class PDFPage;
		
		public:
			Image ();
			Image (int width, int height, int bpp);
			~Image ();
			void convertToARGB32 ();
			int getWidth () const;
			int getHeight () const;
			double getScale () const;
			int getStride () const;
			const unsigned char *getData () const;
			unsigned char *getData ();
			void setData (unsigned char *data, int width, int height, int bpp, size_t size);
			void setDataCopy (const unsigned char *data, int width, int height, int bpp, size_t size);
			
		private:
			unsigned char *m_data;
			int m_height;
			int m_width;
			int m_stride;
			double m_scale;
	};
}

#endif
