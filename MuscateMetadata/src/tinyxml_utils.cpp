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

#include "tinyxml_utils.hpp"

std::string GetAttribute(const TiXmlElement *element, const char *attributeName)
{
    if (const char *at = element->Attribute(attributeName)) {
        return at;
    }

    return std::string();
}

std::string GetText(const TiXmlElement *element)
{
    if (const char *text = element->GetText()) {
        return text;
    }

    return std::string();
}

std::string GetChildText(const TiXmlElement *element, const char *childName)
{
    if (auto el = element->FirstChildElement(childName)) {
        if (const char *text = el->GetText())
            return text;
    }

    return std::string();
}

std::string
 GetChildAttribute(const TiXmlElement *element, const char *childName, const char *attributeName)
{
    if (auto el = element->FirstChildElement(childName)) {
        if (const char *at = el->Attribute(attributeName)) {
            return at;
        }
    }

    return std::string();
}
