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

#include "ProductCreatorSentinelMuscate.h"
#include "GlobalDefs.h"
#include <iostream>
#include "otbImageFileWriter.h"
#include <unordered_map>

using namespace ts;

bool ProductCreatorSentinelMuscate::createProduct(const std::string &destination){
	m_strDestRoot = destination;
	std::cout << "Sentinel ProductFormatter: " << destination << std::endl;
	LoadAllDescriptors(m_xml);

	m_currentDateTime = currentDateTime(); //Production Date

	std::string productVersion = m_version.toString();
	m_strProductDirectoryName = BuildProductDirectoryName(std::string(PRODUCT_DISTRIBUTION_FLAG), productVersion);
	m_strProductFullPathOut = m_strDestRoot + "/" + m_strProductDirectoryName;
	//create folder hierarchy
	bool bDirStructBuiltOk = createAllFolders(m_strProductFullPathOut);
	//Get All composite files: Refls, Weights, Flags, Dates and RGB

	//Determine which product is for which resolution by checking the spacing
	std::string productr1, productr2 = NONE_STRING;
	for(std::string productFilename : m_products){
		ShortVectorImageReaderType::Pointer reader = ShortVectorImageReaderType::New();
		reader->SetFileName(productFilename);
		ShortVectorImageType::Pointer productRaster = reader->GetOutput();
		reader->UpdateOutputInformation();
		std::cout << "Spacing is: " << productRaster->GetSpacing()[0] << std::endl;
		int resolution = productRaster->GetSpacing()[0];
		if(resolution == S2_RESOLUTION_R1){
			productr1 = productFilename;
		}else if(resolution  == S2_RESOLUTION_R2){
			productr2 = productFilename;
		}else{
			itkExceptionMacro("Unknown resolution encountered in ProductFormatter: " << resolution);
		}
	}
	if(productr1 == NONE_STRING || productr2 == NONE_STRING){
		itkExceptionMacro("Cannot find filename for both S2 resolutions. R1: " << productr1 << " R2: " << productr2 << ".");
	}else{
		std::cout << "Found products R1: " << productr1 << " R2: " << productr2 << std::endl;
	}


	///////////
	/// R1 ///
	/////////

	ShortVectorImageReaderType::Pointer readerR1 = ShortVectorImageReaderType::New();
	readerR1->SetFileName(productr1);
	ShortVectorImageType::Pointer reflsImgR1 = readerR1->GetOutput();

	ExtractRasterType::Pointer splitterReflsR1 = ExtractRasterType::New();
	ExtractMaskType::Pointer splitterMasksR1 = ExtractMaskType::New();
	splitterReflsR1->SetInput(reflsImgR1);
	splitterReflsR1->UpdateOutputInformation();
	splitterMasksR1->SetInput(reflsImgR1);
	splitterMasksR1->UpdateOutputInformation();
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR1, 0, COMPOSITE_WEIGHTS_RASTER, S2_R1, true);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR1, 1, COMPOSITE_DATES_MASK, S2_R1, true);
	bDirStructBuiltOk = copyRaster<ExtractMaskType>(splitterMasksR1, 2, COMPOSITE_FLAGS_MASK, S2_R1, true);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR1, 3, COMPOSITE_REFLECTANCE_RASTER, S2_B2, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR1, 4, COMPOSITE_REFLECTANCE_RASTER, S2_B3, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR1, 5, COMPOSITE_REFLECTANCE_RASTER, S2_B4, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR1, 6, COMPOSITE_REFLECTANCE_RASTER, S2_B8, false);

	///////////
	/// R2 ///
	/////////

	ShortVectorImageReaderType::Pointer readerR2 = ShortVectorImageReaderType::New();
	readerR2->SetFileName(productr2);
	ShortVectorImageType::Pointer reflsImgR2 = readerR2->GetOutput();

	ExtractRasterType::Pointer splitterReflsR2 = ExtractRasterType::New();
	ExtractMaskType::Pointer splitterMasksR2 = ExtractMaskType::New();
	splitterReflsR2->SetInput(reflsImgR2);
	splitterReflsR2->UpdateOutputInformation();
	splitterMasksR2->SetInput(reflsImgR2);
	splitterMasksR2->UpdateOutputInformation();
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR2, 0, COMPOSITE_WEIGHTS_RASTER, S2_R2, true);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR2, 1, COMPOSITE_DATES_MASK, S2_R2, true);
	bDirStructBuiltOk = copyRaster<ExtractMaskType>(splitterMasksR2,   2, COMPOSITE_FLAGS_MASK, S2_R2, true);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR2, 3, COMPOSITE_REFLECTANCE_RASTER, S2_B5, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR2, 4, COMPOSITE_REFLECTANCE_RASTER, S2_B6, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR2, 5, COMPOSITE_REFLECTANCE_RASTER, S2_B7, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsR2, 6, COMPOSITE_REFLECTANCE_RASTER, S2_B8A, false);
	copyRaster<ExtractRasterType>(splitterReflsR2, 7, COMPOSITE_REFLECTANCE_RASTER, S2_B11, false);
	copyRaster<ExtractRasterType>(splitterReflsR2, 8, COMPOSITE_REFLECTANCE_RASTER, S2_B12, false);

	//////////////////////////////////
	/// Count Cloudy/Snowy Pixels ///
	////////////////////////////////

	uint64_t cloudCounter = 0, snowCounter = 0, fullCounter = 0;
	size_t maskIndex = 2; //FLG is the third image of the UpdateSynthesis raster
	bDirStructBuiltOk = countSpecialPixels(splitterMasksR2->GetOutput()->GetNthElement(maskIndex).GetPointer(), cloudCounter, snowCounter, fullCounter);
	double valCloudPercent = 100.0 * cloudCounter/fullCounter;
	double valSnowPercent = 100.0 * snowCounter/fullCounter;
	std::cout.setf(std::ios::fixed,std::ios::floatfield);
	std::cout.precision(3);
	std::cout << "Amount of snow   in the final image: " << valSnowPercent << "%" << std::endl;
	std::cout << "Amount of clouds in the final image: " << valCloudPercent << "%" << std::endl;

	///////////////////////////
	/// Generate Quicklook ///
	/////////////////////////

	//Quicklook - Has to be an rgb image cause OTBQuicklook can not take multiple separate images as input
	std::string quicklookPath = m_strProductDirectoryName + "_" + QUICKLOOK_SUFFIX + JPEG_EXTENSION;
	std::vector<int> s2RGBBands = {6,5,4};
	int offset = 2;
	bDirStructBuiltOk = bDirStructBuiltOk? generateQuicklook(productr1, quicklookPath, s2RGBBands, offset) : false;

	if(m_gipp.empty() == false){
		bDirStructBuiltOk = bDirStructBuiltOk? copyGIPPFile(m_gipp) : false;
	}

	if(bDirStructBuiltOk)
	{
		const std::string strMetadataFileName = m_strProductFullPathOut + "/" + m_strProductDirectoryName + "_" + METADATA_CATEG + XML_EXTENSION;
		std::string cloudPercent = std::to_string(int(std::round(valCloudPercent)));
		std::string snowPercent = std::to_string(int(std::round(valSnowPercent)));
		generateMetadataFile(strMetadataFileName, cloudPercent, snowPercent);
		std::cout << "Successfully created the L3A product" << std::endl;
		return true;
	}
	return false;
}

