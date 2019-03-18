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

#include "MuscateMetadataHelper.h"
#include <ViewingAngles.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include <sys/stat.h>

using namespace ts;

MuscateMetadataHelper::MuscateMetadataHelper()
{
	m_ReflQuantifVal = 1;
}

bool MuscateMetadataHelper::DoLoadMetadata()
{
	MuscateMetadataReaderType::Pointer muscateMetadataReader = MuscateMetadataReaderType::New();
	if (m_metadata = muscateMetadataReader->ReadMetadata(m_inputMetadataFileName)) {
		m_Mission = m_metadata->ProductCharacteristics.Platform;
		m_productLevel = m_metadata->ProductCharacteristics.ProductLevel;
		if (IsMissionKnown(m_metadata->ProductCharacteristics.Platform)){
			m_ReflQuantifVal = std::stod(m_metadata->RadiometricInformations.quantificationValues[0].value); //We know that the First one is always the reflectance-QuantValue
			if(m_ReflQuantifVal < 1) {
				m_ReflQuantifVal = (1 / m_ReflQuantifVal);//Inverse, as it will be used to divide the values
			}
			m_fAotQuantificationValue = std::stod(m_metadata->RadiometricInformations.quantificationValues[1].value); //And the second the AOT-quantValue
			// compute the Image file name
			m_ImageFileName = getImageFileName();
			// compute the AOT file name
			m_AotFileName = getAotFileName();
			// compute the Cloud file name
			m_CloudFileName = getCloudFileName();
			// compute the Water file name
			m_WaterFileName = getWaterFileName();
			// compute the Snow file name
			m_SnowFileName = getSnowFileName();
			// set the acquisition date
			m_AcquisitionDate = m_metadata->ProductCharacteristics.AcquisitionDate;
			//Find the "nodata"-Field in the Vec of SpecialValues
			m_strNoDataValue = std::find_if(m_metadata->RadiometricInformations.specialValues.begin(),
					m_metadata->RadiometricInformations.specialValues.end(),
					[](const SpecialValue& sv)->bool{return "nodata" == sv.name; })->value;


			if(m_metadata->ProductCharacteristics.Platform.find(SENTINEL_MISSION_STR) != std::string::npos){
				//Set Mission Specific Values
				UpdateValuesForSentinel();
				//Set the Metadata Angle-Grids
				UpdateAngles();
			}else if(m_metadata->ProductCharacteristics.Platform.find(VENUS_MISSION_STR) != std::string::npos){
				UpdateValuesForVenus();
			}
			return true;
		}else {
			itkExceptionMacro("Unknown mission: " + m_metadata->ProductCharacteristics.Platform);
		}
	}
	return false;
}

bool MuscateMetadataHelper::IsMissionKnown(const std::string &mission){
	if(mission.find(VENUS_MISSION_STR) != std::string::npos){
		return true;
	}else if(mission.find(SENTINEL_MISSION_STR) != std::string::npos){
		return true;
	}
	return false;
}

