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

#ifndef COMMON_INCLUDE_TESTIMAGECREATOR_H_
#define COMMON_INCLUDE_TESTIMAGECREATOR_H_

#include "BaseImageTypes.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

class TestImageCreator : public BaseImageTypes{
public:

	/**
	 * @brief Create test image of size height * width with values 0...height*width
	 * @param height
	 * @param width
	 * @return
	 */
	template<typename TImage>
	typename TImage::Pointer createTestImage(const size_t &height, const size_t &width){
		itk::Size<2> size({width, height});

		itk::Index<2> index;
		index.Fill(0);

		itk::ImageRegion<2> region(index, size);

		typename TImage::Pointer testImg = TImage::New();
		testImg->SetRegions(region);
		testImg->Allocate();
		testImg->FillBuffer(0);
		size_t i = 0;
		for(unsigned int c = 0; c < height; c++)
		{
			for(unsigned int r = 0; r < width; r++)
			{
				typename TImage::IndexType pixelIndex;
				pixelIndex[0] = r;
				pixelIndex[1] = c;
				testImg->SetPixel(pixelIndex, i++);
			}
		}
		return testImg.GetPointer();
	}
};

} //namespace ts


#endif /* COMMON_INCLUDE_TESTIMAGECREATOR_H_ */
