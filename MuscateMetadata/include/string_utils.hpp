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

#include <string>
#include <vector>

/**
 * @brief Split string by a char delimiter
 * @param s The string to be splitted
 * @param delim The delimiter, occuring [0...*]
 * @return A vector containing all the splitted substrings
 */
std::vector<std::string> split(const std::string &s, char delim);

/**
 * @brief Read a double value from a given string
 * @param s The string containing only [0...9] and a single "."
 * @return The double value
 */
double ReadDouble(const std::string &s);

/**
 * @brief Check if a string contains only digits
 * @param s The string to be checked
 * @return True, if only digits 0-9 are found, False if not
 */
bool is_number(const std::string& s);

/**
 * @brief Returns a string with only uppercase letters
 * @param s The original string - Only ASCII characters allowed
 * @return The same string as s in uppercase only
 */
std::string toUppercase(const std::string & s);

/**
 * @brief Append the current resolution to the parameter name
 * @param param The parameter name
 * @param resolution The current resolution as size_t [0;1]
 * @return The parameter with the current resolution
 * @example "in" + resolution 0 = "inr1"; "image" + resolution 1 = "imager2"
 */
std::string getParameterName(const std::string &param, const size_t &resolution);
