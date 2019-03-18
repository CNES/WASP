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

#define BOOST_TEST_MODULE ComputeNDVI
#include <boost/test/unit_test.hpp>
#include "ComputeNDVI.h"
#include "GlobalDefs.h"
#include "TestImageCreator.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkImageRegionIterator.h"

using namespace ts;

#define WASP_TEST									"WASP_TEST"
#define TEST_NAME									"test_Preprocessing"

typedef float										OutputPixelType;
typedef otb::Image<OutputPixelType, 2> 				OutputImageType;
typedef ts::ComputeNDVI								NDVIType;
typedef otb::ImageFileReader<OutputImageType>		OutputImageReaderType;
typedef otb::ImageFileWriter<OutputImageType>		OutputImageWriterType;

BOOST_AUTO_TEST_CASE(testNDVI){
	std::string wasp_test = getEnvVar(WASP_TEST);
	if(wasp_test.empty()){
		std::cout << "Cannot find WASP_TEST environment variable. Exiting..." << std::endl;
		exit(1);
	}
	std::string xml = wasp_test + "/"
			+ TEST_NAME + "/"
			"INPUTS/"
			"SENTINEL2A_20180315-144453-175_L2A_T19LGH_D_V1-6/"
			"SENTINEL2A_20180315-144453-175_L2A_T19LGH_D_V1-6_MTD_ALL.xml";

	NDVIType ndvi;
	ndvi.DoInit(xml);
	OutputImageType::Pointer output = ndvi.DoExecute().GetPointer();
	OutputImageReaderType::Pointer gReader = OutputImageReaderType::New();
	gReader->SetFileName(wasp_test + "/" + TEST_NAME + "/INPUTS/" + "ndvi.tif");
	OutputImageType::Pointer reference = gReader->GetOutput();
	reference->Update();
	output->Update();
	itk::ImageRegionIterator<OutputImageType> imageIteratorRef(reference,reference->GetLargestPossibleRegion());
	itk::ImageRegionIterator<OutputImageType> imageIteratorNew(output,output->GetLargestPossibleRegion());

	BOOST_CHECK_EQUAL(reference->GetLargestPossibleRegion(), output->GetLargestPossibleRegion());
	while(!imageIteratorRef.IsAtEnd())
	{
		BOOST_CHECK_EQUAL(imageIteratorRef.Get(), imageIteratorNew.Get());
		++imageIteratorRef;
		++imageIteratorNew;
	}
};

