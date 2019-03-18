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

#ifndef BANDSDEFS_H
#define BANDSDEFS_H

#define S2_L2A_10M_BANDS_NO     4

#define WEIGHTED_REFLECTANCE_10M_BANDS_NO   S2_L2A_10M_BANDS_NO

//S2 10m Bands defines
#define S2_L2A_10M_BLUE_BAND_IDX        0
#define S2_L2A_10M_RED_BAND_IDX         2

// 20M Positions Definition
#define S2_L2A_20M_BANDS_NO     6
#define WEIGHTED_REFLECTANCE_20M_BANDS_NO   S2_L2A_20M_BANDS_NO

//S2 20m Bands defines
#define S2_L2A_20M_BLUE_BAND_IDX        -1
#define S2_L2A_10M_BLUE_BAND_NAME		"FRE_B2"
#define S2_L2A_20M_RED_BAND_IDX         -1

// These defines are for the case when all the bands of 10 AND 20m are resampled at the specified resolution
// and are all present

#define S2_L2A_ALL_BANDS_NO     10
#define WEIGHTED_REFLECTANCE_ALL_BANDS_NO   S2_L2A_ALL_BANDS_NO

#endif // BANDSDEFS_H
