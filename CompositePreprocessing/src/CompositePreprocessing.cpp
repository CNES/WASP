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

#include "BaseImageTypes.h"
#include "PreprocessingSentinel.h"
#include "PreprocessingVenus.h"

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
 * @brief Perform the directional correction on all TOA-images.
 * First, the approximated NDVI is calculated using B4 and B8 of the Sentinel-2 products.
 * Afterwards, the Angles Rasters are created for each resolution and finally, the Water, Snow and Cloud-masks are read
 * to initialize the directional Correction
 * @note Runs on both Resolutions R1 and R2 sequentially, if both output filenames are set.
 */
class CompositePreprocessing : public Application, public BaseImageTypes
{
public:
	typedef CompositePreprocessing Self;
	typedef Application Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;
	itkNewMacro(Self)
	itkTypeMacro(CompositePreprocessingOld, otb::Application)

private:

	/**
	 * @brief Inits the Documentation as well as the command-line parameters
	 */
	void DoInit()
	{
		SetName("CompositePreprocessing");
		SetDescription("Perform the directional correction of FRE Images");

		SetDocName("CompositePreprocessing");
		SetDocLongDescription("Perform the directional correction of FRE Images");
		SetDocLimitations("None");
		SetDocAuthors("Peter KETTIG");
		SetDocSeeAlso(" ");

		AddParameter(ParameterType_String, "xml", "Muscate L2A XML filepath");

		// Directional Correction parameters
		AddParameter(ParameterType_String, "scatteringcoeffsr1", "Scattering coefficients filename R1");
		MandatoryOff("scatteringcoeffsr1");
		AddParameter(ParameterType_String, "scatteringcoeffsr2", "Scattering coefficients filename R2");
		MandatoryOff("scatteringcoeffsr2");
		AddParameter(ParameterType_OutputImage, "outr1", "Out Image at R1 resolution");
		MandatoryOff("outr1");
		AddParameter(ParameterType_OutputImage, "outr2", "Out Image at R2 resolution");
		MandatoryOff("outr2");
		AddParameter(ParameterType_OutputImage, "outcld", "Out cloud mask image R1 resolution");
		MandatoryOff("outcld");
		AddParameter(ParameterType_OutputImage, "outwat", "Out water mask image R1 resolution");
		MandatoryOff("outwat");
		AddParameter(ParameterType_OutputImage, "outsnw", "Out snow mask image R1 resolution");
		MandatoryOff("outsnw");
		AddParameter(ParameterType_OutputImage, "outaot", "Out aot mask image R1 resolution");
		MandatoryOff("outaot");

		AddRAMParameter();

		SetDocExampleParameterValue("xml", "/path/to/L2Aproduct_muscate.xml");
		SetDocExampleParameterValue("scatteringcoeffsr1", "/path/to/scattering_coeffs_10m.txt");
		SetDocExampleParameterValue("scatteringcoeffsr2", "/path/to/scattering_coeffs_20m.txt");

		SetDocExampleParameterValue("outr1", "/path/to/output_image_r1.tif");
		SetDocExampleParameterValue("outr2", "/path/to/output_image_r2.tif");
		SetDocExampleParameterValue("outcld", "/path/to/output_image_cloud.tif");
		SetDocExampleParameterValue("outwat", "/path/to/output_image_water.tif");
		SetDocExampleParameterValue("outsnw", "/path/to/output_image_snow.tif");
		SetDocExampleParameterValue("outaot", "/path/to/output_image_aot.tif");

	}

	/**
	 * @brief Updates the parameters
	 * @note Not needed here.
	 */
	void DoUpdateParameters()
	{
		// Nothing to do.
	}

	/**
	 * @brief Sets up and runs the pipeline
	 */
	void DoExecute()
	{
		std::string inXml = GetParameterAsString("xml");
		std::cout << "XML found: " << inXml << std::endl;
		auto factory = MetadataHelperFactory::New();
		auto pHelper = factory->GetMetadataHelper(inXml);
		m_processor = GetPreprocessor(pHelper->GetMissionName());
		m_processor->init();


		typename FloatImageType::Pointer cldImg = m_processor->getCloudMask(pHelper->GetCloudImageFileNames()[MAIN_RESOLUTION_INDEX]).GetPointer();
		SetParameterOutputImage("outcld", cldImg.GetPointer());
		FloatImageType::Pointer watImg = m_processor->getWaterMask(pHelper->GetWaterImageFileNames()[MAIN_RESOLUTION_INDEX]).GetPointer();
		SetParameterOutputImage("outwat", watImg.GetPointer());
		FloatImageType::Pointer snowImg = m_processor->getSnowMask(pHelper->GetSnowImageFileNames()[MAIN_RESOLUTION_INDEX]).GetPointer();
		SetParameterOutputImage("outsnw", snowImg.GetPointer());
		FloatImageType::Pointer aotImg = m_processor->getAotMask(pHelper->GetAotImageFileNames()[MAIN_RESOLUTION_INDEX]).GetPointer();
		SetParameterOutputImage("outaot", aotImg.GetPointer());

		size_t totalNRes = pHelper->getResolutions().getNumberOfResolutions();

		if(this->HasValue("scatteringcoeffsr1") && this->HasValue("scatteringcoeffsr2")){
			std::vector<std::string> scatteringCoeffs = {GetParameterAsString("scatteringcoeffsr1"), GetParameterAsString("scatteringcoeffsr2")};
			m_processor->setScatteringCoefficients(scatteringCoeffs);
		}

		std::vector<Int16VectorImageType::Pointer> correctedRasters = m_processor->getCorrectedRasters(inXml, cldImg.GetPointer(), watImg.GetPointer(), snowImg.GetPointer());
		//For all possible resolutions, do
		for(size_t resolution = 0; resolution < totalNRes; resolution++){
			SetParameterOutputImage(std::string("outr" + std::to_string(resolution+1)).c_str(), correctedRasters[resolution].GetPointer());
		}
		std::string parameterStr = "outr2";
		std::string parameter = GetParameterAsString(parameterStr);
		if(!parameter.empty() && totalNRes < size_t(N_RESOLUTIONS_SENTINEL)){
			itkExceptionMacro("Cannot set parameter " << parameterStr << " for a Platform with less than 2 resolutions");
		}
		m_processor.release();
	}

private:

	/**
	 * @brief Get the preprocessor depending on the platform
	 * @param p The Platform string, which can be: SENTINEL, VENUS
	 * @return Unique pointer to the Preprocessor which was chosen
	 */
	std::unique_ptr<preprocessing::PreprocessingAdapter> GetPreprocessor(const std::string& p){

		if(std::string(p).find(SENTINEL_MISSION_STR) != std::string::npos){
		    std::unique_ptr<preprocessing::PreprocessingAdapter> sentinelMuscate(new preprocessing::PreprocessingSentinel);
		    return sentinelMuscate;
		}
		if(std::string(p).find(VENUS_MISSION_STR) != std::string::npos){
			std::unique_ptr<preprocessing::PreprocessingAdapter> venusMuscate(new preprocessing::PreprocessingVenus);
			return venusMuscate;
		}
		itkExceptionMacro("Cannot find PreprocessingAdapter for the given platform: " << p);
	}


	/////////////////////////
	/// Private Variables //
	///////////////////////

	std::unique_ptr<preprocessing::PreprocessingAdapter> m_processor;

};

} //namespace Wrapper
} //namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::CompositePreprocessing)



