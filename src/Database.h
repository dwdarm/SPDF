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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <sqlite3.h>
#include <string>
#include <vector>

namespace spdf {
	
	typedef struct {
		std::string name;
		int index;
	} BookmarkItem;
	
	class Database {
		public:
			Database ();
			int check (std::string name, int index);
			int close ();
			int insert (std::string name, int index);
			int remove (std::string name, int index);
			std::vector<BookmarkItem> retrieve (std::string name) const;
		    int open (std::string filename);
			
		private:
			sqlite3 *m_handle;
	};
}
