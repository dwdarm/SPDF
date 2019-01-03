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

#include "Session.h"
#include <json-glib/json-glib.h>
#include <iostream>

struct spdf::_parser {
	JsonNode *root;
	JsonParser *psr;
};

spdf::Session *spdf::Session::m_instance = NULL;

spdf::Session *
spdf::Session::instance ()
{
	if (!m_instance) {
		m_instance = new Session ();
	}
	
	return m_instance;
}

spdf::Session::Session () : m_is_opened (false) 
{ 
	m_parser = new Parser; 
	m_parser->psr = json_parser_new ();
};

spdf::Session::~Session ()
{
	if (m_parser) {
		if (m_parser->psr) {
			g_object_unref (m_parser->psr);
		}
		delete (m_parser);
	}
}

void
spdf::Session::add (std::string &title, SessionData &data)
{
	JsonObject *obj = NULL;
	JsonObject *dat = NULL;
	
	if (!m_is_opened) {
		return;
	}
	
	obj = json_node_get_object (m_parser->root);
	if (!obj) {
		return;
	}
	
	if (json_object_has_member (obj, title.data ())) {
		dat = json_object_get_object_member (obj, title.data ());
	} else {
		dat = json_object_new ();
		json_object_set_object_member (obj, title.data (), dat);
	}
	
	json_object_set_string_member 
		(dat, "user_pass", (data.m_upass.length () > 0) ? data.m_upass.data () : "");
	json_object_set_string_member 
		(dat, "owner_pass", (data.m_opass.length () > 0) ? data.m_opass.data () : "");
	json_object_set_int_member (dat, "page_mode", data.m_mode);
	json_object_set_int_member (dat, "page_index", data.m_index);
	json_object_set_double_member (dat, "page_scale", data.m_scale);
	save ();
}

void
spdf::Session::erase (std::string &title)
{
	JsonObject *obj = NULL;
	
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
	
	json_object_remove_member (obj, title.data ());
	save ();
}

bool
spdf::Session::get (std::string &title, SessionData &data)
{
	JsonObject *obj = NULL;
	JsonObject *dat = NULL;
	
	if (!m_is_opened) {
		return false;
	}
	
	obj = json_node_get_object (m_parser->root);
	if (!obj) {
		return false;
	}
	
	if (!json_object_has_member (obj, title.data ())) {
		return false;
	} 
	
	dat = json_object_get_object_member (obj, title.data ());
	
	data.m_opass = ((json_object_has_member (dat, "owner_pass") == true) ? 
		json_object_get_string_member (dat, "owner_pass") : "");
	data.m_upass = ((json_object_has_member (dat, "user_pass") == true) ? 
		json_object_get_string_member (dat, "user_pass") : "");
	data.m_mode = ((json_object_has_member (dat, "page_mode") == true) ? 
		json_object_get_int_member (dat, "page_mode") : 1);
	data.m_index = ((json_object_has_member (dat, "page_index") == true) ? 
		json_object_get_int_member (dat, "page_index") : 0);
	data.m_scale = ((json_object_has_member (dat, "page_scale") == true) ? 
		json_object_get_double_member (dat, "page_scale") : 1.0);
		
	return true;
}

void
spdf::Session::open (std::string &filename)
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
spdf::Session::save ()
{
	JsonGenerator *gen = NULL;
	GError *err = NULL;
	
	if (!m_is_opened) {
		return;
	}
	
	gen = json_generator_new ();
	json_generator_set_root (gen, m_parser->root);
	
	if (!json_generator_to_file (gen, m_filename.data (), &err)) {
		std::cout << "ERROR" << "\n";
		if (err) {
			std::cout << err->message << "\n";
			g_error_free (err);
		}
	}
		
	g_object_unref (gen);
}

