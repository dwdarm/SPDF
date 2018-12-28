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
#include "TaskId.h"

spdf::RendererTask::RendererTask (spdf::Document *document, int index, double scale, std::string &key, void *user_data)
{
	m_document = document;
	m_index = index;
	m_scale = scale;
	m_key = key;
	m_user_data = user_data;
}

int 
spdf::RendererTask::onStart ()
{
	spdf::DocumentPage *page = NULL;
	spdf::Image *image = NULL;
	spdf::Message msg;
	std::string text;
	std::string key;
	clock_t t;
	float s;
	
	key = std::to_string (m_document->getId ()) 
		  + "-" + std::to_string (m_scale)
		  + "-" + std::to_string (m_index);
		  
	msg.setId (TASK_ID_RENDERER);
	
	spdf::Cache::instance ()->lock ();
	image = (spdf::Image*) spdf::Cache::instance ()->get (key);
	if (image) {
		msg.setStatus (RENDER_OK);
		msg.setText (m_key);
		msg.setUserData (m_user_data);
		spdf::MessageQueue::instance ()->push (msg);
		spdf::Cache::instance ()->unlock ();
		return DESTROY_ON_RETURN;
	}
	spdf::Cache::instance ()->unlock ();
	
	t = clock ();
	page =  m_document->createPage (m_index);
	if (!page) {
		text = "error on creating page.";
		msg.setText (text);
		msg.setStatus (RENDER_ERROR);
		msg.setUserData (m_user_data);
		return DESTROY_ON_RETURN;
	}
	
	image = page->render (m_scale);
	if (!image) {
		text = "error on creating image.";
		msg.setText (text);
		msg.setStatus (RENDER_ERROR);
		msg.setUserData (m_user_data);
		delete (page);
		return DESTROY_ON_RETURN;
	}
	
	t = clock () - t;
	s = ((float)t)/CLOCKS_PER_SEC;
	std::cout << m_document->getId () << "-" << m_document->getTitle ().data () << " = ";
	std::cout << "rendered in " << s << " seconds\n";
	
	//image->convertToARGB32 ();
	
	spdf::Cache::instance ()->lock ();
	spdf::Cache::instance ()->add (key, (void*)image);
	msg.setStatus (RENDER_OK);
	msg.setText (m_key);
	msg.setUserData (m_user_data);
	spdf::MessageQueue::instance ()->push (msg);
	spdf::Cache::instance ()->unlock ();
	
	delete (page);
	
	return DESTROY_ON_RETURN;
}
