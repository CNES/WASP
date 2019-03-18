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

#ifndef COMPOSITEPREPROCESSING_COMPUTENDVI_NEW_H_
#define COMPOSITEPREPROCESSING_COMPUTENDVI_NEW_H_



#include "otbImage.h"
#include "otbWrapperTypes.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkBinaryFunctorImageFilter.h"
#include "GlobalDefs.h"
#include "ImageResampler.h"
#include "BaseImageTypes.h"

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
 * @brief Functor to Compute the NDVI of two images
 */
template< class TInput, class TOutput>
class NDVIFunctor
{
public:
	NDVIFunctor() {}
	~NDVIFunctor() {}
	bool operator!=(const NDVIFunctor &) const {
		return false;
	}
	bool operator==(const NDVIFunctor & other) const {
		return !( *this != other );
	}

	inline TOutput operator()(const TInput & A, const TInput & B) const {
		const double redVal = static_cast< double >( A );
		const double nirVal = static_cast< double >( B );
		TOutput ret;
		if((fabs(redVal - NO_DATA_VALUE) < 0.000001) || (fabs(nirVal - NO_DATA_VALUE) < 0.000001)) {
			ret = 0;
		} else {
			if(fabs(redVal + nirVal) < 0.000001) {
				ret = 0;
			} else {
				ret = (nirVal - redVal)/(nirVal+redVal);
			}
		}
		return ret;
	}
};
} //namespace Functor

/**
 * @brief Class to compute the NDVI from two images
 * @note Used to compute the Directional correction in the pre-processing step.
 */
class ComputeNDVI : public BaseImageTypes {
public:
	typedef itk::BinaryFunctorImageFilter<ShortImageType,ShortImageType, FloatImageType,
			Functor::NDVIFunctor<ShortImageType::PixelType,FloatImageType::PixelType> > 		NDVIFilterType;

	/**
	 * @brief Constructor
	 */
	ComputeNDVI();

	/**
	 * @brief Init the NDVI calculation
	 * @param xml Metadata-filename
	 */
	void DoInit(const std::string &xml);

	/**
	 * @brief Execute the application
	 * @return FloatImage Containing the NDVI
	 */
	FloatImageType::Pointer DoExecute();

	/**
	 * @brief Return the name of the class
	 * @return
	 */
    const char * GetNameOfClass(){ return "ComputeNDVI";};

private:
	std::string              							            m_inXml;
	ShortImageReaderType::Pointer        						    m_InputImageReaderRed;
	ShortImageReaderType::Pointer							        m_InputImageReaderNIR;
	NDVIFilterType::Pointer 										m_NDVI;
	ImageResampler<ShortImageType, ShortImageType>					m_Resampler;
};

} // namespace ts



#endif /* COMPOSITEPREPROCESSING_COMPUTENDVI_NEW_H_ */
