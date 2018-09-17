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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "Database.h"
#include <cstdio>
#include <cstdlib>

static int retrieve_cb
(void *user_data,int n_col,char **val, char **row)
{
	spdf::BookmarkItem item;
	std::vector<spdf::BookmarkItem> *data = (std::vector<spdf::BookmarkItem>*) user_data;
	
	item.name = std::string (val[1]);
	item.index = atoi (val[0]);
	data->push_back (item);
	
	return 0;
}

static int check_cb
(void *user_data,int n_col,char **val, char **row)
{
	int *data = (int*) user_data;
	*data = *data + 1;
	
	return 0;
}

spdf::Database::Database ()
{
	m_handle = NULL;
}

int 
spdf::Database::check (std::string name, int index)
{
	char query[512];
	int retVal = 0;
	char *err = NULL;
	
	sprintf (query, "SELECT * FROM bookmark WHERE name='%s' AND page=%d;", name.data (), index);
	sqlite3_exec (m_handle, query, check_cb, &retVal, &err);
	if (err) {
		printf ("%s\n", err);
		free (err);
	}
	
	return retVal;
}

int
spdf::Database::close ()
{
	return sqlite3_close (m_handle);
}

int 
spdf::Database::insert (std::string name, int index)
{
	char query[512];
	int ret = 0;
	char *err = NULL;
	
	sprintf (query, "INSERT INTO bookmark (name, page) VALUES ('%s', %d);", name.data (), index);
	ret = sqlite3_exec (m_handle, query, NULL, NULL, &err);
	if (err) {
		printf ("%s\n", err);
		free (err);
	}
	
	return ret;
}

int 
spdf::Database::remove (std::string name, int index)
{
	char query[512];
	int ret = 0;
	char *err = NULL;
	
	sprintf (query, "DELETE FROM bookmark WHERE name='%s' AND page=%d;", name.data (), index);
	ret = sqlite3_exec (m_handle, query, NULL, NULL, &err);
	if (err) {
		printf ("%s\n", err);
		free (err);
	}
	
	return ret;
}

std::vector<spdf::BookmarkItem>
spdf::Database::retrieve (std::string name) const
{
	std::vector<spdf::BookmarkItem> retVal;
	char query[512];
	char *err = NULL;
	
	sprintf (query, "SELECT * FROM bookmark WHERE name='%s';", name.data ());
	sqlite3_exec (m_handle, query, retrieve_cb, &retVal, &err);
	if (err) {
		printf ("%s\n", err);
		free (err);
	}
	
	return retVal;
}

int
spdf::Database::open (std::string filename)
{
	int ret = -1;
	char query[512];
	char *err = NULL;
	
	ret =  sqlite3_open (filename.data (), &m_handle);
	if (ret == SQLITE_OK) {
		sprintf (query, "CREATE TABLE bookmark(page INT, name VARCHAR(255));");
		sqlite3_exec (m_handle, query, NULL, NULL, &err);
		if (err) {
			printf ("%s\n", err);
			free (err);
		}
	}
	
	return ret;
}
