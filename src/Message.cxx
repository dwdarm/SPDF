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

#include "Message.h"

spdf::Message::Message () 
{
	m_id = 0;
}

int 
spdf::Message::getId () const
{
	return m_id;
}

int 
spdf::Message::getStatus () const
{
	return m_status;
}

std::string &
spdf::Message::getText () 
{
	return m_text;
}

void
spdf::Message::setId (int id)
{
	m_id = id;
}

void 
spdf::Message::setStatus (int status)
{
	m_status = status;
}

void
spdf::Message::setText (std::string &text)
{
	m_text = text;
}
