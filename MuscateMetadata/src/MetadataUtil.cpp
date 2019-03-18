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

#include "MetadataUtil.hpp"
#include "MuscateMetadataReader.hpp"

std::string toUpperCase(const std::string & s)
{
	std::string ret(s.size(), char());
	for(unsigned int i = 0; i < s.size(); ++i)
		ret[i] = (s[i] <= 'z' && s[i] >= 'a') ? s[i]-('a'-'A') : s[i];
	return ret;
}

bool compareImageProperties(const bool &a,const bool &b, const std::string &as, const std::string &bs) {
	if(a != b) return false;
	if(a && b){
		if(as != bs) return false;
	}
	return true;
}

std::string MuscateAnglesToString(const MuscateAngles &angles){
	std::string ret;
	ret = "AZ: : " + angles.Azimuth.ColumnStep + " " + angles.Azimuth.ColumnUnit + "\n";
	for(auto line : angles.Azimuth.Values){
		for(auto angle : line){
			ret += std::to_string(angle) + " ";
		}
		ret += "\n";
	}
	ret += "\nZE: " + angles.Zenith.ColumnStep + " " + angles.Zenith.ColumnUnit + "\n";
	for(auto line : angles.Zenith.Values){
		for(auto angle : line){
			ret += std::to_string(angle) + " ";
		}
		ret += "\n";
	}
	return ret;
}

bool operator==(MuscateHeaderMetadata const& lhs, MuscateHeaderMetadata const& rhs) {
	if(lhs.xsi != rhs.xsi) return false;
	if(lhs.SchemaLocation != rhs.SchemaLocation) return false;
	return true;
}

bool operator==(MuscateMetadataIdentification const& lhs, MuscateMetadataIdentification const& rhs) {
	if(lhs.MetadataFormat != rhs.MetadataFormat) return false;
	if(lhs.version != rhs.version) return false;
	if(lhs.MetadataProfile != rhs.MetadataProfile) return false;
	if(lhs.MetadataInformation != rhs.MetadataInformation) return false;
	return true;
}

bool operator==(MuscateDatasetIdentification const& lhs, MuscateDatasetIdentification const& rhs) {
	if(lhs.Identifier != rhs.Identifier) return false;
	if(lhs.Authority != rhs.Authority) return false;
	if(lhs.Producer != rhs.Producer) return false;
	if(lhs.Project != rhs.Project) return false;
	if(lhs.GeographicalZone != rhs.GeographicalZone) return false;
	if(lhs.GeographicalZoneType != rhs.GeographicalZoneType) return false;
	if(lhs.Title != rhs.Title) return false;
	if(lhs.Description != rhs.Description) return false;
	return true;
}

bool operator!=(BandList const& lhs, BandList const& rhs) {
	if(lhs.Count != rhs.Count) return true;
	if(lhs.bandIDs != rhs.bandIDs) return true;
	return false;
}

bool operator!=(BandGroup const& lhs, BandGroup const& rhs) {
	if(lhs.GroupID != rhs.GroupID) return true;
	if(lhs.GroupList != rhs.GroupList) return true;
	return false;
}

bool operator==(BandGroup const& lhs, BandGroup const& rhs) {
	if(lhs.GroupID != rhs.GroupID) return false;
	if(lhs.GroupList != rhs.GroupList) return false;
	return true;
}

bool operator==(MuscateProductCharacteristics const& lhs, MuscateProductCharacteristics const& rhs) {
	if(lhs.ProductID != rhs.ProductID) return false;
	if(lhs.AcquisitionDate != rhs.AcquisitionDate) return false;
	if(lhs.ProductionDate != rhs.ProductionDate) return false;
	if(lhs.ProductVersion != rhs.ProductVersion) return false;
	if(lhs.ProductLevel != rhs.ProductLevel) return false;
	if(lhs.Platform != rhs.Platform) return false;
	if(lhs.SpectralContent != rhs.SpectralContent) return false;
	if(lhs.OrbitNumber != rhs.OrbitNumber) return false;
	if(lhs.OrbitType != rhs.OrbitType) return false;
	if(lhs.hasSynthesisDate == rhs.hasSynthesisDate){
		if(lhs.hasSynthesisDate == true){
			std::cout << lhs.SynthesisDate << " " << rhs.SynthesisDate << "\n" <<
					lhs.SynthesisPeriodBegin << " " << rhs.SynthesisPeriodBegin << "\n" <<
					lhs.SynthesisPeriodEnd << " " << rhs.SynthesisPeriodEnd << std::endl;
			if(lhs.SynthesisDate != rhs.SynthesisDate) return false;
			if(lhs.SynthesisPeriodBegin != rhs.SynthesisPeriodBegin) return false;
			if(lhs.SynthesisPeriodEnd != rhs.SynthesisPeriodEnd) return false;
		}else{
			if(lhs.MeanAcquisitionRange != rhs.MeanAcquisitionRange) return false;
		}
	}else{
		return false;
	}
	if(lhs.DatePrecision != rhs.DatePrecision) return false;
	if(lhs.DatePrecisionUnit != rhs.DatePrecisionUnit) return false;
	if(lhs.BandGlobalList != rhs.BandGlobalList) return false;
	if(lhs.BandGroupList != rhs.BandGroupList) return false;

	return true;
}

bool operator==(ImageInformation const& lhs, ImageInformation const& rhs) {
	if(lhs.path != rhs.path) return false;
	if(lhs.isMask != rhs.isMask) return false;
	if(lhs.bandIDs != rhs.bandIDs) return false;
	if(lhs.isGroup != rhs.isGroup) return false;
	if(compareImageProperties(lhs.hasBandNum,rhs.hasBandNum, lhs.bandNum, rhs.bandNum) == false) return false;
	if(compareImageProperties(lhs.hasBitNum,rhs.hasBitNum, lhs.bitNumber, rhs.bitNumber) == false) return false;
	if(compareImageProperties(lhs.hasDetectorID,rhs.hasDetectorID, lhs.detectorID, rhs.detectorID) == false) return false;
	return true;
}

