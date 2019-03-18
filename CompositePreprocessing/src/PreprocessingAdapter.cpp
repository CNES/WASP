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

#include "PreprocessingAdapter.h"

using namespace ts::preprocessing;

PreprocessingAdapter::FloatImageType::Pointer PreprocessingAdapter::getFloatMask(const std::string &filename, const unsigned char &bit){
	ExtractorFilterType::Pointer extractor = ExtractorFilterType::New();
	extractor->SetBitMask(bit);
	ByteImageReaderType::Pointer reader = ByteImageReaderType::New();
	reader->SetFileName(filename);
	extractor->SetInput(reader->GetOutput());
	extractor->UpdateOutputInformation();
	FloatImageType::Pointer cloudImage = extractor->GetOutput();
	m_MaskList->PushBack(reader);
	m_ExtractorList->PushBack(extractor);
	return cloudImage;
}

void PreprocessingAdapter::setScatteringCoefficients(const std::vector<std::string> &scatteringcoeffs){
	m_scatteringCoeffs = scatteringcoeffs;
}
