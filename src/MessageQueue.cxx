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

#include <mutex>
#include "Message.h"
#include "MessageQueue.h"

static std::mutex m_queue_mutex;
spdf::MessageQueue *spdf::MessageQueue::m_instance = NULL;

spdf::MessageQueue *
spdf::MessageQueue::instance ()
{
	if (!m_instance) {
		m_instance = new MessageQueue ();
	}
	
	return m_instance;
}

bool
spdf::MessageQueue::isEmpty ()
{
	return m_msgs.empty ();
}

spdf::Message 
spdf::MessageQueue::pop ()
{
	m_queue_mutex.lock ();
	Message retVal = m_msgs.front ();
	m_msgs.pop ();
	m_queue_mutex.unlock ();
	
	return retVal;	
}

void
spdf::MessageQueue::push (Message &msg)
{
	m_queue_mutex.lock ();
	m_msgs.push (msg);
	m_queue_mutex.unlock ();
}


