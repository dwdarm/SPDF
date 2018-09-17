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

#ifndef DOCUMENTCREATOR_H
#define DOCUMENTCREATOR_H

#include <string>

class Document;

namespace spdf {
	class DocumentCreator {
		public:
			Document *openDocument (const std::string &filename, 
						  const std::string &user_pass = std::string (), 
						const std::string &owner_pass = std::string ());
						
			static DocumentCreator *m_instance;
			static DocumentCreator *instance ();
			
		private:
			DocumentCreator () { }
	};
}

#endif
