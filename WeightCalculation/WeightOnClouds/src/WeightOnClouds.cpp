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

#include "../include/CloudsInterpolation.h"
#include "CloudMaskBinarization.h"
#include "CloudWeightComputation.h"
#include "CuttingImageFilter.h"
#include "GaussianFilter.h"
#include "PaddingImageHandler.h"
#include "MetadataHelperFactory.h"

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
 * @brief Computes the cloud weight value for the given mask cloud and parameters
 */
class WeightOnClouds : public Application
{

public:
	/** Standard class typedefs. */
	typedef WeightOnClouds                      Self;
	typedef Application                   Superclass;
	typedef itk::SmartPointer<Self>       Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;

	/** Standard macro */
	itkNewMacro(Self);

	itkTypeMacro(WeightOnClouds, otb::Application);

private:
	void DoInit()
	{
		SetName("WeightOnClouds");
		SetDescription("Computes the cloud weight value for the given mask cloud and parameters");

		SetDocName("Weight on Clouds");
		SetDocLongDescription("Computes the cloud weight value for the given mask cloud and parameters");

		SetDocLimitations("None");
		SetDocAuthors("Peter KETTIG");
		SetDocSeeAlso(" ");

		AddParameter(ParameterType_String,  "incldmsk",   "Input cloud mask image");
		SetParameterDescription("incldmsk", "Image containing the cloud mask.");

		AddParameter(ParameterType_Int, "coarseres", "Coarse resolution");
		SetParameterDescription("coarseres", "The resolution for the undersampling.");
		SetDefaultParameterInt("coarseres", 240);
		MandatoryOff("coarseres");

		AddParameter(ParameterType_Float, "sigmasmallcld", "Small cloud sigma");
		SetParameterDescription("sigmasmallcld", "Sigma value for the small cloud gaussian filter.");

		AddParameter(ParameterType_Float, "sigmalargecld", "Large cloud sigma");
		SetParameterDescription("sigmalargecld", "Sigma value for the large cloud gaussian filter.");

		AddParameter(ParameterType_Int, "kernelwidth", "Gaussian filter kernel width");
		SetParameterDescription("kernelwidth", "The gaussian filter kernel width.");
		SetDefaultParameterInt("kernelwidth", 801);
		MandatoryOff("kernelwidth");

		AddParameter(ParameterType_OutputImage, "out", "Output Cloud Weight Image");
		SetParameterDescription("out","The output image containg the computed cloud weight for each pixel.");

		AddParameter(ParameterType_Int, "cut", "Cut Oversampled images");
		SetParameterDescription("cut", "Cut the oversampled images coming out of the Cloud detection to fit the original size again");
		MandatoryOff("cut");

	    AddRAMParameter();

		// Doc example parameter settings
		SetDocExampleParameterValue("incldmsk", "verySmallFSATSW_r.tif");
		SetDocExampleParameterValue("coarseres", "240");
		SetDocExampleParameterValue("sigmasmallcld", "10.0");
		SetDocExampleParameterValue("sigmalargecld", "50.0");
		SetDocExampleParameterValue("kernelwidth", "81");
		SetDocExampleParameterValue("out", "apAOTWeightOutput.tif");
		SetDocExampleParameterValue("cut", "0");

	}

	void DoUpdateParameters()
	{
	}

