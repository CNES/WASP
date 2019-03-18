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

#ifndef PRODUCTFORMATTER_INCLUDE_PRODUCTCREATORSENTINELMUSCATE_H_
#define PRODUCTFORMATTER_INCLUDE_PRODUCTCREATORSENTINELMUSCATE_H_

#include "ProductCreatorAdapter.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

class ProductCreatorSentinelMuscate : public ProductCreatorAdapter{
public:

	/**
	 * @brief Create a Sentinel-2 Muscate product inside the specified destination-folder
	 * @param destination The destination directory
	 * @return True, if the creation succeeded, false if not
	 */
	virtual bool createProduct(const std::string &destination) ;

private:
	/**
	 * @brief Read all metadata files and FillMetadataInfo for each S2-File. Also determines the GeoInfo to be used
	 * @param descriptors Vector of strings of xml-filenames
	 */
	virtual void LoadAllDescriptors(const std::vector<std::string> &descriptors);

	/**
	 * @brief Get the Geoposition information from a Level-2 product
	 * @param desc The filename to the L2-XML
	 * @return Geoposition to be used in the L3 Metadata
	 */
	virtual MuscateGeopositionInformations GetGeopositionInformations(const std::string &desc);

	/**
	 * @brief Set the BandInformation-vector for Sentinel-2
	 * @return Vector, containing the Radiometric band-infos
	 */
	virtual std::vector<BandInformation> getMissionBands();

	/**
	 * @brief Create the product directory name following the given conventions
	 * @param productType The type of the product
	 * @param productVersion The product version
	 * @return string of the product directory name
	 */
	virtual std::string BuildProductDirectoryName(const std::string &productType, const std::string &productVersion);

	/**
	 * @brief Build the product directory according to the Sentinel-2 Muscate conventions
	 * @param datetime Product datetime
	 * @param productType Type of the product
	 * @param productVersion Version of the product
	 * @return string of the S2-product directory
	 */
	std::string BuildFileNameSentinel(const std::string &datetime, const std::string &productType, const std::string &productVersion);

	/**
	 * @brief Copy raster files and create metadata-info for each
	 * @param raster The raster input name
	 * @param rasterTypes The type of raster file to be copied (See rasterTypes enum)
	 * @param bIsMask True if it's a mask file, false if not
	 * @note If copying fails, the metadata info is also not created
	 * @return true, if both copying and metadata creating succeeded, false if not
	 */
	template<typename ExtractorType>
	bool copyRaster(typename ExtractorType::Pointer splitter, const size_t &index,
			const rasterTypes &rasterType, resolutionTypesS2 resolution, const bool &bIsMask);

	/**
	 * @brief determines the BandGroups depending on which mission was chosen
	 * @return BandList struct containing all available bands in the output product
	 */
	virtual std::vector<BandGroup> getBandGroupList(const std::vector<BandInformation> &missionBands);

	/**
	 * @brief Generates the metadata file for the output product in Muscate Unified format
	 * @param strProductMetadataFilePath The path where the metadata file shall be written to
	 * @param cloudPercent The percentage of clouds in the image
	 * @return void - Writes Metadata inside the product folder, named {}_MTD_ALL.xml
	 */
	virtual void generateMetadataFile(const std::string &strProductMetadataFilePath, const std::string &cloudPercent = "1", const std::string &snowPercent = "1");
};
} /* namespace ts */


#endif /* PRODUCTFORMATTER_INCLUDE_PRODUCTCREATORSENTINELMUSCATE_H_ */
