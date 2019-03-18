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

#include "WeightAOTComputation.h"

using namespace ts;

WeightOnAOT::WeightOnAOT()
{
}

void WeightOnAOT::SetInputFileName(std::string &inputImageStr)
{
    if (inputImageStr.empty())
    {
        std::cout << "No input Image set...; please set the input image!" << std::endl;
        itkExceptionMacro("No input Image set...; please set the input image");
    }
    // Read the image
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputImageStr);
    try
    {
        //reader->Update();
        //m_image = reader->GetOutput();
        m_inputReader = reader;
    }
    catch (itk::ExceptionObject& err)
    {
        std::cout << "ExceptionObject caught !" << std::endl;
        std::cout << err << std::endl;
        itkExceptionMacro("Error reading input");
    }
}

void WeightOnAOT::SetInputImageReader(ImageSource::Pointer inputReader)
{
    if (inputReader.IsNull())
    {
        std::cout << "No input Image set...; please set the input image!" << std::endl;
        itkExceptionMacro("No input Image set...; please set the input image");
    }
    m_inputReader = inputReader;
}

void WeightOnAOT::SetOutputFileName(std::string &outFile)
{
    m_outputFileName = outFile;
}

void WeightOnAOT::Initialize(int nBand, float fQuantif, float fAotMax, float fMinWeight, float fMaxWeight) {
    m_nBand = nBand;
    m_fAotQuantificationVal = fQuantif;
    m_fAotMax = fAotMax;
    m_fMinWeightAot = fMinWeight;
    m_fMaxWeightAot = fMaxWeight;
}

WeightOnAOT::OutImageSource::Pointer WeightOnAOT::GetOutputImageSource()
{
    BuildOutputImageSource();
    return (OutImageSource::Pointer)m_filter;
}

int WeightOnAOT::GetInputImageResolution()
{
   m_inputReader->UpdateOutputInformation();
   ImageType::Pointer inputImage = m_inputReader->GetOutput();
   return inputImage->GetSpacing()[0];
}


void WeightOnAOT::BuildOutputImageSource()
{
    m_inputReader->UpdateOutputInformation();
    int nBands = m_inputReader->GetOutput()->GetNumberOfComponentsPerPixel();
    if(m_nBand >= nBands) {
        itkExceptionMacro("Invalid band number " << m_nBand << ". It should be less than " << nBands);
    }
    m_filter = FilterType::New();
    m_filter->GetFunctor().Initialize(m_nBand, m_fAotQuantificationVal,
                                      m_fAotMax, m_fMinWeightAot, m_fMaxWeightAot);
    m_filter->SetInput(m_inputReader->GetOutput());
}

void WeightOnAOT::WriteToOutputFile()
{
    if(!m_outputFileName.empty())
    {
        if(!m_outputFileName.empty())
        {
            WriterType::Pointer writer;
            writer = WriterType::New();
            writer->SetFileName(m_outputFileName);
            writer->SetInput(GetOutputImageSource()->GetOutput());
            try
            {
                writer->Update();
            }
            catch (itk::ExceptionObject& err)
            {
                std::cout << "ExceptionObject caught !" << std::endl;
                std::cout << err << std::endl;
                itkExceptionMacro("Error writing output");
            }
        }
    }
}


