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

#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"

#include "otbVectorImage.h"
#include "itkUnaryFunctorImageFilter.h"
#include "otbVectorImageToImageListFilter.h"
#include "otbImageListToVectorImageFilter.h"

#include "MetadataHelperFactory.h"
#include "ResamplingBandExtractor.h"
#include "UpdateSynthesisFunctor.h"
#include "BandsDefs.h"
#include "string_utils.hpp"

/**
 * @brief otb Namespace for all OTB-related Filters and Applications
 */
namespace otb
{
/**
 * @brief Wrapper namespace for all OTB-Applications
 */
namespace Wrapper
{

using namespace ts;

/**
 * @brief Update synthesis using the recurrent expression of the weighted average.
 */
class UpdateSynthesis : public Application
{
public:
	typedef UpdateSynthesis Self;
	typedef Application Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;

	itkNewMacro(Self)

	itkTypeMacro(UpdateSynthesis, otb::Application)

	typedef FloatVectorImageType                    InputVectorImageType;
	typedef FloatImageType                          InternalBandImageType;
	typedef Int16VectorImageType                    OutputVectorImageType;
	typedef ImageFileReader<FloatVectorImageType>	ReaderType;
	typedef ObjectList<ReaderType>					ReaderListType;

	typedef otb::ImageList<InternalBandImageType>					ImageListType;
	typedef otb::ImageList<FloatVectorImageType>					VectorImageListType;

	typedef ImageListToVectorImageFilter<ImageListType, InputVectorImageType >    ConcatenatorFilterType;
	typedef ObjectList<ConcatenatorFilterType>										ConcatenatorListType;

	typedef ts::Functor::UpdateSynthesisFunctor <InputVectorImageType::PixelType, OutputVectorImageType::PixelType> UpdateSynthesisFunctorType;
	typedef itk::UnaryFunctorImageFilter< InputVectorImageType, OutputVectorImageType, UpdateSynthesisFunctorType >      UpdateSynthesisFilterType;
	typedef ObjectList<UpdateSynthesisFilterType>		UpdateSynthesisListType;

private:

	void DoInit()
	{
		SetName("UpdateSynthesis");
		SetDescription("Update synthesis using the recurrent expression of the weighted average.");

		SetDocName("UpdateSynthesis");
		SetDocLongDescription("Update synthesis using the recurrent expression of the weighted average.");
		SetDocLimitations("None");
		SetDocAuthors("Peter KETTIG");
		SetDocSeeAlso(" ");
		AddDocTag(Tags::Vector);

		AddParameter(ParameterType_InputImage, "inr1", "L2A input product R1");
		AddParameter(ParameterType_InputImage, "inr2", "L2A input product R2");
		MandatoryOff("inr2");
		AddParameter(ParameterType_InputFilename, "xml", "Input L2A XML");
		AddParameter(ParameterType_InputImage, "cld", "Cloud-Shadow Mask");
		AddParameter(ParameterType_InputImage, "wat", "Water Mask");
		AddParameter(ParameterType_InputImage, "snw", "Snow Mask");
		AddParameter(ParameterType_InputImage, "weightl2a", "Weights of L2A product for current date");

		AddParameter(ParameterType_InputImage, "prevproductr1", "Previous l3a product R1");
		MandatoryOff("prevproductr1");
		AddParameter(ParameterType_InputImage, "prevproductr2", "Previous l3a product R2");
		MandatoryOff("prevproductr2");

		AddParameter(ParameterType_InputImage, "prevl3weightsr1", "Previous l3a product weights R1");
		MandatoryOff("prevl3weightsr1");
		AddParameter(ParameterType_InputImage, "prevl3datesr1", "Previous l3a product dates R1");
		MandatoryOff("prevl3datesr1");
		AddParameter(ParameterType_InputImage, "prevl3reflr1", "Previous l3a product reflectances R1");
		MandatoryOff("prevl3reflr1");
		AddParameter(ParameterType_InputImage, "prevl3flagsr1", "Previous l3a product flags R1");
		MandatoryOff("prevl3flagsr1");
		AddParameter(ParameterType_InputImage, "prevl3weightsr2", "Previous l3a product weights R2");
		MandatoryOff("prevl3weightsr2");
		AddParameter(ParameterType_InputImage, "prevl3datesr2", "Previous l3a product dates R2");
		MandatoryOff("prevl3datesr2");
		AddParameter(ParameterType_InputImage, "prevl3reflr2", "Previous l3a product reflectances R2");
		MandatoryOff("prevl3reflr2");
		AddParameter(ParameterType_InputImage, "prevl3flagsr2", "Previous l3a product flags R2");
		MandatoryOff("prevl3flagsr2");

		AddParameter(ParameterType_OutputImage, "outr1", "Out image containing all updated synthesis rasters in R1");
		MandatoryOff("outr1");
		AddParameter(ParameterType_OutputImage, "outr2", "Out image containing all updated synthesis rasters in R2");
		MandatoryOff("outr2");

		m_ConcatenatorList = ConcatenatorListType::New();
		m_UpdateSynthesisList = UpdateSynthesisListType::New();
		m_ReaderList = ReaderListType::New();
	}

