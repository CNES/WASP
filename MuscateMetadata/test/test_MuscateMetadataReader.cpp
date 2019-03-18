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

#define BOOST_TEST_MODULE MuscateReader
#include <boost/test/unit_test.hpp>
#include "GlobalDefs.h"
#include "MuscateMetadataReader.hpp"
#include "stdlib.h"


#define TEST_NAME	"test_MuscateMetadata"
#define TEST_SRC	ts::getEnvVar("WASP_TEST")
#define EXAMPLE_XML1 "SENTINEL2A_20170402-093844-724_L2A_T32MNE_D_V1-4_MTD_ALL.xml"
#define EXAMPLE_XML2 "SENTINEL2B_20171008-105012-463_L2A_T31TCH_C_V1-0_MTD_ALL.xml"
#define EXAMPLE_XML3 "LANDSAT8-OLITIRS-XS_20170504-103532-111_L2A_T31TCH_C_V1-0_MTD_ALL.xml"
#define EXAMPLE_XML4 "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_MTD_ALL.xml"

using namespace ts::muscate;

BOOST_AUTO_TEST_CASE(MuscateReaderExample1)
{
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" + EXAMPLE_XML1) << std::endl;
	auto reader = MuscateMetadataReader::New();

	auto m = reader->ReadMetadata(std::string(TEST_SRC + "/" + TEST_NAME + "/" + EXAMPLE_XML1));

	BOOST_REQUIRE(m);

	BOOST_CHECK_EQUAL(m->Header.SchemaLocation, "..\\PSC-SL-411-0032-CG_Schemas\\Muscate_Metadata.xsd");

	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataFormat, "METADATA_MUSCATE");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.version, "1.16");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataProfile, "DISTRIBUTED");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataInformation, "PUBLIC");

	BOOST_CHECK_EQUAL(m->DatasetIdentification.Identifier, "SENTINEL2A_20170402-093844-724_L2A_T32MNE_D");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Authority, "THEIA");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Producer, "MUSCATE");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Project, "SENTINEL2");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.GeographicalZone, "T32MNE");

	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductID, "SENTINEL2A_20170402-093844-724_L2A_T32MNE_D_V1-4");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.AcquisitionDate, "2017-04-02T09:38:44.724Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductionDate, "2017-06-07T16:12:17.380210Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductVersion, "1.4");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductLevel, "L2A");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.Platform, "SENTINEL2A");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.OrbitNumber, "9285");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.OrbitType, "Orbit");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.MeanAcquisitionRange, "2017-04-02T09:38:44.724Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.DatePrecision, "0.001");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.DatePrecisionUnit, "s");

	std::vector<std::string> bands = {"B2", "B3", "B4", "B5", "B6", "B7", "B8", "B8A", "B11", "B12"};
	BOOST_CHECK_EQUAL(bands.size(), m->ProductCharacteristics.BandGlobalList.Count);
	for(size_t i = 0; i < m->ProductCharacteristics.BandGlobalList.bandIDs.size(); i++){
		BOOST_CHECK_EQUAL(bands[i], m->ProductCharacteristics.BandGlobalList.bandIDs[i]);
	}

	BOOST_CHECK_EQUAL(m->ProductOrganisation.Quicklook.path, "./SENTINEL2A_20170402-093844-724_L2A_T32MNE_D_V1-4_QKL_ALL.jpg");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.Quicklook.bandIDs, "B4,B3,B2");

	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Nature, "Aerosol_Optical_Thickness");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Format, "image/tiff");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Encoding, "uint16");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Endianness, "LittleEndian");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Compression, "None");
	BOOST_CHECK(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].isGroup);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].isMask, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].hasBitNum, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].hasBandNum, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].hasDetectorID, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].path, "SENTINEL2A_20170402-093844-724_L2A_T32MNE_D_V1-4_ATB_R1.tif");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].bandIDs, "R1");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[1].path, "SENTINEL2A_20170402-093844-724_L2A_T32MNE_D_V1-4_ATB_R2.tif");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[1].bandIDs, "R2");

	BOOST_CHECK_EQUAL(m->GeopositionInformations.GeoTables, "EPSG");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSType, "PROJECTED");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSName, "WGS 84 / UTM zone 32S");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSCode, "32732");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.RasterCS.CSType, "CELL");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.RasterCS.PixelOrigin, "0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.MetadataCS.CSType, "CELL");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.MetadataCS.PixelOrigin, "0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].name, "upperLeft");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].lat, "0.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].lon, "8.99982026505");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].Xpos, "499980.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].Ypos, "10000000.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].name, "upperRight");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].lat, "0.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].lon, "9.98651606936");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].Xpos, "609780.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].Ypos, "10000000.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].name, "lowerRight");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].lat, "-0.993245791513");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].lon, "9.98666334254");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].Xpos, "609780.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].Ypos, "9890200.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].name, "lowerLeft");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].lat, "-0.993394044333");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].lon, "8.99982023821");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].Xpos, "499980.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].Ypos, "9890200.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].name, "center");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].lat, "-0.49667887172");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].lon, "9.49320498107");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].Xpos, "554880.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].Ypos, "9945100.0");

	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].groupID, "R1");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ULX, "499980");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ULY, "10000000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].XDim, "10");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].YDim, "-10");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].nrows, "10980");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ncols, "10980");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].groupID, "R2");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].ULX, "499980");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].ULY, "10000000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].XDim, "20");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].YDim, "-20");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].nrows, "5490");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].ncols, "5490");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[2].groupID, "R3");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[2].ULX, "499980");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[2].ULY, "10000000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[2].XDim, "60");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[2].YDim, "-60");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[2].nrows, "1830");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[2].ncols, "1830");

	for(auto it = m->RadiometricInformations.quantificationValues.begin(); it != m->RadiometricInformations.quantificationValues.end(); ++it){
		if(it->name == "REFLECTANCE_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "10000");
			break;
		}
		if(it->name == "WATER_VAPOR_CONTENT_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "20");
			break;
		}
		if(it->name == "AEROSOL_OPTICAL_THICKNESS_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "200");
			break;
		}
		BOOST_FAIL("Found unknown Special_Value");
	}

	for(auto it = m->RadiometricInformations.specialValues.begin(); it != m->RadiometricInformations.specialValues.end(); ++it){
		if(it->name == "nodata"){
			BOOST_CHECK_EQUAL(it->value, "-10000");
			break;
		}
		if(it->name == "water_vapor_content_nodata"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		if(it->name == "aerosol_optical_thickness_nodata"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		BOOST_FAIL("Found unknown Special_Value");
	}

	bands = {"B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B8A", "B9", "B10", "B11", "B12"};
	std::vector<size_t> spatialResolutions = {60, 10, 10, 10, 20, 20, 20, 10, 20, 60, 60, 20, 20};
	for(size_t i = 0; i < bands.size(); i++){
		BOOST_CHECK_EQUAL(bands[i], m->RadiometricInformations.Bands[i].bandID);
		BOOST_CHECK_EQUAL(std::to_string(spatialResolutions[i]), m->RadiometricInformations.Bands[i].SpatialResolution);
		BOOST_CHECK_EQUAL("m", m->RadiometricInformations.Bands[i].SpatialResolutionUnit);
	}

	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.Level, "N2");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.ProductID, "SENTINEL2A_20170402-093844-724_L2A_T32MNE_C_V1-4");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.AcquisitionDate, "2017-04-02T09:38:44.724Z");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.ProductionDate, "2017-06-07T13:08:14.434852Z");

	for(auto it = m->QualityInformations.CurrentProduct.GlobalIndexList.begin(); it != m->QualityInformations.CurrentProduct.GlobalIndexList.end(); ++it){
		if(it->name == "CloudPercent"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		if(it->name == "HotSpotDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		if(it->name == "RainDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		if(it->name == "SnowPercent"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		if(it->name == "SunGlintDetected"){
			BOOST_CHECK_EQUAL(it->value, "true");
			break;
		}
		BOOST_FAIL("Found unknown Quality_Index");
	}

}


BOOST_AUTO_TEST_CASE(MuscateReaderExample2)
{
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" + EXAMPLE_XML2) << std::endl;
	auto reader = MuscateMetadataReader::New();

	auto m = reader->ReadMetadata(std::string(TEST_SRC + "/" + TEST_NAME + "/" + EXAMPLE_XML2));

	BOOST_REQUIRE(m);

	BOOST_CHECK_EQUAL(m->Header.SchemaLocation, "Muscate_Metadata.xsd");

	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataFormat, "METADATA_MUSCATE");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.version, "1.16");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataProfile, "USER");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataInformation, "EXPERT");

	BOOST_CHECK_EQUAL(m->DatasetIdentification.Identifier, "SENTINEL2B_20171008-105012-463_L2A_T31TCH_C");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Authority, "THEIA");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Producer, "MUSCATE");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Project, "SENTINEL2");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.GeographicalZone, "T31TCH");

	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductID, "SENTINEL2B_20171008-105012-463_L2A_T31TCH_C_V1-0");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.AcquisitionDate, "2017-10-08T10:50:12.463Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductionDate, "2017-11-10T13:23:57.46Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductVersion, "1.0");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductLevel, "L2A");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.Platform, "SENTINEL2B");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.OrbitNumber, "3080");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.OrbitType, "Orbit");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.MeanAcquisitionRange, "2017-10-08T10:50:12.463Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.DatePrecision, "0.001");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.DatePrecisionUnit, "s");

	std::vector<std::string> bands = {"B2", "B3", "B4", "B5", "B6", "B7", "B8", "B8A", "B11", "B12"};
	BOOST_CHECK_EQUAL(bands.size(), m->ProductCharacteristics.BandGlobalList.Count);
	for(size_t i = 0; i < m->ProductCharacteristics.BandGlobalList.bandIDs.size(); i++){
		BOOST_CHECK_EQUAL(bands[i], m->ProductCharacteristics.BandGlobalList.bandIDs[i]);
	}

	BOOST_CHECK_EQUAL(m->ProductOrganisation.Quicklook.path, "SENTINEL2B_20171008-105012-463_L2A_T31TCH_C_V1-0_QKL_ALL.jpg");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.Quicklook.bandIDs, "B4,B3,B2");

	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Nature, "Surface_Reflectance");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Format, "image/tiff");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Encoding, "int16");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Endianness, "LittleEndian");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Compression, "None");
	for(size_t i = 0; i < m->ProductOrganisation.ImageProperties[0].ImageFiles.size(); i++){
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].isGroup, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].isMask, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].hasBitNum, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].hasBandNum, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].hasDetectorID, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].path, "SENTINEL2B_20171008-105012-463_L2A_T31TCH_C_V1-0_SRE_" + bands[i] + ".tif");
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].bandIDs, bands[i]);
	}

	BOOST_CHECK_EQUAL(m->GeopositionInformations.GeoTables, "EPSG");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSType, "PROJECTED");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSName, "WGS 84 / UTM zone 31N");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSCode, "32631");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.RasterCS.CSType, "CELL");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.RasterCS.PixelOrigin, "0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.MetadataCS.CSType, "CELL");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.MetadataCS.PixelOrigin, "0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].name, "upperLeft");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].lat, "43.3262463354");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].lon, "0.533217951232");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].Xpos, "300000.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].Ypos, "4800000.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].name, "upperRight");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].lat, "43.3474403884");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].lon, "1.88707783503");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].Xpos, "409800.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].Ypos, "4800000.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].name, "lowerRight");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].lat, "42.3588399172");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].lon, "1.90469075284");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].Xpos, "409800.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].Ypos, "4690200.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].name, "lowerLeft");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].lat, "42.3383618247");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].lon, "0.572219317979");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].Xpos, "300000.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].Ypos, "4690200.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].name, "center");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].lat, "42.8447138198");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].lon, "1.22430138862");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].Xpos, "354900.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].Ypos, "4745100.0");

	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].groupID, "R1");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ULX, "300000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ULY, "4800000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].XDim, "10");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].YDim, "-10");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].nrows, "10980");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ncols, "10980");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].groupID, "R2");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].ULX, "300000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].ULY, "4800000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].XDim, "20");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].YDim, "-20");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].nrows, "5490");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[1].ncols, "5490");

	for(auto it = m->RadiometricInformations.quantificationValues.begin(); it != m->RadiometricInformations.quantificationValues.end(); ++it){
		if(it->name == "REFLECTANCE_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "10000");
			break;
		}
		if(it->name == "WATER_VAPOR_CONTENT_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "20");
			break;
		}
		if(it->name == "AEROSOL_OPTICAL_THICKNESS_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "200");
			break;
		}
		BOOST_FAIL("Found unknown Special_Value");
	}

	for(auto it = m->RadiometricInformations.specialValues.begin(); it != m->RadiometricInformations.specialValues.end(); ++it){
		if(it->name == "nodata"){
			BOOST_CHECK_EQUAL(it->value, "-10000");
			break;
		}
		if(it->name == "water_vapor_content_nodata"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		if(it->name == "aerosol_optical_thickness_nodata"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		BOOST_FAIL("Found unknown Special_Value");
	}

	std::vector<size_t> spatialResolutions = {10, 10, 10, 20, 20, 20, 10, 20, 20, 20};
	for(size_t i = 0; i < bands.size(); i++){
		BOOST_CHECK_EQUAL(bands[i], m->RadiometricInformations.Bands[i].bandID);
		BOOST_CHECK_EQUAL(std::to_string(spatialResolutions[i]), m->RadiometricInformations.Bands[i].SpatialResolution);
		BOOST_CHECK_EQUAL("m", m->RadiometricInformations.Bands[i].SpatialResolutionUnit);
	}

	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.Level, "N2");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.ProductID, "SENTINEL2B_20171008-105012-463_L2A_T31TCH_C_V1-0");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.AcquisitionDate, "2017-10-08T10:50:12.463Z");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.ProductionDate, "2017-11-10T13:23:57.46Z");

	for(auto it = m->QualityInformations.CurrentProduct.GlobalIndexList.begin(); it != m->QualityInformations.CurrentProduct.GlobalIndexList.end(); ++it){
		if(it->name == "CloudPercent"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		if(it->name == "HotSpotDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		if(it->name == "RainDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		if(it->name == "SnowPercent"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		if(it->name == "SunGlintDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		BOOST_FAIL("Found unknown Quality_Index");
	}

}


BOOST_AUTO_TEST_CASE(MuscateReaderExample3)
{
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" + EXAMPLE_XML3) << std::endl;
	auto reader = MuscateMetadataReader::New();

	auto m = reader->ReadMetadata(std::string(TEST_SRC + "/" + TEST_NAME + "/" + EXAMPLE_XML3));

	BOOST_REQUIRE(m);

	BOOST_CHECK_EQUAL(m->Header.SchemaLocation, "Muscate_Metadata.xsd");

	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataFormat, "METADATA_MUSCATE");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.version, "1.0");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataProfile, "USER");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataInformation, "EXPERT");

	BOOST_CHECK_EQUAL(m->DatasetIdentification.Identifier, "LANDSAT8-OLITIRS-XS_20170504-103532-111_L2A_T31TCH_C");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Authority, "THEIA");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Producer, "MUSCATE");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Project, "LANDSAT");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.GeographicalZone, "T31TCH");

	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductID, "LANDSAT8-OLITIRS-XS_20170504-103532-111_L2A_T31TCH_C_V1-0");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.AcquisitionDate, "2017-05-04T10:35:32.111Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductionDate, "2017-09-01T13:18:39Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductVersion, "1.0");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductLevel, "L2A");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.Platform, "LANDSAT8");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.OrbitNumber, "198");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.OrbitType, "Path");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.MeanAcquisitionRange, "2017-05-04T10:35:32.1113979Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.DatePrecision, "0.0000001");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.DatePrecisionUnit, "s");

	std::vector<std::string> bands = {"B1", "B2", "B3", "B4", "B5", "B6", "B7"};
	BOOST_CHECK_EQUAL(bands.size(), m->ProductCharacteristics.BandGlobalList.Count);
	for(size_t i = 0; i < m->ProductCharacteristics.BandGlobalList.bandIDs.size(); i++){
		BOOST_CHECK_EQUAL(bands[i], m->ProductCharacteristics.BandGlobalList.bandIDs[i]);
	}

	BOOST_CHECK_EQUAL(m->ProductOrganisation.Quicklook.path, "LANDSAT8-OLITIRS-XS_20170504-103532-111_L2A_T31TCH_C_V1-0_QKL_ALL.jpg");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.Quicklook.bandIDs, "B4,B3,B2");

	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Nature, "Surface_Reflectance");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Format, "image/tiff");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Encoding, "int16");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Endianness, "LittleEndian");
	for(size_t i = 0; i < m->ProductOrganisation.ImageProperties[0].ImageFiles.size(); i++){
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].isGroup, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].isMask, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].hasBitNum, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].hasBandNum, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].hasDetectorID, false);
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].path, "LANDSAT8-OLITIRS-XS_20170504-103532-111_L2A_T31TCH_C_V1-0_SRE_" + bands[i] + ".tif");
		BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[i].bandIDs, bands[i]);
	}

	BOOST_CHECK_EQUAL(m->GeopositionInformations.GeoTables, "EPSG");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSType, "PROJECTED");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSName, "WGS 84 / UTM zone 31N");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSCode, "32631");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.RasterCS.CSType, "CELL");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.RasterCS.PixelOrigin, "0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.MetadataCS.CSType, "CELL");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.MetadataCS.PixelOrigin, "0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].name, "upperLeft");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].lat, "43.326246335385");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].lon, "0.533217951232");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].Xpos, "300000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].Ypos, "4800000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].name, "lowerLeft");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].lat, "42.338361824681");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].lon, "0.572219317979");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].Xpos, "300000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].Ypos, "4690200");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].name, "lowerRight");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].lat, "42.358839917249");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].lon, "1.904690752837");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].Xpos, "409800");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].Ypos, "4690200");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].name, "upperRight");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].lat, "43.347440388362");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].lon, "1.887077835027");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].Xpos, "409800");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].Ypos, "4800000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].name, "center");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].lat, "42.844713819786");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].lon, "1.224301388621");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].Xpos, "354900");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].Ypos, "4745100");

	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].groupID, "XS");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ULX, "300000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ULY, "4800000");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].XDim, "30");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].YDim, "-30");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].nrows, "3660");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ncols, "3660");

	for(auto it = m->RadiometricInformations.quantificationValues.begin(); it != m->RadiometricInformations.quantificationValues.end(); ++it){
		if(it->name == "REFLECTANCE_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "1000");
			break;
		}
		if(it->name == "WATER_VAPOR_CONTENT_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "20");
			break;
		}
		if(it->name == "AEROSOL_OPTICAL_THICKNESS_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "200");
			break;
		}
		BOOST_FAIL("Found unknown Special_Value");
	}

	for(auto it = m->RadiometricInformations.specialValues.begin(); it != m->RadiometricInformations.specialValues.end(); ++it){
		if(it->name == "nodata"){
			BOOST_CHECK_EQUAL(it->value, "-10000");
			break;
		}
		if(it->name == "water_vapor_content_nodata"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		if(it->name == "aerosol_optical_thickness_nodata"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		BOOST_FAIL("Found unknown Special_Value");
	}

	std::vector<size_t> spatialResolutions = {30, 30, 30, 30, 30, 30, 30};
	for(size_t i = 0; i < bands.size(); i++){
		BOOST_CHECK_EQUAL(bands[i], m->RadiometricInformations.Bands[i].bandID);
		BOOST_CHECK_EQUAL(std::to_string(spatialResolutions[i]), m->RadiometricInformations.Bands[i].SpatialResolution);
		BOOST_CHECK_EQUAL("m", m->RadiometricInformations.Bands[i].SpatialResolutionUnit);
	}

	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.Level, "N2");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.ProductID, "LANDSAT8-OLITIRS-XS_20170504-103532-111_L2A_T31TCH_C_V1-0");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.AcquisitionDate, "2017-05-04T10:35:32.111Z");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.ProductionDate, "2017-09-01T13:18:39Z");

	for(auto it = m->QualityInformations.CurrentProduct.GlobalIndexList.begin(); it != m->QualityInformations.CurrentProduct.GlobalIndexList.end(); ++it){
		if(it->name == "CloudPercent"){
			BOOST_CHECK_EQUAL(it->value, "86");
			break;
		}
		if(it->name == "HotSpotDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		if(it->name == "RainDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		if(it->name == "SnowPercent"){
			BOOST_CHECK_EQUAL(it->value, "4");
			break;
		}
		if(it->name == "SunGlintDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		BOOST_FAIL("Found unknown Quality_Index");
	}

}


BOOST_AUTO_TEST_CASE(MuscateReaderExampleVenus1)
{
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" + EXAMPLE_XML4) << std::endl;
	auto reader = MuscateMetadataReader::New();

	auto m = reader->ReadMetadata(std::string(TEST_SRC + "/" + TEST_NAME + "/" + EXAMPLE_XML4));

	BOOST_REQUIRE(m);

	BOOST_CHECK_EQUAL(m->Header.SchemaLocation, "..\\PSC-SL-411-0032-CG_Schemas\\Muscate_Metadata.xsd");

	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataFormat, "METADATA_MUSCATE");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.version, "1.14");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataProfile, "USER");
	BOOST_CHECK_EQUAL(m->MetadataIdentification.MetadataInformation, "EXPERT");

	BOOST_CHECK_EQUAL(m->DatasetIdentification.Identifier, "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Authority, "THEIA");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Producer, "MUSCATE");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.Project, "VENUS");
	BOOST_CHECK_EQUAL(m->DatasetIdentification.GeographicalZone, "FR-LUS");

	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductID, "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.AcquisitionDate, "2018-02-02T10:55:54.000");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductionDate, "2018-08-06T14:40:28.299Z");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductVersion, "1.0");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.ProductLevel, "L2A");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.Platform, "VENUS");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.SpectralContent, "XS");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.OrbitNumber, "2673");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.OrbitType, "Orbit");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.MeanAcquisitionRange, "2018-02-02T10:55:54.000");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.DatePrecision, "1");
	BOOST_CHECK_EQUAL(m->ProductCharacteristics.DatePrecisionUnit, "s");

	std::vector<std::string> bands = {"B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "B10", "B11", "B12"};
	BOOST_CHECK_EQUAL(bands.size(), m->ProductCharacteristics.BandGlobalList.Count);
	for(size_t i = 0; i < m->ProductCharacteristics.BandGlobalList.bandIDs.size(); i++){
		BOOST_CHECK_EQUAL(bands[i], m->ProductCharacteristics.BandGlobalList.bandIDs[i]);
	}

	BOOST_CHECK_EQUAL(m->ProductOrganisation.Quicklook.path, "./VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_QKL_ALL.jpg");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.Quicklook.bandIDs, "B7,B4,B3");

	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties.size(), size_t(4));
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Nature, "Aerosol_Optical_Thickness");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Format, "image/tiff");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Encoding, "byte");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].Endianness, "LittleEndian");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles.size(), size_t(1));
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].isGroup, true);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].isMask, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].hasBitNum, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].hasBandNum, true);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].hasDetectorID, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].path, "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_ATB_XS.tif");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].bandIDs, "XS");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[0].ImageFiles[0].bandNum, "2");

	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].Nature, "Flat_Reflectance");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].Format, "image/tiff");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].Encoding, "uint16");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].Endianness, "LittleEndian");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].ImageFiles.size(), bands.size());
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].ImageFiles[0].isGroup, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].ImageFiles[0].isMask, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].ImageFiles[0].hasBitNum, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].ImageFiles[0].hasBandNum, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].ImageFiles[0].hasDetectorID, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].ImageFiles[0].path, "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_FRE_B1.tif");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[1].ImageFiles[0].bandIDs, "B1");

	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].Nature, "Surface_Reflectance");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].Format, "image/tiff");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].Encoding, "uint16");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].Endianness, "LittleEndian");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].ImageFiles.size(), bands.size());
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].ImageFiles[0].isGroup, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].ImageFiles[0].isMask, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].ImageFiles[0].hasBitNum, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].ImageFiles[0].hasBandNum, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].ImageFiles[0].hasDetectorID, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].ImageFiles[0].path, "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_SRE_B1.tif");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[2].ImageFiles[0].bandIDs, "B1");

	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].Nature, "Water_Vapor_Content");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].Format, "image/tiff");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].Encoding, "byte");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].Endianness, "LittleEndian");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].ImageFiles.size(), size_t(1));
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].ImageFiles[0].isGroup, true);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].ImageFiles[0].isMask, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].ImageFiles[0].hasBitNum, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].ImageFiles[0].hasBandNum, true);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].ImageFiles[0].hasDetectorID, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].ImageFiles[0].path, "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_ATB_XS.tif");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].ImageFiles[0].bandIDs, "XS");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.ImageProperties[3].ImageFiles[0].bandNum, "1");

	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties.size(), size_t(14));
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].Nature, "AOT_Interpolation");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].Format, "image/tiff");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].Encoding, "byte");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].Endianness, "LittleEndian");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].ImageFiles.size(), size_t(1));
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].ImageFiles[0].isGroup, true);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].ImageFiles[0].isMask, true);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].ImageFiles[0].hasBitNum, true);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].ImageFiles[0].hasBandNum, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].ImageFiles[0].hasDetectorID, false);
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].ImageFiles[0].path, "MASKS/VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_IAB_XS.tif");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].ImageFiles[0].bandIDs, "XS");
	BOOST_CHECK_EQUAL(m->ProductOrganisation.MaskProperties[0].ImageFiles[0].bitNumber, "2");

	//TODO Not testing the rest of the masks

	BOOST_CHECK_EQUAL(m->GeopositionInformations.GeoTables, "EPSG");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSType, "PROJECTED");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSName, "WGS 84 / UTM zone 31N");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.HorizontalCSCode, "32631");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.RasterCS.CSType, "CELL");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.RasterCS.PixelOrigin, "0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.MetadataCS.CSType, "CELL");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.MetadataCS.PixelOrigin, "0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].name, "upperLeft");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].lat, "46.2007058503");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].lon, "0.451031975584");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].Xpos, "303345.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[0].Ypos, "5119506.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].name, "upperRight");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].lat, "46.1845285294");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].lon, "-0.192636393141");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].Xpos, "253615.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[1].Ypos, "5119506.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].name, "lowerRight");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].lat, "46.6179044718");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].lon, "-0.218042295705");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].Xpos, "253615.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[2].Ypos, "5167736.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].name, "lowerLeft");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].lat, "46.6343270846");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].lon, "0.430732742896");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].Xpos, "303345.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[3].Ypos, "5167736.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].name, "center");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].lat, "46.4098275898");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].lon, "0.117767341547");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].Xpos, "278480.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GlobalGeoposition[4].Ypos, "5143621.0");

	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition.size(), size_t(1));
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].groupID, "XS");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ULX, "253615.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ULY, "5167736.0");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].XDim, "10.");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].YDim, "-10.");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].nrows, "4823");
	BOOST_CHECK_EQUAL(m->GeopositionInformations.GroupPosition[0].ncols, "4973");

	for(auto it = m->RadiometricInformations.quantificationValues.begin(); it != m->RadiometricInformations.quantificationValues.end(); ++it){
		if(it->name == "REFLECTANCE_QUANTIFICATION_VALUE"){
			BOOST_CHECK_EQUAL(it->value, "1000");
			break;
		}
		BOOST_FAIL("Found unknown Special_Value");
	}

	for(auto it = m->RadiometricInformations.specialValues.begin(); it != m->RadiometricInformations.specialValues.end(); ++it){
		if(it->name == "nodata"){
			BOOST_CHECK_EQUAL(it->value, "-10000");
			break;
		}
		BOOST_FAIL("Found unknown Special_Value");
	}

	bands = {"B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "B10", "B11", "B12"};
	std::vector<size_t> spatialResolutions = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
	for(size_t i = 0; i < bands.size(); i++){
		BOOST_CHECK_EQUAL(bands[i], m->RadiometricInformations.Bands[i].bandID);
		BOOST_CHECK_EQUAL(std::to_string(spatialResolutions[i]), m->RadiometricInformations.Bands[i].SpatialResolution);
		BOOST_CHECK_EQUAL("m", m->RadiometricInformations.Bands[i].SpatialResolutionUnit);
	}

	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.Level, "N2");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.ProductID, "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.AcquisitionDate, "2018-02-02T10:55:54.000");
	BOOST_CHECK_EQUAL(m->QualityInformations.CurrentProduct.ProductionDate, "2018-08-06T14:40:28.299Z");

	for(auto it = m->QualityInformations.CurrentProduct.GlobalIndexList.begin(); it != m->QualityInformations.CurrentProduct.GlobalIndexList.end(); ++it){
		if(it->name == "CloudPercent"){
			BOOST_CHECK_EQUAL(it->value, "26");
			break;
		}
		if(it->name == "HotSpotDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		if(it->name == "RainDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		if(it->name == "SnowPercent"){
			BOOST_CHECK_EQUAL(it->value, "0");
			break;
		}
		if(it->name == "SunGlintDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		if(it->name == "CirrusDetected"){
			BOOST_CHECK_EQUAL(it->value, "false");
			break;
		}
		BOOST_FAIL("Found unknown Quality_Index");
	}

}

