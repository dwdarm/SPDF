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

#include "Cache.h"
#include <iostream>
#include <mutex>

spdf::Cache *spdf::Cache::m_instance = NULL;
std::mutex cache_mutex;

spdf::Cache *
spdf::Cache::instance ()
{
	if (!m_instance) {
		m_instance = new Cache ();
	}
	return m_instance;
}

spdf::Cache::Cache ()
{
	m_max_size = 3;
	m_time = 1;
}

int
spdf::Cache::add (std::string &key, void *value)
{
	std::map<std::string, spdf::CacheV>::iterator it;
	spdf::CacheV cv;
	std::string key_to_delete;
	int temp = -1;
	
	if (m_cache.size () >= m_max_size) {
		for (it = m_cache.begin (); it != m_cache.end (); it++) {
			if (temp == -1) {
				temp = (it->second).time;
				key_to_delete = it->first;
			} else if ((it->second).time < temp) {
				temp = (it->second).time;
				key_to_delete = it->first;
			}
		}
		erase (key_to_delete);
	}
	cv.time = m_time;
	cv.data = value;
	m_cache[key] = cv;
	m_time = m_time + 1;
}

void
spdf::Cache::clear ()
{
	std::map<std::string, spdf::CacheV>::iterator it;
	
	for (it = m_cache.begin (); it != m_cache.end (); it++) {
		m_delete_handler ((it->second).data);
	}
	
	m_cache.clear ();
}

int
spdf::Cache::erase (std::string &key) 
{
	if (m_cache.find (key) != m_cache.end ()) {
		spdf::CacheV &cv = m_cache[key];
		m_delete_handler (cv.data);
		m_cache.erase (m_cache.find (key));
	}
}

void *
spdf::Cache::get (std::string &key) 
{
	void *retVal = NULL;
	
	if (m_cache.find (key) != m_cache.end ()) {
		spdf::CacheV &cv = m_cache[key];
		retVal = cv.data;
	}
	
	return retVal;
}

int 
spdf::Cache::lock ()
{
	int ret = 0;
	int res = 0;
	
	cache_mutex.lock ();
	
	ret = 1;
	return ret;
}

void
spdf::Cache::setDeleteHandler (deleteValue delete_handler)
{
	m_delete_handler = delete_handler;
}

void
spdf::Cache::setMaxSize (int size)
{
	m_max_size = size;
}

int 
spdf::Cache::unlock ()
{
	int ret = 0;
	int res = 0;
	
	cache_mutex.unlock ();
	
	ret = 1;
	return ret;
}
