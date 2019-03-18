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
#define INPUT_XML1 "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_MTD_ALL.xml"

const std::vector<std::string> FRE_endings = {"FRE_B1", "FRE_B2", "FRE_B3", "FRE_B4", "FRE_B5", "FRE_B6", "FRE_B7", "FRE_B8", "FRE_B9", "FRE_B10", "FRE_B11", "FRE_B12"};
const std::vector<std::string> CLDendings = {"CLM_XS"};
const std::vector<std::string> MG2endings = {"MG2_XS"};
const std::vector<std::string> ATBendings = {"ATB_XS"};

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
		std::string b12 = helper.getFileNameByString(helper.GetImageFileNames(), "B10");
		BOOST_CHECK_EQUAL(b12, helper.GetImageFileNames()[1]); //B12 is the first in the list
		std::string b8 = helper.getFileNameByString(helper.GetImageFileNames(), "B6");
		BOOST_CHECK_EQUAL(b8, helper.GetImageFileNames()[8]);
		std::string b8a = helper.getFileNameByString(helper.GetImageFileNames(), "B7");
		BOOST_CHECK_EQUAL(b8a, helper.GetImageFileNames()[9]);
	}else{
		BOOST_FAIL("Cannot read metadata from" << std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1));
	}
}

BOOST_AUTO_TEST_CASE(TestLoadMetadata_Venus_FR_LUS){
	MuscateMetadataHelper helper;
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1) << std::endl;
	if(helper.LoadMetadataFile(std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1))){
		BOOST_CHECK_EQUAL(helper.GetMissionName(), "VENUS");
		BOOST_CHECK_EQUAL(helper.getProductLevel(), "L2A");
		BOOST_CHECK_EQUAL(helper.GetReflectanceQuantificationValue(), 1000);
		BOOST_CHECK_EQUAL(helper.GetAotQuantificationValue(), 200);
		BOOST_CHECK_EQUAL(helper.GetNoDataValue(), "-10000");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDate(), "20180202");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDateLong(), "2018-02-02T10:55:54.000");
		BOOST_CHECK_EQUAL(helper.GetAcquisitionDateAsDoy(), 32);
		for(auto filename : helper.GetImageFileNames()){
			BOOST_CHECK(ends_with(filename, FRE_endings));
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
		BOOST_CHECK_EQUAL(helper.getResolutions().getNumberOfResolutions(), size_t(1));
		std::vector<std::string> xs = helper.getResolutions().getNthResolutionFilenames(0);
		for(size_t i = 0; i < xs.size(); i++){
			BOOST_CHECK(ends_with(xs[i], FRE_endings));
		}
	}else{
		BOOST_FAIL("Cannot read metadata from" << std::string(TEST_SRC + "/" + TEST_NAME + "/" + INPUT_XML1));
	}
}
