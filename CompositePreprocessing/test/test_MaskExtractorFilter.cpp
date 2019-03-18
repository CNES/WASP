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

#define BOOST_TEST_MODULE MaskExtractorFilter
#include <boost/test/unit_test.hpp>
#include "MaskExtractorFilter.h"
#include "GlobalDefs.h"
#include "TestImageCreator.h"

using namespace ts;

typedef unsigned char								InputPixelType;
typedef float										OutputPixelType;

typedef otb::Image<OutputPixelType, 2> 				OutputImageType;
typedef otb::Image<InputPixelType, 2> 				InputImageType;
typedef ts::MaskExtractorFilter<InputImageType,
		OutputImageType>							ExtractorType;


BOOST_AUTO_TEST_CASE(testMaskExtractorFilterBit1){
	size_t width = 1;
	size_t height = 255;
	unsigned char bit = 0;
	TestImageCreator c;
	InputImageType::Pointer img = c.createTestImage<InputImageType>(height, width);

	ExtractorType::Pointer maskExtractor;
	maskExtractor = ExtractorType::New();
	maskExtractor->SetBitMask(bit);
	maskExtractor->SetInput(img.GetPointer());

	OutputImageType::Pointer output = maskExtractor->GetOutput();
	try
	{
		output->Update();
	}
	catch (itk::ExceptionObject& err)
	{
		std::cout << "ExceptionObject caught !" << std::endl;
		std::cout << err << std::endl;
	}

	size_t i = 0;
	for(unsigned int r = 0; r < width; r++)
	{
		for(unsigned int c = 0; c < height; c++)
		{
			InputImageType::IndexType pixelIndex;
			pixelIndex[0] = r;
			pixelIndex[1] = c;
			BOOST_CHECK_EQUAL(((i & (bit+1)) >> bit), output->GetPixel(pixelIndex));
			i++;
		}
	}
}

BOOST_AUTO_TEST_CASE(testMaskExtractorFilterBit3){
	size_t width = 1;
	size_t height = 6;
	unsigned char bit = 3;
	TestImageCreator c;
	InputImageType::Pointer img = c.createTestImage<InputImageType>(height, width);

	ExtractorType::Pointer maskExtractor;
	maskExtractor = ExtractorType::New();
	maskExtractor->SetBitMask(bit);
	maskExtractor->SetInput(img.GetPointer());

	OutputImageType::Pointer output = maskExtractor->GetOutput();
	try
	{
		output->Update();
	}
	catch (itk::ExceptionObject& err)
	{
		std::cout << "ExceptionObject caught !" << std::endl;
		std::cout << err << std::endl;
	}

	size_t i = 0;
	for(unsigned int r = 0; r < width; r++)
	{
		for(unsigned int c = 0; c < height; c++)
		{
			InputImageType::IndexType pixelIndex;
			pixelIndex[0] = r;
			pixelIndex[1] = c;
			BOOST_CHECK_EQUAL(((i & (bit+1)) >> bit), output->GetPixel(pixelIndex));
			i++;
		}
	}
}