void ProductCreatorSentinelMuscate::LoadAllDescriptors(const std::vector<std::string> &descriptors){
	// load all descriptors
	MuscateMetadataReaderType::Pointer MuscateMetadataReader = MuscateMetadataReaderType::New();
	bool hasSentinel2A = false, hasSentinel2B = false, hasSentinel2X = false;//Array for all possible Missions: S2A,S2B,S2X
	std::string geoInfoXML = NONE_STRING;
	for (const std::string& desc : descriptors) {
		std::unique_ptr<MuscateMetadataHelper> muscate(new MuscateMetadataHelper);
		if (muscate->LoadMetadataFile(desc)) {
			auto meta = muscate->getFullMetadata();
			//only if platform known, add the information to the list:
			if(muscate->GetMissionName().find(SENTINEL2A) !=std::string::npos){
				hasSentinel2A = true;
			}else if(muscate->GetMissionName().find(SENTINEL2B) !=std::string::npos){
				hasSentinel2B = true;
			}else if(muscate->GetMissionName().find(SENTINEL2X) !=std::string::npos){
				hasSentinel2X = true;
			}else{
				itkExceptionMacro("Unknown mission: " + muscate->GetMissionName());
			}
			geoInfoXML = desc;
			FillMetadataInfo(meta);
		} else {
			itkExceptionMacro("Unable to read metadata from " << desc);
		}
	}
	//All files have been read
	//First, check if all Products operate on the same tile (if not quit):
	m_productMetadata.DatasetIdentification.GeographicalZone = m_tileID;
	//Then determine the platform name
	if(hasSentinel2X || (hasSentinel2A && hasSentinel2B)){
		m_mission = S2_Only;
	}else if(hasSentinel2A && !hasSentinel2B){
		m_mission = S2A_Only;
	}else if(!hasSentinel2A && hasSentinel2B){
		m_mission = S2B_Only;
	}else{
		itkExceptionMacro("Fatal Error: Unknown Mission configuration encountered.");
	}
	//Set first Metadata Infos, cause they're used before the whole xml is created:
	m_productMetadata.ProductCharacteristics.Platform = getPlatform(m_mission); //set main platform name
	m_productMetadata.ProductCharacteristics.ProductLevel = "L3A"; //set output level
	m_productMetadata.RadiometricInformations.Bands = getMissionBands(); //set the bands available for this mission
	m_productMetadata.GeopositionInformations = GetGeopositionInformations(geoInfoXML);
}

