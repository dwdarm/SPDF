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

#include "LoaderTask.h"
#include "Message.h"
#include "MessageQueue.h"
#include "TaskId.h"
#include "DocumentCreator.h"
#include <iostream>

spdf::LoaderTask::LoaderTask (spdf::UString &path, spdf::UString &upass,  spdf::UString &opass, std::string &key)
{
	m_path = path;
	m_upass = upass;
	m_opass = opass;
	m_key = key;
}

int
spdf::LoaderTask::onStart ()
{
	spdf::Message msg;
	spdf::Document *doc = NULL;
	spdf::DocumentError *err;
	
	
	std::cout << "lets do it!!!\n";
	msg.setId (TASK_ID_LOADER);
	msg.setText (m_key);
	
	err = new spdf::DocumentError;
	doc = spdf::DocumentCreator::instance ()->openDocument (m_path, m_upass, m_opass, err);
			   
	if (!doc) {
		msg.setStatus (LOAD_ERROR);
		msg.setUserData ((void*)err);
	} else {
		msg.setStatus (LOAD_OK);
		msg.setUserData ((void*)doc);
		delete (err);
	}
	
	spdf::MessageQueue::instance ()->push (msg);
	
	return DESTROY_ON_RETURN;
}
