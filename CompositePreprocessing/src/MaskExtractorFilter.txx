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

#include "MaskExtractorFilter.h"

namespace ts
{

template <class TImageType, class TOutputImageType>
MaskExtractorFilter<TImageType, TOutputImageType>::MaskExtractorFilter() {
	m_BitExtractor = BitExtractorType::New();
}

template <class TImageType, class TOutputImageType>
void MaskExtractorFilter<TImageType, TOutputImageType>::GenerateData() {
	m_BitExtractor->SetInput(this->GetInput());
	m_BitExtractor->GetOutput()->SetNumberOfComponentsPerPixel(1);
	m_BitExtractor->GraftOutput(this->GetOutput());
	m_BitExtractor->Update();

	this->GraftOutput(m_BitExtractor->GetOutput());
}

template <class TImageType, class TOutputImageType>
void MaskExtractorFilter<TImageType, TOutputImageType>::SetBitMask(const int &bit) {
	m_BitExtractor->GetFunctor().SetBitMask(bit);
}

} /* end namespace ts */
