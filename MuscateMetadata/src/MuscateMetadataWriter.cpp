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

#include "MuscateMetadataWriter.hpp"
#include "MetadataUtil.hpp"
#include "string_utils.hpp"

namespace ts {
namespace muscate {

XElement MuscateMetadataWriter::Format(const MuscateMetadataIdentification &meta){
	return XElement("Metadata_Identification",
			XElement("METADATA_FORMAT", XAttribute("version", meta.version), XText(meta.MetadataFormat)),
			XElement("METADATA_PROFILE", meta.MetadataProfile),
			XElement("METADATA_INFORMATION", meta.MetadataInformation));
}

XElement MuscateMetadataWriter::Format(const MuscateDatasetIdentification &meta){
	return XElement("Dataset_Identification",
			XElement("IDENTIFIER", meta.Identifier),
			XElement("AUTHORITY", meta.Authority),
			XElement("PRODUCER", meta.Producer),
			XElement("PROJECT", meta.Project),
			XElement("GEOGRAPHICAL_ZONE", XAttribute("type", meta.GeographicalZoneType), XText(meta.GeographicalZone)),
			XElement("TITLE", meta.Title),
			XElement("DESCRIPTION", meta.Description));
}

XElement MuscateMetadataWriter::Format(const MuscateProductCharacteristics &meta){
	XElement root("Product_Characteristics",
			XElement("PRODUCT_ID", meta.ProductID),
			XElement("ACQUISITION_DATE", meta.AcquisitionDate),
			XElement("PRODUCTION_DATE", meta.ProductionDate),
			XElement("PRODUCT_VERSION", meta.ProductVersion),
			XElement("PRODUCT_LEVEL", meta.ProductLevel),
			XElement("PLATFORM", meta.Platform));
	if(!meta.SpectralContent.empty()){
		root.Append(XElement("SPECTRAL_CONTENT", meta.SpectralContent));
	}
	if(!meta.OrbitNumber.empty() && !meta.OrbitType.empty()){
		root.Append(XElement("ORBIT_NUMBER", XAttribute("type", meta.OrbitType), XText(meta.OrbitNumber)));
	}
	if((!meta.MeanAcquisitionRange.empty() && !meta.DatePrecisionUnit.empty() && !meta.DatePrecision.empty()) ||
			(!meta.SynthesisDate.empty() && !meta.SynthesisPeriodBegin.empty() && !meta.SynthesisPeriodEnd.empty() &&
			!meta.DatePrecision.empty() && !meta.DatePrecisionUnit.empty())){
		XElement acqRange("UTC_Acquisition_Range");
		if(meta.hasSynthesisDate){
			acqRange.Append(XElement("SYNTHESIS_DATE", meta.SynthesisDate));
			acqRange.Append(XElement("SYNTHESIS_PERIOD_BEGIN", meta.SynthesisPeriodBegin));
			acqRange.Append(XElement("SYNTHESIS_PERIOD_END", meta.SynthesisPeriodEnd));
		}else{
			acqRange.Append(XElement("MEAN", XText(meta.MeanAcquisitionRange)));
		}
		acqRange.Append(XElement("DATE_PRECISION", XAttribute("unit", meta.DatePrecisionUnit), XText(meta.DatePrecision)));
		root.Append(acqRange);
	}

	XElement bandGlobalList("Band_Global_List", XAttribute("count", std::to_string(meta.BandGlobalList.Count)));
	for(size_t i = 0; i < meta.BandGlobalList.Count; i++){
		bandGlobalList.Append(XElement("BAND_ID", meta.BandGlobalList.bandIDs[i]));
	}
	root.Append(bandGlobalList);

	XElement bandGroupList("Band_Group_List");
	for(size_t i = 0; i < meta.BandGroupList.size(); i++){
		XElement group("Group", XAttribute("group_id", meta.BandGroupList[i].GroupID));
		XElement bandList("Band_List", XAttribute("count", std::to_string(meta.BandGroupList[i].GroupList.Count)));
		for(auto it = meta.BandGroupList[i].GroupList.bandIDs.begin(); it != meta.BandGroupList[i].GroupList.bandIDs.end(); ++it){
			bandList.Append(XElement("BAND_ID", *it));
		}
		group.Append(bandList);
		bandGroupList.Append(group);
	}
	root.Append(bandGroupList);

	return root;
}

/**
 * @brief creates the Image_list or Mask_List tree, containing information for all images/masks
 * @param vector The vector containing ImageProperties metadata to be written
 * @param specifier Can be "Image", "Mask" or "Data" depending on which section is to be written
 */
static XElement createImageElement(const std::vector<ImageProperty> &meta, std::string specifier){
	XElement root(std::string(specifier + "_List"));
	for(auto it = meta.begin(); it != meta.end(); ++it){
		XElement el(specifier);
		XElement imageProperties(std::string(specifier + "_Properties"),
				XElement("NATURE", it->Nature),
				XElement("FORMAT", it->Format));
		if(specifier == "Image" || specifier == "Mask"){
			imageProperties.Append(XElement("ENCODING", it->Encoding));
			imageProperties.Append(XElement("ENDIANNESS", it->Endianness));
			if(it->hasCompression){
				imageProperties.Append(XElement("COMPRESSION", it->Compression));
			}
			if(it->Description.empty() == false){
				imageProperties.Append(XElement("DESCRIPTION", it->Description));
			}
		}
		XElement imageFileList(std::string(specifier + "_File_List"));
		for(auto jt = it->ImageFiles.begin(); jt != it->ImageFiles.end(); ++jt){
			XElement file(std::string(toUppercase(specifier) + "_FILE"));
			if(jt->isGroup){
				file.Append(XAttribute("group_id", jt->bandIDs));
			}else{
				file.Append(XAttribute("band_id", jt->bandIDs));
			}
			if(jt->hasBandNum){
				file.Append(XAttribute("band_number", jt->bandNum));
			}
			if(jt->hasDetectorID){
				file.Append(XAttribute("detector_id", jt->detectorID));
			}
			if(jt->hasBitNum){
				file.Append(XAttribute("bit_number", jt->bitNumber));
			}
			file.Append(XText(jt->path));
			imageFileList.Append(file);
		}
		el.Append(imageProperties);
		el.Append(imageFileList);
		root.Append(el);
	}

	return root;
}

XElement MuscateMetadataWriter::Format(const MuscateProductOrganisation &meta){
	XElement root("Product_Organisation");
	XElement muscate("Muscate_Product");
	if(meta.Quicklook.bandIDs.length() && meta.Quicklook.path.length()){
		muscate.Append(XElement("QUICKLOOK", XAttribute("bands_id", meta.Quicklook.bandIDs), XText(meta.Quicklook.path)));
	}
	if(meta.ImageProperties.size() > 0){
		muscate.Append(createImageElement(meta.ImageProperties, "Image"));
	}
	if(meta.MaskProperties.size() > 0){
		muscate.Append(createImageElement(meta.MaskProperties, "Mask"));
	}
	if(meta.DataProperties.size() > 0){
		muscate.Append(createImageElement(meta.DataProperties, "Data"));
	}

	root.Append(muscate);

	return root;
}

XElement MuscateMetadataWriter::Format(const MuscateGeometricInformations &meta){
	XElement root("Geometric_Informations", XText(" "));

	// intentionally empty - Not needed for L3 products
	(void) (meta);

	return root;
}

XElement MuscateMetadataWriter::Format(const MuscateGeopositionInformations &meta){
	XElement root("Geoposition_Informations",
			XElement("Coordinate_Reference_System",
					XElement("GEO_TABLES", XText(meta.GeoTables)),
					XElement("Horizontal_Coordinate_System",
							XElement("HORIZONTAL_CS_TYPE", XText(meta.HorizontalCSType)),
							XElement("HORIZONTAL_CS_NAME", XText(meta.HorizontalCSName)),
							XElement("HORIZONTAL_CS_CODE", XText(meta.HorizontalCSCode))
					)));
	root.Append(XElement("Raster_CS",
			XElement("RASTER_CS_TYPE", XText(meta.RasterCS.CSType)),
			XElement("PIXEL_ORIGIN", XText(meta.RasterCS.PixelOrigin))));
	root.Append(XElement("Metadata_CS",
			XElement("METADATA_CS_TYPE", XText(meta.MetadataCS.CSType)),
			XElement("PIXEL_ORIGIN", XText(meta.MetadataCS.PixelOrigin))));

	XElement geopositioning("Geopositioning");
	XElement globalGeopositioning("Global_Geopositioning");
	for(auto it = meta.GlobalGeoposition.begin(); it != meta.GlobalGeoposition.end(); ++it){
		XElement point("Point", XAttribute("name", it->name),
				XElement("LAT", it->lat),
				XElement("LON", it->lon),
				XElement("X", it->Xpos),
				XElement("Y", it->Ypos));
		globalGeopositioning.Append(point);
	}
	geopositioning.Append(globalGeopositioning);

	XElement groupGeopositioning("Group_Geopositioning_List");
	for(auto it = meta.GroupPosition.begin(); it != meta.GroupPosition.end(); ++it){
		XElement group("Group_Geopositioning", XAttribute("group_id", it->groupID),
				XElement("ULX", it->ULX),
				XElement("ULY", it->ULY),
				XElement("XDIM", it->XDim),
				XElement("YDIM", it->YDim),
				XElement("NROWS", it->nrows),
				XElement("NCOLS", it->ncols));
		groupGeopositioning.Append(group);
	}
	geopositioning.Append(groupGeopositioning);
	root.Append(geopositioning);

	return root;
}

XElement MuscateMetadataWriter::Format(const MuscateRadiometricInformations &meta){
	XElement root("Radiometric_Informations");
	for(auto it = meta.quantificationValues.begin(); it != meta.quantificationValues.end(); ++it){
		root.Append(XElement(it->name, XText(it->value)));
	}
	XElement specialValuesList("Special_Values_List");
	for(auto it = meta.specialValues.begin(); it != meta.specialValues.end(); ++it){
		specialValuesList.Append(XElement("SPECIAL_VALUE", XAttribute("name", it->name), XText(it->value)));
	}
	root.Append(specialValuesList);
	XElement spectralBandInformationsList("Spectral_Band_Informations_List");
	for(auto band : meta.Bands){
		XElement spectralBandInfo("Spectral_Band_Informations",
				XAttribute("band_id", band.bandID),
				XElement("SPATIAL_RESOLUTION",
						XAttribute("unit", band.SpatialResolutionUnit),
						XText(band.SpatialResolution)));
		spectralBandInformationsList.Append(spectralBandInfo);
	}
	root.Append(spectralBandInformationsList);
	return root;
}

static XElement appendQualityProduct(const MuscateQualityProduct &meta, bool bIsCurrentProduct){
	XElement root(bIsCurrentProduct? "Current_Product" : "Contributing_Product");
	XElement productQualityList("Product_Quality_List", XAttribute("level", meta.Level));
	if(!bIsCurrentProduct){
		XElement product("Product",
				XElement("PRODUCT_ID", meta.ProductID),
				XElement("ACQUISITION_DATE", meta.AcquisitionDate),
				XElement("PRODUCTION_DATE", meta.ProductionDate)
		);
		root.Append(product);
	}
	XElement productQuality("Product_Quality");
	productQuality.Append(XElement("Source_Product",
			XElement("PRODUCT_ID", meta.ProductID),
			XElement("ACQUISITION_DATE", meta.AcquisitionDate),
			XElement("PRODUCTION_DATE", meta.ProductionDate)
	));
	XElement globalIndexList("Global_Index_List");
	for(auto it = meta.GlobalIndexList.begin(); it != meta.GlobalIndexList.end(); ++it){
		globalIndexList.Append(XElement("QUALITY_INDEX", XAttribute("name", it->name), XText(it->value)));
	}
	productQuality.Append(globalIndexList);
	productQualityList.Append(productQuality);
	root.Append(productQualityList);

	return root;
}

XElement MuscateMetadataWriter::Format(const MuscateQualityInformations &meta){
	XElement root("Quality_Informations");
	root.Append(appendQualityProduct(meta.CurrentProduct, true));
	XElement contributingProductsList("Contributing_Products_List");
	for(auto it = meta.ContributingProducts.begin(); it != meta.ContributingProducts.end(); ++it){
		contributingProductsList.Append(appendQualityProduct(*it, false));
	}
	root.Append(contributingProductsList);
	return root;
}

XElement MuscateMetadataWriter::Format(const MuscateProductionInformations &meta){
	XElement root( "Production_Informations", XText(std::string(" ")));

	// intentionally empty
	(void) (meta);

	return root;
}

XDocument MuscateMetadataWriter::CreateMetadataXml(const MuscateFileMetadata &metadata)
{
	XDocument doc(XDeclaration("1.0", "utf-8", ""));

	XElement root("Muscate_Metadata_Document",
			XAttribute("xmlns:xsi", metadata.Header.xsi),
			XAttribute("xsi:noNamespaceSchemaLocation", metadata.Header.SchemaLocation),
			Format(metadata.MetadataIdentification),
			Format(metadata.DatasetIdentification),
			Format(metadata.ProductCharacteristics),
			Format(metadata.ProductOrganisation),
			Format(metadata.GeopositionInformations),
			Format(metadata.GeometricInformations),
			Format(metadata.RadiometricInformations),
			Format(metadata.QualityInformations),
			Format(metadata.ProductionInformations));
	doc.Append(root);
 	return doc;
}

void MuscateMetadataWriter::WriteMetadata(const MuscateFileMetadata &metadata, const std::string &path)
{
	CreateMetadataXml(metadata).Save(path);
}
} /* namespace muscate */
} /* namespace ts */
