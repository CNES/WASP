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

#ifndef COMMON_INCLUDE_BASEIMAGETYPES_H_
#define COMMON_INCLUDE_BASEIMAGETYPES_H_


#include "itkLightObject.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbObjectList.h"
#include "otbImageList.h"
#include "otbWrapperApplication.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {
/**
 * @brief Stores all base typedefs used in the processing chain
 */
class BaseImageTypes{
public:
	typedef unsigned char											BytePixelType;
	typedef otb::Image<BytePixelType, 2>							ByteImageType;
	typedef otb::ImageFileReader<ByteImageType>                     ByteImageReaderType;
	typedef otb::ObjectList<ByteImageReaderType>					ByteImageReaderListType;
	typedef otb::ImageList<ByteImageType>							ByteImageListType;

	typedef short													ShortPixelType;
	typedef otb::Image<ShortPixelType, 2>							ShortImageType;
	typedef otb::ImageFileReader<ShortImageType>					ShortImageReaderType;
	typedef otb::ObjectList<ShortImageReaderType>					ShortImageReaderListType;
	typedef otb::ImageList<ShortImageType>							ShortImageListType;

    typedef otb::Wrapper::Int16VectorImageType                    	ShortVectorImageType;
	typedef otb::ImageFileReader<ShortVectorImageType>				ShortVectorImageReaderType;
	typedef otb::ObjectList<ShortVectorImageReaderType>				ShortVectorImageReaderListType;
	typedef otb::ImageList<ShortVectorImageType>					ShortVectorImageListType;

	typedef float													FloatPixelType;
	typedef otb::Image<FloatPixelType, 2>							FloatImageType;
	typedef otb::ImageFileReader<FloatImageType>					FloatImageReaderType;
	typedef otb::ObjectList<FloatImageReaderType>					FloatImageReaderListType;
	typedef otb::ImageList<FloatImageType>							FloatImageListType;

	typedef otb::Wrapper::FloatVectorImageType						FloatVectorImageType;
	typedef otb::ImageFileReader<FloatVectorImageType>				FloatVectorImageReaderType;
	typedef otb::ObjectList<FloatVectorImageReaderType>				FloatVectorImageReaderListType;
	typedef otb::ImageList<FloatVectorImageType>					FloatVectorImageListType;

};
} // namespace ts


#endif /* COMMON_INCLUDE_BASEIMAGETYPES_H_ */
