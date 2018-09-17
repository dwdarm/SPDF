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
 
#ifndef DOCUMENTOUTLINE_H
#define DOCUMENTOUTLINE_H

#include <string>
#include <vector>

namespace spdf {
	
	class DocumentOutlineItem {
		public:
			DocumentOutlineItem (std::string &&title, int index, 
							  std::vector<DocumentOutlineItem> &&child);
			std::vector<DocumentOutlineItem> &getChild ();
			int getIndex ();
			std::string &getTitle ();
			
		private:
			std::string m_title;
			int m_index;
			std::vector<DocumentOutlineItem> m_child;
	};
	
	class DocumentOutline {
		public:
			std::vector<DocumentOutlineItem> &getRoot ();
			void setRoot (std::vector<DocumentOutlineItem> &root);
			
		private:
			std::vector<DocumentOutlineItem> m_root;
	};
}

#endif
