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

#define BOOST_TEST_MODULE MuscateWriteRead
#include <boost/test/unit_test.hpp>

#include "MuscateMetadataWriter.hpp"
#include "MuscateMetadataReader.hpp"
#include "MetadataUtil.hpp"
#include "stdlib.h"

#define EMPTY_XML "Empty.xml"
#define EXAMPLE_XML1 "L3A_WriteRead_S2.xml"
#define EXAMPLE_XML2 "L3A_WriteRead_Venus.xml"

using namespace ts::muscate;

BOOST_AUTO_TEST_CASE(MuscateWriteReadS2_L3A)
{
	auto writer = MuscateMetadataWriter::New();

	MuscateFileMetadata m;
	std::string path = std::string(EXAMPLE_XML1);
	m.Header.SchemaLocation = "..\\PSC-SL-411-0032-CG_Schemas\\Muscate_Metadata.xsd";

	m.MetadataIdentification.MetadataFormat = "METADATA_MUSCATE";
	m.MetadataIdentification.version = "1.16";
	m.MetadataIdentification.MetadataProfile = "DISTRIBUTED";
	m.MetadataIdentification.MetadataInformation = "PUBLIC";

	m.DatasetIdentification.Identifier = "SENTINEL2X_20170402-093844-724_L3A_T32MNE_D";
	m.DatasetIdentification.Authority = "THEIA";
	m.DatasetIdentification.Producer = "MUSCATE";
	m.DatasetIdentification.Project = "SENTINEL2";
	m.DatasetIdentification.GeographicalZone = "T32MNE";

	m.ProductCharacteristics.ProductID = "SENTINEL2X_20170402-093844-724_L3A_T32MNE_D_V1-0";
	m.ProductCharacteristics.AcquisitionDate = "2017-04-02T09:38:44.724Z";
	m.ProductCharacteristics.ProductionDate = "2017-06-07T16:12:17.380210Z";
	m.ProductCharacteristics.ProductVersion = "1.4";
	m.ProductCharacteristics.ProductLevel = "L2A";
	m.ProductCharacteristics.Platform = "MULTISAT";
	m.ProductCharacteristics.hasSynthesisDate = true;
	m.ProductCharacteristics.SynthesisDate = "2017-04-02T09:38:44.724Z";
	m.ProductCharacteristics.SynthesisPeriodBegin = "2017-04-02T09:38:44.724Z";
	m.ProductCharacteristics.SynthesisPeriodEnd =  "2017-04-02T09:38:44.724Z";
	m.ProductCharacteristics.DatePrecision =  "0.001";
	m.ProductCharacteristics.DatePrecisionUnit =  "s";

	std::vector<std::string> bands = {"B2", "B3", "B4", "B5", "B6", "B7", "B8", "B8A", "B11", "B12"};
	m.ProductCharacteristics.BandGlobalList.Count = 10;
	m.ProductCharacteristics.BandGlobalList.bandIDs = bands;
	BandGroup grp;
	grp.GroupID = "R1";
	grp.GroupList.Count = 4;
	grp.GroupList.bandIDs = {"B2", "B3", "B4", "B8"};
	m.ProductCharacteristics.BandGroupList.emplace_back(grp);

	m.ProductOrganisation.Quicklook.bandIDs = std::string("B2,B3,B4");
	m.ProductOrganisation.Quicklook.path = "simple_test.tif";
	m.ProductOrganisation.Quicklook.hasBitNum = false;
	m.ProductOrganisation.Quicklook.hasBandNum = false;
	m.ProductOrganisation.Quicklook.hasDetectorID = false;
	m.ProductOrganisation.Quicklook.isGroup = false;
	m.ProductOrganisation.Quicklook.isMask = false;


	ImageProperty img;
	img.Nature = "Test_Nature";
	img.Encoding = "Test_Encoding";
	img.Endianness = "Test_Endianness";
	img.Format = "Test_Format";
	img.Compression = "Test_Compression";
	img.hasCompression = true;
	std::vector<std::string> groups = {"R1", "R2"};
	for(size_t i = 0; i < groups.size(); i++){
		ImageInformation imgInfo;
		imgInfo.path = "Test_Path_" + groups[i] + ".tif";
		imgInfo.isMask = false;
		imgInfo.isGroup = true;
		imgInfo.hasDetectorID = false;
		imgInfo.bandIDs = groups[i];
		imgInfo.hasBandNum = false;
		imgInfo.hasBitNum = false;
		img.ImageFiles.emplace_back(imgInfo);
	}
	m.ProductOrganisation.ImageProperties.emplace_back(img);

	ImageProperty img2;
	img2.Nature = "Test_Nature";
	img2.Encoding = "Test_Encoding";
	img2.Endianness = "Test_Endianness";
	img2.Format = "Test_Format";
	img2.hasCompression = false;
	for(size_t i = 0; i < bands.size(); i++){
		ImageInformation imgInfo;
		imgInfo.path = "Test_Path_" + bands[i] + ".tif";
		imgInfo.isMask = false;
		imgInfo.isGroup = false;
		imgInfo.hasDetectorID = false;
		imgInfo.bandIDs = bands[i];
		imgInfo.hasBandNum = false;
		imgInfo.hasBitNum = true;
		imgInfo.bitNumber = std::to_string(i+1);
		img2.ImageFiles.emplace_back(imgInfo);
	}
	m.ProductOrganisation.ImageProperties.emplace_back(img2);

	ImageProperty mask;
	mask.Nature = "Test_Nature";
	mask.Encoding = "Test_Encoding";
	mask.Endianness = "Test_Endianness";
	mask.Format = "Test_Format";
	mask.Compression = "Test_Compression";
	mask.hasCompression = true;
	for(size_t i = 0; i < bands.size(); i++){
		ImageInformation imgInfo;
		imgInfo.path = "Test_Path_" + bands[i] + ".tif";
		imgInfo.isMask = true;
		imgInfo.isGroup = false;
		imgInfo.hasDetectorID = false;
		imgInfo.bandIDs = bands[i];
		imgInfo.hasBandNum = false;
		imgInfo.bandNum = "1";
		imgInfo.hasBitNum = false;
		mask.ImageFiles.emplace_back(imgInfo);
	}
	m.ProductOrganisation.MaskProperties.emplace_back(mask);

	m.GeopositionInformations.GeoTables = "EPSG";
	m.GeopositionInformations.HorizontalCSType = "PROJECTED";
	m.GeopositionInformations.HorizontalCSName = "WGS 84 / UTM zone 31N";
	m.GeopositionInformations.HorizontalCSCode = "32631";
	m.GeopositionInformations.RasterCS.CSType = "CELL";
	m.GeopositionInformations.RasterCS.PixelOrigin = "0";
	m.GeopositionInformations.MetadataCS.CSType = "CELL";
	m.GeopositionInformations.MetadataCS.PixelOrigin = "0";
	std::vector<std::string> pointNames = {"upperLeft", "upperRight", "lowerRight", "lowerLeft", "center"};
	for(size_t i = 0; i < pointNames.size(); i++){
		GeoPoint point;
		point.name = pointNames[i];
		point.lat = std::to_string(float(i));
		point.lon = std::to_string(float(i));
		point.Xpos = std::to_string(i);
		point.Ypos = std::to_string(i);
		m.GeopositionInformations.GlobalGeoposition.emplace_back(point);
	}
	for(size_t i = 0; i < groups.size(); i++){
		GroupPositioning group;
		group.groupID = groups[i];
		group.ULX = std::to_string(float(i));
		group.ULY = std::to_string(float(i));
		group.XDim = std::to_string(i);
		group.YDim = std::to_string(i);
		group.nrows = std::to_string(i);
		group.ncols = std::to_string(i);
		m.GeopositionInformations.GroupPosition.emplace_back(group);
	}

	m.RadiometricInformations.quantificationValues.emplace_back(SpecialValue("REFLECTANCE_QUANTIFICATION_VALUE", "10000"));
	m.RadiometricInformations.specialValues.emplace_back(SpecialValue("nodata", "-10000"));

	std::vector<size_t> spatialResolutions = {10, 10, 10, 20, 20, 20, 10, 20, 20, 20};
	for(size_t i = 0; i < bands.size(); i++){
		BandInformation b;
		b.bandID = bands[i];
		b.SpatialResolution = std::to_string(spatialResolutions[i]);
		b.SpatialResolutionUnit = "m";
		m.RadiometricInformations.Bands.emplace_back(b);
	}

	MuscateQualityProduct current;
	current.Level = "N3";
	current.ProductID = "SENTINEL2X_20170402-093844-724_L3A_T32MNE_D_V1-4";
	current.AcquisitionDate = "2017-04-02T09:38:44.724Z";
	current.ProductionDate = "2017-11-10T13:23:57.46Z";
	current.GlobalIndexList.emplace_back(MuscateQualityIndex("CloudPercent", "10"));
	current.GlobalIndexList.emplace_back(MuscateQualityIndex("SnowPercent", "0"));
	current.GlobalIndexList.emplace_back(MuscateQualityIndex("RainDetected", "false"));
	current.GlobalIndexList.emplace_back(MuscateQualityIndex("HotSpotDetected", "false"));
	current.GlobalIndexList.emplace_back(MuscateQualityIndex("SunGlintDetected", "false"));

	m.QualityInformations.CurrentProduct = current;

	MuscateQualityProduct contributing;
	contributing.Level = "N2";
	contributing.ProductID = "SENTINEL2X_20170302-000000-000_L3A_T32MNE_D_V1-0";
	contributing.AcquisitionDate = "2017-03-02T00:00:00.000Z";
	contributing.ProductionDate = "2017-11-10T13:23:57.46Z";
	contributing.GlobalIndexList.emplace_back(MuscateQualityIndex("CloudPercent", "50"));
	contributing.GlobalIndexList.emplace_back(MuscateQualityIndex("SnowPercent", "0"));
	contributing.GlobalIndexList.emplace_back(MuscateQualityIndex("RainDetected", "true"));
	contributing.GlobalIndexList.emplace_back(MuscateQualityIndex("HotSpotDetected", "false"));
	contributing.GlobalIndexList.emplace_back(MuscateQualityIndex("SunGlintDetected", "false"));

	m.QualityInformations.ContributingProducts.emplace_back(contributing);
	contributing.Level = "N2";
	contributing.ProductID = "SENTINEL2A_20170402-000000-000_L2A_T32MNE_D_V1-0";
	contributing.AcquisitionDate = "2017-04-02T00:00:00.000Z";
	m.QualityInformations.ContributingProducts.emplace_back(contributing);

	std::cout << path << std::endl;
	writer->WriteMetadata(m, path);

	/**
	 *
	 * READ
	 *
	 */

	auto reader = MuscateMetadataReader::New();
	auto mRead = reader->ReadMetadata(path);

	BOOST_REQUIRE(mRead);

	BOOST_CHECK(m.Header == mRead->Header);
	BOOST_CHECK(m.MetadataIdentification == mRead->MetadataIdentification);
	BOOST_CHECK(m.DatasetIdentification == mRead->DatasetIdentification);
	BOOST_CHECK(m.ProductCharacteristics == mRead->ProductCharacteristics);
	BOOST_CHECK(m.ProductOrganisation == mRead->ProductOrganisation);
	BOOST_CHECK(m.GeopositionInformations == mRead->GeopositionInformations);
	BOOST_CHECK(m.RadiometricInformations == mRead->RadiometricInformations);
	BOOST_CHECK(m.QualityInformations == mRead->QualityInformations);

}
