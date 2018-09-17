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
 
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <queue>

class Message;

namespace spdf {
	class MessageQueue {
		public:
			bool isEmpty ();
			Message pop ();
			void push (Message &msg);
			
			static MessageQueue *m_instance;
			static MessageQueue *instance ();
			
		private:
			MessageQueue () { }
			std::queue<Message> m_msgs;
	};
}

#endif
