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
#include "TotalWeightComputation.h"
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

using namespace ts;

/**
 * @brief Calculate the total weight using the AOT- and cloud-images
 */
class TotalWeight : public Application
{

public:
  /** Standard class typedefs. */
  typedef TotalWeight                      Self;
  typedef Application                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(TotalWeight, otb::Application);

private:
  void DoInit()
  {
    SetName("TotalWeight");
    SetDescription("Calculate the total weight using the AOT- and cloud-images");

    SetDocLongDescription("Calculate the total weight using the AOT- and cloud-images");

    SetDocLimitations("None");
    SetDocAuthors("Peter KETTIG");
    SetDocSeeAlso(" ");

    AddParameter(ParameterType_String,  "xml",   "The input metadata xml");
    SetParameterDescription("xml", "The XML file containing the L2A metadata.");

    AddParameter(ParameterType_String,  "waotfile",   "Input AOT weight file name");
    SetParameterDescription("waotfile", "The file name of the image containing the AOT weigth for each pixel.");

    AddParameter(ParameterType_String,  "wcldfile",   "Input cloud weight file name");
    SetParameterDescription("wcldfile", "The file name of the image containing the cloud weigth for each pixel.");

    AddParameter(ParameterType_String, "l3adate", "L3A date");
    SetParameterDescription("l3adate", "The L3A date in the format YYYYMMDDD");

    AddParameter(ParameterType_Int, "halfsynthesis", "Delta max");
    SetParameterDescription("halfsynthesis", "Half synthesis period expressed in days.");

    AddParameter(ParameterType_Float, "wdatemin", "Minimum date weight");
    SetParameterDescription("wdatemin", "Minimum weight at edge of synthesis time window.");
    SetDefaultParameterFloat("wdatemin", 0.5);
    MandatoryOff("wdatemin");

    AddParameter(ParameterType_OutputImage, "out", "Output Total Weight Image");
    SetParameterDescription("out","The output image containg the computed total weight for each pixel.");

    AddRAMParameter();

    // Doc example parameter settings
    SetDocExampleParameterValue("xml", "example1.xml");
    SetDocExampleParameterValue("waotfile", "example2.tif");
    SetDocExampleParameterValue("wcldfile", "example3.tif");
    SetDocExampleParameterValue("l3adate", "20140502");
    SetDocExampleParameterValue("halfsynthesis", "50");
    SetDocExampleParameterValue("wdatemin", "0.10");
    SetDocExampleParameterValue("out", "apTotalWeightOutput.tif");
  }

  void DoUpdateParameters()
  {
  }

  void DoExecute()
  {
    // Get the input image list
    std::string inXml = GetParameterString("xml");
    if (inXml.empty())
    {
        itkExceptionMacro("No xml file given...; please set the input xml");
    }

    auto factory = MetadataHelperFactory::New();
    auto pHelper = factory->GetMetadataHelper(inXml);
    std::string l2aDate = pHelper->GetAcquisitionDate();
    std::string l3aDate = GetParameterString("l3adate");
    int halfSynthesis = GetParameterInt("halfsynthesis");
    float weightOnDateMin = GetParameterFloat("wdatemin");

    std::string inAotFileName = GetParameterString("waotfile");
    std::string inCloudFileName = GetParameterString("wcldfile");

    // weight on sensor parameters
    std::string missionName = pHelper->GetMissionName();
    m_totalWeightComputation.SetMissionName(missionName);

    // weight on date parameters
    m_totalWeightComputation.SetDates(l2aDate, l3aDate);
    m_totalWeightComputation.SetHalfSynthesisPeriodAsDays(halfSynthesis);
    m_totalWeightComputation.SetWeightOnDateMin(weightOnDateMin);


    // Weights for AOT and Clouds
    m_totalWeightComputation.SetAotWeightFile(inAotFileName);
    m_totalWeightComputation.SetCloudsWeightFile(inCloudFileName);

    // Set the output image
    SetParameterOutputImage("out", m_totalWeightComputation.GetOutputImageSource()->GetOutput());
  }

  TotalWeightComputation m_totalWeightComputation;
};

} // namespace Wrapper
} // namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::TotalWeight)