bool operator!=(ImageInformation const& lhs, ImageInformation const& rhs) {
	if(lhs.path != rhs.path) return true;
	if(lhs.isMask != rhs.isMask) return true;
	if(lhs.bandIDs != rhs.bandIDs) return true;
	if(lhs.isGroup != rhs.isGroup) return true;
	if(compareImageProperties(lhs.hasBandNum,rhs.hasBandNum, lhs.bandNum, rhs.bandNum) == false) return true;
	if(compareImageProperties(lhs.hasBitNum,rhs.hasBitNum, lhs.bitNumber, rhs.bitNumber) == false) return true;
	if(compareImageProperties(lhs.hasDetectorID,rhs.hasDetectorID, lhs.detectorID, rhs.detectorID) == false) return true;
	return false;
}

bool operator==(ImageProperty const& lhs, ImageProperty const& rhs) {
	if(lhs.Nature != rhs.Nature) return false;
	if(lhs.Format != rhs.Format) return false;
	if(lhs.Encoding != rhs.Encoding) return false;
	if(lhs.Endianness != rhs.Endianness) return false;
	if(compareImageProperties(lhs.hasCompression,rhs.hasCompression, lhs.Compression, rhs.Compression) == false) return false;
	if(lhs.ImageFiles != rhs.ImageFiles) return false;
	return true;
}

bool operator==(MuscateProductOrganisation const& lhs, MuscateProductOrganisation const& rhs) {
	if(lhs.Quicklook != rhs.Quicklook) return false;
	if(lhs.ImageProperties != rhs.ImageProperties) return false;
	if(lhs.MaskProperties != rhs.MaskProperties) return false;
	if(lhs.DataProperties != rhs.DataProperties) return false;
	return true;
}

bool operator!=(CSInfo const& lhs, CSInfo const& rhs) {
	if(lhs.CSType != rhs.CSType) return true;
	if(lhs.PixelOrigin != rhs.PixelOrigin) return true;
	return false;
}

bool operator==(GeoPoint const& lhs, GeoPoint const& rhs) {
	if(lhs.name != rhs.name) return false;
	if(lhs.lat != rhs.lat) return false;
	if(lhs.lon != rhs.lon) return false;
	if(lhs.Xpos != rhs.Xpos) return false;
	if(lhs.Ypos != rhs.Ypos) return false;
	return true;
}

bool operator==(GroupPositioning const& lhs, GroupPositioning const& rhs) {
	if(lhs.groupID != rhs.groupID) return false;
	if(lhs.ULX != rhs.ULX) return false;
	if(lhs.ULY != rhs.ULY) return false;
	if(lhs.XDim != rhs.XDim) return false;
	if(lhs.YDim != rhs.YDim) return false;
	if(lhs.nrows != rhs.nrows) return false;
	if(lhs.ncols != rhs.ncols) return false;
	return true;
}

bool operator==(MuscateGeopositionInformations const& lhs, MuscateGeopositionInformations const& rhs) {
	if(lhs.GeoTables != rhs.GeoTables) return false;
	if(lhs.HorizontalCSType != rhs.HorizontalCSType) return false;
	if(lhs.HorizontalCSName != rhs.HorizontalCSName) return false;
	if(lhs.HorizontalCSCode != rhs.HorizontalCSCode) return false;
	if(lhs.RasterCS != rhs.RasterCS) return false;
	if(lhs.MetadataCS != rhs.MetadataCS) return false;
	if(lhs.GlobalGeoposition != rhs.GlobalGeoposition) return false;
	if(lhs.GroupPosition != rhs.GroupPosition) return false;
	return true;
}

bool operator==(SpecialValue const& lhs, SpecialValue const& rhs) {
	if(lhs.name != rhs.name) return false;
	if(lhs.value != rhs.value) return false;
	return true;
}

bool operator==(BandInformation const& lhs, BandInformation const& rhs){
	if(lhs.bandID != rhs.bandID) return false;
	if(lhs.SpatialResolutionUnit != rhs.SpatialResolutionUnit) return false;
	if(lhs.SpatialResolution != rhs.SpatialResolution) return false;
	//TODO: Add other fields here
	return true;
}

bool operator==(MuscateRadiometricInformations const& lhs, MuscateRadiometricInformations const& rhs) {
	if(lhs.quantificationValues != rhs.quantificationValues) return false;
	if(lhs.specialValues != rhs.specialValues) return false;
	if(lhs.Bands != rhs.Bands) return false;
	return true;
}

bool operator==(MuscateQualityIndex const& lhs, MuscateQualityIndex const& rhs) {
	if(lhs.name != rhs.name) return false;
	if(lhs.value != rhs.value) return false;
	return true;
}

bool operator==(MuscateQualityProduct const& lhs, MuscateQualityProduct const& rhs) {
	if(lhs.Level != rhs.Level) return false;
	if(lhs.ProductID != rhs.ProductID) return false;
	if(lhs.AcquisitionDate != rhs.AcquisitionDate) return false;
	if(lhs.ProductionDate != rhs.ProductionDate) return false;
	if(lhs.GlobalIndexList != rhs.GlobalIndexList) return false;
	return true;
}

bool operator==(MuscateQualityInformations const& lhs, MuscateQualityInformations const& rhs) {
	if(lhs.CurrentProduct == rhs.CurrentProduct){
		if(lhs.ContributingProducts != rhs.ContributingProducts) return false;
		return true;
	}
	return false;
}
