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

#ifndef MUSCATEMETADATAHELPER_H
#define MUSCATEMETADATAHELPER_H

#include "MetadataHelper.h"
#include "../../MuscateMetadata/include/MuscateMetadataReader.hpp"
#include <vector>
#include "GlobalDefs.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Class to load a MuscateMetadata-file
 */
class MuscateMetadataHelper : public MetadataHelper
{
public:
	MuscateMetadataHelper();
	const char * GetNameOfClass() { return "MuscateMetadataHelper"; }

	/**
	 * @brief Return Filename by given string
	 * @param filenames The vector of filenames to be searched for
	 * @param toFind The string to be found.
	 * @return The filename containing a string. If multiple results are given, it returns the first element.
	 * 		   If none are found, an empty string is returned
	 */
	std::string getFileNameByString(const productReturnType &filenames, const std::string &toFind);

	/**
	 * @brief Get a pointer to the full metadata Struct
	 * @return Shared Ptr to the internal MuscateFileMetadata
	 */
	std::shared_ptr<MuscateFileMetadata> getFullMetadata();

protected:

	typedef ts::muscate::MuscateMetadataReader		MuscateMetadataReaderType;

	/**
	 * @brief The main Metadata-reader function. Sets all variables and has to be called prior to using them
	 * @return True, if the reading suceeded, false if not
	 */
	bool DoLoadMetadata();

	productReturnType getAllImageFileNames(const std::string &name);
	productReturnType getAllMaskFileNames(const std::string &name);
	productReturnType getImageFilenamesByResolution(const std::string &trigram, const size_t &res);
	productReturnType getImageFileName();
	productReturnType getAotFileName();
	productReturnType getCloudFileName();
	productReturnType getWaterFileName();
	productReturnType getSnowFileName();
	productReturnType getQualityFileName();

private:

	/**
	 * @brief Checks if mission string is known
	 * @param mission The mission string found
	 * @return True, if mission is VENUS/Sentinel, false otherwise
	 */
	bool IsMissionKnown(const std::string &mission);

	/**
	 * @brief Updates all values for Venus products
	 */
	void UpdateValuesForVenus();

	/**
	 * @brief Updates all values for the Sentinel-L2A products
	 */
	void UpdateValuesForSentinel();

	/**
	 * @brief Reads all Angles values
	 * @note Mostly found in the Geometric_Informations section of the Metadata
	 */
	void UpdateAngles();

	/**
	 * @brief Remove duplicate elements inside the given vector
	 * @param vec The vector containing multiple filenames, some can be duplicates.
	 * @note Return by reference
	 */
	void removeDuplicateElements(productReturnType &vec);
	/**
	 * @brief MuscateMetadataHelper::CheckFileExistence
	 * Checks if the file exists and if not, it tries to change the extension to small capitals letters.
	 * If the file with the changed extension exists, it is returned instead otherwise it is not modified
	 * @param fileName - in/out parameter
	 * @return
	 */
	bool CheckFileExistence(std::string &fileName);

	std::shared_ptr<MuscateFileMetadata> m_metadata;
};
}  // namespace ts
#endif // MUSCATEMETADATAHELPER_H
