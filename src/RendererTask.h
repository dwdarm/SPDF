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

#ifndef RENDERERTASK_H
#define RENDERERTASK_H

#ifndef TASK_H
#include "Task.h"
#endif

#ifndef DOCUMENT_H
#include "Document.h"
#endif

#include <string>

namespace spdf {
	
	enum RendererTaskStatus {
		RENDER_ERROR = 0,
		RENDER_OK
	};
	
	class RendererTask : public spdf::Task {
		public:
			RendererTask (spdf::Document *document, int index, double scale, std::string &key, void *user_data = NULL);
			int onStart ();
		
		private:
			Document *m_document;
			int m_index;
			double m_scale;
			std::string m_key;
			void *m_user_data;
	};
}

#endif
