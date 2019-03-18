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

#define BOOST_TEST_MODULE MetadataBands
#include <boost/test/unit_test.hpp>
#include "MetadataBands.h"
#include <string>

using namespace ts;

const std::string SREtype = std::string(FLAT_REFLECTANCE_SUFFIX);
const std::vector<std::string> SREendings = {"B2", "B3", "B4", "B5", "B6", "B7", "B8", "B8A", "B11", "B12"};
const std::vector<size_t> SREresolutions = {10, 10, 10, 20, 20, 20, 10, 20, 20, 20};


Resolution initR1(){
	Resolution R1("R1");
	for(size_t i = 0; i < SREendings.size(); i++){
		Band B(std::string(SREtype + "_" + SREendings[i] + TIF_EXTENSION), SREendings[i], SREresolutions[i]);
		if(SREresolutions[i] == 10){
			R1.addBand(B);
		}
	}

	return R1;
}

Resolution initR2(){
	Resolution R2("R2");
	for(size_t i = 0; i < SREendings.size(); i++){
		Band B(std::string(SREtype + "_" + SREendings[i] + TIF_EXTENSION), SREendings[i], SREresolutions[i]);
		if(SREresolutions[i] == 20){
			R2.addBand(B);
		}
	}

	return R2;
}

BOOST_AUTO_TEST_CASE( testBandPresence ){
	Resolution R1 = initR1(); Resolution R2 = initR2();
	MultiResolution res;
	BOOST_CHECK_EQUAL(res.getNumberOfResolutions(), size_t(0));
	BOOST_CHECK_EQUAL(res.getTotalNumberOfBands(), size_t(0));
	res.addResolution(R1); //Main resolution, therefore has to be added first
	BOOST_CHECK_EQUAL(res.getNumberOfResolutions(), size_t(1));
	BOOST_CHECK_EQUAL(res.getTotalNumberOfBands(), size_t(4));
	res.addResolution(R2);
	BOOST_CHECK_EQUAL(res.getNumberOfResolutions(), size_t(2));
	BOOST_CHECK_EQUAL(res.getTotalNumberOfBands(), size_t(10));

	size_t totalNumber = res.getTotalNumberOfBands();

	for(size_t i = 0; i < totalNumber; i++){
		BOOST_CHECK(res.doesBandTypeExist(SREendings[i]));
	}
}

BOOST_AUTO_TEST_CASE( testBandFilenames ){
	Resolution R1 = initR1(); Resolution R2 = initR2();
	MultiResolution res;
	res.addResolution(R1); //Main resolution, therefore has to be added first
	res.addResolution(R2);

	BOOST_CHECK_EQUAL(res.getSpecificBandTypeFilename(SREendings[0]), "FRE_B2.tif");
	BOOST_CHECK_EQUAL(res.getSpecificBandTypeFilename(SREendings[9]), "FRE_B12.tif");

	productReturnType filenames = res.getNthResolutionFilenames(0); //Main
	BOOST_CHECK_EQUAL(filenames.size(), size_t(4));
	BOOST_CHECK_EQUAL(filenames[0], "FRE_B2.tif");
	BOOST_CHECK_EQUAL(filenames[1], "FRE_B3.tif");
	BOOST_CHECK_EQUAL(filenames[2], "FRE_B4.tif");
	BOOST_CHECK_EQUAL(filenames[3], "FRE_B8.tif");

}
