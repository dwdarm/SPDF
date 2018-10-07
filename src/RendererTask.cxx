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

#ifndef __CACHE_H__
#include "Cache.h"
#endif

#include <ctime>
#include <iostream>

#include "RendererTask.h"
#include "Message.h"
#include "MessageQueue.h"

spdf::RendererTask::RendererTask (spdf::Document *document, int index, 
										 double scale, std::string &key)
{
	m_document = document;
	m_index = index;
	m_scale = scale;
	m_key = key;
}

int 
spdf::RendererTask::onStart ()
{
	spdf::DocumentPage *page = NULL;
	spdf::Image *image = NULL;
	spdf::Message msg;
	std::string text;
	clock_t t;
	float s;
	
	spdf::Cache::instance ()->lock ();
	image = (spdf::Image*) spdf::Cache::instance ()->get (m_key);
	if (image) {
		spdf::Cache::instance ()->unlock ();
		return DESTROY_ON_RETURN;
	}
	spdf::Cache::instance ()->unlock ();
	
	msg.setId (m_document->getId ());
	
	t = clock ();
	page =  m_document->createPage (m_index);
	if (!page) {
		text = "error on creating page.";
		msg.setText (text);
		msg.setStatus (RENDER_ERROR);
		return DESTROY_ON_RETURN;
	}
	
	image = page->render (m_scale);
	if (!image) {
		text = "error on creating image.";
		msg.setText (text);
		msg.setStatus (RENDER_ERROR);
		delete (page);
		return DESTROY_ON_RETURN;
	}
	
	t = clock () - t;
	s = ((float)t)/CLOCKS_PER_SEC;
	std::cout << m_document->getId () << "-" << m_document->getTitle ().data () << " = ";
	std::cout << "rendered in " << s << " seconds\n";
	
	image->convertToARGB32 ();
	
	spdf::Cache::instance ()->lock ();
	spdf::Cache::instance ()->add (m_key, (void*)image);
	spdf::Cache::instance ()->unlock ();
	
	msg.setStatus (RENDER_OK);
	msg.setText (m_key);
	spdf::MessageQueue::instance ()->push (msg);
	
	delete (page);
	
	return DESTROY_ON_RETURN;
}
