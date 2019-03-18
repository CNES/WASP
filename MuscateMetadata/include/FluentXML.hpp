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

#pragma once

#include "otb_tinyxml.h"
#include <ostream>
#include <memory>

struct XNode
{
private:
    std::unique_ptr<TiXmlNode> node;

public:
    XNode(TiXmlNode *node);

    void LinkEndChild(TiXmlNode *child);

    TiXmlDocument *AsDocument();
    const TiXmlDocument *AsDocument() const;
    TiXmlElement *AsElement();
    const TiXmlElement *AsElement() const;
    TiXmlNode *Node();
    const TiXmlNode *Node() const;

protected:
    TiXmlNode *Release();
};

template <typename D>
struct XNodeW : public XNode
{
    template <typename... T>
    XNodeW(TiXmlNode *node, T &&... children)
        : XNode(node)
    {
        Link(std::forward<T>(children)...);
    }

    template <typename T>
    XNodeW &Append(T &&child)
    {
        Link(std::forward<T>(child));
        return *this;
    }

private:
    void Link()
    {
    }

    template <typename T, typename... R>
    void Link(T &&firstChild, R &&... otherChildren)
    {
        std::forward<T>(firstChild).AppendTo(static_cast<D &>(*this));
        Link(std::forward<R>(otherChildren)...);
    }
};

struct XDocument : public XNodeW<XDocument>
{
    template <typename... T>
    XDocument(T &&... children)
        : XNodeW(new TiXmlDocument, std::forward<T>(children)...)
    {
    }

    void Save(const char *path) const;
    void Save(const std::string &path) const;
};

struct XText
{
    std::string text;

    XText(std::string text);

    void AppendTo(XNode &parent);
};

struct XElement : public XNodeW<XElement>
{
    template <typename Name>
    XElement(Name &&name, std::string text)
        : XElement(std::forward<Name>(name), XText(std::move(text)))
    {
    }

    template <typename Name, typename... T>
    XElement(Name &&name, T &&... children)
        : XNodeW(new TiXmlElement(std::forward<Name>(name)), std::forward<T>(children)...)
    {
    }

    void AppendTo(XNode &parent);
    void SetAttribute(const char *name, const char *value);
};

struct XAttribute
{
    std::string name;
    std::string value;

    XAttribute(std::string name, std::string value);

    void AppendTo(XElement &parent);
};

struct XDeclaration
{
    std::unique_ptr<TiXmlDeclaration> declaration;

    XDeclaration(const char *version, const char *encoding, const char *standalone);

    void AppendTo(XNode &parent);
};

struct XUnknown
{
    std::unique_ptr<TiXmlUnknown> unknown;

    XUnknown(const char *s);

    void AppendTo(XNode &parent);
};

std::ostream & operator<<(std::ostream &o, const XDocument &doc);
