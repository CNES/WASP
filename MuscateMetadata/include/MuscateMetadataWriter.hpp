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

#include "itkObjectFactory.h"

#include "FluentXML.hpp"
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
 * @brief Writes muscate metadata files in format Muscate 3.1 (unified)
 */
class MuscateMetadataWriter : public itk::LightObject
{
public:
	/* ITK typedefs */
	typedef MuscateMetadataWriter Self;
	typedef itk::LightObject Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;

	/* ITK macros */
	itkNewMacro(Self)
	itkTypeMacro(MuscateMetadataWriter, itk::LightObject)

	/**
	 * @brief Creates the XDocument from the MuscateFileMetadata struct
	 * @param struct Struct containing all metadata information for the new product
	 * @return XDocument the document, formatted, to be written
	 */
	XDocument CreateMetadataXml(const MuscateFileMetadata &metadata);

	/**
	 * @brief Creates the XDocument file and writes it to the given path
	 * @param struct Struct containing all metadata information for the new product
	 * @param path The path to write the file to
	 * @return void
	 */
	void WriteMetadata(const MuscateFileMetadata &metadata, const std::string &path);

private:
	/**
	 * @brief Format functions to create an XDoc from the MuscateMetadataIdentification struct
	 */
	XElement Format(const MuscateMetadataIdentification &meta);
	/**
	 * @brief Format functions to create an XDoc from the MuscateDatasetIdentification struct
	 */
	XElement Format(const MuscateDatasetIdentification &meta);
	/**
	 * @brief Format functions to create an XDoc from the MuscateProductCharacteristics struct
	 */
	XElement Format(const MuscateProductCharacteristics &meta);
	/**
	 * @brief Format functions to create an XDoc from the MuscateProductOrganisation struct
	 */
	XElement Format(const MuscateProductOrganisation &meta);
	/**
	 * @brief Format functions to create an XDoc from the MuscateGeometricInformations struct
	 */
	XElement Format(const MuscateGeometricInformations &meta);
	/**
	 * @brief Format functions to create an XDoc from the MuscateGeopositionInformations struct
	 */
	XElement Format(const MuscateGeopositionInformations &meta);
	/**
	 * @brief Format functions to create an XDoc from the MuscateRadiometricInformations struct
	 */
	XElement Format(const MuscateRadiometricInformations &meta);
	/**
	 * @brief Format functions to create an XDoc from the MuscateQualityInformations struct
	 */
	XElement Format(const MuscateQualityInformations &meta);
	/**
	 * @brief Format functions to create an XDoc from the MuscateProductionInformations struct
	 */
	XElement Format(const MuscateProductionInformations &meta);

	//Specifies the Type of organizational product to be written
	typedef enum{
		Image,
		Mask,
		Data
	}organisationType;

}; /* class MuscateMetadataWriter */
} /* namespace muscate */
} /* namespace ts */
