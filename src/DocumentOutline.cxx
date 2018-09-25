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

#include "DocumentOutline.h"

// DocumentOutline class definition

std::vector<spdf::DocumentOutlineItem> &
spdf::DocumentOutline::getRoot ()
{
	return m_root;
}

void 
spdf::DocumentOutline::setRoot (std::vector<spdf::DocumentOutlineItem> &root)
{
	m_root = root;
}


// DocumentOutlineItem class definition

spdf::DocumentOutlineItem::DocumentOutlineItem (spdf::UString &title, 
			  int index, std::vector<spdf::DocumentOutlineItem> &child)
{
	m_title = title;
	m_index = index;
	m_child = child;
}

std::vector<spdf::DocumentOutlineItem> &
spdf::DocumentOutlineItem::getChild ()
{
	return m_child;
}

int
spdf::DocumentOutlineItem::getIndex ()
{
	return m_index;
}

spdf::UString &
spdf::DocumentOutlineItem::getTitle ()
{
	return m_title;
}
