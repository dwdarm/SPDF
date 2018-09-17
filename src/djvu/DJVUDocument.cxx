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

#include "DJVUDocument.h"
#include <iostream>

ddjvu_context_t *spdf::DJVUDocument::m_context = NULL;
int spdf::DJVUDocument::m_count = 0;

static bool
wait_for_message_tag (ddjvu_context_t *ctx, ddjvu_message_tag_t tag)
{
	ddjvu_message_t *msg = NULL;
	bool ret = false;
	
	while (true) {
		msg = ddjvu_message_peek(spdf::DJVUDocument::m_context);
		
		if (!msg) {
			continue;
		}
		
		if (msg->m_any.tag == tag) {
			ret = true;
			ddjvu_message_pop(spdf::DJVUDocument::m_context);
			break;
		} else {
			ddjvu_message_pop(spdf::DJVUDocument::m_context);
		}
	}
	
	return ret;
}

static void 
wait_for_messages(ddjvu_context_t *ctx, bool wait)
{
	ddjvu_message_t *msg;
	
	if (wait) {
		ddjvu_message_wait(ctx);
	}
	
	while ((msg = ddjvu_message_peek(ctx))) {
		ddjvu_message_pop(ctx);
	}
}

spdf::DJVUDocument::~DJVUDocument () 
{
	ddjvu_document_release (m_document);
	m_count--;
	if (m_count == 0) {
		ddjvu_context_release (m_context);
		m_context = NULL;
	}
}

spdf::DJVUDocument *
spdf::DJVUDocument::openDocument (const std::string &filename)
{
	spdf::DJVUDocument *retVal = NULL;
	ddjvu_document_t *document = NULL;
	ddjvu_message_t *msg = NULL;
	
	m_context = ddjvu_context_create ("spdf");
	if (!m_context) {
		m_context = ddjvu_context_create ("spdf");
	}
	
	document = ddjvu_document_create_by_filename_utf8 
									   (m_context, filename.data (), 0);
									   
	if (!document) {
		return retVal;
	}
	
	while (!wait_for_message_tag (m_context, DDJVU_DOCINFO)) { }
	
	retVal = new spdf::DJVUDocument ();
	retVal->m_document = document;
	retVal->m_is_loaded = true;
	retVal->m_pages = ddjvu_document_get_pagenum (document);
	retVal->m_type = DJVU;
	retVal->m_title = std::string (filename).substr
						 (std::string (filename).find_last_of("/\\")+1);
	
	m_count++;
	
	return retVal;
}

spdf::DocumentPage *
spdf::DJVUDocument::createPage (int index)
{
	DJVUPage *retVal = NULL;
	ddjvu_page_t *page_handler = NULL;
	ddjvu_pageinfo_s pageinfo; 
	ddjvu_status_t stat;
	
	page_handler = ddjvu_page_create_by_pageno (m_document, index);
	if (!page_handler) {
		return retVal;
	}
	
	stat = ddjvu_page_decoding_status (page_handler);
	while(stat < DDJVU_JOB_OK) {
		wait_for_messages (spdf::DJVUDocument::m_context, true);
		stat = ddjvu_page_decoding_status (page_handler);
	}
	
	if (stat > DDJVU_JOB_OK) {
		ddjvu_page_release (page_handler);
		return retVal;
	}
	
	
	stat = ddjvu_document_get_pageinfo (m_document, index, &pageinfo);
	while(stat < DDJVU_JOB_OK) {
		wait_for_messages (spdf::DJVUDocument::m_context, true);
		stat = ddjvu_document_get_pageinfo (m_document, index, &pageinfo);
	}
	
	retVal = new DJVUPage ();
	retVal->m_page_handler = page_handler;
	retVal->m_index = index;
	retVal->m_height = (double) pageinfo.height;
	retVal->m_width = (double) pageinfo.width;
	retVal->m_dpi = (double) pageinfo.dpi;
	return retVal;
}
