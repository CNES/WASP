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

#ifndef GAUSSIANFILTER_H
#define GAUSSIANFILTER_H

#include "otbWrapperTypes.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts
{

/**
 * @brief Perform a gaussian-blur on the image
 */
template <typename TInput, typename TOutput>
class GaussianFilter
{
public:
	typedef otb::Wrapper::FloatImageType ImageType;
	typedef otb::ImageFileWriter<TOutput> WriterType;

	typedef itk::RescaleIntensityImageFilter<
			ImageType, ImageType> RescaleFilterType;

	typedef itk::DiscreteGaussianImageFilter<
			ImageType, ImageType>  DiscreteGaussianFilterType;

	typedef itk::ImageSource<TInput> ImageSource;
	typedef itk::ImageSource<ImageType> OutImageSource;

public:
	GaussianFilter() {
		m_nKernelWidth = 81;
	}

	void SetOutputFileName(std::string &outFile) {
		m_outputFileName = outFile;
	}

	void SetInputImageReader(typename ImageSource::Pointer inputReader) {
		if (inputReader.IsNull())
		{
			std::cout << "No input Image set...; please set the input image!" << std::endl;
			itkExceptionMacro("No input Image set...; please set the input image");
		}
		m_inputReader = inputReader;
	}

	void SetSigma(float fSigma) {
		m_fSigma = fSigma;
	}

	void SetKernelWidth(int nKernelWidth) {
		m_nKernelWidth = nKernelWidth;
	}

	const char* GetNameOfClass() { return "GaussianFilter"; }
	OutImageSource::Pointer GetOutputImageSource() {
		BuildOutputImageSource();
		return (OutImageSource::Pointer)m_gaussianFilter;

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
				ImageType::Pointer inputImage = m_inputReader->GetOutput();
				ImageType::SpacingType spacing = inputImage->GetSpacing();
				ImageType::PointType origin = inputImage->GetOrigin();
				std::cout << "===============GAUSSIAN==================" << std::endl;
				std::cout << "Origin : " << origin[0] << " " << origin[1] << std::endl;
				std::cout << "Spacing : " << spacing[0] << " " << spacing[1] << std::endl;
				std::cout << "Size : " << inputImage->GetLargestPossibleRegion().GetSize()[0] << " " <<
						inputImage->GetLargestPossibleRegion().GetSize()[1] << std::endl;

				ImageType::SpacingType outspacing = m_gaussianFilter->GetOutput()->GetSpacing();
				ImageType::PointType outorigin = m_gaussianFilter->GetOutput()->GetOrigin();
				std::cout << "Output Origin : " << outorigin[0] << " " << outorigin[1] << std::endl;
				std::cout << "Output Spacing : " << outspacing[0] << " " << outspacing[1] << std::endl;
				std::cout << "Size : " << m_gaussianFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0] << " " <<
						m_gaussianFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1] << std::endl;

				std::cout << "Sigma : " << m_fSigma << std::endl;
				std::cout << "Kernel Width : " << m_nKernelWidth << std::endl;

				std::cout  << "=================================" << std::endl;
				std::cout << std::endl;

				std::stringstream ss;
				m_gaussianFilter.Print(ss);
				std::cout << ss.str() << std::endl;

			}
			catch (itk::ExceptionObject& err)
			{
				std::cout << "ExceptionObject caught !" << std::endl;
				std::cout << err << std::endl;
				itkExceptionMacro("Error writing output");
			}
		}
	}

	void BuildOutputImageSourceAndUpdate() {
		BuildOutputImageSource();
		m_gaussianFilter->Update();
	}
private:
	void BuildOutputImageSource() {
		m_gaussianFilter = DiscreteGaussianFilterType::New();
		m_gaussianFilter->SetInput(m_inputReader->GetOutput());
		// the variance is sigma^2
		m_gaussianFilter->SetVariance(m_fSigma*m_fSigma);
		m_gaussianFilter->SetUseImageSpacing(false);
		//m_gaussianFilter->SetMaximumError(0.00001);
		m_gaussianFilter->SetMaximumKernelWidth(m_nKernelWidth);
	}

	float m_fSigma;
	int m_nKernelWidth;
	std::string m_outputFileName;
	RescaleFilterType::Pointer m_rescaler;
	DiscreteGaussianFilterType::Pointer m_gaussianFilter;
	typename ImageSource::Pointer m_inputReader;
};
} //namespace ts
#endif // GAUSSIANFILTER_H
