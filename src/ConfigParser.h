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

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>
#include <map>

// syntax: <VAR> = <VAL>;

namespace spdf {
	class ConfigParser {
		public:
			std::map<std::string, std::string> parse (const std::string &filename);
			bool save (const std::string &filename, const std::map<std::string, std::string> &data);
	};
}

#endif