	void DoExecute()
	{
		bool bRoiCutOversampledImgs = true;
		if(HasValue("cut")){
			bRoiCutOversampledImgs = GetParameterInt("cut") > 0 ? true : false;
		}
		bool bWriteDebugFiles = false;

		// Get the input image list
		std::string inCldFileName = GetParameterString("incldmsk");
		if (inCldFileName.empty())
		{
			itkExceptionMacro("No input Image set...; please set the input image");
		}
		int inputCloudMaskResolution = -1;
		int outputResolution = -1;
		int coarseResolution = GetParameterInt("coarseres");
		float sigmaSmallCloud = GetParameterFloat("sigmasmallcld");
		float sigmaLargeCloud = GetParameterFloat("sigmalargecld");
		int gaussianKernelWidth = GetParameterInt("kernelwidth");

		long inImageWidth, inImageHeight;

		m_cloudMaskBinarization.SetInputFileName(inCldFileName);

		m_underSampler.SetInputImageReader(m_cloudMaskBinarization.GetOutputImageSource());
		m_underSampler.SetOutputResolution(coarseResolution);
		m_underSampler.SetInputResolution(inputCloudMaskResolution);
		if(inputCloudMaskResolution == -1) {
			inputCloudMaskResolution = m_underSampler.GetInputImageResolution();
		}
		// compute dynamically the BCO radius - it is = (2 * (coarseRes/inputRes))
		m_underSampler.SetBicubicInterpolatorRadius(2*(coarseResolution/inputCloudMaskResolution));
		m_underSampler.GetInputImageDimension(inImageWidth, inImageHeight);

		m_padding1.SetInputImageReader(m_cloudMaskBinarization.GetOutputImageSource(), m_underSampler.GetOutputImageSource());

		m_cloudMaskBinarization2.SetInputImageReader(m_padding1.GetOutputImageSource());
		m_cloudMaskBinarization2.SetThreshold(0.5f);

		// Compute the DistLargeCloud, Low Res
		m_gaussianFilterSmallCloud.SetInputImageReader(m_cloudMaskBinarization2.GetOutputImageSource());
		m_gaussianFilterLargeCloud.SetInputImageReader(m_cloudMaskBinarization2.GetOutputImageSource());

		m_gaussianFilterSmallCloud.SetSigma(sigmaSmallCloud);
		m_gaussianFilterSmallCloud.SetKernelWidth(gaussianKernelWidth);

		m_gaussianFilterLargeCloud.SetSigma(sigmaLargeCloud);
		m_gaussianFilterLargeCloud.SetKernelWidth(gaussianKernelWidth);

		if(outputResolution < 0) {
			outputResolution = inputCloudMaskResolution;
			std::cout << "Resolution: " << outputResolution << std::endl;
		}

		// resample at the current small resolution (10 or 20) the small cloud large resolution image
		m_overSamplerSmallCloud.SetInputImageReader(m_gaussianFilterSmallCloud.GetOutputImageSource());
		m_overSamplerSmallCloud.SetInputResolution(coarseResolution);
		m_overSamplerSmallCloud.SetOutputResolution(outputResolution);
		// NOTE: This was modified compated to DPM
		//m_overSamplerSmallCloud.SetInterpolator(Interpolator_Linear);
		if(!bRoiCutOversampledImgs) {
			m_overSamplerSmallCloud.SetOutputForcedSize(inImageWidth, inImageHeight);
		}

		// resample at the current small resolution (10 or 20) the large cloud large resolution image
		m_overSamplerLargeCloud.SetInputImageReader(m_gaussianFilterLargeCloud.GetOutputImageSource());
		m_overSamplerLargeCloud.SetInputResolution(coarseResolution);
		m_overSamplerLargeCloud.SetOutputResolution(outputResolution);
		// NOTE: This was modified compated to DPM
		//m_overSamplerLargeCloud.SetInterpolator(Interpolator_Linear);
		if(!bRoiCutOversampledImgs) {
			m_overSamplerLargeCloud.SetOutputForcedSize(inImageWidth, inImageHeight);
			// compute the weight on clouds
			m_cloudWeightComputation.SetInputImageReader1(m_overSamplerSmallCloud.GetOutputImageSource());
			m_cloudWeightComputation.SetInputImageReader2(m_overSamplerLargeCloud.GetOutputImageSource());
		} else {
			m_cutting1.SetInputImageReader(m_overSamplerSmallCloud.GetOutputImageSource(), inImageWidth, inImageHeight);
			m_cutting2.SetInputImageReader(m_overSamplerLargeCloud.GetOutputImageSource(), inImageWidth, inImageHeight);

			m_padding2.SetInputImageReader(m_cutting1.GetOutputImageSource(), inImageWidth, inImageHeight);
			m_padding3.SetInputImageReader(m_cutting2.GetOutputImageSource(), inImageWidth, inImageHeight);

			m_cloudWeightComputation.SetInputImageReader1(m_padding2.GetOutputImageSource());
			m_cloudWeightComputation.SetInputImageReader2(m_padding3.GetOutputImageSource());
		}
		// Set the output image
		SetParameterOutputImage("out", m_cloudWeightComputation.GetOutputImageSource()->GetOutput());

		// write debug infos if needed
		if(bWriteDebugFiles) {
			std::string strOutImg = GetParameterAsString("out");
			std::string strBaseName = strOutImg;
			size_t lastDotIdx = strOutImg.find_last_of(".");
			if(lastDotIdx != std::string::npos) {
				strBaseName = strOutImg.substr(0, lastDotIdx);
			}

			std::string coarseResStr = std::to_string(coarseResolution);
			std::string inputResStr = std::to_string(inputCloudMaskResolution);

			std::string binarizedFile = strBaseName + "_1_binarized_clouds_" + inputResStr + "m.tif";
			m_cloudMaskBinarization.SetOutputFileName(binarizedFile);
			m_cloudMaskBinarization.WriteToOutputFile();

			std::string undersamplerFile = strBaseName + "_2_bco_clouds_" + coarseResStr + "m.tif";
			m_underSampler.SetOutputFileName(undersamplerFile);
			m_underSampler.WriteToOutputFile();

			std::string undersamplerFilePan = strBaseName + "_2_bco_clouds_pan_" + coarseResStr + "m.tif";
			m_padding1.SetOutputFileName(undersamplerFilePan);
			m_padding1.WriteToOutputFile();

			std::string binarizedFile2 = strBaseName + "_2_binarized_clouds_" + coarseResStr + "m.tif";
			m_cloudMaskBinarization2.SetOutputFileName(binarizedFile2);
			m_cloudMaskBinarization2.WriteToOutputFile();

			std::string smallCldLowRes = strBaseName + "_3_small_cloud_" + coarseResStr + "m.tif";
			m_gaussianFilterSmallCloud.SetOutputFileName(smallCldLowRes);
			m_gaussianFilterSmallCloud.WriteToOutputFile();

			std::string largeCldLowRes = strBaseName + "_4_large_cloud_" + coarseResStr + "m.tif";
			m_gaussianFilterLargeCloud.SetOutputFileName(largeCldLowRes);
			m_gaussianFilterLargeCloud.WriteToOutputFile();

			std::string smallCldHighRes = strBaseName + "_5_small_cloud_" + inputResStr + "m.tif";
			m_overSamplerSmallCloud.SetOutputFileName(smallCldHighRes);
			m_overSamplerSmallCloud.WriteToOutputFile();

			std::string largeCldHighRes = strBaseName + "_6_large_cloud_" + inputResStr + "m.tif";
			m_overSamplerLargeCloud.SetOutputFileName(largeCldHighRes);
			m_overSamplerLargeCloud.WriteToOutputFile();

			if(bRoiCutOversampledImgs) {
				std::string smallCldHighResCut = strBaseName + "_7_small_cloud_cut_" + inputResStr + "m.tif";
				m_cutting1.SetOutputFileName(smallCldHighResCut);
				m_cutting1.WriteToOutputFile();

				std::string largeCldHighResCut = strBaseName + "_8_large_cloud_cut_" + inputResStr + "m.tif";
				m_cutting2.SetOutputFileName(largeCldHighResCut);
				m_cutting2.WriteToOutputFile();

				std::string smallCldHighResPan = strBaseName + "_9_small_cloud_pan_" + inputResStr + "m.tif";
				m_padding2.SetOutputFileName(smallCldHighResPan);
				m_padding2.WriteToOutputFile();

				std::string largeCldHighResPan = strBaseName + "_10_large_cloud_pan_" + inputResStr + "m.tif";
				m_padding3.SetOutputFileName(largeCldHighResPan);
				m_padding3.WriteToOutputFile();

			}
		}
	}

	CloudsInterpolation<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_underSampler;
	CloudMaskBinarization<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_cloudMaskBinarization;
	CloudMaskBinarization<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_cloudMaskBinarization2;
	GaussianFilter<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_gaussianFilterSmallCloud;
	GaussianFilter<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_gaussianFilterLargeCloud;
	CloudsInterpolation<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_overSamplerSmallCloud;
	CloudsInterpolation<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_overSamplerLargeCloud;
	CloudWeightComputation<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_cloudWeightComputation;

	PaddingImageHandler<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_padding1;

	PaddingImageHandler<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_padding2;
	PaddingImageHandler<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_padding3;

	CuttingImageHandler<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_cutting1;
	CuttingImageHandler<otb::Wrapper::FloatImageType, otb::Wrapper::FloatImageType> m_cutting2;
};

} // namespace Wrapper
} // namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::WeightOnClouds)

