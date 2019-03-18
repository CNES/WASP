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

#ifndef UPDATESYNTHESISFUNCTOR_H
#define UPDATESYNTHESISFUNCTOR_H

#include <vector>
#include "GlobalDefs.h"

#define WEIGHT_QUANTIF_VALUE    1000

#define DEFAULT_COMPOSITION_QUANTIF_VALUE   10000

#define DATE_NO_DATA            NO_DATA_VALUE
#define WEIGHT_NO_DATA          (NO_DATA_VALUE/WEIGHT_QUANTIF_VALUE)      //  NO_DATA / WEIGHT_QUANTIF_VALUE
#define CLOUD_INDEX				1

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Namespace around Functors to be used in the filters defined below
 */
namespace Functor
{

/**
 * @brief Container to store all current pixel information: Reflectance, Weight, Flag and Date
 */
class OutFunctorInfos
{
public:
    OutFunctorInfos(int nSize) {
        m_CurrentWeightedReflectances.resize(nSize);
        m_CurrentPixelWeights.resize(nSize);
        m_nCurrentPixelFlag.resize(nSize);
        m_nCurrentPixelWeightedDate.resize(nSize);
    }

    std::vector<float> m_CurrentWeightedReflectances;
    std::vector<float> m_CurrentPixelWeights;
    std::vector<short> m_nCurrentPixelFlag;
    std::vector<short> m_nCurrentPixelWeightedDate;
    //short m_nCurrentPixelFlag;
    //float m_fCurrentPixelWeightedDate;
} ;

/**
 * @brief Functor to perform the UpdateSynthesis
 */
template< class TInput, class TOutput>
class UpdateSynthesisFunctor
{
public:
    UpdateSynthesisFunctor();
    UpdateSynthesisFunctor& operator =(const UpdateSynthesisFunctor& copy);
    bool operator!=( const UpdateSynthesisFunctor & other) const;
    bool operator==( const UpdateSynthesisFunctor & other ) const;
    TOutput operator()( const TInput & A );
    void Initialize(const std::vector<int> presenceVect, int nExtractedL2ABandsNo, int nBlueBandIdx,
                    bool bHasAppendedPrevL2ABlueBand, bool bPrevL3ABandsAvailable,
                    int nDate, float fReflQuantifVal);
    void printDebugInfo();
    int GetNbOfL3AReflectanceBands() { return m_nNbOfL3AReflectanceBands; }

    /**
     * @brief Get the number of bands in the Output image
     * @return The band number, which is the original naumber of inputs and the three masks WGT, DTS, FLG
     */
    int GetNbOfOutputComponents() { return m_nNbOfL3AReflectanceBands + 3;}

    const char * GetNameOfClass() { return "UpdateSynthesisFunctor"; }

private:
    void ResetCurrentPixelValues(const TInput & A, OutFunctorInfos& outInfos);
    int GetAbsoluteL2ABandIndex(int index);
    float GetL2AReflectanceForPixelVal(float fPixelVal);
    void HandleLandPixel(const TInput & A, OutFunctorInfos& outInfos);
    void HandleSnowOrWaterPixel(const TInput & A, OutFunctorInfos& outInfos);
    void HandleCloudOrShadowPixel(const TInput & A, OutFunctorInfos& outInfos);
    bool IsSnowPixel(const TInput & A);
    bool IsWaterPixel(const TInput & A);
    bool IsCloudPixel(const TInput & A);
    bool IsLandPixel(const TInput & A);
    float GetCurrentL2AWeightValue(const TInput & A);
    float GetPrevL3AWeightValue(const TInput & A, int offset);
    short GetPrevL3AWeightedAvDateValue(const TInput & A, int offset);
    float GetPrevL3AReflectanceValue(const TInput & A, int offset);
    short GetPrevL3APixelFlagValue(const TInput & A, int offset);
    int GetBlueBandIndex();
    bool IsNoDataValue(float fValue, float fNoDataValue);

private:
    float m_fReflQuantifValue;
    int m_nCurrentDate;
    int m_nNbL2ABands;

    bool m_bPrevL3ABandsAvailable;
    int m_nNbOfL3AReflectanceBands;


    int m_nL2ABandStartIndex;
    int m_nCloudMaskBandIndex;
    int m_nSnowMaskBandIndex;
    int m_nWaterMaskBandIndex;
    int m_nCurrentL2AWeightBandIndex;
    int m_nPrevL3AWeightBandStartIndex;
    int m_nPrevL3AWeightedAvDateBandIndex;
    int m_nPrevL3AReflectanceBandStartIndex;
    int m_nPrevL3APixelFlagBandIndex;
    int m_nL2ABlueBandIndex;
    int m_nL3ABlueBandIndex;

    bool m_bHasAppendedPrevL2ABlueBand;

    std::vector<int> m_arrL2ABandPresence;

};
} //namespace Functor
} //namespace ts

#include "../src/UpdateSynthesisFunctor.txx"

#endif // UPDATESYNTHESISFUNCTOR_H
