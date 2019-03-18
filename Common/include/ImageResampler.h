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

#ifndef IMAGE_RESAMPLER_H
#define IMAGE_RESAMPLER_H

#include "otbWrapperTypes.h"
#include "otbStreamingResampleImageFilter.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbImageListToVectorImageFilter.h"

#include "libgen.h"


//Transform
#include "itkScalableAffineTransform.h"
#include "GlobalDefs.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Resample an image to either a given size or a given scale
 * @note Can use the following interpolators: BCO, NearestNeighbor and Linear
 */
template <class TInput, class TOutput>
class ImageResampler
{
public:

	typedef otb::StreamingResampleImageFilter<TInput, TOutput, double>                           ResampleFilterType;
	typedef otb::ObjectList<ResampleFilterType>                                                 ResampleFilterListType;

	typedef itk::NearestNeighborInterpolateImageFunction<TOutput, double>             NearestNeighborInterpolationType;
	typedef itk::LinearInterpolateImageFunction<TOutput, double>                      LinearInterpolationType;
	typedef otb::BCOInterpolateImageFunction<TOutput>                                 BCOInterpolationType;
	typedef itk::IdentityTransform<double, TOutput::ImageDimension>                   IdentityTransformType;

	typedef itk::ScalableAffineTransform<double, TOutput::ImageDimension>             ScalableTransformType;
	typedef typename ScalableTransformType::OutputVectorType     OutputVectorType;

	typedef typename NearestNeighborInterpolationType::Pointer NearestNeighborInterpolationTypePtr;
	typedef typename LinearInterpolationType::Pointer LinearInterpolationTypePtr;
	typedef typename IdentityTransformType::Pointer IdentityTransformTypePtr;
	typedef typename TInput::Pointer ResamplerInputImgPtr;
	typedef typename TInput::PixelType ResamplerInputImgPixelType;
	typedef typename TInput::SpacingType ResamplerInputImgSpacingType;
	typedef typename ResampleFilterType::Pointer ResamplerPtr;
	typedef typename ResampleFilterType::SizeType ResamplerSizeType;
	typedef typename otb::ObjectList<ResampleFilterType>::Pointer   ResampleFilterListTypePtr;

public:
	const char * GetNameOfClass() { return "ImageResampler"; }

	ImageResampler()
	{
		m_ResamplersList = ResampleFilterListType::New();
		m_BCORadius = 2;
		m_fBCOAlpha = -0.5;
	}

	void SetBicubicInterpolatorParameters(int BCORadius, float BCOAlpha = -0.5) {
		m_BCORadius = BCORadius;
		m_fBCOAlpha = BCOAlpha;
	}

	/**
	 * @brief Get Resampler by a wanted size (X,Y)
	 * @param image image pointer
	 * @param wantedWidth Width in pixels
	 * @param wantedHeight Height in pixels
	 * @param interpolator Interpolator type
	 * @return Pointer to the resampler-filter
	 */
	ResamplerPtr getResamplerWantedSize(const ResamplerInputImgPtr& image, const int wantedWidth,
			const int wantedHeight, Interpolator_Type interpolator=Interpolator_Linear)
	{
		auto sz = image->GetLargestPossibleRegion().GetSize();
		OutputVectorType scale;
		scale[0] = (float)sz[0] / wantedWidth;
		scale[1] = (float)sz[1] / wantedHeight;
		ResamplerPtr resampler = getResampler(image, scale, wantedWidth, wantedHeight, interpolator);
		ResamplerSizeType recomputedSize;
		recomputedSize[0] = wantedWidth;
		recomputedSize[1] = wantedHeight;
		resampler->SetOutputSize(recomputedSize);
		return resampler;
	}

	/**
	 * @brief Get Resampler by a Given Ratio
	 * @param image image pointer
	 * @param ratio ratio of the new image
	 * @param interpolator Interpolator type
	 * @return Pointer to the resampler-filter
	 */
	ResamplerPtr getResampler(const ResamplerInputImgPtr& image, const float& ratio,
			Interpolator_Type interpolator=Interpolator_Linear) {
		// Scale Transform
		OutputVectorType scale;
		scale[0] = 1.0 / ratio;
		scale[1] = 1.0 / ratio;
		return getResampler(image, scale, -1, -1, interpolator);
	}

	/**
	 * @brief Get Resampler by a Given Ratio and origin
	 * @param image image pointer
	 * @param ratio ratio of the new image
	 * @brief origin The origin of the output image
	 * @param interpolator Interpolator type
	 * @return Pointer to the resampler-filter
	 */
	ResamplerPtr getResampler(const ResamplerInputImgPtr& image, const float& ratio,
			typename TOutput::PointType origin, Interpolator_Type interpolator=Interpolator_Linear) {
		// Scale Transform
		OutputVectorType scale;
		scale[0] = 1.0 / ratio;
		scale[1] = 1.0 / ratio;
		return getResampler(image, scale, -1, -1, origin, interpolator);
	}

