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
 
#include "TaskScheduler.h"
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#else 
#include <unistd.h>
#endif

static std::mutex m_queue_mutex;
static void set_idle (int ms);
static void scheduler (spdf::RefThreadData pdata);
spdf::TaskScheduler *spdf::TaskScheduler::m_instance = NULL;

spdf::TaskScheduler *
spdf::TaskScheduler::instance ()
{
	if (!m_instance) {
		m_instance = new TaskScheduler ();
	}
	return m_instance;
}

spdf::TaskScheduler::TaskScheduler ()
{
	m_init = 0;
	m_queue = RefTaskQueue (new TaskQueue ());
}

int 
spdf::TaskScheduler::assign (spdf::Task *task)
{
	spdf::Task *temp = NULL;
	int ret = 0;
	
	if (!m_init) {
		return ret;
	}
	
	m_queue_mutex.lock ();
	while ((m_queue->size ()+1) > m_size) {
		temp = m_queue->top ();
		m_queue->pop ();
		delete (temp);
	}
	m_queue->push (task);
	m_queue_mutex.unlock ();
	
	ret = 1;
	return ret;
}

void 
spdf::TaskScheduler::clear ()
{
	m_queue_mutex.lock ();
	
	while (!m_queue->empty ()) {
		m_queue->pop ();
	}
	
	m_queue_mutex.unlock ();
	
}

int
spdf::TaskScheduler::decrementThreadWorker ()
{
	std::vector<RefThreadData>::iterator it;
	int ret = 0;
	
	if (!m_init) {
		return ret;
	}
	
	for (it = m_thread_data.begin (); it != m_thread_data.end (); it++) {
		(*it)->_run = 0;
		m_thread_data.erase (it);
		ret = 1;
		break;
	}
	
	return ret;
}

int
spdf::TaskScheduler::incrementThreadWorker ()
{
	int ret = 0;
	
	if (!m_init) {
		return ret;
	}
	
	RefThreadData thread_data (new ThreadData);
	thread_data->_queue = m_queue;
	thread_data->_run = 1;
	thread_data->_thread = std::thread (scheduler, thread_data);	
	m_thread_data.push_back (thread_data);				
	ret = 1;
	
	return ret;
}

int
spdf::TaskScheduler::init (int numberOfThread)
{
	int ret = 0;
	
	if (m_init) {
		return ret;
	}
	
	m_init = 1;
	m_size = 4;
	
	for (int i = 0; i < numberOfThread; i++) {
		RefThreadData thread_data (new ThreadData);
		thread_data->_queue = m_queue;
		thread_data->_run = 1;
		thread_data->_thread = std::thread (scheduler, thread_data);	
		m_thread_data.push_back (thread_data);
	}
	
	ret = 1;
	return ret;
}

void
spdf::TaskScheduler::size (int s)
{	
	spdf::Task *task = NULL;
	int t = 0;
	
	m_queue_mutex.lock ();
	m_size = s;
	t = m_queue->size ();
	while (t > s) {
		task = m_queue->top ();
		m_queue->pop ();
		delete (task);
		t = m_queue->size ();
	}
	m_queue_mutex.unlock ();
}

static void 
set_idle (int ms) 
{
	#if defined(_WIN32)
	Sleep (ms);
	#else 
	usleep (ms*1000);
	#endif
}

static void 
scheduler (spdf::RefThreadData pdata)
{
	spdf::Task *task = NULL;
	
	while (pdata->_run) {
		m_queue_mutex.lock ();
		if (!pdata->_queue->empty ()) {
			task = pdata->_queue->top ();
			pdata->_queue->pop ();
			m_queue_mutex.unlock ();
			if (task->exec ()) {
				delete (task);
			}
		} else {
			m_queue_mutex.unlock ();
			set_idle (10);
		}
	}
}
