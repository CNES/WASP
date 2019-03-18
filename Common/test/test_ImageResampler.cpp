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

#define BOOST_TEST_MODULE ImageResampler
#include <boost/test/unit_test.hpp>
#include "../include/ImageResampler.h"
#include "TestImageCreator.h"
#include <string>

using namespace ts;

typedef short										InputPixelType;
typedef float										OutputPixelType;

typedef otb::Image<OutputPixelType, 2> 				OutputImageType;
typedef otb::Image<InputPixelType, 2> 				InputImageType;


BOOST_AUTO_TEST_CASE( testResamplingFloat ){

	ts::TestImageCreator t;
	OutputImageType::Pointer testImg = t.createTestImage<OutputImageType>(5,5);
	ImageResampler<OutputImageType, OutputImageType> resampler;

	OutputImageType::Pointer output = resampler.getResamplerWantedSize(testImg.GetPointer(), 2, 2)->GetOutput();
	output->Update();
	itk::ImageRegionIterator<OutputImageType> imageIterator(output,output->GetLargestPossibleRegion());

	std::vector<OutputPixelType> ref = {6,9,21,24};
	size_t i = 0;
	while(!imageIterator.IsAtEnd())
	{
		BOOST_CHECK_EQUAL(imageIterator.Get(), ref[i++]);
		++imageIterator;
	}
}

BOOST_AUTO_TEST_CASE( testResamplingShort ){

	ts::TestImageCreator t;
	InputImageType::Pointer testImg = t.createTestImage<InputImageType>(5,5);
	ImageResampler<InputImageType, InputImageType> resampler;

	InputImageType::Pointer output = resampler.getResamplerWantedSize(testImg.GetPointer(), 2, 2)->GetOutput();
	output->Update();
	itk::ImageRegionIterator<InputImageType> imageIterator(output,output->GetLargestPossibleRegion());

	std::vector<InputPixelType> ref = {6,9,21,24};
	size_t i = 0;
	while(!imageIterator.IsAtEnd())
	{
		BOOST_CHECK_EQUAL(imageIterator.Get(), ref[i++]);
		++imageIterator;
	}
}

