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

#ifndef COMPOSITEPREPROCESSING_INCLUDE_PREPROCESSINGVENUS_H_
#define COMPOSITEPREPROCESSING_INCLUDE_PREPROCESSINGVENUS_H_

#include "PreprocessingAdapter.h"
#include "ComputeNDVI.h"
#include "ResamplingBandExtractor.h"
#include "otbImageListToVectorImageFilter.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Namespace for all preprocessing steps
 */
namespace preprocessing {

class PreprocessingVenus : public PreprocessingAdapter {
public:
	/**
	 * @brief Extract the muscate cloud mask of Venus
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The cloud mask stored as float image
	 */
	virtual FloatImageType::Pointer getCloudMask(const std::string &filename, const unsigned char &bit = 0);

	/**
	 * @brief Extract the muscate AOT mask of Venus
	 * @param filename The filename of the input file
	 * @param band The band number
	 * @return The AOT mask stored as float image
	 */
	virtual FloatImageType::Pointer getAotMask(const std::string &filename, const unsigned char &band = 2);

	/**
	 * @brief Extract the muscate snow mask of Venus
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The snow mask stored as float image
	 */
	virtual FloatImageType::Pointer getSnowMask(const std::string &filename, const unsigned char &bit = 2);

	/**
	 * @brief Extract the muscate water mask of Venus
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The water mask stored as float image
	 */
	virtual FloatImageType::Pointer getWaterMask(const std::string &filename, const unsigned char &bit = 0);

	/**
	 * @brief Extract the rasters of Venus
	 * @param xml The filename to the xml-file
	 * @param cldImg Cloud Image file
	 * @param watImg Water image file
	 * @param snowImg Snow image file
	 * @param angles Angles raster
	 * @param ndvi NDVI image
	 * @return The raster for each resolution type stored in a floatVector-Image
	 */
	virtual std::vector<ShortVectorImageType::Pointer> getCorrectedRasters(
			const std::string &filename,
			FloatImageType::Pointer cloudImage, FloatImageType::Pointer watImage,
			FloatImageType::Pointer snowImage);

private:
	typedef otb::ImageListToVectorImageFilter<
			ShortImageListType, ShortVectorImageType> ShortImageListToVectorImageType;

	ResamplingBandExtractor<FloatPixelType>										m_aotExtractor;
	ShortVectorImageReaderType::Pointer        									m_inputImageReader;
	ShortImageReaderListType::Pointer											m_ReaderList;
	std::vector<ShortImageListType::Pointer>       								m_ImageList;
	std::vector<ShortImageListToVectorImageType::Pointer>						m_RasterExtractorList;
};

} // namespace preprocessing
} // namespace ts

#endif /* COMPOSITEPREPROCESSING_INCLUDE_PREPROCESSINGVENUS_H_ */
