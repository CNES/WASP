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

#include "CreateS2AnglesRaster.h"
#include "MetadataHelperFactory.h"
#include "otbWrapperApplication.h"
using namespace ts;

CreateS2AnglesRaster::CreateS2AnglesRaster() {
	//empty
}

void CreateS2AnglesRaster::DoInit( int res, const std::string &xml) {
    m_inXml = xml;
    m_resolutionIndex = res;
}

CreateS2AnglesRaster::FloatVectorImageType::Pointer CreateS2AnglesRaster::DoExecute() {
    auto factory = MetadataHelperFactory::New();
    auto pHelper = factory->GetMetadataHelper(m_inXml);

    int nGridSize = pHelper->GetDetailedAnglesGridSize();
    if(nGridSize == 0) {
        itkExceptionMacro("Something is wrong. Grid size for angles is said to be 0!");
    }
    std::string fileName = pHelper->getResolutions().getNthResolutionFilenames(m_resolutionIndex)[0];
    std::cout << "S2AnglesRaster Filename found: \n" << fileName << std::endl;
    ShortImageReaderType::Pointer imageReader = ShortImageReaderType::New();
    imageReader->SetFileName(fileName);
    imageReader->UpdateOutputInformation();
    auto sz = imageReader->GetOutput()->GetLargestPossibleRegion().GetSize();
    auto spacing = imageReader->GetOutput()->GetSpacing();

    int width = sz[0];
    int height = sz[1];

    if(width == 0 || height == 0) {
        itkExceptionMacro("The read width/height from the resolution metadata file is/are 0");
    }

    m_AnglesRaster = FloatVectorImageType::New();
    FloatVectorImageType::IndexType start;
    start[0] =   0;  // first index on X
    start[1] =   0;  // first index on Y

    FloatVectorImageType::SizeType size;
    size[0]  = nGridSize;  // size along X
    size[1]  = nGridSize;  // size along Y

    FloatVectorImageType::RegionType region;

    region.SetSize(size);
    region.SetIndex(start);

    //Filter the viewingAngles according to their band existence:
    auto viewingAngles = pHelper->GetDetailedViewingAngles();
    std::vector<MetadataHelperViewingAnglesGrid> viewingAnglesFiltered;
    for(auto angle : viewingAngles){
    	if(pHelper->getResolutions().getResolutionVector()[m_resolutionIndex].doesBandTypeExist(angle.BandId)){
    		viewingAnglesFiltered.push_back(angle);
    	}
    }
    viewingAngles = std::move(viewingAnglesFiltered);

    auto solarAngles = pHelper->GetDetailedSolarAngles();
    int nBandsForRes = pHelper->getResolutions().getNthResolutionFilenames(m_resolutionIndex).size();
    std::cout << "Number of Bands for current Resolution: " << nBandsForRes << std::endl;
    if((viewingAngles.size() == 0) || solarAngles.Zenith.Values.size() == 0 || solarAngles.Azimuth.Values.size() == 0) {
        itkExceptionMacro("Mission does not have any detailed viewing or solar angles or viewing angles differ from the number of bands!");
    }

    if(solarAngles.Zenith.Values.size() != (unsigned int)nGridSize ||
        solarAngles.Azimuth.Values.size() != (unsigned int)nGridSize ) {
        itkExceptionMacro("The width and/or height of solar angles from the xml file is/are not as expected: "
                          << solarAngles.Azimuth.Values.size() << " or " <<
                          solarAngles.Azimuth.Values.size() << " instead " << nGridSize);
    }

    for (int band = 0; band < nBandsForRes; band++) {
        if(viewingAngles[band].Angles.Zenith.Values.size() != (unsigned int)nGridSize ||
            viewingAngles[band].Angles.Azimuth.Values.size() != (unsigned int)nGridSize )
            itkExceptionMacro("The width and/or height of computed angles from the xml file is/are not as expected: "
                              << viewingAngles[band].Angles.Zenith.Values.size() << " or " <<
                              viewingAngles[band].Angles.Azimuth.Values.size() << " instead " << nGridSize);
    }

    m_AnglesRaster->SetRegions(region);
    int nTotalAnglesNo = 2*(nBandsForRes+1);
    // we have 2 solar angles and 2 * N viewing angles
    m_AnglesRaster->SetNumberOfComponentsPerPixel(nTotalAnglesNo);
    FloatVectorImageType::SpacingType anglesRasterSpacing;
    anglesRasterSpacing[0] = (((float)width) * spacing[0]) / nGridSize; // spacing along X
    anglesRasterSpacing[1] = (((float)height) * spacing[1]) / nGridSize; // spacing along Y
    m_AnglesRaster->SetSpacing(anglesRasterSpacing);
    m_AnglesRaster->Allocate();

    for(unsigned int i = 0; i < (unsigned int)nGridSize; i++) {
        for(unsigned int j = 0; j < (unsigned int)nGridSize; j++) {
            itk::VariableLengthVector<float> vct(nTotalAnglesNo);
            vct[0] = solarAngles.Zenith.Values[i][j];
            vct[1] = solarAngles.Azimuth.Values[i][j];
            for (int band = 0; band < nBandsForRes; band++) {
                vct[band * 2 + 2] = viewingAngles[band].Angles.Zenith.Values[i][j];
                vct[band * 2 + 3] = viewingAngles[band].Angles.Azimuth.Values[i][j];
            }

            FloatVectorImageType::IndexType idx;
            idx[0] = j;
            idx[1] = i;
            m_AnglesRaster->SetPixel(idx, vct);
        }
    }
    m_AnglesRaster->UpdateOutputInformation();
    return m_ResampledBandsExtractor.getResamplerWantedSize(m_AnglesRaster, width, height, Interpolator_BCO)->GetOutput();
}
