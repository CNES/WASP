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

#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"

#include "WeightAOTComputation.h"
#include "MetadataHelperFactory.h"

/**
 * @brief otb Namespace for all OTB-related Filters and Applications
 */
namespace otb
{
/**
 * @brief Wrapper namespace for all OTB-Applications
 */
namespace Wrapper
{

/**
 * @brief Calculate the AOT-weight from a given AOT-mask
 */
class WeightAOT : public Application
{

public:
  /** Standard class typedefs. */
  typedef WeightAOT                      Self;
  typedef Application                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(WeightAOT, otb::Application);

private:
  void DoInit()
  {
    SetName("WeightAOT");
    SetDescription("Calculate the AOT-weight from a given AOT-mask");

    SetDocName("WeightAOT");
    SetDocLongDescription("Calculate the AOT-weight from a given AOT-mask");
    SetDocLimitations("None");
    SetDocAuthors("Peter Kettig");
    SetDocSeeAlso(" ");

    AddParameter(ParameterType_String,  "in",   "Input image");
    SetParameterDescription("in", "Image containing AOT.");

    AddParameter(ParameterType_String,  "xml",   "XML metadata file");
    SetParameterDescription("xml", "XML metadata file for the product.");

    AddParameter(ParameterType_OutputImage, "out", "Output Image");
    SetParameterDescription("out","Output image.");

    AddParameter(ParameterType_Float, "waotmin", "WeightAOTMin");
    SetParameterDescription("waotmin", "min weight depending on AOT");

    AddParameter(ParameterType_Float, "waotmax", "WeightAOTMax");
    SetParameterDescription("waotmax", "max weight depending on AOT");

    AddParameter(ParameterType_Float, "aotmax", "AOTMax");
    SetParameterDescription("aotmax", "maximum value of the linear range for weights w.r.t AOT");

    AddRAMParameter();

    // Doc example parameter settings
    SetDocExampleParameterValue("in", "verySmallFSATSW_r.tif");
    SetDocExampleParameterValue("xml", "metadata.xml");
    SetDocExampleParameterValue("waotmin", "0.33");
    SetDocExampleParameterValue("waotmax", "1");
    SetDocExampleParameterValue("aotmax", "50");
    SetDocExampleParameterValue("out", "apAOTWeightOutput.tif");
  }

  void DoUpdateParameters()
  {
  }

  void DoExecute()
  {
    // Get the input image list
    std::string inImgStr = GetParameterString("in");
    if (inImgStr.empty())
    {
        itkExceptionMacro("No input Image set...; please set the input image");
    }

    std::string inMetadataXml = GetParameterString("xml");
    if (inMetadataXml.empty())
    {
        itkExceptionMacro("No input metadata XML set...; please set the input image");
    }

    float fAotMax = GetParameterFloat("aotmax");
    float fWaotMin = GetParameterFloat("waotmin");
    float fWaotMax = GetParameterFloat("waotmax");

    m_weightOnAot.SetInputFileName(inImgStr);
    auto factory = ts::MetadataHelperFactory::New();
    auto pHelper = factory->GetMetadataHelper(inMetadataXml);
    float fAotQuantificationVal = pHelper->GetAotQuantificationValue();
    // the bands in XML are 1 based
    int nBand = 0;//pHelper->GetAotBandIndex()-1;
    std::cout << "fAotQuantificationVal " << fAotQuantificationVal << std::endl;
    m_weightOnAot.Initialize(nBand, fAotQuantificationVal, fAotMax, fWaotMin, fWaotMax);

    // Set the output image
    SetParameterOutputImage("out", m_weightOnAot.GetOutputImageSource()->GetOutput());
  }

  ts::WeightOnAOT m_weightOnAot;
};

} // namespace Wrapper
} // namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::WeightAOT)

