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

#ifndef GLOBALDEFS_H
#define GLOBALDEFS_H

#include <string>
#include <vector>
/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {


/* The product return Type used for all products */
using productReturnType = std::vector<std::string>;

#define DEFAULT_QUANTIFICATION_VALUE	 	 1000
#define NO_DATA_VALUE						-10000

#define WEIGHT_NO_DATA_VALUE				-10000

#define EPSILON								0.0001f

#define TIF_EXTENSION							".tif"
#define JPEG_EXTENSION							".jpg"
#define XML_EXTENSION							".xml"

#define COMPOSITE_REFLECTANCE_SUFFIX			"FRC"
#define REFLECTANCE_SUFFIX						"SRE"
#define FLAT_REFLECTANCE_SUFFIX					"FRE"
#define QUICKLOOK_SUFFIX						"QKL_ALL"
#define COMPOSITE_WEIGHTS_SUFFIX				"WGT"
#define COMPOSITE_DATES_SUFFIX 					"DTS"
#define COMPOSITE_FLAGS_SUFFIX					"FLG"
#define METADATA_CATEG							"MTD_ALL"
#define IPP_DATA_SUFFIX							"IPP_ALL"

#define NO_DATA_VALUE_STR						"-10000"

#define DATA_FOLDER_NAME						"DATA"
#define MASKS_FOLDER_NAME						"MASKS"


#define SENTINEL_MISSION_STR					"SENTINEL"
#define VENUS_MISSION_STR						"VENUS"

#define LANDSAT8								"LANDSAT8"
#define SENTINEL2X								"SENTINEL2X"
#define SENTINEL2A								"SENTINEL2A"
#define SENTINEL2B								"SENTINEL2B"
#define MULTISAT								"S2AS2BL8"
#define VENUS									VENUS_MISSION_STR

#define NONE_STRING								"None"

#define MAIN_RESOLUTION_INDEX					0
#define N_RESOLUTIONS_SENTINEL					2
/**
 * @brief Output Flag-mask definitions
 */
typedef enum {
	IMG_FLG_NO_DATA=0,     //!< IMG_FLG_NO_DATA
	IMG_FLG_CLOUD=1,       //!< IMG_FLG_CLOUD
	IMG_FLG_SNOW=2,        //!< IMG_FLG_SNOW
	IMG_FLG_WATER=3,       //!< IMG_FLG_WATER
	IMG_FLG_LAND=4,        //!< IMG_FLG_LAND
	IMG_FLG_CLOUD_SHADOW=5,//!< IMG_FLG_CLOUD_SHADOW
	IMG_FLG_SATURATION=6   //!< IMG_FLG_SATURATION
} FlagType;


/**
 * @brief Interpolator types
 */
typedef enum
{
	Interpolator_NNeighbor,//!< Interpolator_NNeighbor
	Interpolator_Linear,   //!< Interpolator_Linear
	Interpolator_BCO       //!< Interpolator_BCO
} Interpolator_Type;

/**
 * @brief Used to determine whether the underlying structure has a size()-method or not.
 * @note Used in the ShortToFloat- and FloatToShort-TranslationFunctors
 */
template<typename T>
struct HasSizeMethod
{
	template<typename U, unsigned int (U::*)() const> struct SFINAE {};
	template<typename U> static char Test(SFINAE<U, &U::Size>*);
	template<typename U> static int Test(...);
	static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
};

/////////////////////////
/// GLOBAL FUNCTIONS ///
///////////////////////

/**
 * @brief Returns the base name in a string
 * @param path Reference to the input string
 * @return string containing the base name
 */
std::string getDirname(const std::string &path);

/**
 *@brief Returns the directory name in a string
 *@param path Reference to the input string
 *@return string containing the directory name
 */
std::string getBasename(const std::string &path);

/**
 * @brief get Environment Variable by key name
 * @param key The name of the key, e.g. PATH
 * @return The value of the variable, if existing
 */
std::string getEnvVar( std::string const & key );

}  // namespace ts

#endif //GLOBALDEFS_H
