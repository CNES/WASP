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

#define BOOST_TEST_MODULE MuscateWriter
#include <boost/test/unit_test.hpp>
#include "../include/MuscateMetadataHelper.h"
#include "../include/GlobalDefs.h"
#include <string>

using namespace ts;

#define TEST_NAME	"test_MuscateMetadata"
#define TEST_SRC	getEnvVar("WASP_TEST")
#define INPUT_XML1 "SENTINEL2A_20170402-093844-724_L2A_T32MNE_D_V1-4_MTD_ALL.xml"
#define INPUT_XML2 "SENTINEL2A_20170923-105717-357_L2A_T31TCH_D_V1-4_MTD_ALL.xml"
#define INPUT_XML3 "SENTINEL2B_20171008-105012-463_L2A_T31TCH_C_V1-0_MTD_ALL.xml"

const std::vector<std::string> FREendingsS2 = {"FRE_B2", "FRE_B3", "FRE_B4", "FRE_B5", "FRE_B6", "FRE_B7", "FRE_B8", "FRE_B8A", "FRE_B11", "FRE_B12"};
const std::vector<std::string> FREendingsR1_S2 = {"FRE_B2", "FRE_B3", "FRE_B4", "FRE_B8"};
const std::vector<std::string> FREendingsR2_S2 = {"FRE_B5", "FRE_B6", "FRE_B7", "FRE_B8A", "FRE_B11", "FRE_B12"};
const std::vector<std::string> CLDendings = {"CLM_R1", "CLM_R2"};
const std::vector<std::string> MG2endings = {"MG2_R1", "MG2_R2"};
const std::vector<std::string> ATBendings = {"ATB_R1", "ATB_R2"};

/**
 * @brief Find if string ends with certain ending.
 * @param value The string to be tested
 * @param ending The ending to be searched for
 * @param Append filename ".tif" to every ending, if true
 * @return True, if value ends in ending, False if not
 */
inline bool ends_with(std::string const & value, std::vector<std::string> const & endings, const bool &withFilename = true)
{
	for(auto ending : endings){
		std::string endingFilename = withFilename? ending + ".tif" : ending;
		if (endingFilename.size() > value.size()) return false;
		if(std::equal(endingFilename.rbegin(), endingFilename.rend(), value.rbegin())){
			return true;
		}
	}
	std::cout << "Cannot find ending for " << value << std::endl;
	return false;

}

BOOST_AUTO_TEST_CASE(TestGetFilenameByString){
	MuscateMetadataHelper helper;
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1) << std::endl;
	if(helper.LoadMetadataFile(std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1))){
		std::string b12 = helper.getFileNameByString(helper.GetImageFileNames(), "B12");
		BOOST_CHECK_EQUAL(b12, helper.GetImageFileNames()[1]); //B12 is the first in the list
		std::string b8 = helper.getFileNameByString(helper.GetImageFileNames(), "B8");
		BOOST_CHECK_EQUAL(b8, helper.GetImageFileNames()[8]);
		std::string b8a = helper.getFileNameByString(helper.GetImageFileNames(), "B8A");
		BOOST_CHECK_EQUAL(b8a, helper.GetImageFileNames()[9]);
	}else{
		BOOST_FAIL("Cannot read metadata from" << std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1));
	}
}

BOOST_AUTO_TEST_CASE(TestLoadMetadata_S2A_T32MNE){
	MuscateMetadataHelper helper;
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	if(helper.LoadMetadataFile(std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1))){
		BOOST_CHECK_EQUAL(helper.GetMissionName(), "SENTINEL2A");
		BOOST_CHECK_EQUAL(helper.getProductLevel(), "L2A");
		BOOST_CHECK_EQUAL(helper.GetReflectanceQuantificationValue(), 10000);
		BOOST_CHECK_EQUAL(helper.GetNoDataValue(), "-10000");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDate(), "20170402");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDateLong(), "2017-04-02T09:38:44.724Z");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDateAsDoy(), 91);
		for(auto filename : helper.GetImageFileNames()){
			BOOST_CHECK(ends_with(filename, FREendingsS2));
		}
		for(auto filename : helper.GetCloudImageFileNames()){
			BOOST_CHECK(ends_with(filename, CLDendings));
		}
		for(auto filename : helper.GetAotImageFileNames()){
			BOOST_CHECK(ends_with(filename, ATBendings));
		}
		for(auto filename : helper.GetSnowImageFileNames()){
			BOOST_CHECK(ends_with(filename, MG2endings));
		}
		std::vector<std::string> r1 = helper.getResolutions().getNthResolutionFilenames(0);
		std::vector<std::string> r2 = helper.getResolutions().getNthResolutionFilenames(1);
		for(size_t i = 0; i < r1.size(); i++){
			BOOST_CHECK(ends_with(r1[i], FREendingsR1_S2));
		}
		for(size_t i = 0; i < r1.size(); i++){
			BOOST_CHECK(ends_with(r2[i], FREendingsR2_S2));
		}
	}else{
		BOOST_FAIL("Cannot read metadata from" << std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1));
	}

}

