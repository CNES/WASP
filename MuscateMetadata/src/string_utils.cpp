/*
 * Copyright (C) 2015-2016, CS Romania <office@c-s.ro>
 * Copyright (C) 2018-2019, Centre National d'Etudes Spatiales (CNES)
 * All rights reversed
 *
 * This file is part of:
 * - Sen2agri-Processors (initial work)
 * - Weighted Average Synthesis Processor (WASP)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
*
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <limits>
#include <sstream>

#include <otbMacro.h>
#include <algorithm>

#include "string_utils.hpp"

std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> result;

	std::istringstream is(s);
	std::string item;
	while (std::getline(is, item, delim)) {
		result.emplace_back(std::move(item));
	}

	return result;
}

double ReadDouble(const std::string &s)
{
	try {
		if (s.empty()) {
			return std::numeric_limits<double>::quiet_NaN();
		}
		double ret = std::stod(s);
		return ret;
	} catch (const std::exception &e) {
		otbMsgDevMacro("Invalid double value " << s << ": " << e.what());

		return std::numeric_limits<double>::quiet_NaN();
	}
}

bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
			s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

std::string toUppercase(const std::string & s)
{
	std::string ret(s.size(), char());
	for(unsigned int i = 0; i < s.size(); ++i)
		ret[i] = (s[i] <= 'z' && s[i] >= 'a') ? s[i]-('a'-'A') : s[i];
	return ret;
}


std::string getParameterName(const std::string &param, const size_t &resolution){
	return std::string(param + "r" + std::to_string(resolution+1)).c_str();
}
