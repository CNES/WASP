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

#include "ProductCreatorVenusMuscate.h"
#include "GlobalDefs.h"
#include <unordered_map>

using namespace ts;

bool ProductCreatorVenusMuscate::createProduct(const std::string &destination){
	m_strDestRoot = destination;
	std::cout << "Venus ProductFormatter: " << destination << std::endl;
	LoadAllDescriptors(m_xml);

	m_currentDateTime = currentDateTime(); //Production Date

	std::string productVersion = m_version.toString();
	m_strProductDirectoryName = BuildProductDirectoryName(std::string(PRODUCT_DISTRIBUTION_FLAG), productVersion);
	m_strProductFullPathOut = m_strDestRoot + "/" + m_strProductDirectoryName;
	//create folder hierarchy
	bool bDirStructBuiltOk = createAllFolders(m_strProductFullPathOut);
	//Get All composite files: Refls, Weights, Flags, Dates and RGB

	std::string productxs = m_products[0];
	if(productxs == NONE_STRING){
		itkExceptionMacro("Cannot find filename for Venus XS resolution: " << productxs);
	}else{
		std::cout << "Found products XS: " << productxs << std::endl;
	}

	///////////
	/// R1 ///
	/////////

	ShortVectorImageReaderType::Pointer readerXS = ShortVectorImageReaderType::New();
	readerXS->SetFileName(productxs);
	ShortVectorImageType::Pointer reflsImgXS = readerXS->GetOutput();

	ExtractRasterType::Pointer splitterReflsXS = ExtractRasterType::New();
	ExtractMaskType::Pointer splitterMasksXS = ExtractMaskType::New();
	splitterReflsXS->SetInput(reflsImgXS);
	splitterReflsXS->UpdateOutputInformation();
	splitterMasksXS->SetInput(reflsImgXS);
	splitterMasksXS->UpdateOutputInformation();
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 0, COMPOSITE_WEIGHTS_RASTER, VNS_XS, true);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 1, COMPOSITE_DATES_MASK, VNS_XS, true);
	bDirStructBuiltOk = copyRaster<ExtractMaskType>(splitterMasksXS, 2, COMPOSITE_FLAGS_MASK, VNS_XS, true);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 3, COMPOSITE_REFLECTANCE_RASTER, VNS_B1, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 4, COMPOSITE_REFLECTANCE_RASTER, VNS_B2, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 5, COMPOSITE_REFLECTANCE_RASTER, VNS_B3, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 6, COMPOSITE_REFLECTANCE_RASTER, VNS_B4, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 7, COMPOSITE_REFLECTANCE_RASTER, VNS_B5, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 8, COMPOSITE_REFLECTANCE_RASTER, VNS_B6, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 9, COMPOSITE_REFLECTANCE_RASTER, VNS_B7, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 10, COMPOSITE_REFLECTANCE_RASTER, VNS_B8, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 11, COMPOSITE_REFLECTANCE_RASTER, VNS_B9, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 12, COMPOSITE_REFLECTANCE_RASTER, VNS_B10, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 13, COMPOSITE_REFLECTANCE_RASTER, VNS_B11, false);
	bDirStructBuiltOk = copyRaster<ExtractRasterType>(splitterReflsXS, 14, COMPOSITE_REFLECTANCE_RASTER, VNS_B12, false);


	//////////////////////////////////
	/// Count Cloudy/Snowy Pixels ///
	////////////////////////////////

	uint64_t cloudCounter = 0, snowCounter = 0, fullCounter = 0;
	size_t maskIndex = 2; //FLG is the third image of the UpdateSynthesis raster
	bDirStructBuiltOk = countSpecialPixels(splitterMasksXS->GetOutput()->GetNthElement(maskIndex).GetPointer(), cloudCounter, snowCounter, fullCounter);
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
	std::vector<int> vnsRGBBands = {10,7,6};
	int offset = 3;
	bDirStructBuiltOk = bDirStructBuiltOk? generateQuicklook(productxs, quicklookPath, vnsRGBBands, offset) : false;

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
	return false;}


