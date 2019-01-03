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

#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <vector>

namespace spdf {
	
    struct _parser;
	typedef struct _parser Parser;
	
	typedef struct {
		std::string m_upass;
		std::string m_opass;
		int m_mode;
		int m_index;
		double m_scale;
	} SessionData;
	
	class Session {
		public:
		    ~Session ();
			void add (std::string &title, SessionData &data);
			void erase (std::string &title);
			bool get (std::string &title, SessionData &data);
			void open (std::string &filename);
			void save ();
			
			static Session *m_instance;
			static Session *instance ();
			
		private:
			Session ();
			std::string m_filename;
			bool m_is_opened;
			Parser *m_parser;
	};
}

#endif
