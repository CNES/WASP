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

#ifndef DirectionalCorrectionFilter_H
#define DirectionalCorrectionFilter_H

#include "itkBinaryFunctorImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "otbImageListToVectorImageFilter.h"
#include "otbWrapperTypes.h"
#include "DirectionalCorrectionFunctor.h"
#include "libgen.h"
#include "otbMultiToMonoChannelExtractROI.h"

#include "itkNumericTraits.h"
#include "otbImage.h"
#include "BaseImageTypes.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Filter to perform the directional correction
 */
template <class TImageType, class TOutputImageType>
class ITK_EXPORT DirectionalCorrectionFilter : public itk::ImageToImageFilter<TImageType, TOutputImageType>, public BaseImageTypes{
public:
	typedef DirectionalCorrectionFilter                     	  Self;
	typedef itk::ImageToImageFilter<TImageType, TOutputImageType> Superclass;
	typedef itk::SmartPointer<Self>                        		  Pointer;
	typedef itk::SmartPointer<const Self>                   	  ConstPointer;


	typedef typename TImageType::PixelType			PixelType;
	typedef typename TOutputImageType::PixelType	OutputPixelType;

	/*typedef otb::Wrapper::FloatImageType                                         InternalBandImageType;
  typedef otb::ImageList<InternalBandImageType>                                ImageListType;*/

	typedef otb::ImageListToVectorImageFilter<ImageListType, TImageType>					ListConcatenerFilterType;

	typedef otb::MultiToMonoChannelExtractROI<typename TImageType::InternalPixelType,
			InternalBandImageType::PixelType>												ExtractROIFilterType;
	typedef otb::ObjectList<ExtractROIFilterType>             				                ExtractROIFilterListType;


	typedef DirectionalCorrectionFunctor<PixelType, OutputPixelType> 						DirectionalCorrectionFunctorType;
	typedef itk::UnaryFunctorImageFilter<TImageType, TOutputImageType,
			DirectionalCorrectionFunctorType>												FunctorFilterType;

	/** Method for creation through object factory */
	itkNewMacro(Self);

	/** Run-time type information */
	itkTypeMacro(DirectionalCorrectionFilter, itk::ImageToImageFilter);

	/**
	 * @brief Set Input raster
	 * @param inputImg The original raster from S2
	 */
	void SetInputImage(TImageType inputImg)
	{
		this->SetInput(0, inputImg);
	}

	/**
	 * @brief Set Cloud image
	 * @param inputImg The cloud image
	 */
	void SetCloudImage(TImageType inputImg)
	{
		this->SetInput(1, inputImg);
	}

	/**
	 * @brief Set Water image
	 * @param inputImg The water image
	 */
	void SetWaterImage(TImageType inputImg)
	{
		this->SetInput(2, inputImg);
	}

	/**
	 * @brief Set Snow image
	 * @param inputImg The snow image
	 */
	void SetSnowImage(TImageType inputImg)
	{
		this->SetInput(3, inputImg);
	}

	/**
	 * @brief Set NDVI image
	 * @param inputImg The ndvi image
	 */
	void SetNdviImage(TImageType inputImg)
	{
		this->SetInput(4, inputImg);
	}

	/**
	 * @brief Set Angles image
	 * @param inputImg the angles image
	 */
	void SetAnglesImage(TImageType inputImg)
	{
		this->SetInput(5, inputImg);
	}

	/**
	 * @brief Set scattering coefficients
	 * @param scatCoeffs The vector containing all scattering coeffs for the current resolution
	 */
	void SetScatteringCoefficients(std::vector<ScaterringFunctionCoefficients> &scatCoeffs) {
		m_scatteringCoeffs = scatCoeffs;
	}

	/**
	 * @brief Set Reflectance quantification value
	 * @param fQuantifVal
	 */
	void SetReflQuantifValue(float fQuantifVal) {
		m_fQuantifVal = fQuantifVal;
	}

	/**
	 * @brief Update the output info of the generated image, setting the number of bands
	 */
	virtual void UpdateOutputInformation() ITK_OVERRIDE
			{
		Superclass::UpdateOutputInformation();
		this->GetOutput()->SetNumberOfComponentsPerPixel(m_scatteringCoeffs.size());
			}

	/**
	 * @brief Constructor
	 */
	DirectionalCorrectionFilter()
	{
		m_fQuantifVal = 0;
		m_ImageList = ImageListType::New();
		m_Concat = ListConcatenerFilterType::New();
		m_ExtractorList = ExtractROIFilterListType::New();

		this->SetNumberOfRequiredInputs(6);
		m_directionalCorrectionFunctor = FunctorFilterType::New();
	}

	/**
	 * @brief Creates connections for the output image
	 */
	void GenerateData() ITK_OVERRIDE
			{
		extractBandsFromImage(this->GetInput(0));
		extractBandsFromImage(this->GetInput(1));
		extractBandsFromImage(this->GetInput(2));
		extractBandsFromImage(this->GetInput(3));
		extractBandsFromImage(this->GetInput(4));
		extractBandsFromImage(this->GetInput(5));

		m_Concat->SetInput(m_ImageList);


		DirectionalCorrectionFunctorType functor;
		functor.Initialize(m_scatteringCoeffs, m_fQuantifVal);
		m_directionalCorrectionFunctor->SetFunctor(functor);

		m_directionalCorrectionFunctor->SetInput(m_Concat->GetOutput());
		m_directionalCorrectionFunctor->UpdateOutputInformation();
		m_directionalCorrectionFunctor->GetOutput()->SetNumberOfComponentsPerPixel(m_scatteringCoeffs.size());
		m_directionalCorrectionFunctor->GraftOutput(this->GetOutput());
		m_directionalCorrectionFunctor->Update();
		this->GraftOutput(m_directionalCorrectionFunctor->GetOutput());
			}

private:

	/**
	 * @brief Extract each band from the input vector image
	 * @param imageType The input image containing all needed images
	 * @return The number of bands extracted
	 */
	int extractBandsFromImage(const TImageType *imageType) {
		int nbBands = imageType->GetNumberOfComponentsPerPixel();
		for(int j=0; j < nbBands; j++)
		{
			typename ExtractROIFilterType::Pointer extractor = ExtractROIFilterType::New();
			extractor->SetInput( imageType );
			extractor->SetChannel( j+1 );
			extractor->UpdateOutputInformation();
			m_ExtractorList->PushBack( extractor );
			m_ImageList->PushBack( extractor->GetOutput() );
		}
		return nbBands;
	}


	DirectionalCorrectionFilter(Self &);   // intentionally not implemented
	void operator =(const Self&);          // intentionally not implemented

	typename FunctorFilterType::Pointer 		 m_directionalCorrectionFunctor;
	std::vector<ScaterringFunctionCoefficients>  m_scatteringCoeffs;
	float										 m_fQuantifVal;

	FloatImageListType::Pointer              	 m_ImageList;
	typename ListConcatenerFilterType::Pointer   m_Concat;
	typename ExtractROIFilterListType::Pointer   m_ExtractorList;

};
} //namespace ts

#endif

