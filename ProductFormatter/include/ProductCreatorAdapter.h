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

#ifndef PRODUCTFORMATTER_INCLUDE_PRODUCTCREATORADAPTER_H_
#define PRODUCTFORMATTER_INCLUDE_PRODUCTCREATORADAPTER_H_

#include "GlobalDefs.h"
#include "MuscateMetadataHelper.h"
#include "MuscateMetadataWriter.hpp"
#include "ProductDefinitions.h"
#include "BaseImageTypes.h"
#include "otbVectorImageToImageListFilter.h"
#include "itkLightObject.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Adapter for all Product-formatters of each platform
 */
class ProductCreatorAdapter : public itk::LightObject, public BaseImageTypes{
public:
	/**
	 * @brief Initialize a ProductFormatter
	 * @param products The input product filenames
	 * @param syntdate The syntdate of the product
	 * @param begin Beginning of the synthesis period
	 * @param end End of the synthesis period
	 * @param version Version of the product (Not the Executable!)
	 * @param gipp GIPP-Filename
	 * @param xml L2A XML input list
	 * @param cog Write all rasters as Cloud-Optimized GeoTiff or not
	 * @param cogtemp Temporary CoG filepath to be used
	 */
	virtual void init(
			const std::vector<std::string> &products,
			const std::string &syntdate,
			const std::string &begin,
			const std::string &end,
			const versionType &version,
			const std::string &gipp,
			const std::vector<std::string> &xml,
			const bool &cog,
			const std::string &cogtemp
	) {
		m_products = products;
		m_syntDate = syntdate;
		m_syntPeriodBegin = begin;
		m_syntPeriodEnd = end;
		m_version = version;
		m_gipp = gipp;
		m_xml = xml;
		m_cog = cog;
		m_cogtemp = cogtemp;
	}

	/**
	 * @brief Create the product to the specified destination
	 * @param destination The destination directory
	 * @return True, if the creation succeeded, false if not
	 */
	virtual bool createProduct(const std::string &destination) = 0;

protected:
	typedef muscate::MuscateMetadataReader MuscateMetadataReaderType;
	typedef muscate::MuscateMetadataWriter MuscateMetadataWriterType;
	typedef otb::VectorImageToImageListFilter<ShortVectorImageType, ByteImageListType>   		ExtractMaskType;
	typedef otb::VectorImageToImageListFilter<ShortVectorImageType, ShortImageListType>			ExtractRasterType;

	/**
	 * @brief Read all metadata files and FillMetadataInfo for each file. Also determines the GeoInfo to be used
	 * @param descriptors Vector of strings of xml-filenames
	 */
	virtual void LoadAllDescriptors(const std::vector<std::string> &descriptors) = 0;

	/**
	 * @brief Fill metadata info for each input metadata
	 * @param metadata The loaded input L2 or L3 metadata-file
	 */
	void FillMetadataInfo(std::shared_ptr<MuscateFileMetadata> &metadata);

	/**
	 * @brief Determines the name of the L3A output product, given then XML inputs.
	 * @param mission Mission specifier, see missionTypes
	 * @return String The platform name. Possible choices: SENTINEL2[A,B,X], Venus
	 */
	std::string getPlatform(const missionTypes &mission);

	/**
	 * @brief Get the Geoposition information from a Level-2 product
	 * @param desc The filename to the L2-XML
	 * @return Geoposition to be used in the L3 Metadata
	 */
	virtual MuscateGeopositionInformations GetGeopositionInformations(const std::string &desc) = 0;

	/**
	 * @brief Set the BandInformation-vector for the respective Platform
	 * @return Vector, containing the Radiometric band-infos
	 */
	virtual std::vector<BandInformation> getMissionBands() = 0;

	//////////////////////////////////
	/////// Helping Functions ///////
	////////////////////////////////

	/**
	 * @return String The System time in the format YYYY-MM-DDTHH:mm:SS.sssZ
	 * @note Used for the ProductFormatting of the metadata files
	 */
	const std::string currentDateTime();

