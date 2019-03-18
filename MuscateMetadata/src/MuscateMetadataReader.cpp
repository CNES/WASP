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

#include <limits>
#include <libgen.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include "otbMacro.h"

#include "MuscateMetadataReader.hpp"
#include "tinyxml_utils.hpp"
#include "string_utils.hpp"
#include "MetadataUtil.hpp"

namespace ts {
namespace muscate {

std::unique_ptr<MuscateFileMetadata> MuscateMetadataReader::ReadMetadata(const std::string &path){
	TiXmlDocument doc(path);
	if (!doc.LoadFile()) {
		return nullptr;
	}
	auto metadata = ReadMetadataXml(doc);
	if (metadata) {
		metadata->ProductPath = path;
	}

	return metadata;
}

MuscateMetadataIdentification MuscateMetadataReader::readMetadataIdentification(TiXmlElement *el){
	MuscateMetadataIdentification result;

	if (!el) {
		return result;
	}

	result.MetadataFormat = GetChildText(el, "METADATA_FORMAT");
	result.version = GetChildAttribute(el, "METADATA_FORMAT", "version");
	result.MetadataProfile = GetChildText(el, "METADATA_PROFILE");
	result.MetadataInformation = GetChildText(el, "METADATA_INFORMATION");

	return result;
}

MuscateDatasetIdentification MuscateMetadataReader::readDatasetIdentification(TiXmlElement *el){
	MuscateDatasetIdentification result;

	if (!el) {
		return result;
	}

	result.Identifier = GetChildText(el, "IDENTIFIER");
	result.Authority = GetChildText(el, "AUTHORITY");
	result.Producer = GetChildText(el, "PRODUCER");
	result.Project = GetChildText(el, "PROJECT");
	result.GeographicalZone = GetChildText(el, "GEOGRAPHICAL_ZONE");
	result.GeographicalZoneType = GetChildAttribute(el, "GEOGRAPHICAL_ZONE", "type");
	result.Title = GetChildText(el, "TITLE");
	result.Description = GetChildText(el, "DESCRIPTION");

	return result;
}

/**
 * @brief Read all BAND_ID children and the count attribute from a given subtree
 * @param el The Parent, containing all BAND_ID children
 * @return BandList struct, containing the IDs and the count as string
 */
static BandList ReadBands(const TiXmlElement *el) {
	BandList result;

	if (!el) {
		return result;
	}

	if (auto countStr = el->Attribute("count")) {
		size_t count;
		bool countValid;
		try
		{
			count = std::stoi(countStr);
			countValid = true;
		}
		catch (const std::runtime_error &e)
		{
			countValid = false;

			otbMsgDevMacro("Invalid band count value: " << countStr);
			otbMsgDevMacro(<< e.what());
		}
		if(countValid){
			result.Count = count;
			for (auto bandEl = el->FirstChildElement("BAND_ID"); bandEl; bandEl = bandEl->NextSiblingElement("BAND_ID")) {
				result.bandIDs.emplace_back(bandEl->GetText());
			}
		}

	}

	return result;
}

MuscateProductCharacteristics MuscateMetadataReader::readProductCharacteristics(TiXmlElement *el){
	MuscateProductCharacteristics result;

	if (!el) {
		return result;
	}

	result.ProductID = GetChildText(el, "PRODUCT_ID");
	result.AcquisitionDate = GetChildText(el, "ACQUISITION_DATE");
	result.ProductionDate = GetChildText(el, "PRODUCTION_DATE");
	result.ProductVersion = GetChildText(el, "PRODUCT_VERSION");
	result.ProductLevel = GetChildText(el, "PRODUCT_LEVEL");
	result.Platform = GetChildText(el, "PLATFORM");
	if(auto spectralContent = el->FirstChildElement("SPECTRAL_CONTENT")){
		result.SpectralContent = spectralContent->GetText();
	}
	if(auto orbitNumber = el->FirstChildElement("ORBIT_NUMBER")){
		result.OrbitNumber = orbitNumber->GetText();
		result.OrbitType = orbitNumber->Attribute("type");
	}
	if(auto acqRange = el->FirstChildElement("UTC_Acquisition_Range")){
		result.DatePrecision = GetChildText(acqRange, "DATE_PRECISION");
		result.DatePrecisionUnit = GetChildAttribute(acqRange, "DATE_PRECISION", "unit");
		if(auto mean = acqRange->FirstChildElement("MEAN")){
			result.MeanAcquisitionRange = mean->GetText();
			result.hasSynthesisDate = false;
		}else{
			result.hasSynthesisDate = true;
			result.SynthesisDate = GetChildText(acqRange, "SYNTHESIS_DATE");
			result.SynthesisPeriodBegin = GetChildText(acqRange, "SYNTHESIS_PERIOD_BEGIN");
			result.SynthesisPeriodEnd = GetChildText(acqRange, "SYNTHESIS_PERIOD_END");
		}
	}

	if(auto bandGlobalList = el->FirstChildElement("Band_Global_List")){
		result.BandGlobalList = ReadBands(bandGlobalList);
	}
	if(auto bandGroupList = el->FirstChildElement("Band_Group_List")){
		for(auto groupTree = bandGroupList->FirstChildElement("Group"); groupTree; groupTree = groupTree->NextSiblingElement("Group")){
			BandGroup group;
			group.GroupID = groupTree->Attribute("group_id");
			group.GroupList = ReadBands(groupTree->FirstChildElement("Band_List"));
			result.BandGroupList.emplace_back(group);
		}
	}

	return result;
}

/**
 * @brief Read Image properties from metadata
 * @param el The Image_list or Mask_list parent element
 * @param isMask True, if Mask_List shall be read, False if it's an Image_list
 * @return Vector Containing all Images and their properties (Nature, Format, Encoding, etc.)
 */
static std::vector<ImageProperty> readImageProperty(TiXmlElement *el, bool isMask){
	std::vector<ImageProperty> result;
	if(!el){
		return result;
	}

	for(auto image = el->FirstChildElement(isMask? "Mask" : "Image"); image; image = image->NextSiblingElement(isMask? "Mask" : "Image")){
		ImageProperty img;
		if(auto imageProperties = image->FirstChildElement(isMask? "Mask_Properties" : "Image_Properties")){
			img.Nature = GetChildText(imageProperties, "NATURE");
			img.Format = GetChildText(imageProperties, "FORMAT");
			img.Encoding = GetChildText(imageProperties, "ENCODING");
			img.Endianness = GetChildText(imageProperties, "ENDIANNESS");
			img.Compression = GetChildText(imageProperties, "COMPRESSION");
			if(!img.Compression.empty()){
				img.hasCompression = true;
			}else{
				img.hasCompression = false;
			}
			if(auto description = imageProperties->FirstChildElement("DESCRIPTION")){
				img.Description = description->GetText();
			}
		}
		if(auto imageList = image->FirstChildElement(isMask? "Mask_File_List" : "Image_File_List")){
			for(auto imageFile = imageList->FirstChildElement(isMask? "MASK_FILE" : "IMAGE_FILE"); imageFile; imageFile = imageFile->NextSiblingElement(isMask? "MASK_FILE" : "IMAGE_FILE")){
				ImageInformation imgInfo;
				if(auto imageInfo = imageFile->Attribute("group_id")){
					imgInfo.bandIDs = imageInfo;
					imgInfo.isGroup = true;
				}else if(auto imageInfo = imageFile->Attribute("band_id")){
					imgInfo.bandIDs = imageInfo;
					imgInfo.isGroup = false;
				}
				if(auto imageInfo = imageFile->Attribute("bit_number")){
					imgInfo.bitNumber = imageInfo;
					imgInfo.hasBitNum = true;
				}else{
					imgInfo.hasBitNum = false;

				}
				if(auto imageInfo = imageFile->Attribute("detector_id")){
					imgInfo.detectorID = imageInfo;
					imgInfo.hasDetectorID = true;
				}else{
					imgInfo.hasDetectorID = false;
				}
				if(auto imageInfo = imageFile->Attribute("band_number")){
					imgInfo.bandNum = imageInfo;
					imgInfo.hasBandNum = true;
				}else{
					imgInfo.hasBandNum = false;
				}
				if(isMask){
					imgInfo.isMask = true;
				}else{
					imgInfo.isMask = false;
				}
				imgInfo.path = imageFile->GetText();
				img.ImageFiles.emplace_back(imgInfo);
			}
		}
		result.emplace_back(img);
	}

	return result;
}

MuscateProductOrganisation MuscateMetadataReader::readProductOrganisation(TiXmlElement *el){
	MuscateProductOrganisation result;

	if(!el){
		return result;
	}
	if(auto muscateProduct = el->FirstChildElement("Muscate_Product")){
		if(auto quicklook = muscateProduct->FirstChildElement("QUICKLOOK")){
			result.Quicklook.bandIDs = quicklook->Attribute("bands_id");
			result.Quicklook.path = quicklook->GetText();
			result.Quicklook.hasBandNum = false;
			result.Quicklook.hasBitNum = false;
			result.Quicklook.hasDetectorID = false;
			result.Quicklook.isGroup = false;
			result.Quicklook.isMask = false;
		}

		if(auto imageList = muscateProduct->FirstChildElement("Image_List")){
			result.ImageProperties = readImageProperty(imageList, false);
		}

		if(auto maskList = muscateProduct->FirstChildElement("Mask_List")){
			result.MaskProperties = readImageProperty(maskList, true);
		}
	}

	return result;
}

/**
 * @brief read the Global_Geopositioning element
 * @param el The parent of the Global_Geopositioning
 * @return Vector containing each GeoPoint element with it's data (name, lat, lon, x and y)
 */
static std::vector<GeoPoint> readGlobalGeopositioning(TiXmlElement *el){
	std::vector<GeoPoint> result;
	if(auto globalGeoposition = el->FirstChildElement("Global_Geopositioning")){
		for(auto point = globalGeoposition->FirstChildElement("Point"); point; point = point->NextSiblingElement("Point")){
			GeoPoint p;
			p.name = GetAttribute(point, "name");
			p.lat = GetChildText(point, "LAT");
			p.lon = GetChildText(point, "LON");
			p.Xpos = GetChildText(point, "X");
			p.Ypos = GetChildText(point, "Y");
			result.emplace_back(p);
		}
	}
	return result;
}

/**
 * @brief read the Group_Geopositioning_List
 * @param el The parent of the Group_Geopositioning_List
 * @return Vector containing each GroupPositioning element with it's data (group_id, ULX, ULY, XDIM...)
 */
static std::vector<GroupPositioning> readGroupGeopositioning(TiXmlElement *el){
	std::vector<GroupPositioning> result;
	if(auto groupGeoposition = el->FirstChildElement("Group_Geopositioning_List")){
		for(auto group = groupGeoposition->FirstChildElement("Group_Geopositioning"); group; group = group->NextSiblingElement("Group_Geopositioning")){
			GroupPositioning p;
			p.groupID = GetAttribute(group, "group_id");
			p.ULX = GetChildText(group, "ULX");
			p.ULY = GetChildText(group, "ULY");
			p.XDim = GetChildText(group, "XDIM");
			p.YDim = GetChildText(group, "YDIM");
			p.nrows = GetChildText(group, "NROWS");
			p.ncols = GetChildText(group, "NCOLS");
			result.emplace_back(p);
		}
	}
	return result;
}

MuscateGeopositionInformations MuscateMetadataReader::readGeopositionInformations(TiXmlElement *el){
	MuscateGeopositionInformations result;

	if (!el) {
		return result;
	}

	if(auto coordReferenceSystem = el->FirstChildElement("Coordinate_Reference_System")){
		result.GeoTables = GetChildText(coordReferenceSystem, "GEO_TABLES");
		if(auto horizontalCoordSystem = coordReferenceSystem->FirstChildElement("Horizontal_Coordinate_System")){
			result.HorizontalCSType = GetChildText(horizontalCoordSystem, "HORIZONTAL_CS_TYPE");
			result.HorizontalCSName = GetChildText(horizontalCoordSystem, "HORIZONTAL_CS_NAME");
			result.HorizontalCSCode = GetChildText(horizontalCoordSystem, "HORIZONTAL_CS_CODE");
		}
	}

	if(auto rasterCS = el->FirstChildElement("Raster_CS")){
		result.RasterCS.CSType = GetChildText(rasterCS, "RASTER_CS_TYPE");
		result.RasterCS.PixelOrigin = GetChildText(rasterCS, "PIXEL_ORIGIN");
	}
	if(auto rasterCS = el->FirstChildElement("Metadata_CS")){
		result.MetadataCS.CSType = GetChildText(rasterCS, "METADATA_CS_TYPE");
		result.MetadataCS.PixelOrigin = GetChildText(rasterCS, "PIXEL_ORIGIN");
	}
	if(auto geopositioning = el->FirstChildElement("Geopositioning")){
		result.GlobalGeoposition = readGlobalGeopositioning(geopositioning);
		result.GroupPosition = readGroupGeopositioning(geopositioning);
	}
	return result;
}

/**
 * @brief read the MeanViewingIncidenceAngles field
 * @param el The parent element
 * @return The MuscateAnglePair containing the given zenith and azimuth values as well as band/detectorID if existing
 */
MuscateAnglePair getMeanViewingIncidenceAngle(TiXmlElement *el){
	MuscateAnglePair result;


	if(auto band = el->Attribute("band_id")){
		result.bandId = band;
	}else if(auto detector = el->Attribute("detector_id")){
		result.detectorId = detector;
	}
	result.AzimuthUnit = GetChildAttribute(el, "AZIMUTH_ANGLE", "unit");
	result.AzimuthValue = ReadDouble(GetChildText(el, "AZIMUTH_ANGLE"));
	result.ZenithUnit = GetChildAttribute(el, "ZENITH_ANGLE", "unit");
	result.ZenithValue = ReadDouble(GetChildText(el, "ZENITH_ANGLE"));

	return result;
}

/**
 * @brief Read a list of space separated doubles to a double-vector
 * @param s The line containing doubles, of any size
 * @return A vector containing the double-values
 */
std::vector<double> ReadDoubleList(const std::string &s){
	std::vector<double> result;
	std::istringstream is(s);
	std::string value;
	while (is >> value) {
		result.emplace_back(ReadDouble(value));
	}

	return result;
}

MuscateAngles getMuscateAngles(TiXmlElement * el){
	MuscateAngles result;
	bool hasZenith, hasAzimuth = false;
	if(auto zenith = el->FirstChildElement("Zenith")){
		hasZenith = true;
		result.Zenith.ColumnUnit = zenith->Attribute("step_unit");
		result.Zenith.RowUnit = zenith->Attribute("step_unit");
		result.Zenith.ColumnStep = GetChildText(zenith, "COL_STEP");
		result.Zenith.RowStep = GetChildText(zenith, "ROW_STEP");
		if(auto valuesList = zenith->FirstChildElement("Values_List")){
			for(auto value = valuesList->FirstChildElement("VALUES");
					value; value = value->NextSiblingElement("VALUES")){
				result.Zenith.Values.emplace_back(ReadDoubleList(value->GetText()));
			}
		}
	}
	if(auto azimuth = el->FirstChildElement("Azimuth")){
		hasAzimuth = true;
		result.Azimuth.ColumnUnit = azimuth->Attribute("step_unit");
		result.Azimuth.RowUnit = azimuth->Attribute("step_unit");
		result.Azimuth.ColumnStep = GetChildText(azimuth, "COL_STEP");
		result.Azimuth.RowStep = GetChildText(azimuth, "ROW_STEP");
		if(auto valuesList = azimuth->FirstChildElement("Values_List")){
			for(auto value = valuesList->FirstChildElement("VALUES");
					value; value = value->NextSiblingElement("VALUES")){
				result.Azimuth.Values.emplace_back(ReadDoubleList(value->GetText()));
			}
		}
	}
	if(!hasAzimuth || !hasZenith){
		otbMsgDevMacro("Muscate Angles missing Zenith or Azimuth component!");
	}

	return result;
}

MuscateGeometricInformations MuscateMetadataReader::readGeometricInformations(TiXmlElement *el){
	MuscateGeometricInformations result;
	if (!el) {
		return result;
	}
	if(auto meanValueList = el->FirstChildElement("Mean_Value_List")){
		if(auto meanSunAngles = meanValueList->FirstChildElement("Sun_Angles")){
			result.MeanSunAngle = getMeanViewingIncidenceAngle(meanSunAngles);
		}
		if(auto meanViewingIndicendeAngles = meanValueList->FirstChildElement("Mean_Viewing_Incidence_Angle_List")){
			for(auto meanAngle = meanViewingIndicendeAngles->FirstChildElement("Mean_Viewing_Incidence_Angle");
					meanAngle; meanAngle = meanAngle->NextSiblingElement("Mean_Viewing_Incidence_Angle")){
				result.MeanViewingIncidenceAngles.emplace_back(getMeanViewingIncidenceAngle(meanAngle));
			}
		}
	}
	if(auto anglesGridList = el->FirstChildElement("Angles_Grids_List")){
		if(auto sunAnglesGrids = anglesGridList->FirstChildElement("Sun_Angles_Grids")){
			result.SolarAngles = getMuscateAngles(sunAnglesGrids);
		}
		if(auto viewingIndicenceAnglesGrids = anglesGridList->FirstChildElement("Viewing_Incidence_Angles_Grids_List")){
			for(auto bandViewingIndicenceAnglesGrids = viewingIndicenceAnglesGrids->FirstChildElement("Band_Viewing_Incidence_Angles_Grids_List");
					bandViewingIndicenceAnglesGrids; bandViewingIndicenceAnglesGrids = bandViewingIndicenceAnglesGrids->NextSiblingElement("Band_Viewing_Incidence_Angles_Grids_List")){
				for(auto viewingIndicenceAnglesGrid = bandViewingIndicenceAnglesGrids->FirstChildElement("Viewing_Incidence_Angles_Grids");
						viewingIndicenceAnglesGrid; viewingIndicenceAnglesGrid = viewingIndicenceAnglesGrid->NextSiblingElement("Viewing_Incidence_Angles_Grids")){
					MuscateViewingAnglesGrid grid;
					if(auto band = bandViewingIndicenceAnglesGrids->Attribute("band_id")){
						grid.bandID = band;
					}
					if(auto detector = viewingIndicenceAnglesGrid->Attribute("detector_id")){
						grid.detectorID = detector;
					}
					grid.ViewIncidenceAnglesGrid = getMuscateAngles(viewingIndicenceAnglesGrid);
					result.ViewingAngles.emplace_back(grid);
				}
			}
		}
	}

	return result;
}

MuscateRadiometricInformations MuscateMetadataReader::readRadiometricInformations(TiXmlElement *el){
	MuscateRadiometricInformations result;

	if (!el) {
		return result;
	}
	std::vector<const char*> quantificationNames = {
			"REFLECTANCE_QUANTIFICATION_VALUE",
			"AEROSOL_OPTICAL_THICKNESS_QUANTIFICATION_VALUE",
			"WATER_VAPOR_CONTENT_QUANTIFICATION_VALUE"
	};

	for(auto name : quantificationNames){
		if(auto child = el->FirstChildElement(name)){
			result.quantificationValues.emplace_back(SpecialValue(std::string(name), child->GetText()));
		}
	}

	if(auto specialValuesList = el->FirstChildElement("Special_Values_List")){
		for(auto specialValue = specialValuesList->FirstChildElement("SPECIAL_VALUE"); specialValue; specialValue = specialValue->NextSiblingElement("SPECIAL_VALUE")){
			result.specialValues.emplace_back(SpecialValue(specialValue->Attribute("name"), specialValue->GetText()));
		}
	}
	if(auto spectralBandInformationsList = el->FirstChildElement("Spectral_Band_Informations_List")){
		for(auto spectralBandInfo = spectralBandInformationsList->FirstChildElement("Spectral_Band_Informations");
				spectralBandInfo; spectralBandInfo = spectralBandInfo->NextSiblingElement("Spectral_Band_Informations")){
			BandInformation band(
					spectralBandInfo->Attribute("band_id"),
					GetChildText(spectralBandInfo, "SPATIAL_RESOLUTION"),
					GetChildAttribute(spectralBandInfo, "SPATIAL_RESOLUTION", "unit"));
			//TODO: Read the other fields as well
			result.Bands.emplace_back(band);
		}
	}
	return result;
}

/**
 * @brief read each of the QualityIndices (CloudPercent, SnowPercent etc.)
 * @param el The ProductQualityList parent
 * @return vector containing all MuscateQualityIndices
 */
static std::vector<MuscateQualityIndex> readQualityIndices(TiXmlElement *el){
	std::vector<MuscateQualityIndex> result;

	if (!el) {
		return result;
	}

	if (auto globalIndexList = el->FirstChildElement("Global_Index_List")) {
		for(auto qualityIndex = globalIndexList->FirstChildElement("QUALITY_INDEX");
				qualityIndex; qualityIndex = qualityIndex->NextSiblingElement("QUALITY_INDEX")){
			result.emplace_back(MuscateQualityIndex(qualityIndex->Attribute("name"), qualityIndex->GetText()));
		}
	}


	return result;
}

/**
 * @brief Reads a single QualityProduct, either a contributing or a current one
 * @param The product parent - Current_Product or Contributing_Product
 * @return The MuscateQualityProduct with all of its QualityIndices
 */
static MuscateQualityProduct readQualityProduct(TiXmlElement *el){
	MuscateQualityProduct result;


	for(auto productQuality = el->FirstChildElement("Product_Quality_List");
			productQuality; productQuality = productQuality->NextSiblingElement("Product_Quality_List")){
		std::string productQualityLevel = productQuality->Attribute("level");
		if(productQualityLevel == "N2" || productQualityLevel == "N3"){
			result.Level = productQuality->Attribute("level");
			if(auto productQualityItem = productQuality->FirstChildElement("Product_Quality")){
				if(auto sourceProduct = productQualityItem->FirstChildElement("Source_Product")){
					result.ProductID = GetChildText(sourceProduct, "PRODUCT_ID");
					result.AcquisitionDate = GetChildText(sourceProduct, "ACQUISITION_DATE");
					result.ProductionDate = GetChildText(sourceProduct, "PRODUCTION_DATE");
				}
				result.GlobalIndexList = readQualityIndices(productQualityItem);
			}
		}
	}
	return result;
}

MuscateQualityInformations MuscateMetadataReader::readQualityInformations(TiXmlElement *el){
	MuscateQualityInformations result;

	if (!el) {
		return result;
	}

	if (auto currentProduct = el->FirstChildElement("Current_Product")) {
		result.CurrentProduct = readQualityProduct(currentProduct);
	}
	if(auto contributingProductsList = el->FirstChildElement("Contributing_Products_List")){
		for(auto contributingProduct = contributingProductsList->FirstChildElement("Contributing_Product");
				contributingProduct; contributingProduct = contributingProduct->NextSiblingElement("Contributing_Product")){
			result.ContributingProducts.emplace_back(readQualityProduct(contributingProduct));
		}
	}

	return result;
}

std::unique_ptr<MuscateFileMetadata> MuscateMetadataReader::ReadMetadataXml(const TiXmlDocument &doc){
	TiXmlHandle hDoc(const_cast<TiXmlDocument *>(&doc));

	// BUG: TinyXML can't properly read stylesheet declarations, see
	// http://sourceforge.net/p/tinyxml/patches/37/
	// Our files start with one, but we can't read it in.
	auto rootElement = hDoc.FirstChildElement("Muscate_Metadata_Document").ToElement();

	if (!rootElement) {
		return nullptr;
	}

	auto file = std::unique_ptr<MuscateFileMetadata>(new MuscateFileMetadata);
	file->Header.xsi = GetAttribute(rootElement, "xmlns:xsi");
	file->Header.SchemaLocation = GetAttribute(rootElement, "xsi:noNamespaceSchemaLocation");
	file->MetadataIdentification = readMetadataIdentification(rootElement->FirstChildElement("Metadata_Identification"));
	file->DatasetIdentification = readDatasetIdentification(rootElement->FirstChildElement("Dataset_Identification"));
	file->ProductCharacteristics = readProductCharacteristics(rootElement->FirstChildElement("Product_Characteristics"));
	file->ProductOrganisation = readProductOrganisation(rootElement->FirstChildElement("Product_Organisation"));
	file->GeopositionInformations = readGeopositionInformations(rootElement->FirstChildElement("Geoposition_Informations"));
	file->GeometricInformations = readGeometricInformations(rootElement->FirstChildElement("Geometric_Informations"));
	file->RadiometricInformations = readRadiometricInformations(rootElement->FirstChildElement("Radiometric_Informations"));
	file->QualityInformations = readQualityInformations(rootElement->FirstChildElement("Quality_Informations"));
	return file;
}

} /* namespace muscate */
} /* namespace ts */
