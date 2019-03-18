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

#ifndef CREATE_S2_ANGLES_RASTER_H
#define CREATE_S2_ANGLES_RASTER_H

#include "itkLightObject.h"
#include "ViewingAngles.hpp"

#include "ImageResampler.h"
#include "BaseImageTypes.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Creates the Sentinel-2 angle rasters to perform the directional correction
 */
class CreateS2AnglesRaster: public BaseImageTypes {
public:
	/**
	 * @brief Constructor
	 */
    CreateS2AnglesRaster();

    /**
     * @brief Init the angles raster creation
     * @param res Current resolution
     * @param xml Metadata-file
     */
    void DoInit(int res, const std::string &xml);

    /**
	 * @brief Execute the application
     * @return FloatVectorImage containing the angle rasters for S2
     */
    FloatVectorImageType::Pointer DoExecute();

	/**
	 * @brief Return the name of the class
	 * @return
	 */
    const char * GetNameOfClass(){ return "CreateS2AnglesRaster";};
private:
    FloatVectorImageType::Pointer								m_AnglesRaster;
    std::string													m_inXml;
    size_t														m_resolutionIndex;
    ImageResampler<FloatVectorImageType, FloatVectorImageType>	m_ResampledBandsExtractor;
};

} //namespace ts

#endif // CREATE_S2_ANGLES_RASTER_H
