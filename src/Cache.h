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

#ifndef CACHE_H
#define CACHE_H

#include <map>

typedef void (*deleteValue) (void*);

namespace spdf {
	
	typedef struct {
		void *data;
		int time;
	} CacheV;
	
	class Cache {
		public:
			int add (std::string &key, void *value); 
			void clear ();
			int erase (std::string &key);
			void *get (std::string &key);
			int lock ();
			void setDeleteHandler (deleteValue delete_handler);
			void setMaxSize (int size);
			int unlock ();
			
			static Cache *m_instance;
			static Cache *instance ();
			
		private:
			Cache ();
			std::map<std::string, CacheV> m_cache;
			int m_max_size;
			int m_time;
			deleteValue m_delete_handler;
	};
}

#endif
