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

#ifndef METADATAHELPER_H
#define METADATAHELPER_H

#include <string>
#include <vector>
#include "otbImage.h"
#include "MetadataAngles.h"
#include "GlobalDefs.h"
#include "MetadataBands.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Abstract class to load a metadata-file
 */
class MetadataHelper
{
public:
	MetadataHelper();
	virtual ~MetadataHelper();

	/**
	 * @brief Main Metadata-reader function. Loads the file and initialises the common variables.
	 * @param file The path to the file
	 * @return True, if the reading suceeded, false if not.
	 */
	bool LoadMetadataFile(const std::string& file);

	///////////////////////////
	/// GENETAL FIELDS API ///
	/////////////////////////

	/**
	 * @return Return the main mission/platform name
	 */
	virtual std::string GetMissionName() { return m_Mission; }

	////////////////////////
	/// MAIN RASTER API ///
	//////////////////////

	/**
	 * @return Get the Filename vector corresponding to all SRE-image files
	 */
	virtual productReturnType GetImageFileNames() { return m_ImageFileName; }

	/**
	 * @return Get the NODATA value as string
	 */
	virtual std::string GetNoDataValue() { return m_strNoDataValue; }

	/**
	 * @brief Return Filename by given string
	 * @param filenames The vector of filenames to be searched for
	 * @param toFind The string to be found.
	 * @return The filename containing a string. If multiple results are given, it returns the first element.
	 * 		   If none are found, an empty string is returned
	 */
	virtual std::string getFileNameByString(const productReturnType &filenames, const std::string &toFind) = 0;

	//////////////////
	/// MASKS API ///
	////////////////

	virtual productReturnType GetCloudImageFileNames() { return m_CloudFileName; }
	virtual productReturnType GetWaterImageFileNames() { return m_WaterFileName; }
	virtual productReturnType GetSnowImageFileNames() { return m_SnowFileName; }
	virtual productReturnType GetSaturationImageFileNames() { return m_SaturationFileName; }
	virtual productReturnType GetAotImageFileNames() { return m_AotFileName; }

	/////////////////
	/// DATE API ///
	///////////////

	/**
	 * @brief Returns the acquisition date as string in the format YYYY-MM-DDTHH:mm:ss.SSSZ
	 */
	virtual std::string GetAcquisitionDateLong() { return m_AcquisitionDate; }

	/**
	 * @brief Returns the acquisition date as string in the format YYYYMMDD
	 */
	virtual std::string GetAcquisitionDate();

	/**
	 * @return Returns the Acquisition date as Date of Year
	 */
	virtual int GetAcquisitionDateAsDoy();

	////////////////
	/// AOT API ///
	//////////////

	/**
	 * @return Return the Reflectance Quantification field as double
	 */
	virtual double GetReflectanceQuantificationValue() {return m_ReflQuantifVal; }

	/**
	 * @return Return the AOT Quantification field as double
	 */
	virtual double GetAotQuantificationValue() { return m_fAotQuantificationValue; }

	/**
	 * @return Return the AOT NODATA field as double
	 */
	virtual double GetAotNoDataValue() { return m_fAotNoDataVal; }

	///////////////////
	/// ANGLES API ///
	/////////////////

	virtual bool HasGlobalMeanAngles() { return m_bHasGlobalMeanAngles; }
	virtual bool HasBandMeanAngles() { return m_bHasBandMeanAngles; }
	virtual MeanAngles_Type GetSolarMeanAngles() { return m_solarMeanAngles;}
	virtual MeanAngles_Type GetSensorMeanAngles();
	virtual double GetRelativeAzimuthAngle();
	virtual MeanAngles_Type GetSensorMeanAngles(int nBand);

	/**
	 * @return True, if detailed viewing angles were found in the product and successfully read, false if not.
	 */
	virtual bool HasDetailedAngles() { return m_bHasDetailedAngles; }

	virtual int GetDetailedAnglesGridSize() { return m_detailedAnglesGridSize; }
	virtual MetadataHelperAngles GetDetailedSolarAngles() { return m_detailedSolarAngles; }
	virtual std::vector<MetadataHelperViewingAnglesGrid> GetDetailedViewingAngles() { return m_detailedViewingAngles; }

	////////////////////
	/// PRODUCT API ///
	//////////////////

	/**
	 * @return Return the Metadata product level
	 */
	virtual std::string getProductLevel(){ return m_productLevel;}

	///////////////////////////
	/// BANDS HANDLING API ///
	/////////////////////////

	/**
	 * @brief Get the total number of resolutions for a platform.
	 * @note In the case of Sentinel: 2, By default: 0
	 */
	virtual size_t getNumberOfResolutions() { return m_Resolutions.getNumberOfResolutions();}

	/**
	 * @brief Get the combined number of bands in all resolutions
	 * @return The total number of bands as size_t
	 */
	virtual size_t GetTotalBandsNo() { return m_Resolutions.getTotalNumberOfBands(); }

	/**
	 * @brief Return the product resolutions
	 * @return The MultiResolution struct
	 * @note The values are specific to each platform and therefore hardcoded
	 */
	virtual MultiResolution getResolutions() { return m_Resolutions;};

protected:
	/**
	 * @brief The main Metadata-reader function. Sets all variables and has to be called prior to using them
	 * @return True, if the reading suceeded, false if not
	 * @note Has to be implemented in all platforms in order to connect to the API above.
	 */
	virtual bool DoLoadMetadata() = 0;

	/**
	 * @brief Resets all member variables. To be used, when a Metadata-Reader is used more than once
	 */
	void Reset();

	/**
	 * @brief Builds full path to a filename by append the Directory name (@var m_Dirname)
	 * @param fileName The path to a file
	 * @return The filename with the dir name prepended
	 */
	std::string buildFullPath(const std::string& fileName);

	std::string m_Mission;
	std::string m_Instrument;
	std::string m_productLevel;

	productReturnType m_AotFileName;
	productReturnType m_CloudFileName;
	productReturnType m_WaterFileName;
	productReturnType m_SnowFileName;
	productReturnType m_SaturationFileName;
	productReturnType m_ImageFileName;

	std::string m_AcquisitionDate;

	std::string m_strNoDataValue;
	double m_ReflQuantifVal;
	double m_fAotQuantificationValue;
	double m_fAotNoDataVal;

	int m_nTotalBandsNo;

	MeanAngles_Type m_solarMeanAngles;
	std::vector<MeanAngles_Type> m_sensorBandsMeanAngles;
	bool m_bHasGlobalMeanAngles;
	bool m_bHasBandMeanAngles;

	bool m_bHasDetailedAngles;
	int m_detailedAnglesGridSize;
	MetadataHelperAngles m_detailedSolarAngles;
	std::vector<MetadataHelperViewingAnglesGrid> m_detailedViewingAngles;

	std::string m_inputMetadataFileName;
	std::string m_DirName;
	std::string m_basename;

	MultiResolution m_Resolutions;


};

}  // namespace ts
#endif // METADATAHELPER_H
