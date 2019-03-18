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

#pragma once

#include <string>
#include <vector>
#include <iostream>

struct MuscateHeaderMetadata
{
	std::string xsi = "";
	std::string SchemaLocation = "";
};

struct MuscateMetadataIdentification{
	std::string MetadataFormat;
	std::string version;
	std::string MetadataProfile;
	std::string MetadataInformation;
};

struct MuscateDatasetIdentification{
	std::string Identifier;
	std::string Authority;
	std::string Producer;
	std::string Project;
	std::string GeographicalZone;
	std::string GeographicalZoneType;
	std::string Title;
	std::string Description;
};

struct BandList{
	size_t Count;
	std::vector<std::string> bandIDs;
};

struct BandGroup{
	std::string GroupID;
	BandList GroupList;
};

struct MuscateProductCharacteristics{
	std::string ProductID;
	std::string AcquisitionDate;
	std::string ProductionDate;
	std::string ProductVersion;
	std::string ProductLevel;
	std::string Platform;
	std::string SpectralContent;
	std::string OrbitNumber;
	std::string OrbitType;
	std::string MeanAcquisitionRange;
	bool hasSynthesisDate;
	std::string SynthesisPeriodBegin;
	std::string SynthesisPeriodEnd;
	std::string SynthesisDate;
	std::string DatePrecision;
	std::string DatePrecisionUnit;
	BandList BandGlobalList;
	std::vector<BandGroup> BandGroupList;
};

struct ImageInformation{
	std::string path;
	bool isMask;
	std::string bandIDs;
	bool isGroup;
	std::string bandNum;
	bool hasBandNum;
	std::string bitNumber;
	bool hasBitNum;
	std::string detectorID;
	bool hasDetectorID;
};

struct ImageProperty{
	std::string Nature;
	std::string Format;
	std::string Encoding;
	std::string Endianness;
	std::string Compression;
	std::string Description;
	bool hasCompression;
	std::vector<ImageInformation> ImageFiles;
};

struct MuscateProductOrganisation{
	ImageInformation Quicklook;
	std::vector<ImageProperty> ImageProperties;
	std::vector<ImageProperty> MaskProperties;
	std::vector<ImageProperty> DataProperties;
};

struct CSInfo{
	std::string CSType;
	std::string PixelOrigin;
};

struct GeoPoint{
	std::string name;
	std::string lat;
	std::string lon;
	std::string Xpos;
	std::string Ypos;
};

struct GroupPositioning{
	std::string groupID;
	std::string ULX;
	std::string ULY;
	std::string XDim;
	std::string YDim;
	std::string nrows;
	std::string ncols;
};

struct MuscateGeopositionInformations{
	std::string GeoTables;
	std::string HorizontalCSType;
	std::string HorizontalCSName;
	std::string HorizontalCSCode;
	CSInfo RasterCS;
	CSInfo MetadataCS;
	std::vector<GeoPoint> GlobalGeoposition;
	std::vector<GroupPositioning> GroupPosition;
};


struct MuscateAngleList
{
	std::string ColumnUnit;
	std::string ColumnStep;
	std::string RowUnit;
	std::string RowStep;
	std::vector<std::vector<double> > Values;
};

struct MuscateAngles
{
	MuscateAngleList Zenith;
	MuscateAngleList Azimuth;
};

struct MuscateViewingAnglesGrid{
	std::string bandID;
	std::string detectorID;
	MuscateAngles ViewIncidenceAnglesGrid;
};


struct MuscateAnglePair
{
	std::string ZenithUnit;
	std::string AzimuthUnit;
	double ZenithValue;
	double AzimuthValue;
	std::string bandId;
	std::string detectorId;
};

struct MuscateBandViewingAnglesGrid
{
    std::string BandId;
    MuscateAngles Angles;
};

struct MuscateGeometricInformations{
	MuscateAnglePair MeanSunAngle;
	MuscateAngles SolarAngles;
	std::vector<MuscateAnglePair> MeanViewingIncidenceAngles;
	std::vector<MuscateViewingAnglesGrid> ViewingAngles;
};

struct SpecialValue{
	std::string name;
	std::string value;
	SpecialValue(): name(""), value("1") {};
	SpecialValue(std::string _name, std::string _value):name(_name), value(_value) {};
};

struct BandInformation{
	std::string bandID;
	std::string PhysicalGain;
	std::string LuminanceMax;
	std::string LuminanceMin;
	std::string QuantizeCalMax;
	std::string QuantizeCalMin;
	std::string RadianceAdd;
	std::string RadianceMult;
	std::string ReflectanceAdd;
	std::string ReflectanceMult;
	std::string SolarIrradiance;
	std::string SpatialResolution;
	std::string SpatialResolutionUnit;
	std::string WavelengthMin;
	std::string WavelengthMax;
	std::string WavelengthCentral;
	std::string SpectralResponseStep;
	std::string SpectralResponseValues;

	BandInformation(std::string _ID, std::string _Res, std::string _ResUnit):
		bandID(_ID),
		SpatialResolution(_Res),
		SpatialResolutionUnit(_ResUnit)
	{}

	BandInformation(): bandID(""), SpatialResolution(""), SpatialResolutionUnit("") {}
};


struct MuscateRadiometricInformations{
	std::vector<SpecialValue> quantificationValues;
	std::vector<SpecialValue> specialValues;
	std::vector<BandInformation> Bands;
};

struct MuscateQualityIndex{
	std::string name;
	std::string value;

	MuscateQualityIndex(std::string _name, std::string _value): name(_name), value(_value) {}
};

struct MuscateQualityProduct{
	std::string Level;
	std::string ProductID;
	std::string AcquisitionDate;
	std::string ProductionDate;
	std::vector<MuscateQualityIndex> GlobalIndexList;
};

struct MuscateQualityInformations{
	MuscateQualityProduct CurrentProduct;
	std::vector<MuscateQualityProduct> ContributingProducts;
};

struct MuscateProductionInformations{
	//intentionally empty;
};

struct MuscateFileMetadata
{
	MuscateHeaderMetadata Header;
	MuscateMetadataIdentification MetadataIdentification;
	MuscateDatasetIdentification DatasetIdentification;
	MuscateProductCharacteristics ProductCharacteristics;
	MuscateProductOrganisation ProductOrganisation;
	MuscateGeopositionInformations GeopositionInformations;
	MuscateGeometricInformations GeometricInformations;
	MuscateRadiometricInformations RadiometricInformations;
	MuscateQualityInformations QualityInformations;
	MuscateProductionInformations ProductionInformations;
	std::string ProductPath;
};
