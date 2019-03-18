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

#ifndef MASKEXTRACTORFUNCTOR_H
#define MASKEXTRACTORFUNCTOR_H

#include "GlobalDefs.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Namespace around Functors to be used in the filters defined below
 */
namespace Functor
{

/**
 * @brief Functor to extract a bit from an image
 */
template< class TInput, class TOutput>
class MaskExtractorFunctor {
public:
	MaskExtractorFunctor() {
		m_bit = -1;
	}

	~MaskExtractorFunctor() {

	}

	MaskExtractorFunctor& operator =(const MaskExtractorFunctor& copy)
	{
		return *this;
	}

	bool operator!=( const MaskExtractorFunctor & other) const {
		return true;
	}

	bool operator==( const MaskExtractorFunctor & other ) const {
		return false;
	}

	/**
	 * @brief Operator to run on every pixel sequentially
	 * @param A The pixel value
	 * @return The new pixel value for the output image
	 */
	TOutput operator()( const TInput & A) {
		TOutput var(NO_DATA_VALUE);
		if(m_bit > -1 && m_bit < 8) //Min and Max borders - if not return NODATA
			var = static_cast<TOutput>((A & m_MasksArray[m_bit]) >> m_bit);
		return var;
	}

	/**
	 * @brief Set bit to be extracted
	 * @param bit Value between 0 to 7
	 */
	void SetBitMask(const int &bit) {
		m_bit = bit;
	}

private:
	int m_MasksArray[8] = {0x01, 0x02, 0x04, 0x08, 0x16, 0x32, 0x64, 0x128};
	int m_bit;
};

} //namespace Functor
} //namespace ts

#endif // MASKEXTRACTORFUNCTOR_H
