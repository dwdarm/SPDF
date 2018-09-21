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

#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <string>
#include <map>
#include <vector>

namespace spdf {
	class Bookmark {
		public:
			void add (std::string &title, int index);
			void erase (std::string &title, int index);
			bool find (std::string &title, int index);
			std::vector<int> get (std::string &title);
			void open (std::string &filename);
			
			static Bookmark *m_instance;
			static Bookmark *instance ();
			
		private:
			Bookmark () : m_is_opened (false) { };
			std::map<std::string, std::vector<int>> m_marks;
			std::string m_filename;
			bool m_is_opened;
	};
}

#endif
