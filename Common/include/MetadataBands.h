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

#ifndef COMMON_INCLUDE_METADATABANDS_H_
#define COMMON_INCLUDE_METADATABANDS_H_

#include "GlobalDefs.h"
#include <vector>

namespace ts {

/**
 * @brief Class to store information about a single band, including Filename and Type
 */
class Band{
public:

	/**
	 * @brief Default Constructor, setting an empty band
	 */
	Band(): m_path(""), m_type(""), m_res(0) {};

	/**
	 * @brief Default constructor, setting all member variables
	 * @param path The Path to the Band File
	 * @param type The Type or Nature of the Band as a trigram.
	 * @param res The Resolution of the band
	 */
	Band(std::string path, std::string type, size_t res): m_path(path), m_type(type), m_res(res) {};

	/**
	 * @return The path to the band-file
	 */
	std::string getPath(){return m_path;}

	/**
	 * @return The type of band
	 */
	std::string getType(){return m_type;}

	/**
	 * @return The Resolution of the band
	 */
	size_t getResolution(){return m_res;}

private:
	std::string m_path;
	std::string m_type;
	size_t m_res;
};

/**
 * @brief Class to represent a resolution, consisting of multiple bands
 */
class Resolution {
public:

	/**
	 * @brief Default constructor, initializing a resolution
	 */
	Resolution(std::string name): m_bands({}), m_groupName(name) {};

	/**
	 * @brief Constructor, setting all member variables
	 * @param bands The Bands belonging to the resolution
	 * @param groupName The name of the Group (e.g. R1, R2...)
	 */
	Resolution(std::vector<Band> bands, std::string groupName): m_bands(bands), m_groupName(groupName) {};

	/**
	 * @brief Adds a band to the vector
	 * @param b The Band to be added
	 */
	void addBand(const Band &b){ m_bands.emplace_back(b); };

	/**
	 * @return The bands of the resolution
	 */
	std::vector<Band> getBands(){ return m_bands;};

	/**
	 * @return The name of the group
	 */
	std::string getGroupName(){ return m_groupName; };

	/**
	 * @brief Return the index of a given band inside the Band vector
	 * @param type The type to be searched for as string (e.g. "B1" or "B8A")
	 * @return The index, if band is present or -1 if not
	 */
	int getBandIndexByType(const std::string &type);

	/**
	 * @brief Checks, if a given BandType is present
	 * @param type The type to be searched for as string (e.g. "B1" or "B8A")
	 * @return True, if the band was found in one of the resolutions, false if not
	 */
	bool doesBandTypeExist(const std::string &type);

private:
	std::vector<Band> m_bands;
	std::string m_groupName;
};

/**
 * @brief Store multiple resolutions in this class, giving an API to search through it
 */
class MultiResolution {
public:

	/**
	 * @brief Default Constructor, initializing the vector to {}
	 */
	MultiResolution();

	/**
	 * @Add a resolution to the internal resolution vector
	 * @param res The Resolution to be added.
	 */
	void addResolution(const Resolution &res);

	/**
	 * @brief Returns the number of Resolutions present in the vector
	 * @return 0, if none are present, otherwise the vectors size()
	 */
	size_t getNumberOfResolutions();

	/**
	 * @brief Get the combined number of bands in all resolutions
	 * @return The total number of bands as size_t
	 */
	size_t getTotalNumberOfBands();

	/**
	 * @brief Get a resolution by index.
	 * @param n The index of the resolution
	 * @return The resolution stored inside the vector at the n-th position
	 * @note IMPORTANT: The first resolution is always the main-one!
	 */
	productReturnType getNthResolutionFilenames(const size_t &n);

	/**
	 * @brief Checks, if a given BandType is present
	 * @param type The type to be searched for as string (e.g. "B1" or "B8A")
	 * @return True, if the band was found in one of the resolutions, false if not
	 */
	bool doesBandTypeExist(const std::string &type) const;

	/**
	 * @brief Returns the filename corresponding to a specific band type
	 * @param type The type to be searched for as string (e.g. "B1" or "B8A")
	 * @return The filename, if the band was found or an empty string if it wasn't
	 */
	std::string getSpecificBandTypeFilename(const std::string &type);

	/**
	 * @brief Return the Resolution vector
	 * @return The vector of resolutions
	 */
	std::vector<Resolution> getResolutionVector(){ return m_resolutions;}
private:
	/**
	 * @brief The Resolution vector, storing all given resolutions.
	 * @note The main resolution has to be stored at the first position
	 */
	std::vector<Resolution> m_resolutions;
};

}  // namespace ts

#endif /* COMMON_INCLUDE_METADATABANDS_H_ */
