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

#ifndef COMPOSITENAMINGHELPERFACTORY_H
#define COMPOSITENAMINGHELPERFACTORY_H

#include "itkLightObject.h"
#include "itkObjectFactory.h"

#include "MetadataHelper.h"
#include <vector>
#include <memory>

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Factory to load a metadata-helper
 */
class MetadataHelperFactory : public itk::LightObject
{
public:
    typedef MetadataHelperFactory Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self)

    itkTypeMacro(MetadataHelperFactory, itk::LightObject)

    std::unique_ptr<MetadataHelper> GetMetadataHelper(const std::string& metadataFileName);
};
}  // namespace ts

#endif // COMPOSITENAMINGHELPERFACTORY_H