MuscateGeopositionInformations ProductCreatorSentinelMuscate::GetGeopositionInformations(const std::string &desc){
	std::unique_ptr<MuscateMetadataHelper> muscate(new MuscateMetadataHelper);
	if (muscate->LoadMetadataFile(desc)) {
		auto meta = muscate->getFullMetadata();
		//Delete Group R3 if it's still there - Bug from older Muscate products
		for (std::vector<GroupPositioning>::iterator it=meta->GeopositionInformations.GroupPosition.begin();
				it != meta->GeopositionInformations.GroupPosition.end();){
			if(it->groupID == std::string("R3")){
				it =  meta->GeopositionInformations.GroupPosition.erase(it);
				break;
			}else{
				++it;
			}
		}
		return meta->GeopositionInformations;
	}

	itkExceptionMacro("Error Reading GeopositionInformations from " << desc);
}

std::vector<BandInformation> ProductCreatorSentinelMuscate::getMissionBands(){
	return {
		BandInformation("B2", "10", "m"),
				BandInformation("B3", "10", "m"),
				BandInformation("B4", "10", "m"),
				BandInformation("B5", "20", "m"),
				BandInformation("B6", "20", "m"),
				BandInformation("B7", "20", "m"),
				BandInformation("B8", "10", "m"),
				BandInformation("B8A", "20", "m"),
				BandInformation("B11", "20", "m"),
				BandInformation("B12", "20", "m")
	};
}

std::string ProductCreatorSentinelMuscate::BuildProductDirectoryName(const std::string &productType, const std::string &productVersion) {
	return BuildFileNameSentinel(convertDateTime(m_syntDate), productType, productVersion);
}

