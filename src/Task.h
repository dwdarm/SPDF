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
 
#ifndef TASK_H
#define TASK_H

#define NODESTROY_ON_RETURN 0
#define DESTROY_ON_RETURN 1

namespace spdf {
	
	class Task {
		public:
			friend class TaskScheduler;
			virtual ~Task () { }
			int exec ();
			virtual int onStart ();
		
		private:
			int m_destroy;
	};
}

#endif
