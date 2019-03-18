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

#define BOOST_TEST_MODULE MuscateReadWrite
#include <boost/test/unit_test.hpp>

#include "GlobalDefs.h"
#include "MuscateMetadataWriter.hpp"
#include "MuscateMetadataReader.hpp"
#include "MetadataUtil.hpp"
#include "stdlib.h"

#define TEST_NAME	"test_MuscateMetadata"
#define TEST_SRC	ts::getEnvVar("WASP_TEST")
#define EXAMPLE_XML1 "SENTINEL2A_20170402-093844-724_L2A_T32MNE_D_V1-4_MTD_ALL.xml"
#define EXAMPLE_XML2 "SENTINEL2B_20171008-105012-463_L2A_T31TCH_C_V1-0_MTD_ALL.xml"
#define EXAMPLE_XML3 "LANDSAT8-OLITIRS-XS_20170504-103532-111_L2A_T31TCH_C_V1-0_MTD_ALL.xml"
#define EXAMPLE_XML4 "VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_MTD_ALL.xml"

#define EXAMPLE_XML_OUT "S2A_MuscateWriteRead.xml"
#define EXAMPLE_XML_OUT2 "S2B_MuscateWriteRead.xml"
#define EXAMPLE_XML_OUT3 "L8_MuscateWriteRead.xml"
#define EXAMPLE_XML_OUT4 "VENUS_MuscateWriteRead.xml"

using namespace ts::muscate;

BOOST_AUTO_TEST_CASE(MuscateReadWriteS2A){
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" EXAMPLE_XML1) << std::endl;
	auto reader = MuscateMetadataReader::New();
	auto writer = MuscateMetadataWriter::New();

	std::string pathIn =  TEST_SRC + "/" + TEST_NAME + "/" EXAMPLE_XML1;
	std::string pathOut = EXAMPLE_XML_OUT;
	auto m = reader->ReadMetadata(pathIn);
	MuscateFileMetadata m_toWrite = MuscateFileMetadata(*m);
	writer->WriteMetadata(m_toWrite, pathOut);
	auto mRead = reader->ReadMetadata(pathOut);

	BOOST_REQUIRE(mRead);

	BOOST_CHECK(m_toWrite.Header == mRead->Header);
	BOOST_CHECK(m_toWrite.MetadataIdentification == mRead->MetadataIdentification);
	BOOST_CHECK(m_toWrite.DatasetIdentification == mRead->DatasetIdentification);
	BOOST_CHECK(m_toWrite.ProductCharacteristics == mRead->ProductCharacteristics);
	BOOST_CHECK(m_toWrite.ProductOrganisation == mRead->ProductOrganisation);
	BOOST_CHECK(m_toWrite.GeopositionInformations == mRead->GeopositionInformations);
	BOOST_CHECK(m_toWrite.RadiometricInformations == mRead->RadiometricInformations);
	BOOST_CHECK(m_toWrite.QualityInformations == mRead->QualityInformations);
}

BOOST_AUTO_TEST_CASE(MuscateReadWriteS2B){
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" EXAMPLE_XML2) << std::endl;
	auto reader = MuscateMetadataReader::New();
	auto writer = MuscateMetadataWriter::New();

	std::string pathIn =  TEST_SRC + "/" + TEST_NAME + "/" EXAMPLE_XML2;
	std::string pathOut = EXAMPLE_XML_OUT2;
	auto m = reader->ReadMetadata(pathIn);
	MuscateFileMetadata m_toWrite = MuscateFileMetadata(*m);
	writer->WriteMetadata(m_toWrite, pathOut);
	auto mRead = reader->ReadMetadata(pathOut);

	BOOST_REQUIRE(mRead);

	BOOST_CHECK(m_toWrite.Header == mRead->Header);
	BOOST_CHECK(m_toWrite.MetadataIdentification == mRead->MetadataIdentification);
	BOOST_CHECK(m_toWrite.DatasetIdentification == mRead->DatasetIdentification);
	BOOST_CHECK(m_toWrite.ProductCharacteristics == mRead->ProductCharacteristics);
	BOOST_CHECK(m_toWrite.ProductOrganisation == mRead->ProductOrganisation);
	BOOST_CHECK(m_toWrite.GeopositionInformations == mRead->GeopositionInformations);
	BOOST_CHECK(m_toWrite.RadiometricInformations == mRead->RadiometricInformations);
	BOOST_CHECK(m_toWrite.QualityInformations == mRead->QualityInformations);
}

BOOST_AUTO_TEST_CASE(MuscateReadWriteL8){
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" EXAMPLE_XML3) << std::endl;
	auto reader = MuscateMetadataReader::New();
	auto writer = MuscateMetadataWriter::New();

	std::string pathIn =  TEST_SRC + "/" + TEST_NAME + "/" EXAMPLE_XML3;
	std::string pathOut = EXAMPLE_XML_OUT3;
	auto m = reader->ReadMetadata(pathIn);
	MuscateFileMetadata m_toWrite = MuscateFileMetadata(*m);
	writer->WriteMetadata(m_toWrite, pathOut);
	auto mRead = reader->ReadMetadata(pathOut);

	BOOST_REQUIRE(mRead);

	BOOST_CHECK(m_toWrite.Header == mRead->Header);
	BOOST_CHECK(m_toWrite.MetadataIdentification == mRead->MetadataIdentification);
	BOOST_CHECK(m_toWrite.DatasetIdentification == mRead->DatasetIdentification);
	BOOST_CHECK(m_toWrite.ProductCharacteristics == mRead->ProductCharacteristics);
	BOOST_CHECK(m_toWrite.ProductOrganisation == mRead->ProductOrganisation);
	BOOST_CHECK(m_toWrite.GeopositionInformations == mRead->GeopositionInformations);
	BOOST_CHECK(m_toWrite.RadiometricInformations == mRead->RadiometricInformations);
	BOOST_CHECK(m_toWrite.QualityInformations == mRead->QualityInformations);
}

BOOST_AUTO_TEST_CASE(MuscateReadWriteVenus){
	if(TEST_SRC == ""){
		BOOST_FAIL("Did not set env-var WASP_TEST!");
	}
	std::cout << std::string(TEST_SRC + "/" + TEST_NAME + "/" EXAMPLE_XML4) << std::endl;
	auto reader = MuscateMetadataReader::New();
	auto writer = MuscateMetadataWriter::New();

	std::string pathIn =  TEST_SRC + "/" + TEST_NAME + "/" EXAMPLE_XML4;
	std::string pathOut = EXAMPLE_XML_OUT4;
	auto m = reader->ReadMetadata(pathIn);
	MuscateFileMetadata m_toWrite = MuscateFileMetadata(*m);
	writer->WriteMetadata(m_toWrite, pathOut);
	auto mRead = reader->ReadMetadata(pathOut);

	BOOST_REQUIRE(mRead);

	BOOST_CHECK(m_toWrite.Header == mRead->Header);
	BOOST_CHECK(m_toWrite.MetadataIdentification == mRead->MetadataIdentification);
	BOOST_CHECK(m_toWrite.DatasetIdentification == mRead->DatasetIdentification);
	BOOST_CHECK(m_toWrite.ProductCharacteristics == mRead->ProductCharacteristics);
	BOOST_CHECK(m_toWrite.ProductOrganisation == mRead->ProductOrganisation);
	BOOST_CHECK(m_toWrite.GeopositionInformations == mRead->GeopositionInformations);
	BOOST_CHECK(m_toWrite.RadiometricInformations == mRead->RadiometricInformations);
	BOOST_CHECK(m_toWrite.QualityInformations == mRead->QualityInformations);
}
