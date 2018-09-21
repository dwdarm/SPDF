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
 
#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#ifndef TASK_H
#include "Task.h"
#endif

#include <thread>
#include <mutex>
#include <stack>
#include <vector>
#include <memory>

namespace spdf {
	
	typedef std::stack<Task*> TaskQueue;
	typedef std::shared_ptr<TaskQueue> RefTaskQueue;
	
	struct _thread_data {
		std::thread _thread;
		RefTaskQueue _queue;
		int _run;
	};
	
	typedef struct _thread_data ThreadData;
	typedef std::shared_ptr<ThreadData> RefThreadData;
	
	class TaskScheduler {
		public:
			int assign (Task *task);
			void clear ();
			int decrementThreadWorker ();
			int incrementThreadWorker ();
			int init (int numberOfThread);
			
			static TaskScheduler *instance ();
			static TaskScheduler *m_instance;
			
		private:
			TaskScheduler ();
			int m_init;
			std::vector<RefThreadData> m_thread_data;
			RefTaskQueue m_queue;
	};
}

#endif
