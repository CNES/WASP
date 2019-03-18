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

#ifndef MASKEXTRACTORFILTER_H
#define MASKEXTRACTORFILTER_H

#include "otbWrapperTypes.h"
#include "otbMultiToMonoChannelExtractROI.h"
#include "otbImageFileReader.h"
#include "otbUnaryFunctorImageFilter.h"
#include "MaskExtractorFunctor.h"

#include "itkNumericTraits.h"
#include "otbImage.h"

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Extract a certain bit from an image
 */
template <class TImageType, class TOutputImageType>
class ITK_EXPORT MaskExtractorFilter : public itk::ImageToImageFilter<TImageType, TOutputImageType> {
public:
  typedef MaskExtractorFilter                     		  Self;
  typedef itk::ImageToImageFilter<TImageType, TOutputImageType> Superclass;
  typedef itk::SmartPointer<Self>                         Pointer;
  typedef itk::SmartPointer<const Self>                   ConstPointer;

  typedef typename TImageType::PixelType				  PixelType;
  typedef typename TOutputImageType::PixelType			  OutputPixelType;

  itkNewMacro(Self);
  itkTypeMacro(MaskExtractorFilter, itk::ImageToImageFilter);

  /**
   * @brief Set the bit to be extracted
   * @param bit The bit, starting at 0
   */
  void SetBitMask(const int & bit);

  MaskExtractorFilter();

protected:
  typedef Functor::MaskExtractorFunctor<PixelType, OutputPixelType> MaskHandlerFunctorType;
  typedef itk::UnaryFunctorImageFilter< TImageType, TOutputImageType, MaskHandlerFunctorType > BitExtractorType;

  /**
   * @brief Run the extraction
   */
  virtual void GenerateData() ITK_OVERRIDE;

private:

  MaskExtractorFilter(Self &);  		 // intentionally not implemented
  void operator =(const Self&);          // intentionally not implemented

  typename BitExtractorType::Pointer          m_BitExtractor;
};

} /* namespace ts */

#include "../src/MaskExtractorFilter.txx"

#endif

