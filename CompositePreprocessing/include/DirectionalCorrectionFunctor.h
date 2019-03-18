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

#ifndef DIRECTIONALCORRECTIONFUNCTOR_H
#define DIRECTIONALCORRECTIONFUNCTOR_H

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
 * @brief Contains scattering coefficients used for the directional correction
 */
class ScatteringFunctionCoefficients {
public:
    float V0;
    float V1;
    float R0;
    float R1;
};

/**
 * @brief Functor to perform the directional correction
 */
template< class TInput, class TOutput>
class DirectionalCorrectionFunctor
{
public:
    DirectionalCorrectionFunctor();
    DirectionalCorrectionFunctor& operator =(const DirectionalCorrectionFunctor& copy);
    bool operator!=( const DirectionalCorrectionFunctor & other) const;
    bool operator==( const DirectionalCorrectionFunctor & other ) const;
    TOutput operator()( const TInput & A );
    void Initialize(const std::vector<ScatteringFunctionCoefficients> &coeffs);

    const char * GetNameOfClass() { return "DirectionalCorrectionFunctor"; }

    bool IsSnowPixel(const TInput & A);
    bool IsWaterPixel(const TInput & A);
    bool IsCloudPixel(const TInput & A);
    bool IsLandPixel(const TInput & A);
    float GetCurrentL2AWeightValue(const TInput & A);
    bool IsNoDataValue(float fValue, float fNoDataValue);

private:
    std::vector<ScatteringFunctionCoefficients> m_ScatteringCoeffs;

    int m_nReflBandsCount;

    int m_nCloudMaskBandIndex;
    int m_nSnowMaskBandIndex;
    int m_nWaterMaskBandIndex;
    int m_nNdviBandIdx;
    int m_nSunAnglesBandStartIdx;
    int m_nSensoAnglesBandStartIdx;

    float m_fReflNoDataValue;

};
} //namespace Functor
} //namespace ts

#include "../src/DirectionalCorrectionFunctor.txx"

#endif // DIRECTIONALCORRECTIONFUNCTOR_H