	/**
	 * @brief convert datetime string from format YYYY-MM-DDTHH:mm:SS.sssZ to YYYYMMDD-HHMMSS-sss
	 * @param String The System time in the format YYYY-MM-DDTHH:mm:SS.sssZ
	 * @return String Datetime in format YYYYMMDD-HHMMSS-sss
	 */
	const std::string convertDateTime(const std::string &datetime);

	/**
	 * @brief Creates a directory given by full path
	 * @param path String Full path to the new directory
	 * @return Bool True, if successful creation
	 */
	bool mkPath(const std::string &path);

	/**
	 * @brief Extract an image and write it to the given filename
	 * @param splitter VectorImageToImageList-Filter containing the desired image
	 * @param filename The path to write the image to
	 * @param index The index of the image list. It is not checked, whether the index exists - An error will be thrown by ITK in this case
	 * @return True, if writing succeeded.
	 */
	template<typename ExtractorType>
	bool writeExtractedImage(typename ExtractorType::Pointer splitter, const std::string &filename, const size_t &index){
		typedef otb::ImageFileWriter<typename ExtractorType::OutputImageType> WriterType;
		typename WriterType::Pointer writer = WriterType::New();
		writer->SetInput(splitter->GetOutput()->GetNthElement(index));
		writer->SetFileName(filename);

		writer->Update();
		return true;
	}

	/**
	 * @brief Extract an image and write it to the given filename as CO-GeoTiff
	 * @param splitter VectorImageToImageList-Filter containing the desired image
	 * @param filename The path to write the image to
	 * @param index The index of the image list. It is not checked, whether the index exists - An error will be thrown by ITK in this case
	 * @return True, if writing succeeded.
	 * @note Writes the image two times: Once for creating the overviews to temp-dir and second as the final output
	 */
	template<typename ExtractorType>
	bool writeExtractedImageCoGeoTiff(typename ExtractorType::Pointer splitter, const std::string &temp,
			const std::string &filename, const size_t &index){
		typedef otb::ImageFileWriter<typename ExtractorType::OutputImageType> WriterType;
		typename WriterType::Pointer writer = WriterType::New();
		writer->SetInput(splitter->GetOutput()->GetNthElement(index));
		std::string tempName = temp + "/tempGeotiff.tif";
		writer->SetFileName(tempName + CoG_TRANSLATE_PREPARE);
		writer->Update();
		GeoTiffConfig_t geo;
		geo.initToDefaultValues(tempName); //Init gdaladdo config
		std::vector<std::string> addoGenerator = geo.toString();
		if(!ExecuteExternalProgram(GDAL_ADDO, addoGenerator)){
			itkExceptionMacro("Error when executing " << GDAL_ADDO);
		}
		std::vector<std::string> cogGenerator = {tempName, filename};
		std::vector<std::string> cogGeneratorConfig = geo.getTranslateConfig();
		std::move(cogGenerator.begin(), cogGenerator.end(), std::back_inserter(cogGeneratorConfig));

		if(!ExecuteExternalProgram(GDAL_TRANSLATE, cogGeneratorConfig)){
			itkExceptionMacro("Error when executing " << GDAL_TRANSLATE);
		}
		//remove tempfile
		remove(tempName.c_str());

		return true;
	}

	/**
	 * @brief Copy File from source to destination
	 * @param strDest The Destination for the file
	 * @param strSrc The source file
	 * @return Bool True, if the copying succeeded, false if not
	 */
	bool CopyFile(const std::string &strDest, const std::string &strSrc);

	/**
	 * @brief Replace substring with another string
	 * @param subject The input string which has to be modified
	 * @param search The substring inside the subject to be replaced
	 * @param replace The string to replace the search string
	 * @return string The new string with the replaced part
	 */
	std::string ReplaceString(std::string subject, const std::string& search,
			const std::string& replace);

	/**
	 * @brief Executes external application using posix
	 * @param appExe The application name
	 * @param appArgs The Arguments to go with the main application
	 * @return True, if the program finished successfully, false if not
	 */
	bool ExecuteExternalProgram(const char *appExe, const std::vector<std::string> &appArgs);