std::string ProductCreatorSentinelMuscate::BuildFileNameSentinel(const std::string &datetime, const std::string &productType, const std::string &productVersion) {
	std::string strFileName = "{platformName}_{acquisitionDate}_{productLevel}_{tileId}_{productType}_V{productVersion}";
	strFileName = ReplaceString(strFileName, "{platformName}", m_productMetadata.ProductCharacteristics.Platform);
	strFileName = ReplaceString(strFileName, "{acquisitionDate}", datetime);
	strFileName = ReplaceString(strFileName, "{productLevel}", m_productMetadata.ProductCharacteristics.ProductLevel);
	strFileName = ReplaceString(strFileName, "{tileId}", m_productMetadata.DatasetIdentification.GeographicalZone);
	strFileName = ReplaceString(strFileName, "{productType}", productType);
	strFileName = ReplaceString(strFileName, "{productVersion}", productVersion);

	return strFileName;
}


template<typename ExtractorType>
bool ProductCreatorSentinelMuscate::copyRaster(typename ExtractorType::Pointer splitter, const size_t &index,
		const rasterTypes &rasterType, resolutionTypesS2 resolution, const bool &bIsMask){

	std::unordered_map<int, std::string> rasterNameMap = {
			{COMPOSITE_REFLECTANCE_RASTER, std::string(COMPOSITE_REFLECTANCE_SUFFIX)},
			{COMPOSITE_DATES_MASK, std::string(COMPOSITE_DATES_SUFFIX)},
			{COMPOSITE_FLAGS_MASK, std::string(COMPOSITE_FLAGS_SUFFIX)},
			{COMPOSITE_WEIGHTS_RASTER, std::string(COMPOSITE_WEIGHTS_SUFFIX)}};
	if(m_productMetadata.RadiometricInformations.Bands.empty()){
		itkExceptionMacro("Mission bands not set yet!")
	}
	std::vector<std::string> bandType;
	if(bIsMask || resolution < 0){
		resolution = resolutionTypesS2(int(resolution) + 2);
		bandType = {"R1", "R2"};
	}else{
		for(auto band : m_productMetadata.RadiometricInformations.Bands){
			bandType.emplace_back(band.bandID);
		}
	}
	//Add MASK-folder path if needed
	std::string filename = (bIsMask ? std::string(MASKS_FOLDER_NAME) + "/" : "") +
			m_strProductDirectoryName + "_" +
			rasterNameMap[rasterType] + "_" +
			bandType[resolution] +
			TIF_EXTENSION;
	std::string maskFullPathOut = m_strProductFullPathOut + "/" + filename;
	std::cout << "Writing " << maskFullPathOut << std::endl;
	bool copied = false;
	if(m_cog){
		copied = writeExtractedImageCoGeoTiff<ExtractorType>(splitter, m_cogtemp, maskFullPathOut, index);
	}else{
		copied = writeExtractedImage<ExtractorType>(splitter,
				maskFullPathOut + (bIsMask ? "?&gdal:co:COMPRESS=DEFLATE" : ""),
				index);
	}
	if(copied){
		//create Metadatafile
		switch(rasterType){
		case COMPOSITE_REFLECTANCE_RASTER:
		{
			ImageInformation imageFile;
			imageFile.hasBandNum = false;
			imageFile.hasBitNum = false;
			imageFile.hasDetectorID = false;
			imageFile.isMask = false;
			imageFile.isGroup = false;
			imageFile.bandIDs = bandType[resolution];
			imageFile.path = filename;
			m_FRCs.emplace_back(imageFile);
			return true;
		}
		case COMPOSITE_DATES_MASK:
		{
			ImageInformation imageFile;
			imageFile.hasBandNum = false;
			imageFile.hasBitNum = false;
			imageFile.hasDetectorID = false;
			imageFile.isMask = false;
			imageFile.isGroup = true;
			imageFile.bandIDs = bandType[resolution];
			imageFile.path = filename;
			m_DTSs.emplace_back(imageFile);
			return true;
		}
		case COMPOSITE_FLAGS_MASK:
		{
			ImageInformation imageFile;
			imageFile.hasBandNum = false;
			imageFile.hasBitNum = false;
			imageFile.hasDetectorID = false;
			imageFile.isMask = false;
			imageFile.isGroup = true;
			imageFile.bandIDs = bandType[resolution];
			imageFile.path = filename;
			m_FLGs.emplace_back(imageFile);
			return true;
		}
		case COMPOSITE_WEIGHTS_RASTER:
		{
			ImageInformation imageFile;
			imageFile.hasBandNum = false;
			imageFile.hasBitNum = false;
			imageFile.hasDetectorID = false;
			imageFile.isMask = false;
			imageFile.isGroup = true;
			imageFile.bandIDs = bandType[resolution];
			imageFile.path = filename;
			m_WGTs.emplace_back(imageFile);
			return true;
		}
		default:
			itkExceptionMacro("Unkown Raster Type found: " << rasterNameMap[rasterType]);
		}
	}
	itkWarningMacro("Error creating metadata info for " << filename);

	return false;
}

