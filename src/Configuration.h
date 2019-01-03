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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <vector>

namespace spdf {
	
    struct _parser;
	typedef struct _parser Parser;
	
	typedef struct {
		int m_cache;
		int m_thread;
		int m_scroll;
		int m_padding;
	} ConfigData;
	
	class Configuration {
		public:
		    ~Configuration ();
			void add (ConfigData &data);
			bool get (ConfigData &data);
			void open (std::string &filename);
			void save ();
			
			static Configuration *m_instance;
			static Configuration *instance ();
			
		private:
			Configuration ();
			std::string m_filename;
			bool m_is_opened;
			Parser *m_parser;
	};
}

#endif
