#ifndef OUTLINEVIEW_H
#define OUTLINEVIEW_H

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/cellrenderertext.h>
#include <gtkmm/treestore.h>
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

#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treeiter.h>

namespace spdf {
	
	class OutlineColumns : public Gtk::TreeModel::ColumnRecord {
		public:
			OutlineColumns () { add (m_col_title); add (m_col_index);}

			Gtk::TreeModelColumn<Glib::ustring> m_col_title;
			Gtk::TreeModelColumn<int> m_col_index;
	};
	
	class OutlineView : public Gtk::ScrolledWindow {
		public:
			OutlineView ();
			Gtk::TreeIter append (Glib::ustring &title, int index);
			Gtk::TreeIter append (Glib::ustring &title, int index, Gtk::TreeIter &parent);
			void clear ();
			int getIndex (Gtk::TreeIter &iter) const;
			int getSize () const;
			Glib::ustring getTitle (Gtk::TreeIter &iter);
			Glib::RefPtr<Gtk::TreeSelection> &getTreeSelection ();
			Gtk::TreeIter getTreeIter ();
			Glib::RefPtr<Gtk::TreeStore> &getTreeStore ();
			Gtk::TreeView &getTreeView ();
		
		private:
			int m_size;
			OutlineColumns m_columns;
			Gtk::TreeView m_treeview;
			Glib::RefPtr<Gtk::TreeStore> m_ref_treestore;
			Glib::RefPtr<Gtk::TreeSelection> m_selection;
	};
}

#endif
