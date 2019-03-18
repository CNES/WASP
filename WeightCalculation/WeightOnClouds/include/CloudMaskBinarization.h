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

#ifndef CLOUDMASKBINARIZATION_H
#define CLOUDMASKBINARIZATION_H

#include "otbWrapperTypes.h"
#include "itkUnaryFunctorImageFilter.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkImageSource.h"
#include "GlobalDefs.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts
{
/**
 * @brief Namespace around Functors to be used in the filters defined below
 */
namespace Functor
{

/**
 * @brief Binarize a Mask-image using a threshold
 */
template< class TInput, class TOutput>
class BinarizeCloudMask
{
public:
    BinarizeCloudMask() { m_fThreshold = 0.0; }
    ~BinarizeCloudMask() {}
    void SetThreshold(float fThreshold) { m_fThreshold = fThreshold; }
  bool operator!=( const BinarizeCloudMask & ) const
    {
    return false;
    }
  bool operator==( const BinarizeCloudMask & other ) const
    {
    return !(*this != other);
    }
  inline TOutput operator()( const TInput & A ) const
    {
      float val = static_cast< float >( A );
      int nRoundedVal = std::round(val);

      return ((nRoundedVal == NO_DATA_VALUE) ? NO_DATA_VALUE : ((val > (m_fThreshold + EPSILON)) ? 1.0 : 0.0));
    }
private:
    float m_fThreshold;
};
} //namespace functor

template <typename TInput, typename TOutput>
class CloudMaskBinarization
{
public:
    typedef itk::UnaryFunctorImageFilter<TInput,TOutput,
                    Functor::BinarizeCloudMask<
                        typename TInput::PixelType,
                        typename TOutput::PixelType> > FilterType;


    typedef itk::ImageSource<TInput> ImageSource;
    typedef typename itk::ImageSource<TOutput> OutImageSource;

    typedef otb::ImageFileReader<TInput> ReaderType;
    typedef otb::ImageFileWriter<TOutput> WriterType;

public:
    CloudMaskBinarization() {
        m_fThreshold = 0.0;
    }

    void SetInputFileName(std::string &inputImageStr) {
        if (inputImageStr.empty())
        {
            std::cout << "No input Image set...; please set the input image!" << std::endl;
            itkExceptionMacro("No input Image set...; please set the input image");
        }
        // Read the image
        typename ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(inputImageStr);
        try
        {
            m_inputReader = reader;
        }
        catch (itk::ExceptionObject& err)
        {
            std::cout << "ExceptionObject caught !" << std::endl;
            std::cout << err << std::endl;
            itkExceptionMacro("Error reading input");
        }
    }

    void SetInputImageReader(typename ImageSource::Pointer reader) {
        if (reader.IsNull())
        {
            std::cout << "No input Image set...; please set the input image!" << std::endl;
            itkExceptionMacro("No input Image set...; please set the input image");
        }
        m_inputReader = reader;
    }

    void SetOutputFileName(std::string &outFile) {
        m_outputFileName = outFile;
    }

    const char *GetNameOfClass() { return "CloudMaskBinarization";}

    typename OutImageSource::Pointer GetOutputImageSource() {
        BuildOutputImageSource();
        return (typename OutImageSource::Pointer)m_filter;
    }

    int GetInputImageResolution() {
        m_inputReader->UpdateOutputInformation();
        typename TInput::Pointer inputImage = m_inputReader->GetOutput();
        return inputImage->GetSpacing()[0];
    }

    void SetThreshold(float fThreshold) {
        m_fThreshold = fThreshold;
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
                typename TInput::Pointer image = m_inputReader->GetOutput();
                typename TInput::SpacingType spacing = image->GetSpacing();
                typename TInput::PointType origin = image->GetOrigin();
                std::cout << "============= CLOUD BINARIZATION ====================" << std::endl;
                std::cout << "Input Origin : " << origin[0] << " " << origin[1] << std::endl;
                std::cout << "Input Spacing : " << spacing[0] << " " << spacing[1] << std::endl;
                std::cout << "Size : " << image->GetLargestPossibleRegion().GetSize()[0] << " " <<
                             image->GetLargestPossibleRegion().GetSize()[1] << std::endl;

                typename TInput::SpacingType outspacing = m_filter->GetOutput()->GetSpacing();
                typename TInput::PointType outorigin = m_filter->GetOutput()->GetOrigin();
                std::cout << "Output Origin : " << outorigin[0] << " " << outorigin[1] << std::endl;
                std::cout << "Output Spacing : " << outspacing[0] << " " << outspacing[1] << std::endl;
                std::cout << "Size : " << m_filter->GetOutput()->GetLargestPossibleRegion().GetSize()[0] << " " <<
                             m_filter->GetOutput()->GetLargestPossibleRegion().GetSize()[1] << std::endl;

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

    void BuildOutputImageSourceAndUpdate() {
    	BuildOutputImageSource();
        m_filter->Update();
    }

private:
    void BuildOutputImageSource() {
        m_filter = FilterType::New();
        m_filter->GetFunctor().SetThreshold(m_fThreshold);
        m_filter->SetInput(m_inputReader->GetOutput());
    }

    typename ImageSource::Pointer m_inputReader;
    std::string m_outputFileName;
    typename FilterType::Pointer m_filter;
    float m_fThreshold;
};
} //namespace ts
#endif // CLOUDMASKBINARIZATION_H