	/**
	 * @brief Get resampler by scale and enforce a given size
	 * @param image Image pointer
	 * @param scale Scale of the output
	 * @param forcedWidth Forced width of the output
	 * @param forcedHeight Forced height of the output
	 * @param interpolatorType Interpolator type
	 * @return
	 */
	ResamplerPtr getResampler(const ResamplerInputImgPtr& image, const OutputVectorType& scale,
			int forcedWidth, int forcedHeight, Interpolator_Type interpolatorType=Interpolator_Linear) {
		ResamplerInputImgSpacingType spacing = image->GetSpacing();
		typename TOutput::PointType origin = image->GetOrigin();
		typename TOutput::PointType outputOrigin;
		outputOrigin[0] = std::round(origin[0] + 0.5 * spacing[0] * (scale[0] - 1.0));
		outputOrigin[1] = std::round(origin[1] + 0.5 * spacing[1] * (scale[1] - 1.0));

		return getResampler(image, scale, forcedWidth, forcedHeight, outputOrigin, interpolatorType);
	}

	/**
	 * @brief Get a resampler using the scale, forced size and origin
	 * @param image Image Pointer
	 * @param scale Scale of the output
	 * @param forcedWidth Forced width of output
	 * @param forcedHeight Forced height of output
	 * @param origin Origin of the output
	 * @param interpolatorType Interpolator type
	 * @return
	 */
	ResamplerPtr getResampler(const ResamplerInputImgPtr& image, const OutputVectorType& scale,
			int forcedWidth, int forcedHeight, typename TOutput::PointType origin,
			Interpolator_Type interpolatorType=Interpolator_Linear) {
		ResamplerPtr resampler = ResampleFilterType::New();
		resampler->SetInput(image);

		// Set the interpolator
		switch ( interpolatorType )
		{
		case Interpolator_Linear:
		{
			typename LinearInterpolationType::Pointer interpolatorPtr = LinearInterpolationType::New();
			resampler->SetInterpolator(interpolatorPtr);
		}
		break;
		case Interpolator_NNeighbor:
		{
			typename NearestNeighborInterpolationType::Pointer interpolatorPtr = NearestNeighborInterpolationType::New();
			resampler->SetInterpolator(interpolatorPtr);
		}
		break;
		case Interpolator_BCO:
		{
			typename BCOInterpolationType::Pointer interpolatorPtr = BCOInterpolationType::New();
			interpolatorPtr->SetRadius(m_BCORadius);
			interpolatorPtr->SetAlpha(m_fBCOAlpha);
			resampler->SetInterpolator(interpolatorPtr);
		}
		break;
		}

		IdentityTransformTypePtr transform = IdentityTransformType::New();
		resampler->SetOutputParametersFromImage( image );

		// Evaluate spacing
		ResamplerInputImgSpacingType spacing = image->GetSpacing();
		ResamplerInputImgSpacingType OutputSpacing;
		OutputSpacing[0] = std::round(spacing[0] * scale[0]);
		OutputSpacing[1] = std::round(spacing[1] * scale[1]);

		resampler->SetOutputSpacing(OutputSpacing);

		resampler->SetOutputOrigin(origin);
		resampler->SetTransform(transform);
		// Evaluate size
		ResamplerSizeType recomputedSize;
		if((forcedWidth != -1) && (forcedHeight != -1))
		{
			recomputedSize[0] = forcedWidth;
			recomputedSize[1] = forcedHeight;
		} else {
			recomputedSize[0] = image->GetLargestPossibleRegion().GetSize()[0] / scale[0];
			recomputedSize[1] = image->GetLargestPossibleRegion().GetSize()[1] / scale[1];
		}

		resampler->SetOutputSize(recomputedSize);

		ResamplerInputImgPixelType defaultValue;
		itk::NumericTraits<ResamplerInputImgPixelType>::SetLength(defaultValue, image->GetNumberOfComponentsPerPixel());
		if(interpolatorType != Interpolator_NNeighbor) {
			defaultValue = NO_DATA_VALUE;
		}
		resampler->SetEdgePaddingValue(defaultValue);

		m_ResamplersList->PushBack(resampler);
		return resampler;
	}



private:
	ResampleFilterListTypePtr             m_ResamplersList;
	int     m_BCORadius;
	float   m_fBCOAlpha;
};
}  // namespace ts
#endif // IMAGE_RESAMPLER_H