void ProductCreatorVenusMuscate::LoadAllDescriptors(const std::vector<std::string> &descriptors){
	// load all descriptors
	MuscateMetadataReaderType::Pointer MuscateMetadataReader = MuscateMetadataReaderType::New();
	std::string geoInfoXML = NONE_STRING;
	for (const std::string& desc : descriptors) {
		std::unique_ptr<MuscateMetadataHelper> muscate(new MuscateMetadataHelper);
		if (muscate->LoadMetadataFile(desc)) {
			auto meta = muscate->getFullMetadata();
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
	m_mission = Venus;
	//Set first Metadata Infos, cause they're used before the whole xml is created:
	m_productMetadata.ProductCharacteristics.Platform = getPlatform(m_mission); //set main platform name
	m_productMetadata.ProductCharacteristics.ProductLevel = "L3A"; //set output level
	m_productMetadata.RadiometricInformations.Bands = getMissionBands(); //set the bands available for this mission
	m_productMetadata.GeopositionInformations = GetGeopositionInformations(geoInfoXML);
}

MuscateGeopositionInformations ProductCreatorVenusMuscate::GetGeopositionInformations(const std::string &desc){
	std::unique_ptr<MuscateMetadataHelper> muscate(new MuscateMetadataHelper);
	if (muscate->LoadMetadataFile(desc)) {
		auto meta = muscate->getFullMetadata();
		if(meta->GeopositionInformations.GroupPosition.size() > 1){
			itkExceptionMacro("More than one Group_Geopositioning found for Venus in " << desc);
		}
		return meta->GeopositionInformations;
	}

	itkExceptionMacro("Error Reading GeopositionInformations from " << desc);
}

std::vector<BandInformation> ProductCreatorVenusMuscate::getMissionBands(){
	return {
		BandInformation("B1", "10", "m"),
				BandInformation("B2", "10", "m"),
				BandInformation("B3", "10", "m"),
				BandInformation("B4", "10", "m"),
				BandInformation("B5", "10", "m"),
				BandInformation("B6", "10", "m"),
				BandInformation("B7", "10", "m"),
				BandInformation("B8", "10", "m"),
				BandInformation("B9", "10", "m"),
				BandInformation("B10", "10", "m"),
				BandInformation("B11", "10", "m"),
				BandInformation("B12", "10", "m")
	};
}

std::string ProductCreatorVenusMuscate::BuildProductDirectoryName(const std::string &productType, const std::string &productVersion) {
	return BuildFileNameVenus(convertDateTime(m_syntDate), productType, productVersion);
}

std::string ProductCreatorVenusMuscate::BuildFileNameVenus(const std::string &datetime, const std::string &productType, const std::string &productVersion) {
	std::string strFileName = "{platformName}-XS_{acquisitionDate}_{productLevel}_{zone}_{productType}_V{productVersion}";
	strFileName = ReplaceString(strFileName, "{platformName}", m_productMetadata.ProductCharacteristics.Platform);
	strFileName = ReplaceString(strFileName, "{acquisitionDate}", datetime);
	strFileName = ReplaceString(strFileName, "{productLevel}", m_productMetadata.ProductCharacteristics.ProductLevel);
	strFileName = ReplaceString(strFileName, "{zone}", m_productMetadata.DatasetIdentification.GeographicalZone);
	strFileName = ReplaceString(strFileName, "{productType}", productType);
	strFileName = ReplaceString(strFileName, "{productVersion}", productVersion);

	return strFileName;
}

template<typename ExtractorType>
bool ProductCreatorVenusMuscate::copyRaster(typename ExtractorType::Pointer splitter, const size_t &index,
		const rasterTypes &rasterType, resolutionTypesVenus resolution, const bool &bIsMask){
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
		resolution = resolutionTypesVenus(int(resolution) + 1);
		bandType = {"XS"};
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

std::vector<BandGroup> ProductCreatorVenusMuscate::getBandGroupList(const std::vector<BandInformation> &missionBands){
	std::vector<BandGroup> result;
	if(missionBands.empty()){
		itkExceptionMacro("Mission bands not set!");
	}
	std::vector<size_t> bandsXS = {0,1,2,3,4,5,6,7,8,9,10,11};

	BandGroup XS;
	XS.GroupID = "XS";
	for(size_t i : bandsXS){
		XS.GroupList.bandIDs.emplace_back(missionBands[i].bandID);
	}
	XS.GroupList.Count = XS.GroupList.bandIDs.size();
	result.emplace_back(XS);

	return result;

}


void ProductCreatorVenusMuscate::generateMetadataFile(const std::string &strProductMetadataFilePath, const std::string &cloudPercent, const std::string &snowPercent){
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
	m_productMetadata.DatasetIdentification.Project = getPlatform(m_mission);
	//TileID is already set
	m_productMetadata.DatasetIdentification.GeographicalZoneType = "Site";
	m_productMetadata.ProductCharacteristics.ProductID = m_strProductDirectoryName;
	m_productMetadata.ProductCharacteristics.AcquisitionDate = m_syntDate;
	m_productMetadata.ProductCharacteristics.hasSynthesisDate = false;
	m_productMetadata.ProductCharacteristics.ProductionDate = m_currentDateTime;
	m_productMetadata.ProductCharacteristics.ProductVersion = m_version.toString(".");
	m_productMetadata.ProductCharacteristics.SpectralContent = "XS";
	m_productMetadata.ProductCharacteristics.OrbitNumber = "-1";
	m_productMetadata.ProductCharacteristics.OrbitType = "Orbit";
	m_productMetadata.ProductCharacteristics.MeanAcquisitionRange = m_syntDate;
	m_productMetadata.ProductCharacteristics.DatePrecision = "1";
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

	m_productMetadata.RadiometricInformations.quantificationValues = {SpecialValue("REFLECTANCE_QUANTIFICATION_VALUE", "1000")};
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




