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

#include "ComputeNDVI.h"
#include "MetadataHelperFactory.h"

using namespace ts;

ComputeNDVI::ComputeNDVI() {
}

void ComputeNDVI::DoInit(const std::string &xml) {
	m_inXml = xml;
}

ComputeNDVI::FloatImageType::Pointer ComputeNDVI::DoExecute() {
	auto factory = MetadataHelperFactory::New();
	auto pHelper = factory->GetMetadataHelper(m_inXml);
	//Read all input parameters
	m_InputImageReaderRed = ShortImageReaderType::New();
	m_InputImageReaderNIR = ShortImageReaderType::New();
	std::string imgFileNameRed = pHelper->getFileNameByString(pHelper->GetImageFileNames(), "B4");
	std::string imgFileNameNIR = pHelper->getFileNameByString(pHelper->GetImageFileNames(), "B8");//Approximation used. Normally B8A should be used here.

	std::cout << "ComputeNDVI Filenames found: \n" << imgFileNameRed << "\n" << imgFileNameNIR << std::endl;

	m_InputImageReaderRed->SetFileName(imgFileNameRed);
	m_InputImageReaderRed->UpdateOutputInformation();
	ShortImageType::Pointer imgRed = m_InputImageReaderRed->GetOutput();
	int curResRed = imgRed->GetSpacing()[0];
	m_InputImageReaderNIR->SetFileName(imgFileNameNIR);
	m_InputImageReaderNIR->UpdateOutputInformation();
	ShortImageType::Pointer imgNIR = m_InputImageReaderNIR->GetOutput();
	int curResNIR = imgNIR->GetSpacing()[0];
	m_NDVI = NDVIFilterType::New();
	if(curResRed != curResNIR){
		//Rescale image in this case
		ShortImageType::Pointer imgNIR_Resized = m_Resampler.getResampler(imgNIR.GetPointer(), double(curResNIR) / curResRed)->GetOutput();
		m_NDVI->SetInput2(imgNIR_Resized);
	}else{
		m_NDVI->SetInput2(imgNIR);
	}
	m_NDVI->SetInput1(imgRed);
	m_NDVI->UpdateOutputInformation();
	return m_NDVI->GetOutput();
}