void MuscateMetadataHelper::UpdateValuesForVenus(){
	//Hardcoded values for Venus
	std::string FREtype = (getProductLevel().find("L2A") != std::string::npos) ?
			std::string(FLAT_REFLECTANCE_SUFFIX) : std::string(COMPOSITE_REFLECTANCE_SUFFIX);
	const std::vector<std::string> FREendings = {"B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "B10", "B11", "B12"};
	const std::vector<size_t> FREresolutions = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
	Resolution XS("XS");
	for(size_t i = 0; i < FREendings.size(); i++){
		Band B(getFileNameByString(m_ImageFileName, std::string(FREtype + "_" + FREendings[i] + TIF_EXTENSION)), FREendings[i], FREresolutions[i]);
		XS.addBand(B);
	}
	m_Resolutions.addResolution(XS);
}

void MuscateMetadataHelper::UpdateValuesForSentinel(){
	//Hardcoded values for Sentinel
	std::string FREtype = (getProductLevel().find("L2A") != std::string::npos) ?
			std::string(FLAT_REFLECTANCE_SUFFIX) : std::string(COMPOSITE_REFLECTANCE_SUFFIX);
	const std::vector<std::string> FREendings = {"B2", "B3", "B4", "B5", "B6", "B7", "B8", "B8A", "B11", "B12"};
	const std::vector<size_t> FREresolutions = {10, 10, 10, 20, 20, 20, 10, 20, 20, 20};
	Resolution R1("R1");
	Resolution R2("R2");
	for(size_t i = 0; i < FREendings.size(); i++){
		Band B(getFileNameByString(m_ImageFileName, std::string(FREtype + "_" + FREendings[i] + TIF_EXTENSION)), FREendings[i], FREresolutions[i]);
		if(FREresolutions[i] == 10){
			R1.addBand(B);
		}else{
			R2.addBand(B);
		}
	}
	m_Resolutions.addResolution(R1); //Main resolution, therefore has to be added first
	m_Resolutions.addResolution(R2);

}

void MuscateMetadataHelper::UpdateAngles(){
	m_bHasGlobalMeanAngles = true;
	m_bHasBandMeanAngles = true;
	m_bHasDetailedAngles = true;
	m_detailedAnglesGridSize = 23;

	// update the solar mean angle
	m_solarMeanAngles.azimuth = m_metadata->GeometricInformations.MeanSunAngle.AzimuthValue;
	m_solarMeanAngles.zenith = m_metadata->GeometricInformations.MeanSunAngle.ZenithValue;

	// first compute the total number of bands to add into m_sensorBandsMeanAngles
	size_t nMaxBandId = 0;
	std::vector<MuscateAnglePair> angles = m_metadata->GeometricInformations.MeanViewingIncidenceAngles;

	// compute the array size
	unsigned int nArrSize = (nMaxBandId > angles.size() ? nMaxBandId+1 : angles.size());
	// update the viewing mean angle
	m_sensorBandsMeanAngles.resize(nArrSize);
	for(unsigned int i = 0; i<angles.size(); i++) {
		m_sensorBandsMeanAngles[i].azimuth = angles[i].AzimuthValue;
		m_sensorBandsMeanAngles[i].zenith = angles[i].ZenithValue;
	}

	// extract the detailed viewing and solar angles
	std::vector<MuscateBandViewingAnglesGrid> muscateAngles = ComputeViewingAngles(m_metadata->GeometricInformations.ViewingAngles);
	for(unsigned int i = 0; i<muscateAngles.size(); i++) {
		MuscateBandViewingAnglesGrid muscateGrid = muscateAngles[i];
		//Return only the angles of the bands for the current resolution
		if(m_Resolutions.doesBandTypeExist(muscateAngles[i].BandId)) {
			//Copy MuscateBandViewingAnglesGrid to MetadataHelperViewingAnglesGrid
			MetadataHelperViewingAnglesGrid mhGrid;
			mhGrid.BandId = muscateGrid.BandId;
			mhGrid.Angles.Azimuth.ColumnStep = muscateGrid.Angles.Azimuth.ColumnStep;
			mhGrid.Angles.Azimuth.ColumnUnit = muscateGrid.Angles.Azimuth.ColumnUnit;
			mhGrid.Angles.Azimuth.RowStep = muscateGrid.Angles.Azimuth.RowStep;
			mhGrid.Angles.Azimuth.RowUnit = muscateGrid.Angles.Azimuth.RowUnit;
			mhGrid.Angles.Azimuth.Values = muscateGrid.Angles.Azimuth.Values;

			mhGrid.Angles.Zenith.ColumnStep = muscateGrid.Angles.Zenith.ColumnStep;
			mhGrid.Angles.Zenith.ColumnUnit = muscateGrid.Angles.Zenith.ColumnUnit;
			mhGrid.Angles.Zenith.RowStep = muscateGrid.Angles.Zenith.RowStep;
			mhGrid.Angles.Zenith.RowUnit = muscateGrid.Angles.Zenith.RowUnit;
			mhGrid.Angles.Zenith.Values = muscateGrid.Angles.Zenith.Values;

			m_detailedViewingAngles.push_back(mhGrid);
		}
	}

	//Copy Muscate angles to MetadataHelper Angles
	m_detailedSolarAngles.Azimuth.ColumnStep = m_metadata->GeometricInformations.SolarAngles.Azimuth.ColumnStep;
	m_detailedSolarAngles.Azimuth.ColumnUnit = m_metadata->GeometricInformations.SolarAngles.Azimuth.ColumnUnit;
	m_detailedSolarAngles.Azimuth.RowStep = m_metadata->GeometricInformations.SolarAngles.Azimuth.RowStep;
	m_detailedSolarAngles.Azimuth.RowUnit = m_metadata->GeometricInformations.SolarAngles.Azimuth.RowUnit;
	m_detailedSolarAngles.Azimuth.Values = m_metadata->GeometricInformations.SolarAngles.Azimuth.Values;

	m_detailedSolarAngles.Zenith.ColumnStep = m_metadata->GeometricInformations.SolarAngles.Zenith.ColumnStep;
	m_detailedSolarAngles.Zenith.ColumnUnit = m_metadata->GeometricInformations.SolarAngles.Zenith.ColumnUnit;
	m_detailedSolarAngles.Zenith.RowStep = m_metadata->GeometricInformations.SolarAngles.Zenith.RowStep;
	m_detailedSolarAngles.Zenith.RowUnit = m_metadata->GeometricInformations.SolarAngles.Zenith.RowUnit;
	m_detailedSolarAngles.Zenith.Values = m_metadata->GeometricInformations.SolarAngles.Zenith.Values;
}

productReturnType MuscateMetadataHelper::getAllImageFileNames(const std::string &name) {
	auto freList = std::find_if(m_metadata->ProductOrganisation.ImageProperties.begin(),
			m_metadata->ProductOrganisation.ImageProperties.end(),
			[name](const ImageProperty& ip)->bool{return ip.Nature == name;});
	if(freList != m_metadata->ProductOrganisation.ImageProperties.end()){
		productReturnType imgFileList;
		for(auto image : freList->ImageFiles){
			std::string fullPath = m_DirName + "/" + image.path;
			if(!CheckFileExistence(fullPath)){
				itkWarningMacro("Cannot find filename " << fullPath);
			}
			imgFileList.push_back(fullPath);//return full path
		}
		return imgFileList;
	}
	return {};
}

productReturnType MuscateMetadataHelper::getAllMaskFileNames(const std::string &name) {
	auto maskList = std::find_if(m_metadata->ProductOrganisation.MaskProperties.begin(),
			m_metadata->ProductOrganisation.MaskProperties.end(),
			[name](const ImageProperty& ip)->bool{return ip.Nature == name;});
	if(maskList != m_metadata->ProductOrganisation.MaskProperties.end()){
		productReturnType imgFileList;
		for(auto mask : maskList->ImageFiles){
			std::string fullPath = m_DirName + "/" + mask.path;
			if(!CheckFileExistence(fullPath)){
				itkWarningMacro("Cannot find filename " << fullPath);
			}
			imgFileList.push_back(fullPath);//return full path
		}
		return imgFileList;
	}
	return {};
}

productReturnType MuscateMetadataHelper::getImageFileName(){
	return getAllImageFileNames("Flat_Reflectance");
}

productReturnType MuscateMetadataHelper::getAotFileName(){
	productReturnType maskList = getAllImageFileNames("Aerosol_Optical_Thickness");
	if(maskList.size() > getNumberOfResolutions()){
		removeDuplicateElements(maskList);
	}
	return maskList;
}
productReturnType MuscateMetadataHelper::getCloudFileName(){
	productReturnType maskList = getAllMaskFileNames("Detailed_Cloud");
	if(maskList.size() == 0){
		maskList = getAllMaskFileNames("Cloud");
	}
	if(maskList.size() > getNumberOfResolutions()){
		removeDuplicateElements(maskList);
	}
	return maskList;
}
productReturnType MuscateMetadataHelper::getWaterFileName(){
	return getQualityFileName(); //They located in the same file
}
productReturnType MuscateMetadataHelper::getSnowFileName(){
	return getQualityFileName(); //They located in the same file
}
productReturnType MuscateMetadataHelper::getQualityFileName(){
	productReturnType maskList = getAllMaskFileNames("Geophysics");
	if(maskList.size() == 0){
		maskList = getAllMaskFileNames("Cloud_Shadow");
	}
	if(maskList.size() > getNumberOfResolutions()){
		removeDuplicateElements(maskList);
	}
	return maskList;
}
std::string MuscateMetadataHelper::getFileNameByString(const productReturnType &vec, const std::string &toFind){
	auto el = std::find_if(vec.begin(), vec.end(), [toFind](const std::string& i)->bool{return i.find(toFind) != std::string::npos;});
	return *el;
}

void MuscateMetadataHelper::removeDuplicateElements(productReturnType &vec){
	vec.erase( unique( vec.begin(), vec.end() ), vec.end() );
}

bool MuscateMetadataHelper::CheckFileExistence(std::string &fileName) {
	struct stat buffer;
	return (stat(fileName.c_str(), &buffer) == 0);
}

std::shared_ptr<MuscateFileMetadata> MuscateMetadataHelper::getFullMetadata(){
	return m_metadata;
}
