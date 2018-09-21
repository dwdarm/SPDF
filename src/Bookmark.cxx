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

#include "Bookmark.h"
#include "ConfigParser.h"
#include <iostream>

spdf::Bookmark *spdf::Bookmark::m_instance = NULL;

spdf::Bookmark *
spdf::Bookmark::instance ()
{
	if (!m_instance) {
		m_instance = new Bookmark ();
	}
	
	return m_instance;
}

void
spdf::Bookmark::add (std::string &title, int index)
{
	std::map<std::string, std::string> marks;
	ConfigParser parser;
	
	if (!m_is_opened) {
		return;
	}
	
	m_marks[title].push_back (index);
	for (auto itx = m_marks.begin (); itx != m_marks.end (); itx++) {
		for (auto ity = m_marks[(*itx).first].begin (); ity != m_marks[(*itx).first].end (); ity++) {
			marks[(*itx).first] += std::to_string((*ity) + 1);
			marks[(*itx).first] += ',';
		}
		marks[(*itx).first].pop_back ();
	}
	
	parser.save (m_filename, marks);
}

void
spdf::Bookmark::erase (std::string &title, int index)
{
	std::map<std::string, std::string> marks;
	ConfigParser parser;
	
	if (!m_is_opened) {
		return;
	}
	
	if (m_marks.find (title) == m_marks.end ()) {
		return;
	}
	
	for (auto it = m_marks[title].begin (); it != m_marks[title].end (); it++) {
		if (*it == index) {
			m_marks[title].erase (it);
			break;
		}
	}
	
	if (!m_marks[title].size ()) {
		m_marks.erase (title);
	}
	
	for (auto itx = m_marks.begin (); itx != m_marks.end (); itx++) {
		for (auto ity = m_marks[(*itx).first].begin (); ity != m_marks[(*itx).first].end (); ity++) {
			marks[(*itx).first] += std::to_string((*ity) + 1);
			marks[(*itx).first] += ',';
		}
		marks[(*itx).first].pop_back ();
	}
	
	parser.save (m_filename, marks);
}

bool 
spdf::Bookmark::find (std::string &title, int index)
{
	bool ret = false;
	
	if (!m_is_opened) {
		return ret;
	}
	
	if (m_marks.find (title) == m_marks.end ()) {
		return ret;
	}
	
	for (auto it = m_marks[title].begin (); it != m_marks[title].end (); it++) {
		if (*it == index) {
			ret = true;
			break;
		}
	}
	
	return ret;
}

std::vector<int> 
spdf::Bookmark::get (std::string &title)
{
	std::vector<int> ret;
	
	if (!m_is_opened) {
		return ret;
	}
	
	if (m_marks.find (title) == m_marks.end ()) {
		return ret;
	}
	
	ret = m_marks[title];
	
	return ret;
}

void
spdf::Bookmark::open (std::string &filename)
{
	ConfigParser parser;
	std::map<std::string, std::string> marks;
	
	m_is_opened = true;
	m_filename = filename;
	marks = parser.parse (filename);
	if (!marks.size ()) {
		parser.save (filename, marks);
		return;
	} 
	
	for (auto it = marks.begin (); it != marks.end (); it++) {
		std::string &sindex = (*it).second;
		int start = 0, end = 0;
		while ((end = sindex.find_first_of (',', start)) != -1) {
			m_marks[(*it).first].push_back (std::stoi (std::string 
									   (sindex, start, end-start)) - 1);
			start = end + 1;
		}
		m_marks[(*it).first].push_back (std::stoi (std::string 
										   (sindex, start, end-start)));
	}
}

