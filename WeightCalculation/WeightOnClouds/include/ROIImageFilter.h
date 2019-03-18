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

#ifndef ROIIMAGEHANDLER_H
#define ROIIMAGEHANDLER_H

#include "otbVectorImage.h"
#include "otbWrapperTypes.h"

//Transform
#include "otbImageFileWriter.h"
#include "otbExtractROI.h"
#include "itkCropImageFilter.h"
#include "ImageResampler.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts
{

/**
 * @brief Crop an image from a given ROI
 */
template <typename TInput1, typename TInput2>
class ROIImageHandler
{
public:

	typedef otb::ImageFileWriter<TInput2> WriterType;
	typedef itk::ImageSource<TInput1> ImageSource;
	//typedef itk::ImageSource<TInput2> ResampledImageSource;
	typedef itk::CropImageFilter<TInput2, TInput2> ROIFilterType;
	typedef itk::ImageSource<TInput2> OutImageSource;

public:
	ROIImageHandler() {
		m_outForcedWidth = -1;
		m_outForcedHeight = -1;
	}

	void SetOutputFileName(std::string &outFile) {
		m_outputFileName = outFile;
	}

	void SetInputImageReader(typename ImageSource::Pointer inputImgReader, int width, int height) {
		if (inputImgReader.IsNull())
		{
			std::cout << "No input Image set...; please set the input image!" << std::endl;
			itkExceptionMacro("No input Image set...; please set the input image");
		}
		m_inputImgReader = inputImgReader;
		m_outForcedWidth = width;
		m_outForcedHeight = height;
	}

	const char *GetNameOfClass() { return "CuttingImageHandler";}

	typename OutImageSource::Pointer GetOutputImageSource() {
		BuildOutputImage();
		if(m_extractor.IsNull()) {
			return m_inputImgReader;
		}

		return (typename OutImageSource::Pointer)m_extractor;
	}

	void WriteToOutputFile() {
		if(!m_outputFileName.empty())
		{
			//Empty for now
		}
	}

	void BuildOutputImageAndUpdate() {
		BuildOutputImage();
		m_extractor->Update();
	}

private:
	void BuildOutputImage() {
		m_inputImgReader->UpdateOutputInformation();

		int width = m_inputImgReader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
		int height = m_inputImgReader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
		int fMinWidth = width;
		int fMinHeight = height;
		if(m_outForcedWidth > 0 && width >= m_outForcedWidth) {
			fMinWidth = m_outForcedWidth;
		}
		if(m_outForcedHeight > 0 && height >= m_outForcedHeight) {
			fMinHeight = m_outForcedHeight;
		}
		std::cout << "---------ROIImageFilter---------" << std::endl;
		std::cout << "width: " << width << " height: " << height << std::endl;
		std::cout << "fMinWidth: " << fMinWidth << " fMinHeight: " << fMinHeight << std::endl;
		std::cout << "m_outForcedWidth: " << m_outForcedWidth << " m_outForcedHeight: " << m_outForcedHeight << std::endl;

		m_extractor = ROIFilterType::New();
		m_extractor->SetInput( m_inputImgReader->GetOutput() );
		typename TInput2::SizeType size;
		size[0] = int((width - m_outForcedWidth) /2);
		size[1] = int((height - m_outForcedHeight) /2);
		m_extractor->SetBoundaryCropSize(size);
		std::cout << "CropSize: " << size[0] << " : " << size[1] << std::endl;
		m_extractor->UpdateOutputInformation();

		float widthNew = m_extractor->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
		float heightNew = m_extractor->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
		std::cout << "widthNew: " << widthNew << " heightNew: " << heightNew << std::endl;
		std::cout << "---------ROIImageFilter---------" << std::endl;
	}

	typename ImageSource::Pointer m_inputImgReader;
	typename ROIFilterType::Pointer m_extractor;
	std::string m_outputFileName;
	// during resampling at higher resolutions it might be needed to return with a specified dimension
	int m_outForcedWidth;
	int m_outForcedHeight;
};
} //namespace ts;
#endif // ROIIMAGEHANDLER_H