BOOST_AUTO_TEST_CASE(TestLoadMetadata_S2A_T31TCH){
	MuscateMetadataHelper helper;
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	if(helper.LoadMetadataFile(std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML2))){
		BOOST_CHECK_EQUAL(helper.GetMissionName(), "SENTINEL2A");
		BOOST_CHECK_EQUAL(helper.getProductLevel(), "L2A");
		BOOST_CHECK_EQUAL(helper.GetReflectanceQuantificationValue(), 10000);
		BOOST_CHECK_EQUAL(helper.GetNoDataValue(), "-10000");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDate(), "20170923");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDateLong(), "2017-09-23T10:57:17.357Z");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDateAsDoy(), 265);
		for(auto filename : helper.GetImageFileNames()){
			BOOST_CHECK(ends_with(filename, FREendingsS2));
		}
		for(auto filename : helper.GetCloudImageFileNames()){
			BOOST_CHECK(ends_with(filename, CLDendings));
		}
		for(auto filename : helper.GetAotImageFileNames()){
			BOOST_CHECK(ends_with(filename, ATBendings));
		}
		for(auto filename : helper.GetSnowImageFileNames()){
			BOOST_CHECK(ends_with(filename, MG2endings));
		}
		std::vector<std::string> r1 = helper.getResolutions().getNthResolutionFilenames(0);
		std::vector<std::string> r2 = helper.getResolutions().getNthResolutionFilenames(1);
		for(size_t i = 0; i < r1.size(); i++){
			BOOST_CHECK(ends_with(r1[i], FREendingsR1_S2));
		}
		for(size_t i = 0; i < r1.size(); i++){
			BOOST_CHECK(ends_with(r2[i], FREendingsR2_S2));
		}
	}else{
		BOOST_FAIL("Cannot read metadata from" << std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1));
	}

}

BOOST_AUTO_TEST_CASE(TestLoadMetadata_S2B_T31TCH){
	MuscateMetadataHelper helper;
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	if(helper.LoadMetadataFile(std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML3))){

		BOOST_CHECK_EQUAL(helper.GetMissionName(), "SENTINEL2B");
		BOOST_CHECK_EQUAL(helper.getProductLevel(), "L2A");
		BOOST_CHECK_EQUAL(helper.GetReflectanceQuantificationValue(), 10000);
		BOOST_CHECK_EQUAL(helper.GetNoDataValue(), "-10000");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDate(), "20171008");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDateLong(), "2017-10-08T10:50:12.463Z");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDateAsDoy(), 280);
		for(auto filename : helper.GetImageFileNames()){
			BOOST_CHECK(ends_with(filename, FREendingsS2));
		}
		for(auto filename : helper.GetCloudImageFileNames()){
			BOOST_CHECK(ends_with(filename, CLDendings));
		}
		for(auto filename : helper.GetAotImageFileNames()){
			BOOST_CHECK(ends_with(filename, ATBendings));
		}
		for(auto filename : helper.GetSnowImageFileNames()){
			BOOST_CHECK(ends_with(filename, MG2endings));
		}
		std::vector<std::string> r1 = helper.getResolutions().getNthResolutionFilenames(0);
		std::vector<std::string> r2 = helper.getResolutions().getNthResolutionFilenames(1);
		for(size_t i = 0; i < r1.size(); i++){
			BOOST_CHECK(ends_with(r1[i], FREendingsR1_S2));
		}
		for(size_t i = 0; i < r1.size(); i++){
			BOOST_CHECK(ends_with(r2[i], FREendingsR2_S2));
		}
	}else{
		BOOST_FAIL("Cannot read metadata from" << std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1));
	}
}
