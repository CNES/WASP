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

#ifndef PADDINGIMAGEHANDLER_H
#define PADDINGIMAGEHANDLER_H

#include "otbVectorImage.h"
#include "otbWrapperTypes.h"

//Transform
#include "otbImageFileWriter.h"
#include "itkZeroFluxNeumannPadImageFilter.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts
{

/**
 * @brief Pad values around an image using a ZeroFluxNeumannPadImageFilter
 */
template <typename TInput1, typename TInput2>
class PaddingImageHandler
{
public:

    typedef otb::ImageFileWriter<TInput2> WriterType;
    typedef itk::ImageSource<TInput1> ImageSource;
    typedef itk::ImageSource<TInput2> ResampledImageSource;
    typedef itk::ZeroFluxNeumannPadImageFilter<TInput2, TInput2> EdgePaddingResizer;

    typedef typename itk::ImageSource<TInput2> OutImageSource;

public:
    PaddingImageHandler() {
        m_outForcedWidth = -1;
        m_outForcedHeight = -1;
        // This will remain to 0 as we consider that the padding will be done only in the uper extend region
        m_lowerExtendRegion[0] = 0;
        m_lowerExtendRegion[1] = 0;
        m_upperExtendRegion[0] = 0;
        m_upperExtendRegion[1] = 0;
    }

    void SetOutputFileName(std::string &outFile) {
        m_outputFileName = outFile;
    }

    void SetInputImageReader(typename ImageSource::Pointer inputImgReader, typename ImageSource::Pointer resampledImgReader) {
        if (inputImgReader.IsNull() || resampledImgReader.IsNull())
        {
            std::cout << "No input Image set...; please set the input image!" << std::endl;
            itkExceptionMacro("No input Image set...; please set the input image");
        }
        m_resampledImgReader = resampledImgReader;

        inputImgReader->UpdateOutputInformation();
        m_resampledImgReader->UpdateOutputInformation();

        int inputRes = inputImgReader->GetOutput()->GetSpacing()[0];
        int resampledRes = m_resampledImgReader->GetOutput()->GetSpacing()[0];
        float scaleXY = ((float)resampledRes)/((float)inputRes);
        float sizeX = inputImgReader->GetOutput()->GetLargestPossibleRegion().GetSize()[0] / scaleXY;
        float sizeY = inputImgReader->GetOutput()->GetLargestPossibleRegion().GetSize()[1] / scaleXY;

        typename TInput1::SizeType size;
        size[0] = sizeX;
        size[1] = sizeY;

        typename TInput1::SizeType size2;
        size2[0] = vcl_ceil(sizeX);
        size2[1] = vcl_ceil(sizeY);

        if(size2[0] > size[0]) {
            m_upperExtendRegion[0] = (size2[0] - size[0]);
        }
        if(size2[1] > size[1]) {
            // we update only the index 1
            m_upperExtendRegion[1] = (size2[1] - size[1]);
        }
    }

    void SetInputImageReader(typename ImageSource::Pointer resampledImgReader,
                             int expectedWidth, int expectedHeight) {
        if (resampledImgReader.IsNull())
        {
            std::cout << "No input Image set...; please set the input image!" << std::endl;
            itkExceptionMacro("No input Image set...; please set the input image");
        }
        m_resampledImgReader = resampledImgReader;
        m_resampledImgReader->UpdateOutputInformation();
        int width = m_resampledImgReader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
        int height = m_resampledImgReader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
        if(expectedWidth > width) {
            m_upperExtendRegion[0] = (expectedWidth - width);
        }

        if(expectedHeight > height) {
            m_upperExtendRegion[1] = (expectedHeight - height);
        }
    }

    const char *GetNameOfClass() { return "PaddingImageHandler";}

    typename OutImageSource::Pointer GetOutputImageSource() {
    	BuildOutputImage();
        if(m_resizer.IsNull()) {
            return m_resampledImgReader;
        }

        return (typename OutImageSource::Pointer)m_resizer;
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
                typename TInput2::Pointer image = m_resampledImgReader->GetOutput();
                typename TInput2::SpacingType spacing = image->GetSpacing();
                typename TInput2::PointType origin = image->GetOrigin();
                std::cout << "===============PADDING==================" << std::endl;
                std::cout << "Origin : " << origin[0] << " " << origin[1] << std::endl;
                std::cout << "Spacing : " << spacing[0] << " " << spacing[1] << std::endl;
                std::cout << "Size : " << image->GetLargestPossibleRegion().GetSize()[0] << " " <<
                             image->GetLargestPossibleRegion().GetSize()[1] << std::endl;

                typename TInput2::SpacingType outspacing = m_resizer->GetOutput()->GetSpacing();
                typename TInput2::PointType outorigin = m_resizer->GetOutput()->GetOrigin();
                std::cout << "Output Origin : " << outorigin[0] << " " << outorigin[1] << std::endl;
                std::cout << "Output Spacing : " << outspacing[0] << " " << outspacing[1] << std::endl;
                std::cout << "Size : " << m_resizer->GetOutput()->GetLargestPossibleRegion().GetSize()[0] << " " <<
                             m_resizer->GetOutput()->GetLargestPossibleRegion().GetSize()[1] << std::endl;

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
        m_resizer->Update();
    }

private:
    void BuildOutputImage() {
        if((m_upperExtendRegion[0] != 0) || (m_upperExtendRegion[1] != 0)) {
            m_resizer = EdgePaddingResizer::New();
            m_resizer->SetInput(m_resampledImgReader->GetOutput());
            m_resizer->SetPadLowerBound(m_lowerExtendRegion);
            m_resizer->SetPadUpperBound(m_upperExtendRegion);
        }
    }

    typename ImageSource::Pointer m_resampledImgReader;

    typename EdgePaddingResizer::Pointer m_resizer;
    std::string m_outputFileName;
    // during resampling at higher resolutions it might be needed to return with a specified dimension
    long m_outForcedWidth;
    long m_outForcedHeight;

    typename TInput2::SizeType m_upperExtendRegion;
    typename TInput2::SizeType m_lowerExtendRegion;
};
} //namespace ts
#endif // PADDINGIMAGEHANDLER_H
