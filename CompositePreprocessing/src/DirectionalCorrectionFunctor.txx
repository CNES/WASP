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

#include "DirectionalCorrectionFunctor.h"
#include "DirectionalModel.h"
#include "GlobalDefs.h"

#ifdef __GNUC__ 
#  if __GNUC_PREREQ(5,4) 
using namespace std; 
#  endif 
#else 
#endif 

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Namespace around Functors to be used in the filters defined below
 */
namespace Functor {

template< class TInput, class TOutput>
DirectionalCorrectionFunctor<TInput,TOutput>::DirectionalCorrectionFunctor() {
    m_nReflBandsCount = 0;
}

template< class TInput, class TOutput>
DirectionalCorrectionFunctor<TInput,TOutput>& DirectionalCorrectionFunctor<TInput,TOutput>::operator =(const DirectionalCorrectionFunctor& copy) {
    this->m_ScatteringCoeffs = copy.m_ScatteringCoeffs;
    m_nReflBandsCount = copy.m_nReflBandsCount;

    m_nCloudMaskBandIndex = copy.m_nCloudMaskBandIndex;
    m_nSnowMaskBandIndex = copy.m_nSnowMaskBandIndex;
    m_nWaterMaskBandIndex = copy.m_nWaterMaskBandIndex;
    m_nNdviBandIdx = copy.m_nNdviBandIdx;
    m_nSunAnglesBandStartIdx = copy.m_nSunAnglesBandStartIdx;
    m_nSensoAnglesBandStartIdx = copy.m_nSensoAnglesBandStartIdx;

    m_fReflNoDataValue = copy.m_fReflNoDataValue;

    return *this;
}

template< class TInput, class TOutput>
void DirectionalCorrectionFunctor<TInput,TOutput>::Initialize(const std::vector<ScatteringFunctionCoefficients> &coeffs) {
    m_nReflBandsCount = coeffs.size();
    m_ScatteringCoeffs = coeffs;
    // first we have the reflectance bands then the cloud mask
    m_nCloudMaskBandIndex = m_nReflBandsCount;
    m_nSnowMaskBandIndex = m_nCloudMaskBandIndex+1;
    m_nWaterMaskBandIndex = m_nSnowMaskBandIndex + 1;

    m_nNdviBandIdx = m_nWaterMaskBandIndex+1;
    m_nSunAnglesBandStartIdx = m_nNdviBandIdx + 1;
    // we have 2 sun angles bands (for azimuth and zenith)
    m_nSensoAnglesBandStartIdx = m_nSunAnglesBandStartIdx+2;
    m_fReflNoDataValue = NO_DATA_VALUE;
}

template< class TInput, class TOutput>
bool DirectionalCorrectionFunctor<TInput,TOutput>::operator!=( const DirectionalCorrectionFunctor & other) const {
	(void) other;
    return true;
}

template< class TInput, class TOutput>
bool DirectionalCorrectionFunctor<TInput,TOutput>::operator==( const DirectionalCorrectionFunctor & other ) const {
    return !(*this != other);
}

template< class TInput, class TOutput>
TOutput DirectionalCorrectionFunctor<TInput,TOutput>::operator()( const TInput & A ) {
    int bandsNo = m_ScatteringCoeffs.size();
    TOutput var(bandsNo);

    double thetaS = A[m_nSunAnglesBandStartIdx];
    double phiS = A[m_nSunAnglesBandStartIdx+1];

    for(int i = 0; i<bandsNo; i++) {
        float fReflVal = (float)(A[i]);
        if(IsNoDataValue(fReflVal, m_fReflNoDataValue)) {
            var[i] = m_fReflNoDataValue;
        } else {
            // if is water, snow or cloud, there is made no correction
            if(IsCloudPixel(A) || IsWaterPixel(A) || IsSnowPixel(A)) {
                var[i] = fReflVal;
            } else {
                double thetaV = A[m_nSensoAnglesBandStartIdx + 2*i];
                double phiV = A[m_nSensoAnglesBandStartIdx + 2*i + 1];
                if(isnan(thetaV) || isnan(phiV)) {
                    var[i] = fReflVal;
                } else {
                    DirectionalModel dirModel0(thetaS, 0, 0, 0);
                    DirectionalModel dirModel(thetaS, phiS, thetaV, phiV);

                    ScatteringFunctionCoefficients &coeffs = m_ScatteringCoeffs[i];
                    double kV = coeffs.V0 + coeffs.V1 * A[m_nNdviBandIdx];
                    double kR = coeffs.R0 + coeffs.R1 * A[m_nNdviBandIdx];
                    float fNewReflVal = fReflVal * dirModel0.dir_mod(kV, kR)/dirModel.dir_mod(kV, kR);
                    if(fNewReflVal < 0) {
                        fNewReflVal = fReflVal;
                    }
                    var[i] = fNewReflVal;
                }
            }
        }
    }

    return var;
}

template< class TInput, class TOutput>
bool DirectionalCorrectionFunctor<TInput,TOutput>::IsSnowPixel(const TInput & A) {
    if(m_nSnowMaskBandIndex == -1)
        return false;

    int val = (int)static_cast<float>(A[m_nSnowMaskBandIndex]);
    return (val != 0);
}

template< class TInput, class TOutput>
bool DirectionalCorrectionFunctor<TInput,TOutput>::IsWaterPixel(const TInput & A) {
    if(m_nWaterMaskBandIndex == -1)
        return false;

    int val = (int)static_cast<float>(A[m_nWaterMaskBandIndex]);
    return (val != 0);
}

template< class TInput, class TOutput>
bool DirectionalCorrectionFunctor<TInput,TOutput>::IsCloudPixel(const TInput & A) {
    if(m_nCloudMaskBandIndex== -1)
        return false;

    int val = (int)static_cast<float>(A[m_nCloudMaskBandIndex]);
    return (val != 0);
}

template< class TInput, class TOutput>
bool DirectionalCorrectionFunctor<TInput,TOutput>::IsNoDataValue(float fValue, float fNoDataValue) {
    return fabs(fValue - fNoDataValue) < EPSILON;
}

} //namespace Functor
} //namespace ts
