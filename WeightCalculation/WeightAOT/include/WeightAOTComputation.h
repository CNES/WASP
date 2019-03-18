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

#ifndef WEIGHTONAOT_H
#define WEIGHTONAOT_H

#include "otbWrapperTypes.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkImageSource.h"
#include "otbObjectList.h"
#include "itkUnaryFunctorImageFilter.h"

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
 * @brief Functor to calculate the AOT-weight
 */
template< class TInput, class TOutput>
class AotWeightCalculationFunctor
{
public:
    AotWeightCalculationFunctor() {}
    ~AotWeightCalculationFunctor() {}
  bool operator!=( const AotWeightCalculationFunctor & ) const
    {
    return false;
    }
  bool operator==( const AotWeightCalculationFunctor & other ) const
    {
    return !(*this != other);
    }
  void Initialize(int nBand, float fQuantif, float fAotMax, float fMinWeight, float fMaxWeight) {
      m_nBand = nBand;
      m_fAotQuantificationVal = fQuantif;
      m_fAotMax = fAotMax;
      m_fMinWeightAot = fMinWeight;
      m_fMaxWeightAot = fMaxWeight;
  }

  inline TOutput operator()( const TInput & A ) const
  {
      float val = static_cast< float >( A[m_nBand] )/m_fAotQuantificationVal;
      if(val < 0) {
          return 0;
      }

      float fRetAOT;
      if(val < m_fAotMax) {
          fRetAOT = m_fMinWeightAot + (m_fMaxWeightAot - m_fMinWeightAot) * (1.0 - val/m_fAotMax);
      } else {
          fRetAOT = m_fMinWeightAot;
      }

      return fRetAOT;
  }

private:
  int m_nBand;
  float m_fAotQuantificationVal;
  float m_fAotMax;
  float m_fMinWeightAot;
  float m_fMaxWeightAot;
};
} //namespace Functor

class WeightOnAOT
{
public:
    typedef otb::Wrapper::FloatVectorImageType ImageType;
    typedef otb::Wrapper::FloatImageType OutImageType;

    typedef itk::ImageSource<ImageType> ImageSource;

    typedef otb::ImageFileReader<ImageType> ReaderType;
    typedef otb::ImageFileWriter<OutImageType> WriterType;

    typedef itk::UnaryFunctorImageFilter<ImageType,OutImageType,
                    Functor::AotWeightCalculationFunctor<
                        ImageType::PixelType,
                        OutImageType::PixelType> > FilterType;
    typedef FilterType::Superclass::Superclass::Superclass OutImageSource;

public:
    WeightOnAOT();

    void SetInputFileName(std::string &inputImageStr);
    void SetInputImageReader(ImageSource::Pointer inputReader);
    void SetOutputFileName(std::string &outFile);

    void Initialize(int nBand, float fQuantif, float fAotMax, float fMinWeight, float fMaxWeight);

    const char *GetNameOfClass() { return "WeightOnAOT";}
    OutImageSource::Pointer GetOutputImageSource();
    int GetInputImageResolution();

    void WriteToOutputFile();

private:
    void BuildOutputImageSource();
    ImageSource::Pointer m_inputReader;
    std::string m_outputFileName;

    int m_nBand;
    float m_fAotQuantificationVal;
    float m_fAotMax;
    float m_fMinWeightAot;
    float m_fMaxWeightAot;

    FilterType::Pointer m_filter;
};

} //namespace ts
#endif // WEIGHTONAOT_H
