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

#include "MetadataHelper.h"
#include <time.h>
#include <ctime>
#include <cmath>
#include "libgen.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>

using namespace ts;

MetadataHelper::MetadataHelper()
{
    m_detailedAnglesGridSize = 0;
    m_Resolutions = {};
}

MetadataHelper::~MetadataHelper()
{

}

bool MetadataHelper::LoadMetadataFile(const std::string& file)
{
    Reset();
    m_inputMetadataFileName = file;
    m_DirName = getDirname(m_inputMetadataFileName);
    m_basename = getBasename(m_inputMetadataFileName);
    return DoLoadMetadata();
}

void MetadataHelper::Reset()
{
    m_Mission = "";
    m_productLevel = "";

    m_AotFileName = {""};
    m_CloudFileName = {""};
    m_WaterFileName = {""};
    m_SnowFileName = {""};
    m_ImageFileName = {""};

    m_AcquisitionDate = "";

    m_ReflQuantifVal = 1.0;

    m_fAotQuantificationValue = 0.0;
    m_fAotNoDataVal = 0;

    m_solarMeanAngles.azimuth = m_solarMeanAngles.zenith = 0.0;
    m_bHasDetailedAngles = false;
    m_Resolutions = {};
}

std::string MetadataHelper::GetAcquisitionDate(){
	std::string acqDateShort = m_AcquisitionDate.substr(0, m_AcquisitionDate.find("T", 0)); //Cut After "T"
	acqDateShort.erase(std::remove(acqDateShort.begin(), acqDateShort.end(), '-'), acqDateShort.end()); //Remove the two "-" separators

	return acqDateShort;
}

int MetadataHelper::GetAcquisitionDateAsDoy()
{
    struct tm tmDate = {};
    if (strptime(GetAcquisitionDate().c_str(), "%Y%m%d", &tmDate) == NULL) {
        return -1;
    }
    auto curTime = std::mktime(&tmDate);

    std::tm tmYearStart = {};
    tmYearStart.tm_year = tmDate.tm_year;
    tmYearStart.tm_mon = 0;
    tmYearStart.tm_mday = 1;

    auto yearStart = std::mktime(&tmYearStart);
    auto diff = curTime - yearStart;

    return lrintf(diff / 86400 /* 60*60*24*/);
}

MeanAngles_Type MetadataHelper::GetSensorMeanAngles() {
    MeanAngles_Type angles = {0,0};

    if(HasBandMeanAngles()) {
        size_t nBandsCnt = m_sensorBandsMeanAngles.size();
        int nValidAzimuths = 0;
        int nValidZeniths = 0;
        for(size_t i = 0; i < nBandsCnt; i++) {
            MeanAngles_Type bandAngles = m_sensorBandsMeanAngles[i];
            if(!std::isnan(bandAngles.azimuth)) {
                angles.azimuth += bandAngles.azimuth;
                nValidAzimuths++;
            }
            if(!std::isnan(bandAngles.zenith)) {
                angles.zenith += bandAngles.zenith;
                nValidZeniths++;
            }
        }
        if(nValidAzimuths > 0) {
            angles.azimuth = angles.azimuth / nValidAzimuths;
        } else {
            angles.azimuth = 0;
        }
        if(nValidZeniths > 0) {
            angles.zenith = angles.zenith / nValidZeniths;
        } else {
            angles.zenith = 0;
        }
    } else {
        angles = GetSensorMeanAngles(0);
    }

    return angles;
}

MeanAngles_Type MetadataHelper::GetSensorMeanAngles(int nBand) {
    MeanAngles_Type angles = {0,0};
    if(nBand >= 0 && nBand < (int)m_sensorBandsMeanAngles.size()) {
        angles = m_sensorBandsMeanAngles[nBand];
    } else if(m_sensorBandsMeanAngles.size() > 0) {
        angles = m_sensorBandsMeanAngles[0];
    }
    return angles;
}

double MetadataHelper::GetRelativeAzimuthAngle()
{
    MeanAngles_Type solarAngle = GetSolarMeanAngles();
    MeanAngles_Type sensorAngle = GetSensorMeanAngles();

    double relAzimuth = solarAngle.azimuth - sensorAngle.azimuth/* - 180.0 */;
/*
    if (relAzimuth < -180.0) {
        relAzimuth = relAzimuth + 360.0;
    }
    if (relAzimuth > 180.0) {
        relAzimuth = relAzimuth - 360.0;
    }
*/
    return relAzimuth;
}

std::string MetadataHelper::buildFullPath(const std::string& fileName)
{
    boost::filesystem::path p(m_DirName);
    p /= fileName;
    return p.string();
}



