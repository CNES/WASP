/*
 * Copyright (C) 2018-2019, Centre National d'Etudes Spatiales (CNES)
 * All rights reserved
 *
 * This file is part of Weighted Average Synthesis Processor (WASP)
 *
 * Authors:
 * - Peter KETTIG <peter.kettig@cnes.fr>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * See the LICENSE.md file for more details.
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

#include <memory>

#include "itkObjectFactory.h"
#include "otb_tinyxml.h"

#include "MuscateMetadata.hpp"

/**
 * @brief Temporal synthesis namespace
 */
namespace ts {
/**
 * @brief Muscate Metadata namespace
 */

namespace muscate {

/**
 * @brief Reads muscate metadata files in format Muscate 3.1 (unified) and the old one (distributed)
 */
class MuscateMetadataReader : public itk::LightObject
{
public:
	/* ITK typedefs */
	typedef MuscateMetadataReader Self;
	typedef itk::LightObject Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;

	/* ITK Macros */
	itkNewMacro(Self)
	itkTypeMacro(MuscateMetadataReader, itk::LightObject)

	/**
	 * @brief Read the main metadata file
	 * @param path Reference to the input string
	 * @return Pointer to the Muscate metadata file
	 */
	std::unique_ptr<MuscateFileMetadata> ReadMetadata(const std::string &path);

	/**
	 * @brief Read all metadata fields inside the TiXml Document
	 * @param doc The metadata file opened with TiXml
	 * @return Pointer to the Muscate metadata file
	 */
	std::unique_ptr<MuscateFileMetadata> ReadMetadataXml(const TiXmlDocument &doc);

private:

	/**
	 * @brief checks if Element has Attribute of given name
	 * @param el The element
	 * @param s The name of the attribute
	 * @return True, if Attribute exists, false if not
	 */
	bool hasAttribute(TiXmlElement *el, const std::string &s);

	/**
	 * @brief Read the MetadataIdentification section of the main metadata file
	 * @param el Pointer to the MetadataIdentification parent
	 * @return MuscateMetadataIdentification struct containing all fields
	 */
	MuscateMetadataIdentification readMetadataIdentification(TiXmlElement *el);

	/**
	 * @brief Read the DatasetIdentification section of the main metadata file
	 * @param el Pointer to the DatasetIdentification parent
	 * @return MuscateDatasetIdentification struct containing all fields
	 */
	MuscateDatasetIdentification readDatasetIdentification(TiXmlElement *el);

	/**
	 * @brief Read the ProductCharacteristics section of the main metadata file
	 * @param el Pointer to the ProductCharacteristics parent
	 * @return MuscateProductCharacteristics struct containing all fields
	 */
	MuscateProductCharacteristics readProductCharacteristics(TiXmlElement *el);

	/**
	 * @brief Read the ProductOrganisation section of the main metadata file
	 * @param el Pointer to the ProductOrganisation parent
	 * @return MuscateProductOrganisation struct containing all fields
	 */
	MuscateProductOrganisation readProductOrganisation(TiXmlElement *el);

	/**
	 * @brief Read the GeopositionInformations section of the main metadata file
	 * @param el Pointer to the GeopositionInformations parent
	 * @return MuscateGeopositionInformations struct containing all fields
	 */
	MuscateGeopositionInformations readGeopositionInformations(TiXmlElement *el);

	/**
	 * @brief Read the GeometricInformations section of the main metadata file
	 * @param el Pointer to the GeometricInformations parent
	 * @return MuscateGeometricInformations struct containing all fields
	 */
	MuscateGeometricInformations readGeometricInformations(TiXmlElement *el);

	/**
	 * @brief Read the RadiometricInformations section of the main metadata file
	 * @param el Pointer to the RadiometricInformations parent
	 * @return MuscateRadiometricInformations struct containing all fields
	 */
	MuscateRadiometricInformations readRadiometricInformations(TiXmlElement *el);

	/**
	 * @brief Read the QualityInformations section of the main metadata file
	 * @param el Pointer to the QualityInformations parent
	 * @return MuscateQualityInformations struct containing all fields
	 */
	MuscateQualityInformations readQualityInformations(TiXmlElement *el);

}; /* class MuscateMetadataReader */
} /* namespace muscate */
} /* namespace ts */

