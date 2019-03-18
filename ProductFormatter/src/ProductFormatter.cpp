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
#include "string_utils.hpp"
#include "ProductCreatorSentinelMuscate.h"
#include "ProductCreatorVenusMuscate.h"

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
 * @brief Creates the final output product using the previously generated files from UpdateSynthesis as well as all Metadata Information
 */
class ProductFormatter : public Application
{

public:
	typedef ProductFormatter Self;
	typedef Application Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;

	itkNewMacro(Self)
	itkTypeMacro(ProductFormatter, otb::Application)

private:

	void DoInit()
	{
		SetName("ProductFormatter");
		SetDescription("Creates the final output product using the previously generated files from UpdateSynthesis as well as all Metadata Information");

		SetDocName("ProductFormatter");
		SetDocLongDescription("Creates the final output product using the previously generated files from UpdateSynthesis as well as all Metadata Information");
		SetDocLimitations("None");
		SetDocAuthors("Peter KETTIG, CNES");
		SetDocSeeAlso(" ");

		//composite parameters
		AddParameter(ParameterType_StringList, "products", "L3A Product Files for each resolution containing the following rasters as bands B(0-n): B0=Weights, B1=Dates, B2-Bn-1=Refls, Bn=Flags");
		AddParameter(ParameterType_String, "platform", "The platform to be create the product for: Sentinel/Venus");
		AddParameter(ParameterType_String, "gipp", "GIPP-Files used for the execution");
		MandatoryOff("gipp");


		// Other parameters
		AddParameter(ParameterType_Int, "cog", "Create all raster as Cloud-Optimized GeoTiffs");
		MandatoryOff("cog");
		AddParameter(ParameterType_String, "cogtemp", "Temp-Directory for the creation of Cloud-Optimized GeoTiffs");
		MandatoryOff("cogtemp");
		AddParameter(ParameterType_String, "destination", "Destination root directory");
		AddParameter(ParameterType_String, "syntdate", "Synthesis date of the final product in the format YYYY-MM-DDTHH:mm:SS.sssZ");
		AddParameter(ParameterType_String, "begin", "Begin of the synthesis period in the format YYYY-MM-DDTHH:mm:SS.sssZ");
		AddParameter(ParameterType_String, "end", "End of the synthesis period in the format YYYY-MM-DDTHH:mm:SS.sssZ");
		AddParameter(ParameterType_InputFilenameList, "xml", "The L2A/L3A MUSCATE xml files");
		AddParameter(ParameterType_String, "vcurrent", "Current version of the Processing chain in format 'N.N'");
		MandatoryOff("vcurrent");

		SetDocExampleParameterValue("products", "L3AResult0_10m.tif L3AResult0_20m.tif");
		SetDocExampleParameterValue("platform", "SENTINEL");
		SetDocExampleParameterValue("gipp", "gipp_file.xml");
		SetDocExampleParameterValue("destination", "/path/to/desired/Output");
		SetDocExampleParameterValue("xml", "SENTINEL2A_20170402-093844-724_L2A_T32MNE_D_V1-4_MTD_ALL.xml image2.hdr");
		SetDocExampleParameterValue("syntdate", "2017-04-02T09:38:44.724Z");
		SetDocExampleParameterValue("begin", "2017-04-02T09:38:44.724Z");
		SetDocExampleParameterValue("end", "2017-04-02T09:38:44.724Z");
		SetDocExampleParameterValue("vcurrent", "1-1");
	}

	void DoUpdateParameters()
	{
		//Nothing to do here
	}

	void DoExecute()
	{
		versionType version;
		if(HasValue("vcurrent")){
			std::string versionStr = GetParameterString("vcurrent");
			version = setProductVersion(versionStr);
		}else{
			version = setProductVersion(std::string(DEFAULT_PRODUCT_VERSION));
		}

		std::vector<std::string> products = this->GetParameterStringList("products");

		//get destination root
		std::string destination = this->GetParameterString("destination");

		//Get synthesis date
		std::string syntdate = this->GetParameterString("syntdate");

		//Get synthesis date beginning and end
		std::string syntPeriodBegin = this->GetParameterString("begin");
		std::string syntPeriodEnd = this->GetParameterString("end");

		std::string gippFilename = this->GetParameterString("gipp");

		std::string cogtemp = "";
		bool cog = false;
		if(HasValue("cog") && HasValue("cogtemp")){
			cogtemp = this->GetParameterString("cogtemp");
			cog = this->GetParameterInt("cog") > 0 ? true : false;
		}

		//read .xml or .HDR files to fill the metadata structures
		// Get the list of input files
		std::vector<std::string> descriptors = this->GetParameterStringList("xml");
		m_creator = GetProductCreator(GetParameterAsString("platform"));
		m_creator->init(products, syntdate, syntPeriodBegin, syntPeriodEnd, version, gippFilename, descriptors, cog, cogtemp);
		if(m_creator->createProduct(destination) == false){
			itkExceptionMacro("Error in product creation of " << destination);
		}
		m_creator.release(); //needed as otherwise ITK will throw a warning about deletion of a still-referenced object
		return;
	}

	/**
	 * @brief Set the private Product Version variables from a given string to Major and Minor
	 * @param version The string in the format "N.M"
	 * @return versionType
	 */
	versionType setProductVersion(const std::string &version){
		if(version.length() < 3 || version.find(std::string("-")) == std::string::npos){
			itkExceptionMacro("ERROR! Version " + version + " is not of valid length or format.");
		}
		std::string strMajor = version.substr(0, version.find("-"));
		std::string strMinor = version.substr(version.find("-") + 1);
		if(is_number(strMajor) && is_number(strMinor)){
			versionType v;
			v.major = std::stoul(strMajor);
			v.minor = std::stoul(strMinor);

			std::cout << "Setting product Version to: " << v.major << "." << v.minor << std::endl;
			return v;
		}
		itkExceptionMacro("ERROR: Version numbers are invalid: " + strMajor + " " + strMinor);
	}


	/**
	 * @brief Get the product creator depending on the platform
	 * @param p The Platform string, which can be: Sentinel, Venus
	 * @return Unique pointer to the productCreator which was chosen
	 */
	std::unique_ptr<ProductCreatorAdapter> GetProductCreator(const std::string& p){
		std::string platform = toUppercase(p);

		if(std::string(platform).find(SENTINEL_MISSION_STR) != std::string::npos){
			std::unique_ptr<ProductCreatorAdapter> sentinelMuscate(new ProductCreatorSentinelMuscate);
			return sentinelMuscate;
		}
		if(std::string(platform).find(VENUS_MISSION_STR) != std::string::npos){
			std::unique_ptr<ProductCreatorAdapter> venusMuscate(new ProductCreatorVenusMuscate);
			return venusMuscate;
		}
		itkExceptionMacro("Cannot find Product Creator for the given platform: " << platform << "/" << p);
	}

private:

	//Product Creator
	std::unique_ptr<ProductCreatorAdapter> m_creator;
};
} //namespace Wrapper
} //namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::ProductFormatter)
