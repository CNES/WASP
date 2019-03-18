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

#include "PreprocessingSentinel.h"
#include "PreprocessingAdapter.h"
#include "DirectionalCorrection.h"
#include "MetadataHelperFactory.h"

#include "otbWrapperApplication.h"

using namespace ts::preprocessing;

PreprocessingAdapter::FloatImageType::Pointer PreprocessingSentinel::getCloudMask(const std::string &filename, const unsigned char &bit){
	return getFloatMask(filename, bit);
}

PreprocessingAdapter::FloatImageType::Pointer PreprocessingSentinel::getAotMask(const std::string &filename, const unsigned char &band){

	m_aotReader = PreprocessingAdapter::FloatVectorImageReaderType::New();
	m_aotReader->SetFileName(filename);
	return m_aotExtractor.ExtractImgResampledBand(m_aotReader->GetOutput(), band, Interpolator_Linear);
}

PreprocessingAdapter::FloatImageType::Pointer PreprocessingSentinel::getWaterMask(const std::string &filename, const unsigned char &bit ){
	return getFloatMask(filename, bit);
}

PreprocessingAdapter::FloatImageType::Pointer PreprocessingSentinel::getSnowMask(const std::string &filename, const unsigned char &bit){
	return getFloatMask(filename, bit);
}

std::vector<PreprocessingAdapter::ShortVectorImageType::Pointer> PreprocessingSentinel::getCorrectedRasters(
		const std::string &filename,
		FloatImageType::Pointer cloudImage, FloatImageType::Pointer watImage,
		FloatImageType::Pointer snowImage){

	std::vector<PreprocessingAdapter::ShortVectorImageType::Pointer> outputRasters;

	auto factory = ts::MetadataHelperFactory::New();
	auto pHelper = factory->GetMetadataHelper(filename);

	if(m_scatteringCoeffs.empty()){
		itkExceptionMacro("Need to set scattering coefficients before running correction for S2.");
	}

	// Compute NDVI for Primary resolution
	m_computeNdvi.DoInit(filename);
	PreprocessingAdapter::FloatImageType::Pointer ndviImg = m_computeNdvi.DoExecute();

	size_t totalNRes = pHelper->getResolutions().getNumberOfResolutions();

	//For all possible resolutions, do the following
	for(size_t resolution = 0; resolution < totalNRes; resolution++){
		CreateS2AnglesRaster createAngles;
		createAngles.DoInit(resolution, filename);
		PreprocessingAdapter::FloatVectorImageType::Pointer anglesImg = createAngles.DoExecute();
		m_createAngles.push_back(createAngles);

		ts::DirectionalCorrection dirCorr;
		//If Resolution is not principal Resolution, then resize the additional images
		std::cout << "Current resolution: " << pHelper->getResolutions().getResolutionVector()[resolution].getBands()[0].getResolution() << std::endl;
		if(cloudImage.GetPointer()->GetSpacing()[0] != pHelper->getResolutions().getResolutionVector()[resolution].getBands()[0].getResolution()){
			PreprocessingAdapter::FloatImageType::Pointer ndviImgResampled = m_Resampler.getResampler(ndviImg.GetPointer(), 0.5f)->GetOutput();
			PreprocessingAdapter::FloatImageType::Pointer cldImgResampled = m_Resampler.getResampler(cloudImage.GetPointer(), 0.5f)->GetOutput();
			PreprocessingAdapter::FloatImageType::Pointer watImgResampled = m_Resampler.getResampler(watImage.GetPointer(), 0.5f)->GetOutput();
			PreprocessingAdapter::FloatImageType::Pointer snowImgResampled = m_Resampler.getResampler(snowImage.GetPointer(), 0.5f)->GetOutput();
			dirCorr.Init(resolution, filename, m_scatteringCoeffs[resolution], cldImgResampled, watImgResampled, snowImgResampled, anglesImg, ndviImgResampled);
		}else{
			dirCorr.Init(resolution, filename, m_scatteringCoeffs[resolution], cloudImage, watImage, snowImage, anglesImg, ndviImg);
		}
		dirCorr.DoExecute();
		m_dirCorr.push_back(dirCorr);
		outputRasters.push_back(dirCorr.GetCorrectedImg().GetPointer());
	}
	return outputRasters;
}

