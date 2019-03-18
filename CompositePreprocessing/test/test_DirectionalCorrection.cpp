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

#define BOOST_TEST_MODULE DirectionalCorrection
#include <boost/test/unit_test.hpp>
#include "DirectionalCorrection.h"
#include "GlobalDefs.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "otbWrapperApplication.h"
#include "ImageResampler.h"

using namespace ts;

#define WASP_TEST									"WASP_TEST"
#define TEST_NAME									"test_Preprocessing"

typedef float										FloatPixelType;
typedef otb::Image<FloatPixelType, 2>				InputImageType;
typedef otb::Wrapper::FloatVectorImageType			FloatVectorImageType;
typedef ts::DirectionalCorrection					DirectionalCorrectionType;
typedef otb::ImageFileReader<FloatVectorImageType>	FloatVectorImageReaderType;
typedef otb::ImageFileReader<InputImageType>		InputImageReaderType;
typedef short										ShortPixelType;
typedef otb::Wrapper::Int16VectorImageType			ShortVectorImageType;
typedef otb::ImageFileReader<ShortVectorImageType>	ShortVectorImageReaderType;

BOOST_AUTO_TEST_CASE(testDirectionalCorrectionR2){
	size_t resolution = 1;
	std::string wasp_test = getEnvVar(WASP_TEST);
	if(wasp_test.empty()){
		std::cout << "Cannot find WASP_TEST environment variable. Exiting..." << std::endl;
		exit(1);
	}
	std::string xml = wasp_test + "/"
			+ TEST_NAME + "/"
			"INPUTS/"
			"SENTINEL2A_20180315-144453-175_L2A_T19LGH_D_V1-6/"
			"SENTINEL2A_20180315-144453-175_L2A_T19LGH_D_V1-6_MTD_ALL.xml";
	std::cout << "XML : "<< xml << std::endl;
	ImageResampler<InputImageType, InputImageType> resampler;

	DirectionalCorrectionType dirCorr;
	std::string scatteringcoeffs = std::string(wasp_test + "/" + TEST_NAME + "/INPUTS/" + "scattering_coeffs_20m.txt");
	InputImageReaderType::Pointer cldReader = InputImageReaderType::New();
	cldReader->SetFileName(wasp_test + "/" + TEST_NAME + "/" + "INPUTS/" + "0_cld10.tif");
	cldReader->UpdateOutputInformation();
	InputImageType::Pointer cldImg = resampler.getResampler(cldReader->GetOutput(), 0.5f)->GetOutput();
	InputImageReaderType::Pointer watReader = InputImageReaderType::New();
	watReader->SetFileName(wasp_test + "/" + TEST_NAME + "/" + "INPUTS/" + "0_wat10.tif");
	watReader->UpdateOutputInformation();
	InputImageType::Pointer watImg = resampler.getResampler(watReader->GetOutput(), 0.5f)->GetOutput();
	InputImageReaderType::Pointer snwReader = InputImageReaderType::New();
	snwReader->SetFileName(wasp_test + "/" + TEST_NAME + "/" + "INPUTS/" + "0_snw10.tif");
	snwReader->UpdateOutputInformation();
	InputImageType::Pointer snwImg = resampler.getResampler(snwReader->GetOutput(), 0.5f)->GetOutput();
	FloatVectorImageReaderType::Pointer anglesReader = FloatVectorImageReaderType::New();
	anglesReader->SetFileName(wasp_test + "/" + TEST_NAME + "/INPUTS/" + "s2angles_raster_r2.tif");
	anglesReader->UpdateOutputInformation();
	FloatVectorImageType::Pointer anglesImg = anglesReader->GetOutput();
	InputImageReaderType::Pointer ndviReader = InputImageReaderType::New();
	ndviReader->SetFileName(wasp_test + "/" + TEST_NAME + "/INPUTS/" + "ndvi.tif");
	ndviReader->UpdateOutputInformation();
	InputImageType::Pointer ndviImg = resampler.getResampler(ndviReader->GetOutput(), 0.5f)->GetOutput();
	dirCorr.Init(resolution, xml, scatteringcoeffs, cldImg, watImg, snwImg, anglesImg, ndviImg);
	dirCorr.DoExecute();
	ShortVectorImageType::Pointer output = dirCorr.GetCorrectedImg();
	ShortVectorImageReaderType::Pointer gReader = ShortVectorImageReaderType::New();
	gReader->SetFileName(wasp_test + "/" + TEST_NAME + "/INPUTS/" + "/0_CP_R2.tif");
	ShortVectorImageType::Pointer reference = gReader->GetOutput();
	reference->Update();
	output->Update();
	itk::ImageRegionIterator<ShortVectorImageType> imageIteratorRef(reference,reference->GetLargestPossibleRegion());
	itk::ImageRegionIterator<ShortVectorImageType> imageIteratorNew(output,output->GetLargestPossibleRegion());

	BOOST_CHECK_EQUAL(reference->GetLargestPossibleRegion(), output->GetLargestPossibleRegion());
	while(!imageIteratorRef.IsAtEnd())
	{
		itk::VariableLengthVector<short> referenceBands = imageIteratorRef.Get();
		itk::VariableLengthVector<short> newBands = imageIteratorNew.Get();
		BOOST_CHECK_EQUAL(referenceBands.GetSize(), newBands.GetSize());
		for(size_t i = 0; i < referenceBands.GetSize(); i++){
			if(!std::isnan(referenceBands[i]) && !std::isnan(newBands[i])){
				BOOST_CHECK_EQUAL(referenceBands[i], newBands[i]);
			}
		}
		++imageIteratorRef;
		++imageIteratorNew;
	}
};

