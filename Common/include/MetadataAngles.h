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

#ifndef COMMON_METADATAANGLES_H_
#define COMMON_METADATAANGLES_H_

#include <stdlib.h>

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

typedef struct {
    double zenith;
    double azimuth;
} MeanAngles_Type;

struct MetadataHelperAngleList
{
    std::string ColumnUnit;
    std::string ColumnStep;
    std::string RowUnit;
    std::string RowStep;
    std::vector<std::vector<double> > Values;
};

struct MetadataHelperAngles
{
    MetadataHelperAngleList Zenith;
    MetadataHelperAngleList Azimuth;
};

struct MetadataHelperViewingAnglesGrid
{
    std::string BandId;
    std::string DetectorId;
    MetadataHelperAngles Angles;
};

typedef enum {MSK_CLOUD=1, MSK_SNOW=2, MSK_WATER=4, MSK_SAT=8, MSK_VALID=16, ALL=0x1F} MasksFlagType;

}  // namespace ts
#endif /* COMMON_METADATAANGLES_H_ */
