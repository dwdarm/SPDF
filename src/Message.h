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
 
#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

namespace spdf {
	class Message {
		public:
			Message ();
			int getId () const;
			int getStatus () const;
			std::string &getText ();
			void setId (int id);
			void setStatus (int status);
			void setText (std::string &text);
			
		protected:
			int m_id;
			int m_status;
			std::string m_text;
	};
}

#endif
