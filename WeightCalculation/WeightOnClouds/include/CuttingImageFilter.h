/*
 * Copyright (C) 2015-2016, CS Romania <office@c-s.ro>
 * Copyright (C) 2018-2019, Centre National d'Etudes Spatiales (CNES)
 * All rights reversed
 *
 * This file is part of:
 * - Sen2agri-Processors (initial work)
 * - Weighted Average Synthesis Processor (WASP)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
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

#ifndef CUTTINGIMAGEHANDLER_H
#define CUTTINGIMAGEHANDLER_H

#include "otbVectorImage.h"
#include "otbWrapperTypes.h"

//Transform
#include "otbImageFileWriter.h"
#include "otbExtractROI.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts
{

/**
 * @brief Cut an image to a given size
 */
template <typename TInput1, typename TInput2>
class CuttingImageHandler
{
public:

	typedef otb::ImageFileWriter<TInput2> WriterType;
	typedef itk::ImageSource<TInput1> ImageSource;
	typedef itk::ImageSource<TInput2> ResampledImageSource;
	typedef otb::ExtractROI<typename TInput1::InternalPixelType,
			typename TInput2::PixelType>     ExtractROIFilterType;

	typedef typename itk::ImageSource<TInput2> OutImageSource;

public:
	CuttingImageHandler() {
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
			typename WriterType::Pointer writer;
			writer = WriterType::New();
			writer->SetFileName(m_outputFileName);
			writer->SetInput(GetOutputImageSource()->GetOutput());
			try
			{
				writer->Update();
				typename TInput2::Pointer image = m_inputImgReader->GetOutput();
				typename TInput2::SpacingType spacing = image->GetSpacing();
				typename TInput2::PointType origin = image->GetOrigin();
				std::cout << "==============CUT IMAGE===================" << std::endl;
				std::cout << "Origin : " << origin[0] << " " << origin[1] << std::endl;
				std::cout << "Spacing : " << spacing[0] << " " << spacing[1] << std::endl;
				std::cout << "Size : " << image->GetLargestPossibleRegion().GetSize()[0] << " " <<
						image->GetLargestPossibleRegion().GetSize()[1] << std::endl;

				typename TInput2::SpacingType outspacing = m_extractor->GetOutput()->GetSpacing();
				typename TInput2::PointType outorigin = m_extractor->GetOutput()->GetOrigin();
				std::cout << "Output Origin : " << outorigin[0] << " " << outorigin[1] << std::endl;
				std::cout << "Output Spacing : " << outspacing[0] << " " << outspacing[1] << std::endl;
				std::cout << "Size : " << m_extractor->GetOutput()->GetLargestPossibleRegion().GetSize()[0] << " " <<
						m_extractor->GetOutput()->GetLargestPossibleRegion().GetSize()[1] << std::endl;

				std::cout  << "=================================" << std::endl;
				std::cout << std::endl;
			}
			catch (itk::ExceptionObject& err)
			{
				std::cout << "ExceptionObject caught !" << std::endl;
				std::cout << err << std::endl;
				itkExceptionMacro("Error writing output");
			}
		}
	}

	void BuildOutputImageAndUpdate() {
		BuildOutputImage();
		m_extractor->Update();
	}

private:
	void BuildOutputImage() {
		m_inputImgReader->UpdateOutputInformation();

		float width = m_inputImgReader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
		float height = m_inputImgReader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
		float fMinWidth = width;
		float fMinHeight = height;
		if(m_outForcedWidth > 0 && width >= m_outForcedWidth) {
			fMinWidth = m_outForcedWidth;
		}
		if(m_outForcedHeight > 0 && height >= m_outForcedHeight) {
			fMinHeight = m_outForcedHeight;
		}

		m_extractor = ExtractROIFilterType::New();
		m_extractor->SetInput( m_inputImgReader->GetOutput() );
		m_extractor->SetSizeX(fMinWidth);
		m_extractor->SetSizeY(fMinHeight);
		m_extractor->UpdateOutputInformation();
	}

	typename ImageSource::Pointer m_inputImgReader;

	typename ExtractROIFilterType::Pointer m_extractor;
	std::string m_outputFileName;
	// during resampling at higher resolutions it might be needed to return with a specified dimension
			long m_outForcedWidth;
	long m_outForcedHeight;
};
} //namespace ts;
#endif // CUTTINGIMAGEHANDLER_H
