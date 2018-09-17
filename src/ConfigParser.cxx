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

#include "ConfigParser.h"
#include <cstdio>

std::map<std::string, std::string> 
spdf::ConfigParser::parse (const std::string &filename)
{
	char *buff = NULL;
	int len = 0;
	FILE *file;
	std::string key;
	std::string val;
	std::map<std::string, std::string> retVal;
	
	
	file = fopen (filename.data (), "r");
	if (!file) {
		return retVal;
	}
	
	fseek (file, 0, SEEK_END);
    len = ftell (file);
    fseek (file, 0, SEEK_SET);
    buff = new char[len];
	if (fread (buff, 1, len, file) != len) {
		delete [] buff;
		fclose (file);
		return retVal;
	}
	
	int i = -1;
	while (i < len) {
		i++;
		if (buff[i] != '=') {
			key += buff[i];
		} else {
			int j = i;
			while (true) {
				j++;
				if (buff[j] != ';') {
					val += buff[j];
				} else {
					i = j;
					break;
				}
			}
			
			retVal[key] = val;
			key.clear ();
			val.clear ();
		}
	}
	
	delete [] buff;
	fclose (file);
	return retVal;
}

bool 
spdf::ConfigParser::save (const std::string &filename, const std::map<std::string, std::string> &data)
{
	FILE *file;
	std::string buff;
	
	file = fopen (filename.data (), "w");
	if (!file) {
		return false;
	}
	
	for (auto it = data.cbegin (); it != data.cend (); it++) {
		buff += (*it).first;
		buff += '=';
		buff += (*it).second;
		buff += ';';
	}
	
	fwrite (buff.data (), 1, buff.length (), file);
	
	fclose (file);
	return true;
}
