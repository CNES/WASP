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

#ifndef COMPOSITEPREPROCESSING_INCLUDE_PREPROCESSINGSENTINEL_H_
#define COMPOSITEPREPROCESSING_INCLUDE_PREPROCESSINGSENTINEL_H_


#include "PreprocessingAdapter.h"
#include "ComputeNDVI.h"
#include "CreateS2AnglesRaster.h"
#include "DirectionalCorrection.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Namespace for all preprocessing steps
 */
namespace preprocessing {

class PreprocessingSentinel : public PreprocessingAdapter {
public:
	/**
	 * @brief Extract the muscate cloud mask of Sentinel
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The cloud mask stored as float image
	 */
	virtual FloatImageType::Pointer getCloudMask(const std::string &filename, const unsigned char &bit = 0);

	/**
	 * @brief Extract the muscate AOT mask of Sentinel
	 * @param filename The filename of the input file
	 * @param band The band number
	 * @return The AOT mask stored as float image
	 */
	virtual FloatImageType::Pointer getAotMask(const std::string &filename, const unsigned char &band = 2);

	/**
	 * @brief Extract the muscate snow mask of Sentinel
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The snow mask stored as float image
	 */
	virtual FloatImageType::Pointer getSnowMask(const std::string &filename, const unsigned char &bit = 2);

	/**
	 * @brief Extract the muscate water mask of Sentinel
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The water mask stored as float image
	 */
	virtual FloatImageType::Pointer getWaterMask(const std::string &filename, const unsigned char &bit = 0);

	/**
	 * @brief Extract the rasters of Sentinel, using a directional correction
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

	ComputeNDVI												m_computeNdvi;
	ImageResampler<FloatImageType, FloatImageType>			m_Resampler;
	std::vector<CreateS2AnglesRaster>						m_createAngles;
	std::vector<DirectionalCorrection>						m_dirCorr;
	ResamplingBandExtractor<FloatPixelType>					m_aotExtractor;

};

} // namespace preprocessing
} // namespace ts

#endif /* COMPOSITEPREPROCESSING_INCLUDE_PREPROCESSINGSENTINEL_H_ */
