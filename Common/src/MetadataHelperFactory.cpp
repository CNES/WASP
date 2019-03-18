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

#include "MetadataHelperFactory.h"
#include "MuscateMetadataHelper.h"

using namespace ts;

std::unique_ptr<MetadataHelper> MetadataHelperFactory::GetMetadataHelper(const std::string& metadataFileName)
{

	//Only Muscate existing so far here.
    std::unique_ptr<MetadataHelper> muscateMetadataHelper(new MuscateMetadataHelper);
    if (muscateMetadataHelper->LoadMetadataFile(metadataFileName)){
        return muscateMetadataHelper;
    }
    itkExceptionMacro("Unable to read metadata from " << metadataFileName);
    return NULL;
}