	void DoUpdateParameters()
	{
		// Nothing to do.
	}

	void DoExecute()
	{
		std::string inXml = GetParameterAsString("xml");
		m_CloudMask = GetParameterFloatVectorImage("cld");
		m_WaterMask = GetParameterFloatVectorImage("wat");
		m_SnowMask = GetParameterFloatVectorImage("snw");
		m_WeightsL2A = GetParameterFloatVectorImage("weightl2a");

		m_CloudMask->UpdateOutputInformation();
		m_WaterMask->UpdateOutputInformation();
		m_SnowMask->UpdateOutputInformation();
		m_WeightsL2A->UpdateOutputInformation();

		auto factory = MetadataHelperFactory::New();
		auto pHelper = factory->GetMetadataHelper(inXml);
		std::string missionName = pHelper->GetMissionName();
		size_t nTotalRes = pHelper->getResolutions().getNumberOfResolutions();

		int productDate = pHelper->GetAcquisitionDateAsDoy();
		std::cout << "Product DOY: " << productDate << std::endl;

		/**
		 * LOOP HERE:
		 */
		for(size_t resolution = 0; resolution < nTotalRes; resolution++){
			VectorImageListType::Pointer imageListPerResolution = VectorImageListType::New();
			ResamplingBandExtractor<float> ResampledBandsExtractor;
			ImageListType::Pointer rasterList = ImageListType::New();


			InputVectorImageType::Pointer L2AImage = GetParameterFloatVectorImage(getParameterName("in", resolution));
			L2AImage->UpdateOutputInformation();
			imageListPerResolution->PushBack(L2AImage);
			imageListPerResolution->PushBack(m_CloudMask);
			imageListPerResolution->PushBack(m_WaterMask);
			imageListPerResolution->PushBack(m_SnowMask);
			imageListPerResolution->PushBack(m_WeightsL2A);

			auto szL2A = L2AImage->GetLargestPossibleRegion().GetSize();
			int nL2AWidth = szL2A[0];
			int nL2AHeight = szL2A[1];

			int nDesiredWidth = nL2AWidth;
			int nDesiredHeight = nL2AHeight;
			auto spacingL2A = L2AImage->GetSpacing();

			int nExtractedBandsNo = 0;

			/**
			 * Build reflectance image
			 */

			std::vector<int> bandsPresenceVector;
			for(size_t i = 0; i < L2AImage->GetNumberOfComponentsPerPixel(); i++){
				bandsPresenceVector.emplace_back((int)i);
				nExtractedBandsNo++;
			}

			int nBandsL2A = ResampledBandsExtractor.ExtractAllResampledBands(L2AImage, rasterList, Interpolator_NNeighbor, spacingL2A[0], spacingL2A[0], nDesiredWidth, nDesiredHeight);

			int nRelBlueBandIdx = S2_L2A_10M_BLUE_BAND_IDX;
			bool bHasAppendedPrevL2ABlueBand = false;

			/**
			 * R2 special case, where blue band needs to be extracted and resampled
			 */
			if(spacingL2A[0] > 10){
				ReaderType::Pointer reader = ReaderType::New();
				bHasAppendedPrevL2ABlueBand = true;
				std::string strL2ABlueBandImg = pHelper->getFileNameByString(pHelper->GetImageFileNames(), std::string(S2_L2A_10M_BLUE_BAND_NAME));
				reader->SetFileName(strL2ABlueBandImg);
				m_ReaderList->PushBack(reader);
				reader->UpdateOutputInformation();
				imageListPerResolution->PushBack(reader->GetOutput());

				ResampledBandsExtractor.ExtractAllResampledBands(reader->GetOutput(), rasterList, Interpolator_NNeighbor, 10, spacingL2A[0], nDesiredWidth, nDesiredHeight);
				nRelBlueBandIdx = nExtractedBandsNo++;
			}

			nBandsL2A += ResampledBandsExtractor.ExtractAllResampledBands(m_CloudMask, rasterList, Interpolator_NNeighbor, 10, spacingL2A[0], nDesiredWidth, nDesiredHeight);
			nBandsL2A += ResampledBandsExtractor.ExtractAllResampledBands(m_WaterMask, rasterList, Interpolator_NNeighbor, 10, spacingL2A[0], nDesiredWidth, nDesiredHeight);
			nBandsL2A += ResampledBandsExtractor.ExtractAllResampledBands(m_SnowMask, rasterList, Interpolator_NNeighbor, 10, spacingL2A[0], nDesiredWidth, nDesiredHeight);
			ResampledBandsExtractor.ExtractAllResampledBands(m_WeightsL2A, rasterList, Interpolator_Linear, 10, spacingL2A[0], nDesiredWidth, nDesiredHeight);

			m_VectorObjectList.push_back(imageListPerResolution);

			int nL3AWidth = -1;
			int nL3AHeight = -1;
			bool l3aExist = false;
			//int nNbL3ABands = 0;


			if(HasValue(getParameterName("prevproduct", resolution))) {
				/**
				 * Previous L3 Product found - Case 1 - One file from an ongoing execution:
				 */
				l3aExist = true;
				InputVectorImageType::Pointer prevL3A = GetParameterFloatVectorImage(getParameterName("prevproduct", resolution));
				prevL3A->UpdateOutputInformation();
				size_t nBandsL3A = prevL3A->GetNumberOfComponentsPerPixel();
				if(size_t(nBandsL2A) != prevL3A->GetNumberOfComponentsPerPixel()){
					itkExceptionMacro("ERROR: Number of L2A and L3A bands are not equal:"
							+ std::to_string(nBandsL2A) + " " + std::to_string(nBandsL3A));
				}
				auto szL3A = prevL3A->GetLargestPossibleRegion().GetSize();
				nL3AWidth = szL3A[0];
				nL3AHeight = szL3A[1];
				auto spacingPrevL3A = L2AImage->GetSpacing();

				if((nL3AWidth != nL2AWidth) || (nL3AHeight != nL2AHeight)) {
					otbMsgDevMacro("WARNING: L3A and L2A product sizes differ: " << "L2A: " << nL2AWidth << " " << nL2AHeight << ", "
							<< "L3A: " << nL3AWidth << " " << nL3AHeight << std::endl;)
				}
				InternalBandImageType::Pointer weights = ResampledBandsExtractor.ExtractImgResampledBand(prevL3A, 1, Interpolator_Linear, spacingPrevL3A[0], spacingL2A[0], nDesiredWidth, nDesiredHeight);
				rasterList->PushBack(weights);
				InternalBandImageType::Pointer dates = ResampledBandsExtractor.ExtractImgResampledBand(prevL3A, 2, Interpolator_Linear, spacingPrevL3A[0], spacingL2A[0], nDesiredWidth, nDesiredHeight);
				rasterList->PushBack(dates);
				//Starting at #4, cause the three previous ones are the masks:
				for(size_t i = 4; i < nBandsL3A+1; i ++){
					InternalBandImageType::Pointer refl = ResampledBandsExtractor.ExtractImgResampledBand(prevL3A, i, Interpolator_Linear, spacingPrevL3A[0], spacingL2A[0], nDesiredWidth, nDesiredHeight);
					rasterList->PushBack(refl);
				}
				//Adding the Flags later, because of the internal order of the Functor
				InternalBandImageType::Pointer flags = ResampledBandsExtractor.ExtractImgResampledBand(prevL3A, 3, Interpolator_NNeighbor, spacingPrevL3A[0], spacingL2A[0], nDesiredWidth, nDesiredHeight);
				rasterList->PushBack(flags);
			}else if(HasValue(getParameterName("prevl3weights", resolution)) && HasValue(getParameterName("prevl3dates", resolution)) &&
					HasValue(getParameterName("prevl3refl", resolution)) && HasValue(getParameterName("prevl3flags", resolution)) &&
					HasValue(getParameterName("prevproduct", resolution)) == false) {
				/**
				 * Previous L3 Product found - Case 2 - Single files from a previously finished product:
				 */
				l3aExist = true;
				InputVectorImageType::Pointer prevL3AWeight = GetParameterFloatVectorImage(getParameterName("prevl3weights", resolution));
				InputVectorImageType::Pointer prevL3AAvgDate = GetParameterFloatVectorImage(getParameterName("prevl3dates", resolution));
				InputVectorImageType::Pointer prevL3ARefl = GetParameterFloatVectorImage(getParameterName("prevl3refl", resolution));
				InputVectorImageType::Pointer prevL3AFlags = GetParameterFloatVectorImage(getParameterName("prevl3flags", resolution));
				prevL3AFlags->UpdateOutputInformation();
				auto szL3A = prevL3AFlags->GetLargestPossibleRegion().GetSize();
				nL3AWidth = szL3A[0];
				nL3AHeight = szL3A[1];
				auto spacingPrevL3A = L2AImage->GetSpacing();

				if((nL3AWidth != nL2AWidth) || (nL3AHeight != nL2AHeight)) {
					otbMsgDevMacro("WARNING: L3A and L2A product sizes differ: " << "L2A: " << nL2AWidth << " " << nL2AHeight << ", "
							<< "L3A: " << nL3AWidth << " " << nL3AHeight << std::endl;)
				}
				int nL3Weights = ResampledBandsExtractor.ExtractAllResampledBands(prevL3AWeight, rasterList, Interpolator_Linear, spacingPrevL3A[0], spacingL2A[0], nDesiredWidth, nDesiredHeight);
				int nL3Dates= ResampledBandsExtractor.ExtractAllResampledBands(prevL3AAvgDate, rasterList, Interpolator_Linear, spacingPrevL3A[0], spacingL2A[0], nDesiredWidth, nDesiredHeight);
				int l3bReflBandsNo = ResampledBandsExtractor.ExtractAllResampledBands(prevL3ARefl, rasterList, Interpolator_Linear, spacingPrevL3A[0], spacingL2A[0], nDesiredWidth, nDesiredHeight);
				int nL3Flags = ResampledBandsExtractor.ExtractAllResampledBands(prevL3AFlags, rasterList, Interpolator_NNeighbor, spacingPrevL3A[0], spacingL2A[0], nDesiredWidth, nDesiredHeight);

				if(nL3Flags > 1 || nL3Weights > 1 || nL3Dates > 1){
					otbMsgDevMacro("WARNING: Level3 mask bands contain more than one channel - Only the first one of each will be used");
				}
				if(l3bReflBandsNo != nBandsL2A){
					otbMsgDevMacro("WARNING: Level3 image bands unequal to the Level2 image band");
				}

				imageListPerResolution->PushBack(prevL3AWeight);
				imageListPerResolution->PushBack(prevL3AAvgDate);
				imageListPerResolution->PushBack(prevL3ARefl);
				imageListPerResolution->PushBack(prevL3AFlags);

			}
			m_ResamplerExtractorList.push_back(ResampledBandsExtractor);
			ConcatenatorFilterType::Pointer concatenator = ConcatenatorFilterType::New();
			concatenator->SetInput(rasterList);
			m_ObjectList.push_back(rasterList.GetPointer());
			m_ConcatenatorList->PushBack(concatenator);

			UpdateSynthesisFunctorType updateSynthesisFunctor;
			updateSynthesisFunctor.Initialize(bandsPresenceVector, nExtractedBandsNo, nRelBlueBandIdx, bHasAppendedPrevL2ABlueBand, l3aExist,
					productDate, pHelper->GetReflectanceQuantificationValue());

			UpdateSynthesisFilterType::Pointer updateSynthesisFilter = UpdateSynthesisFilterType::New();
			updateSynthesisFilter->SetFunctor(updateSynthesisFunctor);
			updateSynthesisFilter->SetInput(concatenator->GetOutput());

			updateSynthesisFilter->UpdateOutputInformation();
			int nbComponents = updateSynthesisFunctor.GetNbOfOutputComponents();
			std::cout << "Total Components for UpdateSynthesis: " << nbComponents << std::endl;

			updateSynthesisFilter->GetOutput()->SetNumberOfComponentsPerPixel(nbComponents);
			m_UpdateSynthesisList->PushBack(updateSynthesisFilter);
			SetParameterOutputImagePixelType(getParameterName("out", resolution), ImagePixelType_int16);
			SetParameterOutputImage(getParameterName("out", resolution), updateSynthesisFilter->GetOutput());

		}
		return;
	}

	InputVectorImageType::Pointer             m_CloudMask, m_WaterMask, m_SnowMask, m_WeightsL2A;
	std::vector<ImageListType::Pointer>		  m_ObjectList;
	std::vector<VectorImageListType::Pointer> m_VectorObjectList;
	ReaderListType::Pointer					  m_ReaderList;
	ConcatenatorListType::Pointer			  m_ConcatenatorList;
	UpdateSynthesisListType::Pointer		  m_UpdateSynthesisList;
	std::vector<ResamplingBandExtractor<float>> m_ResamplerExtractorList;
};

} //namespace Wrapper
} //namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::UpdateSynthesis)


