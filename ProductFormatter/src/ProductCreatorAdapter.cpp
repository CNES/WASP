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

#include "ProductCreatorAdapter.h"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include <sys/wait.h>
#include <sys/time.h>
#include <spawn.h>
#include <fstream>
#include "itkImageRegionConstIterator.h"
#include "itkImageFileWriter.h"

using namespace ts;

void ProductCreatorAdapter::FillMetadataInfo(std::shared_ptr<MuscateFileMetadata> &metadata)
{
	m_tileID = metadata->DatasetIdentification.GeographicalZone;
	MuscateQualityProduct currentProduct;

	currentProduct.Level = metadata->QualityInformations.CurrentProduct.Level;
	currentProduct.ProductID = metadata->ProductCharacteristics.ProductID;
	currentProduct.AcquisitionDate = metadata->ProductCharacteristics.AcquisitionDate;
	currentProduct.ProductionDate = metadata->ProductCharacteristics.ProductionDate;
	currentProduct.GlobalIndexList = metadata->QualityInformations.CurrentProduct.GlobalIndexList;
	m_productMetadata.QualityInformations.ContributingProducts.emplace_back(currentProduct);
}

std::string ProductCreatorAdapter::getPlatform(const missionTypes &mission){
	switch(mission){
	case S2_Only:
		return std::string(SENTINEL2X);
	case S2A_Only:
		return std::string(SENTINEL2A);
	case S2B_Only:
		return std::string(SENTINEL2B);
	case Venus:
		return std::string(VENUS);
	default:
		itkExceptionMacro("Fatal Error: MissionType invalid: " << mission);
	}
}

const std::string ProductCreatorAdapter::currentDateTime() {
	timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000;

	char buffer [80];
	strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S", localtime(&curTime.tv_sec));

	char currentTime[84] = "";
	sprintf(currentTime, "%s.%dZ", buffer, milli);
	return std::string(currentTime);
}

const std::string ProductCreatorAdapter::convertDateTime(const std::string &datetime) {
	std::string dt = datetime;
	// normally, the product date time respects already the standard ISO 8601
	// we need just to replace - and : from it
	dt.erase(std::remove(dt.begin(), dt.end(), '-'), dt.end());
	dt.erase(std::remove(dt.begin(), dt.end(), ':'), dt.end());
	dt.erase(std::remove(dt.begin(), dt.end(), 'Z'), dt.end());
	// Change it to Format YYYYMMDD-HHmmSS-sss
	std::replace(dt.begin(),dt.end(), '.', '-');
	std::replace(dt.begin(),dt.end(), 'T', '-');
	return dt;
}

bool ProductCreatorAdapter::mkPath(const std::string &path) {
	boost::system::error_code ec;
	boost::filesystem::create_directories(path, ec);
	return !ec;
}
/*
template<typename ExtractorType>
bool ProductCreatorAdapter::writeExtractedImage(typename ExtractorType::Pointer splitter, const std::string &filename, const size_t &index){
	typedef otb::ImageFileWriter<typename ExtractorType::OutputImageType> WriterType;
	typename WriterType::Pointer writer = WriterType::New();
	writer->SetInput(splitter->GetOutput()->GetNthElement(index));
	writer->SetFileName(filename);

	writer->Update();
	return true;
}
 */
bool ProductCreatorAdapter::CopyFile(const std::string &strDest, const std::string &strSrc) {
	struct stat buf;
	if (stat(strSrc.c_str(), &buf) != -1) {
		std::ifstream src(strSrc, std::ios::binary);
		std::ofstream dst(strDest, std::ios::binary);
		dst << src.rdbuf();
		return true;
	}

	itkWarningMacro("Error copying file " << strSrc << " to file " << strDest);
	return false;
}

