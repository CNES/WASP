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

#define BOOST_TEST_MODULE CreateS2AnglesRaster
#include <boost/test/unit_test.hpp>
#include "CreateS2AnglesRaster.h"
#include "GlobalDefs.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "otbWrapperApplication.h"

using namespace ts;

#define WASP_TEST									"WASP_TEST"
#define TEST_NAME									"test_Preprocessing"

typedef float										OutputPixelType;
typedef otb::Wrapper::FloatVectorImageType			OutputImageType;
typedef ts::CreateS2AnglesRaster					RasterCreationType;
typedef otb::ImageFileReader<OutputImageType>		OutputImageReaderType;
typedef otb::ImageFileWriter<OutputImageType>		OutputImageWriterType;

BOOST_AUTO_TEST_CASE(testS2AnglesRasterCreationR2){
	int resolution = 1;
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
	std::cout << "XML : "<< xml << std::endl;
	RasterCreationType rasterCreator;
	rasterCreator.DoInit(resolution, xml);
	OutputImageType::Pointer output = rasterCreator.DoExecute();
	OutputImageReaderType::Pointer gReader = OutputImageReaderType::New();
	gReader->SetFileName(wasp_test + "/" + TEST_NAME + "/INPUTS/" + "s2angles_raster_r" + std::to_string(resolution+1) + ".tif");
	OutputImageType::Pointer reference = gReader->GetOutput();
	reference->Update();
	output->Update();
	itk::ImageRegionIterator<OutputImageType> imageIteratorRef(reference,reference->GetLargestPossibleRegion());
	itk::ImageRegionIterator<OutputImageType> imageIteratorNew(output,output->GetLargestPossibleRegion());

	BOOST_CHECK_EQUAL(reference->GetLargestPossibleRegion(), output->GetLargestPossibleRegion());
	while(!imageIteratorRef.IsAtEnd())
	{
		itk::VariableLengthVector<float> referenceBands = imageIteratorRef.Get();
		itk::VariableLengthVector<float> newBands = imageIteratorNew.Get();
		BOOST_CHECK_EQUAL(referenceBands.GetSize(), newBands.GetSize());
		for(size_t i = 0; i < referenceBands.GetSize(); i++){
			if(!std::isnan(referenceBands[i]) && !std::isnan(newBands[i])){
				BOOST_CHECK_EQUAL(referenceBands[i], newBands[i]);
			}
		}
		++imageIteratorRef;
		++imageIteratorNew;
	}
};

#ifdef R1
BOOST_AUTO_TEST_CASE(testS2AnglesRasterCreationR1){
	int resolution = 0;
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
	std::cout << "XML : "<< xml << std::endl;
	RasterCreationType rasterCreator;
	rasterCreator.DoInit(resolution, xml);
	OutputImageType::Pointer output = rasterCreator.DoExecute();
	OutputImageReaderType::Pointer gReader = OutputImageReaderType::New();
	gReader->SetFileName(wasp_test + "/" + TEST_NAME + "/INPUTS/" + "s2angles_raster_r" + std::to_string(resolution+1) + ".tif");
	OutputImageType::Pointer reference = gReader->GetOutput();
	reference->Update();
	output->Update();
	itk::ImageRegionIterator<OutputImageType> imageIteratorRef(reference,reference->GetLargestPossibleRegion());
	itk::ImageRegionIterator<OutputImageType> imageIteratorNew(output,output->GetLargestPossibleRegion());

	BOOST_CHECK_EQUAL(reference->GetLargestPossibleRegion(), output->GetLargestPossibleRegion());
	while(!imageIteratorRef.IsAtEnd())
	{
		itk::VariableLengthVector<float> referenceBands = imageIteratorRef.Get();
		itk::VariableLengthVector<float> newBands = imageIteratorNew.Get();
		BOOST_CHECK_EQUAL(referenceBands.GetSize(), newBands.GetSize());
		for(size_t i = 0; i < referenceBands.GetSize(); i++){
			if(!std::isnan(referenceBands[i]) && !std::isnan(newBands[i])){
				BOOST_CHECK_EQUAL(referenceBands[i], newBands[i]);
			}
		}
		++imageIteratorRef;
		++imageIteratorNew;
	}
};
#endif
