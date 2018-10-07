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

#include "OutlineView.h"
#include <pangomm/layout.h>

spdf::OutlineView::OutlineView () : Gtk::ScrolledWindow ()
{
	Gtk::CellRendererText *m_text_renderer = NULL;
	
	m_treeview.set_headers_visible (false);
	m_treeview.set_enable_search (false);
	m_treeview.append_column ("Title", m_columns.m_col_title);
	
	m_ref_treestore = Gtk::TreeStore::create (m_columns);
	m_treeview.set_model (m_ref_treestore);
	
	m_selection = m_treeview.get_selection ();
	
	m_text_renderer = (Gtk::CellRendererText*)
								m_treeview.get_column_cell_renderer (0);
	m_text_renderer->property_ellipsize ().set_value (Pango::ELLIPSIZE_END);
	
	m_size = 0;
	
	m_event_box.add (m_treeview);
	
	add (m_event_box);
}

Gtk::TreeIter
spdf::OutlineView::append (Glib::ustring &title, int index)
{
	Gtk::TreeModel::Row row = *(m_ref_treestore->append ());
	
	row[m_columns.m_col_title] = title;
	row[m_columns.m_col_index] = index;
	
	m_size++;
	
	return row;
}

Gtk::TreeIter
spdf::OutlineView::append (Glib::ustring &title, int index, Gtk::TreeIter &parent)
{
	Gtk::TreeModel::Row parent_row = *(parent);
	Gtk::TreeNodeChildren child_node = parent_row.children ();
	Gtk::TreeModel::Row child_row = *(m_ref_treestore->append (child_node));
	
	child_row[m_columns.m_col_title] = title;
	child_row[m_columns.m_col_index] = index;
	
	m_size++;
	
	return child_row;
}

void
spdf::OutlineView::clear ()
{
	m_ref_treestore->clear ();
	m_size = 0;
}

Gtk::EventBox &
spdf::OutlineView::getEventBox ()
{
	return m_event_box;
}

int 
spdf::OutlineView::getIndex (Gtk::TreeIter &iter) const
{
	Gtk::TreeModel::Row row = *(iter);
	
	return row[m_columns.m_col_index];
}

int 
spdf::OutlineView::getSize () const
{
	return m_size;
}

Glib::ustring 
spdf::OutlineView::getTitle (Gtk::TreeIter &iter)
{
	Gtk::TreeModel::Row row = *(iter);
	
	return row[m_columns.m_col_title];
}

Glib::RefPtr<Gtk::TreeSelection> &
spdf::OutlineView::getTreeSelection ()
{
	return m_selection;
}

Gtk::TreeIter 
spdf::OutlineView::getTreeIter ()
{
	Gtk::TreeIter iter = m_ref_treestore->children ();
	return iter;
}

Glib::RefPtr<Gtk::TreeStore> &
spdf::OutlineView::getTreeStore ()
{
	return m_ref_treestore;
}

Gtk::TreeView &
spdf::OutlineView::getTreeView ()
{
	return m_treeview;
}
