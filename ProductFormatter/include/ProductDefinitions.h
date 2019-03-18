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

#ifndef PRODUCTFORMATTER_INCLUDE_PRODUCTDEFINITIONS_H_
#define PRODUCTFORMATTER_INCLUDE_PRODUCTDEFINITIONS_H_

#define DEFAULT_PRODUCT_VERSION					"0-8"
#define PRODUCT_DISTRIBUTION_FLAG				"C"
#define CLOUD_INDEX								1
#define SNOW_INDEX								2
#define S2_RESOLUTION_R1						10
#define S2_RESOLUTION_R2						20

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

typedef enum{
	COMPOSITE_REFLECTANCE_RASTER,
	COMPOSITE_WEIGHTS_RASTER,
	COMPOSITE_FLAGS_MASK,
	COMPOSITE_DATES_MASK,
}rasterTypes;

/**
 * @brief Struct to store the current version number
 */
typedef struct{
	size_t minor;
	size_t major;

	std::string toString(std::string delimiter = "-"){
		return std::to_string(major) + delimiter + std::to_string(minor);
	}
}versionType;

/**
 * @brief Types of resolutions for Sentinel-2
 */
typedef enum{
	S2_R1 = -2,//!< R1
	S2_R2 = -1,//!< R2
	S2_B2,     //!< B2
	S2_B3,     //!< B3
	S2_B4,     //!< B4
	S2_B5,     //!< B5
	S2_B6,     //!< B6
	S2_B7,     //!< B7
	S2_B8,     //!< B8
	S2_B8A,    //!< B8A
	S2_B11,    //!< B11
	S2_B12,    //!< B12
}resolutionTypesS2;


/**
 * @brief Types of resolutions for Venus
 */
typedef enum{
	VNS_XS = -1,//!< XS
	VNS_B1,     //!< B2
	VNS_B2,     //!< B2
	VNS_B3,     //!< B3
	VNS_B4,     //!< B4
	VNS_B5,     //!< B5
	VNS_B6,     //!< B6
	VNS_B7,     //!< B7
	VNS_B8,     //!< B8
	VNS_B9,     //!< B9
	VNS_B10,    //!< B10
	VNS_B11,    //!< B11
	VNS_B12,    //!< B12
}resolutionTypesVenus;

/**
 * @brief Specifies the type of the mission that is found in the product
 */
typedef enum{
	NoMission = 0,//!< NoMission
	S2A_Only,     //!< S2A_Only
	S2B_Only,     //!< S2B_Only
	S2_Only,      //!< S2_Only
	L8_Only,
	Multisat,
	Venus,     	  //!< Venus
}missionTypes;


//////////////////////////
/// CO-GeoTiff Config ///
////////////////////////

#define GDAL_ADDO					"gdaladdo"
#define GDAL_TRANSLATE				"gdal_translate"
#define CoG_TRANSLATE_PREPARE		"?&gdal:co:TILED=YES&gdal:co:COMPRESS=DEFLATE"

/**
 * @brief Struct to store the Geotiff config for all rasters
 */
struct GeoTiffConfig_t{
	std::string resamplingMethod;
	std::string filename;
	std::vector<size_t> levels;
	std::string tiled;
	std::string compress;
	std::string copySrcOverviews;
	bool quiet;
	/**
	 * @brief Init the struct to the default values according to the GeoTiff-Doc
	 * @note Doc can be found under https://trac.osgeo.org/gdal/wiki/CloudOptimizedGeoTIFF#Performancetesting
	 */
	void initToDefaultValues(const std::string &f){
		resamplingMethod = "average";
		levels = {2,4,8,16,32};
		filename = f;
		tiled = "YES";
		compress = "DEFLATE";
		copySrcOverviews = "YES";
	}

	/**
	 * @brief Create config to be used with gdaladdo
	 * @return Vector containing string of current values
	 */
	std::vector<std::string> toString(){
		std::vector<std::string> str {"-r", resamplingMethod, filename};
		for(auto level : levels){
			str.push_back(std::to_string(level));
		}
		str.push_back("-q");
		return str;
	}

	/**
	 * @brief Return the config for gdal_translate
	 * @return Vector containing the config strings
	 */
	std::vector<std::string> getTranslateConfig(){
		return {"-co", "TILED=" + tiled, "-co", "COPY_SRC_OVERVIEWS=" + copySrcOverviews, "-co", "COMPRESS=" + compress, "-a_nodata", NO_DATA_VALUE_STR, "-q"};
	}
};

} /* namespace ts */

#endif /* PRODUCTFORMATTER_INCLUDE_PRODUCTDEFINITIONS_H_ */
