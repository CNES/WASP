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

#include "DirectionalCorrection.h"
#include <vector>

using namespace ts;

DirectionalCorrection::DirectionalCorrection() {
}

void DirectionalCorrection::Init(const size_t &res, const std::string &xml, const std::string &scatcoef,
                                 FloatImageType::Pointer &cldImg, FloatImageType::Pointer &watImg,
                                 FloatImageType::Pointer &snowImg, FloatVectorImageType::Pointer &angles,
                                 FloatImageType::Pointer &ndvi) {
    m_nRes = res;
    m_strXml = xml;
    m_strScatCoeffs = scatcoef;

    m_AnglesImg = angles;
    m_NdviImg = ndvi;
    m_CSM = cldImg;
    m_WM = watImg;
    m_SM = snowImg;

    m_ImageList = ImageListType::New();
    m_Concat = ListConcatenerFilterType::New();
    m_ReaderList = FloatVectorImageReaderListType::New();
}

void DirectionalCorrection::DoExecute() {
    auto factory = ts::MetadataHelperFactory::New();
    auto pHelper = factory->GetMetadataHelper(m_strXml);
    ts::productReturnType inputImageFiles = pHelper->getResolutions().getNthResolutionFilenames(m_nRes);
    std::cout << "Directional Correction Filenames found: " << std::endl;
    for(auto filename : inputImageFiles){
    	FloatVectorImageReaderType::Pointer reader = FloatVectorImageReaderType::New();
        reader->SetFileName(filename);
        std::cout << filename << std::endl;
        reader->UpdateOutputInformation();
        m_ReaderList->PushBack(reader);
        FloatVectorImageType::Pointer inputImg = reader->GetOutput();
        extractBandsFromImage(inputImg);
    }
    // extract the cloud, water and snow masks from the masks file
    m_ImageList->PushBack(m_CSM);
    m_ImageList->PushBack(m_SM);
    m_ImageList->PushBack(m_WM);
    m_ImageList->PushBack(m_NdviImg);
    extractBandsFromImage(m_AnglesImg);

    m_Concat->SetInput(m_ImageList);

    std::vector<Functor::ScatteringFunctionCoefficients> scatteringCoeffs;
    scatteringCoeffs = loadScatteringFunctionCoeffs(m_strScatCoeffs);
    unsigned int nBandsForRes = inputImageFiles.size();
    if(nBandsForRes != scatteringCoeffs.size()) {
        itkExceptionMacro("Scattering coefficients file contains only " << scatteringCoeffs.size()
                          << " but are expected coefficients for " << nBandsForRes << " bands!");
    }

    m_Functor.Initialize(scatteringCoeffs);
    m_DirectionalCorrectionFunctor = FunctorFilterType::New();
    m_DirectionalCorrectionFunctor->SetFunctor(m_Functor);
    m_DirectionalCorrectionFunctor->SetInput(m_Concat->GetOutput());
    m_DirectionalCorrectionFunctor->UpdateOutputInformation();
    m_DirectionalCorrectionFunctor->GetOutput()->SetNumberOfComponentsPerPixel(scatteringCoeffs.size());
}

int DirectionalCorrection::extractBandsFromImage(FloatVectorImageType::Pointer & imageType) {
    imageType->UpdateOutputInformation();
    int nbBands = imageType->GetNumberOfComponentsPerPixel();
    for(int j=0; j < nbBands; j++)
    {
        // extract the band without resampling
        m_ImageList->PushBack(m_ResampledBandsExtractor.ExtractImgResampledBand(imageType, j+1, ts::Interpolator_Linear));
    }
    return nbBands;
}

std::vector<Functor::ScatteringFunctionCoefficients> DirectionalCorrection::loadScatteringFunctionCoeffs(std::string &strFileName) {
    std::vector<Functor::ScatteringFunctionCoefficients> scatteringCoeffs;

    std::ifstream coeffsFile;
    coeffsFile.open(strFileName);
    if (!coeffsFile.is_open()) {
        itkExceptionMacro("Need to provide scattering_coeffs file for current resolution!");
    }

    std::string line;
    int curLine = 0;
    while (std::getline(coeffsFile, line))
    {
        trim(line);
        if ((line.size() > 0) && (line[0] != '#') && (line[0] != '\r'))
        {
        	Functor::ScatteringFunctionCoefficients coeffs;
            std::istringstream iss(line);
            float num; // The number in the line

            //while the iss is a number
            int i = 0;
            while ((iss >> num))
            {
                //look at the number
                switch(i) {
                case 0:
                    coeffs.V0 = num;
                    break;
                case 1:
                    coeffs.V1 = num;
                    break;
                case 2:
                    coeffs.R0 = num;
                    break;
                case 3:
                    coeffs.R1 = num;
                    break;
                default:
                    break;
                }
                i++;
            }
            if(i >= 4) {
                scatteringCoeffs.push_back(coeffs);
            } else {
                itkExceptionMacro("Invalid values line found at position " << curLine);
            }
        }
        curLine++;
    }

    return scatteringCoeffs;
}

std::string DirectionalCorrection::trim(std::string const& str) {
    if(str.empty())
        return str;

    std::size_t firstScan = str.find_first_not_of(' ');
    std::size_t first     = firstScan == std::string::npos ? str.length() : firstScan;
    std::size_t last      = str.find_last_not_of(' ');
    return str.substr(first, last-first+1);
}


DirectionalCorrection::ShortVectorImageType::Pointer DirectionalCorrection::GetCorrectedImg() {
    return m_DirectionalCorrectionFunctor->GetOutput();
}
