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

#include "Configuration.h"
#include <json-glib/json-glib.h>
#include <iostream>

struct spdf::_parser {
	JsonNode *root;
	JsonParser *psr;
};

spdf::Configuration *spdf::Configuration::m_instance = NULL;

spdf::Configuration *
spdf::Configuration::instance ()
{
	if (!m_instance) {
		m_instance = new Configuration ();
	}
	
	return m_instance;
}

spdf::Configuration::Configuration () : m_is_opened (false) 
{ 
	m_parser = new Parser; 
	m_parser->psr = json_parser_new ();
};

spdf::Configuration::~Configuration ()
{
	if (m_parser) {
		if (m_parser->psr) {
			g_object_unref (m_parser->psr);
		}
		delete (m_parser);
	}
}

void
spdf::Configuration::add (ConfigData &data)
{
	JsonObject *obj = NULL;
	
	if (!m_is_opened) {
		return;
	}
	
	obj = json_node_get_object (m_parser->root);
	if (!obj) {
		return;
	}
	
	json_object_set_int_member (obj, "cache_num", data.m_cache);
	json_object_set_int_member (obj, "thread_num", data.m_thread);
	json_object_set_int_member (obj, "scroll_speed", data.m_scroll);
	save ();
}

bool
spdf::Configuration::get (ConfigData &data)
{
	JsonObject *obj = NULL;
	
	if (!m_is_opened) {
		return false;
	}
	
	obj = json_node_get_object (m_parser->root);
	if (!obj) {
		return false;
	}

	data.m_cache = ((json_object_has_member (obj, "cache_num") == true) ? 
		json_object_get_int_member (obj, "cache_num") : 0);
	data.m_thread = ((json_object_has_member (obj, "thread_num") == true) ? 
		json_object_get_int_member (obj, "thread_num") : 0);
	data.m_scroll = ((json_object_has_member (obj, "scroll_speed") == true) ? 
		json_object_get_int_member (obj, "scroll_speed") : 0);
		
	return true;
}

void
spdf::Configuration::open (std::string &filename)
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
spdf::Configuration::save ()
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

