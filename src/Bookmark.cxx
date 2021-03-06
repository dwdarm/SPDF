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
#include <json-glib/json-glib.h>
#include <iostream>

struct spdf::_parser {
	JsonNode *root;
	JsonParser *psr;
};

spdf::Bookmark *spdf::Bookmark::m_instance = NULL;

spdf::Bookmark *
spdf::Bookmark::instance ()
{
	if (!m_instance) {
		m_instance = new Bookmark ();
	}
	
	return m_instance;
}

spdf::Bookmark::Bookmark () : m_is_opened (false) 
{ 
	m_parser = new Parser; 
	m_parser->psr = json_parser_new ();
};

spdf::Bookmark::~Bookmark ()
{
	if (m_parser) {
		if (m_parser->psr) {
			g_object_unref (m_parser->psr);
		}
		delete (m_parser);
	}
}

void
spdf::Bookmark::add (std::string &title, int index)
{
	JsonObject *obj = NULL;
	JsonArray *in = NULL;
	
	if (!m_is_opened) {
		return;
	}
	
	obj = json_node_get_object (m_parser->root);
	if (!obj) {
		return;
	}
	
	if (!json_object_has_member (obj, title.data ())) {
		in = json_array_new ();
		json_array_add_int_element (in, index);
		json_object_set_array_member (obj, title.data (), in);
		save ();
		//json_array_unref(in);
		return;
	} 
	
	in = json_object_get_array_member (obj, title.data ());
	json_array_add_int_element (in, index);
	save ();
}

void
spdf::Bookmark::erase (std::string &title, int index)
{
	JsonObject *obj = NULL;
	JsonArray *in = NULL;
	
	if (!m_is_opened) {
		return;
	}
	
	obj = json_node_get_object (m_parser->root);
	if (!obj) {
		return;
	}
	
	if (!json_object_has_member (obj, title.data ())) {
		return;
	}
	
	in = json_object_get_array_member (obj, title.data ());
	for (int i = 0; i < json_array_get_length (in); i++) {
		if (json_array_get_int_element (in, i) == index) {
			json_array_remove_element (in, i);
			save ();
			break;
		}
	}
}

bool 
spdf::Bookmark::find (std::string &title, int index)
{
	JsonObject *obj = NULL;
	JsonArray *in = NULL;
	
	if (!m_is_opened) {
		return false;
	}
	
	obj = json_node_get_object (m_parser->root);
	if (!obj) {
		return false;
	}
	
	if (!json_object_get_size (obj)) {
		return false;
	}
	
	if (!json_object_has_member (obj, title.data ())) {
		return false;
	}
	
	in = json_object_get_array_member (obj, title.data ());
	for (int i = 0; i < json_array_get_length (in); i++) {
		if (json_array_get_int_element (in, i) == index) {
			return true;
		}
	}
	
	return false;
}

bool 
spdf::Bookmark::find (std::string &title)
{
	if (!m_is_opened) {
		return false;
	}
	
	JsonObject *obj = json_node_get_object (m_parser->root);
	
	if (!obj) {
		return false;
	}
	
	if (!json_object_get_size (obj)) {
		return false;
	}
	
	if (!json_object_has_member (obj, title.data ())) {
		return false;
	}
	
	return true;
}

std::vector<int> 
spdf::Bookmark::get (std::string &title)
{
	std::vector<int> ret;
	JsonArray *in = NULL;
	JsonObject *obj = NULL;
	
	if (!m_is_opened) {
		return ret;
	}
	
	obj = json_node_get_object (m_parser->root);
	
	if (!obj) {
		return ret;
	}
	
	if (!json_object_get_size (obj)) {
		return ret;
	}
	
	if (!json_object_has_member (obj, title.data ())) {
		return ret;
	}
	
	in = json_object_get_array_member (obj, title.data ());
	for (int i = 0; i < json_array_get_length (in); i++) {
		ret.push_back (json_array_get_int_element (in, i));
	}
	
	return ret;
}

void
spdf::Bookmark::open (std::string &filename)
{
	GError *err = NULL;
	m_filename = filename;
	m_is_opened = true;
	
	if (!json_parser_load_from_file (m_parser->psr, m_filename.data (), &err)) {
		if (err) {
			std::cout << err->message << "\n";
			g_error_free (err);
		}
	}
	
	m_parser->root = json_parser_get_root (m_parser->psr);
	if (!m_parser->root) {
		m_parser->root = json_node_new (JSON_NODE_OBJECT);
		json_node_set_object (m_parser->root, json_object_new ());
	}
}

void
spdf::Bookmark::save ()
{
	JsonGenerator *gen = NULL;
	GError *err = NULL;
	
	if (!m_is_opened) {
		return;
	}
	
	gen = json_generator_new ();
	json_generator_set_root (gen, m_parser->root);
	
	if (!json_generator_to_file (gen, m_filename.data (), &err)) {
		if (err) {
			std::cout << err->message << "\n";
			g_error_free (err);
		}
	}
		
	g_object_unref (gen);
}

