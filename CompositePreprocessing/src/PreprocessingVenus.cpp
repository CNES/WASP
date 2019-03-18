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

#include "PreprocessingVenus.h"
#include "PreprocessingAdapter.h"
#include "DirectionalCorrection.h"
#include "MetadataHelperFactory.h"

using namespace ts::preprocessing;

PreprocessingAdapter::FloatImageType::Pointer PreprocessingVenus::getCloudMask(const std::string &filename, const unsigned char &bit){
	return getFloatMask(filename, bit);
}

PreprocessingAdapter::FloatImageType::Pointer PreprocessingVenus::getAotMask(const std::string &filename, const unsigned char &band){
	m_aotReader = PreprocessingAdapter::FloatVectorImageReaderType::New();
	m_aotReader->SetFileName(filename);
	return m_aotExtractor.ExtractImgResampledBand(m_aotReader->GetOutput(), band, Interpolator_Linear);
}

PreprocessingAdapter::FloatImageType::Pointer PreprocessingVenus::getWaterMask(const std::string &filename, const unsigned char &bit ){
	return getFloatMask(filename, bit);
}

PreprocessingAdapter::FloatImageType::Pointer PreprocessingVenus::getSnowMask(const std::string &filename, const unsigned char &bit){
	return getFloatMask(filename, bit);
}

std::vector<PreprocessingAdapter::ShortVectorImageType::Pointer> PreprocessingVenus::getCorrectedRasters(
		const std::string &filename,
		FloatImageType::Pointer cloudImage, FloatImageType::Pointer watImage,
		FloatImageType::Pointer snowImage){

	m_ReaderList = ShortImageReaderListType::New();
	std::vector<PreprocessingAdapter::ShortVectorImageType::Pointer> outputRasters;

	auto factory = ts::MetadataHelperFactory::New();
	auto pHelper = factory->GetMetadataHelper(filename);

	size_t totalNRes = pHelper->getResolutions().getNumberOfResolutions();

	//For all possible resolutions, do the following
	for(size_t resolution = 0; resolution < totalNRes; resolution++){
		std::vector<std::string> inputImageFiles = pHelper->getResolutions().getNthResolutionFilenames(resolution);
		ShortImageListType::Pointer imgList = ShortImageListType::New();
		for(auto filename : inputImageFiles){
			std::cout << "Raster Filename: " << filename << std::endl;
			ShortImageReaderType::Pointer reader = ShortImageReaderType::New();
			reader->SetFileName(filename);
			m_ReaderList->PushBack(reader);
			ShortImageType::Pointer inputImg = reader->GetOutput();
			inputImg->UpdateOutputInformation();
			imgList->PushBack(inputImg);
		}
		ShortImageListToVectorImageType::Pointer toVec = ShortImageListToVectorImageType::New();
		toVec->SetInput(imgList);
		m_ImageList.push_back(imgList);
		m_RasterExtractorList.push_back(toVec);
		outputRasters.push_back(toVec->GetOutput());
	}
	return outputRasters;
}
