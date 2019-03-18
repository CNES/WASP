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

#include "FluentXML.hpp"

XNode::XNode(TiXmlNode *node) : node(node)
{
}

void XNode::LinkEndChild(TiXmlNode *child)
{
    node->LinkEndChild(child);
}

TiXmlDocument *XNode::AsDocument()
{
    return static_cast<TiXmlDocument *>(node.get());
}

const TiXmlDocument *XNode::AsDocument() const
{
    return static_cast<TiXmlDocument *>(node.get());
}

TiXmlElement *XNode::AsElement()
{
    return static_cast<TiXmlElement *>(node.get());
}

const TiXmlElement *XNode::AsElement() const
{
    return static_cast<TiXmlElement *>(node.get());
}

TiXmlNode *XNode::Node()
{
    return node.get();
}

const TiXmlNode *XNode::Node() const
{
    return node.get();
}

TiXmlNode *XNode::Release()
{
    return node.release();
}

void XDocument::Save(const char *path) const
{
    AsDocument()->SaveFile(path);
}

void XDocument::Save(const std::string &path) const
{
    AsDocument()->SaveFile(path);
}

XText::XText(std::string text) : text(std::move(text))
{
}

void XText::AppendTo(XNode &parent)
{
    if (!text.empty()) {
        parent.LinkEndChild(new TiXmlText(std::move(text)));
    }
}

void XElement::AppendTo(XNode &parent)
{
    if (AsElement()->FirstChild() || AsElement()->FirstAttribute()) {
        parent.LinkEndChild(Release());
    }
}

void XElement::SetAttribute(const char *name, const char *value)
{
    AsElement()->SetAttribute(name, value);
}

XAttribute::XAttribute(std::string name, std::string value)
    : name(std::move(name)), value(std::move(value))
{
}

void XAttribute::AppendTo(XElement &parent)
{
    parent.SetAttribute(name.c_str(), value.c_str());
}

XDeclaration::XDeclaration(const char *version, const char *encoding, const char *standalone)
    : declaration(new TiXmlDeclaration(version, encoding, standalone))
{
}

void XDeclaration::AppendTo(XNode &parent)
{
    parent.LinkEndChild(declaration.release());
}

XUnknown::XUnknown(const char *s) : unknown(new TiXmlUnknown)
{
    std::istringstream ss(s);
    ss >> *unknown;
}

void XUnknown::AppendTo(XNode &parent)
{
    parent.LinkEndChild(unknown.release());
}

std::ostream & operator<<(std::ostream &o, const XDocument &doc)
{
    return o << *doc.AsDocument();
}