#ifdef R1
BOOST_AUTO_TEST_CASE(testDirectionalCorrectionR1){
	size_t resolution = 0;
	std::string wasp_test = getEnvVar(WASP_TEST);
	if(wasp_test.empty()){
		std::cout << "Cannot find WASP_TEST environment variable. Exiting..." << std::endl;
		exit(1);
	}
	std::string xml = wasp_test + "/"
			+ TEST_NAME + "/"
			"INPUTS/"
			"SENTINEL2A_20180315-144453-175_L2A_T19LGH_D_V1-6/"
			"SENTINEL2A_20180315-144453-175_L2A_T19LGH_D_V1-6_MTD_ALL.xml";
	std::cout << "XML : "<< xml << std::endl;

	DirectionalCorrectionType dirCorr;
	std::string scatteringcoeffs = std::string(wasp_test + TEST_NAME + "/INPUTS/" + "scattering_coeffs_10m.txt");
	InputImageReaderType::Pointer cldReader = InputImageReaderType::New();
	cldReader->SetFileName(wasp_test + "/" + TEST_NAME + "/" + "INPUTS/" + "0_cld10.tif");
	cldReader->UpdateOutputInformation();
	InputImageType::Pointer cldImg = cldReader->GetOutput();
	InputImageReaderType::Pointer watReader = InputImageReaderType::New();
	watReader->SetFileName(wasp_test + "/" + TEST_NAME + "/" + "INPUTS/" + "0_wat10.tif");
	watReader->UpdateOutputInformation();
	InputImageType::Pointer watImg = watReader->GetOutput();
	InputImageReaderType::Pointer snwReader = InputImageReaderType::New();
	snwReader->SetFileName(wasp_test + "/" + TEST_NAME + "/" + "INPUTS/" + "0_snw10.tif");
	snwReader->UpdateOutputInformation();
	InputImageType::Pointer snwImg = snwReader->GetOutput();
	FloatVectorImageReaderType::Pointer anglesReader = FloatVectorImageReaderType::New();
	anglesReader->SetFileName(wasp_test + "/" + TEST_NAME + "/" + "s2angles_raster_r1.tif");
	anglesReader->UpdateOutputInformation();
	FloatVectorImageType::Pointer anglesImg = anglesReader->GetOutput();
	InputImageReaderType::Pointer ndviReader = InputImageReaderType::New();
	ndviReader->SetFileName(wasp_test + "/" + TEST_NAME + "/" + "ndvi.tif");
	ndviReader->UpdateOutputInformation();
	InputImageType::Pointer ndviImg = ndviReader->GetOutput();
	dirCorr.Init(resolution, xml, scatteringcoeffs, cldImg, watImg, snwImg, anglesImg, ndviImg);
	dirCorr.DoExecute();
	ShortVectorImageType::Pointer output = dirCorr.GetCorrectedImg();
	ShortVectorImageReaderType::Pointer gReader = ShortVectorImageReaderType::New();
	gReader->SetFileName(wasp_test + "/" + TEST_NAME + "/0_CP_R1.tif");
	ShortVectorImageType::Pointer reference = gReader->GetOutput();
	reference->Update();
	output->Update();
	itk::ImageRegionIterator<ShortVectorImageType> imageIteratorRef(reference,reference->GetLargestPossibleRegion());
	itk::ImageRegionIterator<ShortVectorImageType> imageIteratorNew(output,output->GetLargestPossibleRegion());

	BOOST_CHECK_EQUAL(reference->GetLargestPossibleRegion(), output->GetLargestPossibleRegion());
	while(!imageIteratorRef.IsAtEnd())
	{
		itk::VariableLengthVector<short> referenceBands = imageIteratorRef.Get();
		itk::VariableLengthVector<short> newBands = imageIteratorNew.Get();
		BOOST_CHECK_EQUAL(referenceBands.GetSize(), newBands.GetSize());
		for(size_t i = 0; i < referenceBands.GetSize(); i++){
			if(!std::isnan(referenceBands[i]) && !std::isnan(newBands[i])){
				BOOST_CHECK_EQUAL(referenceBands[i], newBands[i]);
			}
		}
		++imageIteratorRef;
		++imageIteratorNew;
	}
};
#endif
