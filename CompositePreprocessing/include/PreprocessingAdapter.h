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

#ifndef COMPOSITEPREPROCESSING_INCLUDE_PREPROCESSINGADAPTER_H_
#define COMPOSITEPREPROCESSING_INCLUDE_PREPROCESSINGADAPTER_H_

#include "itkLightObject.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbObjectList.h"

#include "BaseImageTypes.h"
#include "MaskExtractorFilter.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Namespace for all preprocessing steps
 */
namespace preprocessing {

class PreprocessingAdapter  : public itk::LightObject, public BaseImageTypes{
public:
	typedef MaskExtractorFilter<ByteImageType,FloatImageType>		ExtractorFilterType;
	typedef otb::ObjectList<ExtractorFilterType>					ExtractorListType;

	void init(){
		m_MaskList = ByteImageReaderListType::New();
		m_ExtractorList = ExtractorListType::New();
	}

	/**
	 * @brief Extract the muscate cloud mask of the mission
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The cloud mask stored as float image
	 */
	virtual FloatImageType::Pointer getCloudMask(const std::string &filename, const unsigned char &bit = 0) = 0;

	/**
	 * @brief Extract the muscate AOT mask of the mission
	 * @param filename The filename of the input file
	 * @param band The band number
	 * @return The AOT mask stored as float image
	 */
	virtual FloatImageType::Pointer getAotMask(const std::string &filename, const unsigned char &band = 2) = 0;

	/**
	 * @brief Extract the muscate snow mask of the mission
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The snow mask stored as float image
	 */
	virtual FloatImageType::Pointer getSnowMask(const std::string &filename, const unsigned char &bit = 2) = 0;

	/**
	 * @brief Extract the muscate water mask of the mission
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The water mask stored as float image
	 */
	virtual FloatImageType::Pointer getWaterMask(const std::string &filename, const unsigned char &bit = 0) = 0;

	/**
	 * @brief Set the scattering coefficient filenames
	 * @param scatteringcoeffs The path to each scattering coefficient file
	 */
	void setScatteringCoefficients(const std::vector<std::string> &scatteringcoeffs);

	virtual std::vector<ShortVectorImageType::Pointer> getCorrectedRasters(
			const std::string &filename,
			FloatImageType::Pointer cloudImage, FloatImageType::Pointer watImage,
			FloatImageType::Pointer snowImage) = 0;
protected:
	/**
	 * @brief Extract a float image
	 * @param filename The filename of the input file
	 * @param bit The bit number
	 * @return The pointer to the image
	 */
	FloatImageType::Pointer getFloatMask(const std::string &filename, const unsigned char &bit);


	FloatVectorImageReaderType::Pointer						m_aotReader;
	ByteImageReaderListType::Pointer						m_MaskList;
	ExtractorListType::Pointer								m_ExtractorList;
	std::vector<std::string>								m_scatteringCoeffs;
};

} // namespace preprocessing
} // namespace ts

#endif /* COMPOSITEPREPROCESSING_INCLUDE_PREPROCESSINGADAPTER_H_ */
