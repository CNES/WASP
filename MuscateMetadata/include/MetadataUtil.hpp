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
#include "MuscateMetadata.hpp"


/**
 * @brief compares two image properties, each having a has*** and value. E.g. hasBitNum and bitNumber
 * @return True, if both have the property and they're equal or they don't have it and are not equal. False otherwise.
 */
bool compareImageProperties(const bool &a,const bool &b, const std::string &as, const std::string &bs);

/**
 * @brief print Muscate angles as a string, for debugging
 * @param angles
 * @return String formatted to print to console
 */
std::string MuscateAnglesToString(const MuscateAngles &angles);

/////////////////////////////////////////////
/// COMPARATORS FOR ALL METADATA STRUCTS ///
///////////////////////////////////////////

bool operator==(MuscateHeaderMetadata const& lhs, MuscateHeaderMetadata const& rhs);
bool operator==(MuscateMetadataIdentification const& lhs, MuscateMetadataIdentification const& rhs);
bool operator==(MuscateDatasetIdentification const& lhs, MuscateDatasetIdentification const& rhs);
bool operator!=(BandList const& lhs, BandList const& rhs);
bool operator!=(BandGroup const& lhs, BandGroup const& rhs);
bool operator==(BandGroup const& lhs, BandGroup const& rhs);
bool operator==(MuscateProductCharacteristics const& lhs, MuscateProductCharacteristics const& rhs);
bool operator==(ImageInformation const& lhs, ImageInformation const& rhs);
bool operator!=(ImageInformation const& lhs, ImageInformation const& rhs);
bool operator==(ImageProperty const& lhs, ImageProperty const& rhs);
bool operator==(MuscateProductOrganisation const& lhs, MuscateProductOrganisation const& rhs);
bool operator!=(CSInfo const& lhs, CSInfo const& rhs);
bool operator==(GeoPoint const& lhs, GeoPoint const& rhs);
bool operator==(GroupPositioning const& lhs, GroupPositioning const& rhs);
bool operator==(MuscateGeopositionInformations const& lhs, MuscateGeopositionInformations const& rhs);
bool operator==(SpecialValue const& lhs, SpecialValue const& rhs);
bool operator==(MuscateRadiometricInformations const& lhs, MuscateRadiometricInformations const& rhs);
bool operator==(MuscateQualityIndex const& lhs, MuscateQualityIndex const& rhs);
bool operator==(MuscateQualityProduct const& lhs, MuscateQualityProduct const& rhs);
bool operator==(MuscateQualityInformations const& lhs, MuscateQualityInformations const& rhs);