std::vector<BandGroup> ProductCreatorSentinelMuscate::getBandGroupList(const std::vector<BandInformation> &missionBands){
	std::vector<BandGroup> result;
	if(missionBands.empty()){
		itkExceptionMacro("Mission bands not set!");
	}
	std::vector<size_t> bandsR1 = {0,1,2,6}; //band B2,3,4,8
	std::vector<size_t> bandsR2 = {3,4,5,7,8,9};//band B5,6,7,8A,11,12

	BandGroup R1;
	R1.GroupID = "R1";
	for(size_t i : bandsR1){
		R1.GroupList.bandIDs.emplace_back(missionBands[i].bandID);
	}
	R1.GroupList.Count = R1.GroupList.bandIDs.size();
	result.emplace_back(R1);
	BandGroup R2;
	R2.GroupID = "R2";
	for(size_t i : bandsR2){
		R2.GroupList.bandIDs.emplace_back(missionBands[i].bandID);
	}
	R2.GroupList.Count = R2.GroupList.bandIDs.size();
	result.emplace_back(R2);
	return result;

}

void ProductCreatorSentinelMuscate::generateMetadataFile(const std::string &strProductMetadataFilePath, const std::string &cloudPercent, const std::string &snowPercent){
	auto writer = ts::muscate::MuscateMetadataWriter::New();

	//Header
	m_productMetadata.Header.SchemaLocation = "Muscate_Metadata.xsd";
	m_productMetadata.Header.xsi = "http://www.w3.org/2001/XMLSchema-instance";
	//MetadataIdentification
	m_productMetadata.MetadataIdentification.MetadataFormat = "METADATA_MUSCATE";
	m_productMetadata.MetadataIdentification.version = "1.0";
	m_productMetadata.MetadataIdentification.MetadataProfile = "GENERIC";
	m_productMetadata.MetadataIdentification.MetadataInformation = "EXPERT";
	//DatasetIdentification
	m_productMetadata.DatasetIdentification.Identifier = m_strProductDirectoryName.substr(0, m_strProductDirectoryName.find("_V"));
	m_productMetadata.DatasetIdentification.Authority = "THEIA";
	m_productMetadata.DatasetIdentification.Producer = "MUSCATE";
	std::string platform = getPlatform(m_mission);
	//Remove last char from platform if needed (In case of S2X, S2A, S2B)
	if(m_mission != Multisat){
		m_productMetadata.DatasetIdentification.Project = platform.substr(0, platform.size()-1);
	}else{
		m_productMetadata.DatasetIdentification.Project = platform;
	}
	//TileID is already set
	m_productMetadata.DatasetIdentification.GeographicalZoneType = "Tile";
	m_productMetadata.ProductCharacteristics.ProductID = m_strProductDirectoryName;
	m_productMetadata.ProductCharacteristics.AcquisitionDate = m_syntDate;
	m_productMetadata.ProductCharacteristics.hasSynthesisDate = false;
	m_productMetadata.ProductCharacteristics.ProductionDate = m_currentDateTime;
	m_productMetadata.ProductCharacteristics.ProductVersion = m_version.toString(".");
	m_productMetadata.ProductCharacteristics.OrbitNumber = "-1";
	m_productMetadata.ProductCharacteristics.OrbitType = "Orbit";
	m_productMetadata.ProductCharacteristics.MeanAcquisitionRange = m_syntDate;
	m_productMetadata.ProductCharacteristics.DatePrecision = "0.001";
	m_productMetadata.ProductCharacteristics.DatePrecisionUnit = "s";
	//SyntPeriod is currently not supported in Theia (Feb. 18)
	m_productMetadata.ProductCharacteristics.SynthesisPeriodBegin = m_syntPeriodBegin;
	m_productMetadata.ProductCharacteristics.SynthesisPeriodEnd = m_syntPeriodEnd;
	m_productMetadata.ProductCharacteristics.BandGlobalList = getBandGlobalList(m_productMetadata.RadiometricInformations.Bands);
	m_productMetadata.ProductCharacteristics.BandGroupList = getBandGroupList(m_productMetadata.RadiometricInformations.Bands);

	ImageProperty frc;
	frc.Compression = "None";
	frc.Encoding = "int16";
	frc.Endianness = "LittleEndian";
	frc.Format = "image/tiff";
	frc.Nature = "Flat_Reflectance_Composite";
	frc.hasCompression = true;
	if(m_cog){
		frc.Compression = "DEFLATE";
		frc.Description = "CloudOptimized-GeoTiff";
	}
	frc.ImageFiles = m_FRCs;
	m_productMetadata.ProductOrganisation.ImageProperties.emplace_back(frc);
	ImageProperty dts;
	dts.hasCompression = true;
	dts.Compression = "DEFLATE";
	dts.Nature = "Weighted_Average_Dates";
	dts.Format = "image/tiff";
	dts.Endianness = "LittleEndian";
	dts.Encoding = "int16";
	if(m_cog){
		dts.Description = "CloudOptimized-GeoTiff";
	}
	dts.ImageFiles = m_DTSs;
	m_productMetadata.ProductOrganisation.MaskProperties.emplace_back(dts);
	ImageProperty wgt;
	wgt.hasCompression = true;
	wgt.Compression = "DEFLATE";
	wgt.Nature = "Pixel_Weight";
	wgt.Format = "image/tiff";
	wgt.Endianness = "LittleEndian";
	wgt.Encoding = "int16";
	if(m_cog){
		wgt.Description = "CloudOptimized-GeoTiff";
	}
	wgt.ImageFiles = m_WGTs;
	m_productMetadata.ProductOrganisation.MaskProperties.emplace_back(wgt);
	ImageProperty flg;
	flg.hasCompression = true;
	flg.Compression = "DEFLATE";
	flg.Nature = "Pixel_Status_Flag";
	flg.Format = "image/tiff";
	flg.Endianness = "LittleEndian";
	flg.Encoding = "byte";
	if(m_cog){
		flg.Description = "CloudOptimized-GeoTiff";
	}
	flg.ImageFiles = m_FLGs;
	m_productMetadata.ProductOrganisation.MaskProperties.emplace_back(flg);

	m_productMetadata.RadiometricInformations.quantificationValues = {SpecialValue("REFLECTANCE_QUANTIFICATION_VALUE", "10000")};
	m_productMetadata.RadiometricInformations.specialValues = {SpecialValue("nodata", std::string(NO_DATA_VALUE_STR))};
	MuscateQualityProduct current;
	current.Level = "N3";
	current.AcquisitionDate = m_syntDate;
	current.ProductionDate = m_currentDateTime;
	current.ProductID = m_strProductDirectoryName;
	current.GlobalIndexList = {MuscateQualityIndex("CloudPercent", cloudPercent), MuscateQualityIndex("SnowPercent", snowPercent)};
	m_productMetadata.QualityInformations.CurrentProduct = current;
	writer->WriteMetadata(m_productMetadata, strProductMetadataFilePath);

}


