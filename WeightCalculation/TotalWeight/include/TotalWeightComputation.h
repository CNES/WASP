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

#ifndef TOTALWEIGHTCOMPUTATION_H
#define TOTALWEIGHTCOMPUTATION_H

#include "otbWrapperTypes.h"
#include "itkBinaryFunctorImageFilter.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "GlobalDefs.h"
#include "ImageResampler.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts
{
/**
 * @brief Namespace around Functors to be used in the filters defined below
 */
namespace Functor
{

/**
 * @brief Functor to calculate the TotalWeight
 */
template< class TPixel>
class TotalWeightCalculationFunctor
{
public:
  TotalWeightCalculationFunctor() {}
  ~TotalWeightCalculationFunctor() {}
  bool operator!=(const TotalWeightCalculationFunctor &) const
  {
    return false;
  }

  bool operator==(const TotalWeightCalculationFunctor & other) const
  {
    return !( *this != other );
  }

  void SetFixedWeight(float weightSensor, float weightDate)
  {
    m_fixedWeight = weightSensor * weightDate;
  }

  inline TPixel operator()(const TPixel & A,
                            const TPixel & B) const
  {
    const float dA = static_cast< float >( A );
    const float dB = static_cast< float >( B );
    if(dA < 0 || dB < 0) {
        return WEIGHT_NO_DATA_VALUE;
    }
    const float totalWeight = fabs(dA * dB * m_fixedWeight);

    return static_cast< TPixel >( totalWeight );
  }
private:
  float m_fixedWeight;
};

}//namespace Functor

class TotalWeightComputation
{
public:
    typedef otb::Wrapper::FloatImageType ImageType;
    typedef enum {S2, VNS, UNKNOWN} SensorType;
    typedef itk::BinaryFunctorImageFilter< ImageType, ImageType, ImageType,
                              Functor::TotalWeightCalculationFunctor<ImageType::PixelType> > FilterType;
    typedef otb::ImageFileReader<ImageType> ReaderType;
    typedef otb::ImageFileWriter<ImageType> WriterType;

    typedef itk::ImageSource<ImageType> ImageSource;
    typedef FilterType::Superclass::Superclass OutImageSource;

public:
    TotalWeightComputation();

    void SetMissionName(std::string &missionName);
    void SetDates(std::string &L2ADate, std::string &L3ADate);
    void SetHalfSynthesisPeriodAsDays(int deltaMax);
    void SetWeightOnDateMin(float fMinWeight);
    void SetAotWeightFile(std::string &aotWeightFileName);
    void SetCloudsWeightFile(std::string &cloudsWeightFileName);
    void SetTotalWeightOutputFileName(std::string &outFileName);

    const char *GetNameOfClass() { return "TotalWeightComputation";}
    OutImageSource::Pointer GetOutputImageSource();

    void WriteToOutputFile();

protected:
    void ComputeTotalWeight();
    void ComputeWeightOnSensor();
    void ComputeWeightOnDate();

protected:
    float m_fWeightOnSensor;
    float m_fWeightOnDate;
    int m_res;

private:
    void BuildOutputImageSource();

    SensorType m_sensorType;
    int m_nDaysTimeInterval;
    int m_nDeltaMax;
    float m_fWeightOnDateMin;
    std::string m_strOutFileName;

    ImageSource::Pointer m_inputReaderAot;
    ImageSource::Pointer m_inputReaderCld;

    FilterType::Pointer m_filter;
    ImageResampler<ImageType, ImageType> m_AotResampler;
    void CheckTolerance();
};
}//namespace ts

#endif // TOTALWEIGHTCOMPUTATION_H
