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

#ifndef LOADERTASK_H
#define LOADERTASK_H

#ifndef TASK_H
#include "Task.h"
#endif

#ifndef DOCUMENT_H
#include "Document.h"
#endif

#include <string>

namespace spdf {
	
	enum LoaderTaskStatus {
		LOAD_ERROR = 0,
		LOAD_OK
	};
	
	class LoaderTask : public spdf::Task {
		public:
			LoaderTask (spdf::UString &path, spdf::UString &upass,  spdf::UString &opass, std::string &key);
			int onStart ();
		
		private:
			spdf::UString m_path;
			spdf::UString m_upass;
			spdf::UString m_opass;
			std::string m_key;
			void *m_user_data;
	};
}

#endif