	/**
	 * @brief Create the product directory name following the given conventions
	 * @param productType The type of the product
	 * @param productVersion The product version
	 * @return string of the product directory name
	 */
	virtual std::string BuildProductDirectoryName(const std::string &productType, const std::string &productVersion) = 0;

	/**
	 * @brief Create Root folder as well as DATA/ and MASKS/ inside for the new product
	 * @param strMainFolderPath The name of the product folder
	 * @return bool True, if all folders were created successfully. False, if not
	 */
	bool createAllFolders(const std::string &strMainFolderPath);

	/**
	 * @brief Count Cloudy, Snowy pixels in the L3-Product
	 * @param image The input FLG-image
	 * @param cloudCounter Counter for clouds
	 * @param snowCounter Counter for snow
	 * @param fullCounter Counter for the full image
	 * @return True, if the counting succeeded.
	 */
	bool countSpecialPixels(ByteImageType::Pointer image, uint64_t &cloudCounter, uint64_t &snowCounter, uint64_t &fullCounter);

	/**
	 * @brief determines the BandGlobalList depending on which mission was chosen
	 * @param missionBands The BandInformation for the specific platform
	 * @return BandList struct containing all available bands in the output product
	 */
	BandList getBandGlobalList(const std::vector<BandInformation> &missionBands);

	/**
	 * @brief determines the BandGroups depending on which mission was chosen
	 * @return BandList struct containing all available bands in the output product
	 */
	virtual std::vector<BandGroup> getBandGroupList(const std::vector<BandInformation> &missionBands) = 0;

	/**
	 * @brief Copies the GIPP file to the DATA/ folder
	 * @param gippPathIn The path to the gipp file
	 * @return true, if both copying and metadata creating succeeded, false if not
	 */
	bool copyGIPPFile(const std::string &gippPathIn);

	/**
	 * @brief Creates Quicklook using the otbApp from an RGB image
	 * @param rasterFullFilePath The input RGB image with band order Ch1=Red,Ch2=Green,Ch3=Blue
	 * @param jpegFilename The output filename for the jpeg. The full path is prepended in this function
	 * @param offset the offset of the bandIDs
	 * @param bandIDs The IDs of the bands to be used
	 * @return Bool True, if the creation succeeded, false if not
	 */
	bool generateQuicklook(const std::string &rasterFullFilePath, const std::string &jpegFilename, const std::vector<int> &bandIDs, const int &offset);

	/**
	 * @brief Generates the metadata file for the output product in Muscate Unified format
	 * @param strProductMetadataFilePath The path where the metadata file shall be written to
	 * @param cloudPercent The percentage of clouds in the image
	 * @return void - Writes Metadata inside the product folder, named {}_MTD_ALL.xml
	 */
	virtual void generateMetadataFile(const std::string &strProductMetadataFilePath, const std::string &cloudPercent, const std::string &snowPercent) = 0;

	std::vector<std::string> m_products;
	versionType m_version;
	std::string m_gipp;
	std::vector<std::string> m_xml;
	std::string m_cogtemp;
	bool m_cog;

	//Synthesis period informations
	std::string m_syntDate;
	std::string m_syntPeriodBegin;
	std::string m_syntPeriodEnd;
	//The main metadata file
	MuscateFileMetadata m_productMetadata;
	size_t m_totalRes;
	//ImgInformation about each of the masks/images to be set in the metadata.
	//These vec's are filled in the copyRaster method
	std::vector<ImageInformation> m_FRCs;
	std::vector<ImageInformation> m_FLGs;
	std::vector<ImageInformation> m_WGTs;
	std::vector<ImageInformation> m_DTSs;
	//Output basepath and Full path
	std::string m_strProductDirectoryName;
	std::string m_strProductFullPathOut;
	std::string m_tileID;
	//Main Mission name
	missionTypes m_mission;
	std::string m_currentDateTime;
	std::string m_strDestRoot;
	std::string m_strTileID;
};
} /* namespace ts */


#endif /* PRODUCTFORMATTER_INCLUDE_PRODUCTCREATORADAPTER_H_ */