std::string ProductCreatorAdapter::ReplaceString(std::string subject, const std::string& search,
		const std::string& replace) {
	size_t pos = 0;
	while((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

bool ProductCreatorAdapter::ExecuteExternalProgram(const char *appExe, const std::vector<std::string> &appArgs) {
	int error, status;
	pid_t pid, waitres;
	std::vector<const char *> args;
	std::string cmdInfo;
	args.emplace_back(appExe);
	cmdInfo = appExe;
	for(unsigned int i = 0; i<appArgs.size(); i++) {
		args.emplace_back(appArgs[i].c_str());
		cmdInfo += " ";
		cmdInfo += appArgs[i];
	}

	args.emplace_back(nullptr);

	posix_spawnattr_t attr;
	posix_spawnattr_init(&attr);
	posix_spawnattr_setflags(&attr, POSIX_SPAWN_USEVFORK);
	error = posix_spawnp(&pid, args[0], NULL, &attr, (char *const *)args.data(), environ);
	if(error != 0) {
		itkWarningMacro("Error creating process for \n" << cmdInfo << "\nThe resulting files will not be created. Error was: " << error);
		return false;
	}
	waitres = waitpid(pid, &status, 0);
	if(waitres == pid && (WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
		return true;
	}
	itkWarningMacro("Error running \n" << cmdInfo << "\n The resulting file(s) might not be created. The return was: " << status);
	return false;
}

bool ProductCreatorAdapter::createAllFolders(const std::string &strMainFolderPath) {
	bool bResult = true;
	/* create destination product folder */
	if(mkPath(strMainFolderPath) == false)
	{
		//fail to create main destination product folder
		bResult = false;
		itkExceptionMacro("Failed to create directory " << strMainFolderPath);
	}

	/*create DATA subfolder */
	if(mkPath(strMainFolderPath + "/" + DATA_FOLDER_NAME) == false)
	{
		bResult = false;
		itkExceptionMacro("Failed to create DATA subfolder in " << strMainFolderPath);
	}

	/*create MASKS subfolder */
	if(mkPath(strMainFolderPath + "/" + MASKS_FOLDER_NAME) == false)
	{
		bResult = false;
		//fail to create destination directory
		itkExceptionMacro("Failed to create MASKS subfolder in " << strMainFolderPath);
	}
	return bResult;
}

bool ProductCreatorAdapter::countSpecialPixels(ByteImageType::Pointer image, uint64_t &cloudCounter, uint64_t &snowCounter, uint64_t &fullCounter){
	image->Update();
	itk::ImageRegionConstIterator<ByteImageType> MyIt(image,image->GetLargestPossibleRegion());
	MyIt.GoToBegin();
	while (!MyIt.IsAtEnd()) {
		fullCounter += 1;
		ByteImageType::PixelType pixel = MyIt.Get();
		if (pixel == CLOUD_INDEX) {
			cloudCounter+=1;
		}
		if (pixel == SNOW_INDEX) {
			snowCounter +=1;
		}
		++MyIt;
	}
	return true;
}

bool ProductCreatorAdapter::copyGIPPFile(const std::string &gippPathIn){
	std::string filename = std::string(DATA_FOLDER_NAME) + "/" + m_strProductDirectoryName + "_" + IPP_DATA_SUFFIX + XML_EXTENSION;
	std::string gippFullPathOut = m_strProductFullPathOut + "/" + filename;
	std::cout << "Copying: " << gippPathIn << " to " << gippFullPathOut << std::endl;
	bool copied = CopyFile(gippFullPathOut, gippPathIn);
	if(copied){
		ImageProperty gipp;
		gipp.Nature = "Ground_Image_Processing_Parameters";
		gipp.Format = "application/xml";
		ImageInformation gippFile;
		gippFile.hasBandNum = false;
		gippFile.hasBitNum = false;
		gippFile.hasDetectorID = false;
		gippFile.isMask = false;
		gippFile.isGroup = true;
		gippFile.bandIDs = "ALL";
		gippFile.path = filename;
		gipp.ImageFiles.emplace_back(gippFile);
		m_productMetadata.ProductOrganisation.DataProperties.emplace_back(gipp);
		return true;
	}
	return false;
}

BandList ProductCreatorAdapter::getBandGlobalList(const std::vector<BandInformation> &missionBands){
	BandList result;
	if(missionBands.empty()){
		itkExceptionMacro("Mission bands not set!");
	}
	result.Count = missionBands.size();
	for(auto band : missionBands){
		result.bandIDs.emplace_back(band.bandID);
	}
	return result;
}

bool ProductCreatorAdapter::generateQuicklook(const std::string &rasterFullFilePath, const std::string &jpegFilename, const std::vector<int> &bandIDs, const int &offset){
	std::string jpegFullFilePath = m_strProductFullPathOut + "/" + jpegFilename;
	std::vector<std::string> args;

	if(bandIDs.size() != 3){
		itkWarningMacro("WARNING: Quicklook does not contain three bands")
	}
	for(size_t i = 0; i < bandIDs.size(); i++){
		args.emplace_back("-b");
		args.emplace_back(std::to_string(bandIDs[i]));
	}
	args.emplace_back("-ot");
	args.emplace_back("Byte");
	args.emplace_back("-of");
	args.emplace_back("JPEG");
	args.emplace_back("-scale");
	args.emplace_back("0");
	args.emplace_back("3000");
	args.emplace_back("0");
	args.emplace_back("255");
	args.emplace_back("-outsize");
	args.emplace_back("1000");
	args.emplace_back("1000");
	args.emplace_back("-q");
	args.emplace_back(rasterFullFilePath);
	args.emplace_back(jpegFullFilePath);

	bool bRet = ExecuteExternalProgram("gdal_translate", args);
	ImageInformation quicklook;
	quicklook.hasBandNum = false;
	quicklook.hasBitNum = false;
	quicklook.hasDetectorID = false;
	quicklook.isGroup = false;
	for(auto bandID : bandIDs){
		quicklook.bandIDs += std::string("B" + std::to_string(bandID - offset) + ",");
	}
	quicklook.isMask = false;
	quicklook.path = jpegFilename;
	m_productMetadata.ProductOrganisation.Quicklook = quicklook; //Attach to main metadata file
	//remove file with extension .jpg.aux.xml generated after preview obtained
	std::string strFileToBeRemoved = jpegFullFilePath + ".aux.xml";
	remove(strFileToBeRemoved.c_str());
	return bRet;
}
