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

#ifndef DIRECTIONAL_CORRECTION_H
#define DIRECTIONAL_CORRECTION_H

#include "otbWrapperTypes.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkCastImageFilter.h"
#include "otbVectorImageToImageListFilter.h"
#include "otbImageListToVectorImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkIndent.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkBinaryFunctorImageFilter.h"
#include "itkVariableLengthVector.h"
#include "otbMultiToMonoChannelExtractROI.h"
#include "DirectionalCorrectionFunctor.h"
#include "MetadataHelperFactory.h"
#include "ResamplingBandExtractor.h"
#include "BaseImageTypes.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Perform the directional correction on a set of images of the same resolution
 */
class DirectionalCorrection : public BaseImageTypes {
public:
	/*    typedef float                                   PixelType;
    typedef short                                   PixelShortType;
    typedef otb::Wrapper::FloatVectorImageType                    InputImageType1;
    typedef otb::Image<PixelType, 2>                              FloatImageType;
    typedef otb::Wrapper::FloatImageType                          InternalBandImageType;
    typedef otb::Wrapper::Int16VectorImageType                    OutImageType;*/

	typedef otb::ImageList<FloatImageType>												ImageListType;
	typedef otb::ImageListToVectorImageFilter<ImageListType, FloatVectorImageType>		ListConcatenerFilterType;

	typedef Functor::DirectionalCorrectionFunctor <FloatVectorImageType::PixelType,
			ShortVectorImageType::PixelType>                							DirectionalCorrectionFunctorType;
	typedef itk::UnaryFunctorImageFilter< FloatVectorImageType,
			ShortVectorImageType,
			DirectionalCorrectionFunctorType >      									FunctorFilterType;

	//typedef otb::ImageFileReader<FloatVectorImageType>									ReaderType;
	//typedef otb::ObjectList<FloatVectorImageReaderType>									FloatVectorImageReaderListType;

public:
	/**
	 * @brief Constructor
	 */
	DirectionalCorrection();

	/**
	 * @brief Init the Directional Correction
	 * @param res Current resolution
	 * @param xml Metadata file
	 * @param scatcoef Scattering coefficient filename
	 * @param cldImg Cloud Image
	 * @param watImg Water Image
	 * @param snowImg Snow Image
	 * @param angles Angles Image
	 * @param ndvi NDVI Image
	 */
	void Init(const size_t &res, const std::string &xml, const std::string &scatcoef, FloatImageType::Pointer &cldImg,
			FloatImageType::Pointer &watImg, FloatImageType::Pointer &snowImg,
			FloatVectorImageType::Pointer &angles, FloatImageType::Pointer &ndvi);

	/**
	 * @brief Execute the application
	 */
	void DoExecute();

	/**
	 * @brief Return the corrected image
	 * @return ShortVectorImage containing the corrected S2-rasters
	 */
	ShortVectorImageType::Pointer GetCorrectedImg();

	/**
	 * @brief Return the name of the class
	 * @return
	 */
	const char * GetNameOfClass() { return "DirectionalCorrection"; }

private:
	/**
	 * @brief Extract each band from the angles image to the image list
	 * @param imageType FloatVectorImage of the angles image
	 * @return The number of bands extracted
	 */
	int extractBandsFromImage(FloatVectorImageType::Pointer & imageType);

	/**
	 * @brief Load the scattering coefficients file
	 * @param strFileName Filename to the scattering-coefficients
	 * @return Vector containing the coeffs
	 */
	std::vector<Functor::ScatteringFunctionCoefficients> loadScatteringFunctionCoeffs(std::string &strFileName);

	/**
	 * @brief Trim a string with whitespaces
	 * @param str The input string
	 * @return The string between the whitespaces
	 */
	std::string trim(std::string const& str);

private:
	size_t                                  	m_nRes;
	std::string                            		m_strXml;
	std::string                            		m_strScatCoeffs;

	FloatVectorImageType::Pointer               m_L2AIn;
	FloatVectorImageType::Pointer               m_AnglesImg;
	FloatImageType::Pointer                		m_NdviImg, m_CSM, m_WM, m_SM;
	ImageListType::Pointer                  	m_ImageList;
	ListConcatenerFilterType::Pointer       	m_Concat;
	FunctorFilterType::Pointer              	m_DirectionalCorrectionFunctor;
	DirectionalCorrectionFunctorType        	m_Functor;

	FloatVectorImageReaderType::Pointer         m_inputImageReader;
	FloatVectorImageReaderListType::Pointer		m_ReaderList;
	ts::ResamplingBandExtractor<float>          m_ResampledBandsExtractor;
};
} //namespace ts
#endif // DIRECTIONAL_CORRECTION_H
