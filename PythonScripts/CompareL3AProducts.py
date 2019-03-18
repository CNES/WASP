#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Copyright (C) 2018-2019, Centre National d'Etudes Spatiales (CNES)
All rights reserved

This file is part of Weighted Average Synthesis Processor (WASP)

Authors:
- Peter KETTIG <peter.kettig@cnes.fr>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

See the LICENSE.md file for more details.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

SPDX-License-Identifier: GPL-3.0-or-later
"""

from lxml import etree
import os
import subprocess
from timeit import default_timer as timer
import sys
class L3Product():
    def __init__(self):
        self.profile = ""
        self.tile = ""
        self.acquisitionDate = ""
        self.level = ""
        self.orbit = ""
        self.platform = ""
        self.version = ""
        self.productionDate = ""
        self.bandCount = ""
        self.bandGroups = []
        self.imageList = []
        self.imagePath = []
        self.imageType = ""
        self.maskList = []
        self.maskPath = []
        self.dataList = []
        self.CloudPercent = ""
        self.ContributingProducts = ""

    def __eq__(self, other):
        bReturnValue = True
        bReturnValue = True if self.compareItem(self.profile, other.profile, "Profile", ignore = True) and bReturnValue else False
        bReturnValue = True if self.compareItem(self.tile, other.tile, "Tile") and bReturnValue else False
        bReturnValue = True if self.compareItem(self.acquisitionDate, other.acquisitionDate, "AcquisitionDate", ignore=True) and bReturnValue else False
        bReturnValue = True if self.compareItem(self.productionDate, other.productionDate, "ProductionDate", ignore = True) and bReturnValue else False
        bReturnValue = True if self.compareItem(self.version, other.version, "Version", ignore = True) and bReturnValue else False
        bReturnValue = True if self.compareItem(self.level, other.level, "Level") and bReturnValue else False
        bReturnValue = True if self.compareItem(self.orbit, other.orbit, "Orbit") and bReturnValue else False
        bReturnValue = True if self.compareItem(self.platform, other.platform, "Platform") and bReturnValue else False
        bReturnValue = True if self.compareItem(self.bandCount, other.bandCount, "BandCount") and bReturnValue else False
        bReturnValue = True if self.compareItem(self.bandGroups, other.bandGroups, "BandGroups") and bReturnValue else False
        bReturnValue = True if self.compareItem(self.imageList, other.imageList, "ImageList") and bReturnValue else False
        bReturnValue = True if self.compareItem(self.imageType, other.imageType, "ImageType", ignore = True) and bReturnValue else False
        bReturnValue = True if self.compareItem(self.maskList, other.maskList, "MaskList") and bReturnValue else False
        bReturnValue = True if self.compareItem(self.dataList, other.dataList, "DataList") and bReturnValue else False
        bReturnValue = True if self.compareItem(self.CloudPercent, other.CloudPercent, "CloudPercent", ignore = True) and bReturnValue else False
        bReturnValue = True if self.compareItem(self.ContributingProducts, other.ContributingProducts, "ContributingProducts") and bReturnValue else False

        if(not bReturnValue):
            raise RuntimeError("L3 products are unequal.")

        print("+++++++++Product Metadata equal+++++++++")
        return bReturnValue

    def compareItem(self, item, other, name, ignore = False):
        if(item != other):
            print(str(name) + ": " + str(item) + " != " + str(other))
            if(ignore): return True #Only to be used with fields that should be ignored. E.g. Acquisition date
            return False
        return True

class Comparator():
    def __init__(self, golden, new, mse = 1000):
        self.goldenXML = golden
        self.newXML = new
        self.mse = mse
        return

    def getMetadata(self, xml):
        try:
            tree = etree.parse(str(xml))
        except:
            raise Exception("Cannot open XML " + xml)
        l3 = L3Product()
        l3.profile = str(tree.xpath("/Muscate_Metadata_Document/Metadata_Identification/METADATA_PROFILE")[0].text)
        l3.tile = str(tree.xpath("/Muscate_Metadata_Document/Dataset_Identification/GEOGRAPHICAL_ZONE")[0].text)
        l3.acquisitionDate = str(tree.xpath("/Muscate_Metadata_Document/Product_Characteristics/ACQUISITION_DATE")[0].text)
        l3.productionDate = str(tree.xpath("/Muscate_Metadata_Document/Product_Characteristics/PRODUCTION_DATE")[0].text)
        l3.version = str(tree.xpath("/Muscate_Metadata_Document/Product_Characteristics/PRODUCT_VERSION")[0].text)
        l3.level = str(tree.xpath("/Muscate_Metadata_Document/Product_Characteristics/PRODUCT_LEVEL")[0].text)
        l3.orbit = str(tree.xpath("/Muscate_Metadata_Document/Product_Characteristics/ORBIT_NUMBER")[0].text)
        l3.platform = str(tree.xpath("/Muscate_Metadata_Document/Product_Characteristics/PLATFORM")[0].text)
        l3.bandCount = str(tree.xpath("/Muscate_Metadata_Document/Product_Characteristics/Band_Global_List")[0].attrib.get("count"))
        for group in tree.xpath("/Muscate_Metadata_Document/Product_Characteristics/Band_Group_List/Group"):
            l3.bandGroups.append(group.attrib.get("group_id"))
        for imageList in tree.xpath("/Muscate_Metadata_Document/Product_Organisation/Muscate_Product/Image_List/Image"):
            for image in imageList.xpath("Image_File_List/IMAGE_FILE"):
                image_split = image.text.split("_")
                l3.imageList.append([image_split[4], image.attrib.get("band_id")[1:]])
                l3.imagePath.append(os.path.join(os.path.dirname(xml), image.text))
                l3.imageType = image_split[6]
        l3.imageList = sorted(l3.imageList, key=lambda x: x[1])
        l3.imagePath = sorted(l3.imagePath)
        for maskList in tree.xpath("/Muscate_Metadata_Document/Product_Organisation/Muscate_Product/Mask_List/Mask"):
            for mask in maskList.xpath("Mask_File_List/MASK_FILE"):
                mask_split = mask.text.split("_")
                l3.maskPath.append(os.path.join(os.path.dirname(xml), mask.text))
                l3.maskList.append([mask_split[4], mask_split[6], mask_split[7]])
        l3.maskList = sorted(l3.maskList, key=lambda x: x[1])
        l3.maskPath = sorted(l3.maskPath)

        for dataList in tree.xpath("/Muscate_Metadata_Document/Product_Organisation/Muscate_Product/Data_List/Data"):
            for data in dataList.xpath("Data_File_List/DATA_FILE"):
                data_split = data.text.split("_")
                l3.dataList.append([data_split[4], data_split[6]])
        l3.dataList = sorted(l3.dataList, key=lambda x: x[1])
        for qualityIndex in tree.xpath("/Muscate_Metadata_Document/Quality_Informations/Current_Product/Product_Quality_List/Product_Quality/Global_Index_List/QUALITY_INDEX"):
            if(qualityIndex.get("name") == "CloudPercent"):
                l3.CloudPercent = str(qualityIndex.text)
        l3.ContributingProducts = str(len(tree.xpath("/Muscate_Metadata_Document/Contributing_Products_List")))
        return l3

    def checkApplicationAvailability(self):
        """
        @brief Check if all TemporalSynthesis Applications can be found
        @return none, but throws exception if App is missing.
        """
        availableApplications = self.runOTBApplication("test", [], testRun = True)

        if(not availableApplications):
            raise NotImplementedError("List of available OTB applications is empty.")

        for item in ["CompareImages"]:
            if(item not in availableApplications):
                raise NotImplementedError("Cannot find " + item + " in the list of OTB Apps. Exiting now.")
        return

    def runOTBApplication(self, name, args, testRun = False):
        fullArgs = ["otbcli", name] + args + ["-progress", str(1)]
        if(not testRun):
            print(" ".join(a for a in fullArgs))
            fullArgs = fullArgs #Prepend other programs here
        start = timer()
        proc = subprocess.Popen(fullArgs, shell=False, bufsize=1, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        while (True):
            # Read line from stdout, break if EOF reached, append line to output
            line = proc.stdout.readline()
            line = line.decode()
            if (line == ""): break
            if(line.find("MSE:") > 0):
                mse = line.split(" ")[-1][:-1] #First -1: Get last Element, Second: Cut \n from string
                print("MSE: " + str(mse))

        end = timer()
        if(not testRun): print("OTB App " + str(name) + " took: " + str(end - start) + "s")
        return mse

    def compareImage(self, pathGolden, pathNew, strict = True):
        appName = "CompareImages"
        args = ["-ref.in", str(pathGolden),
                "-meas.in", str(pathNew)]
        mse = self.runOTBApplication(appName, args)
        if(strict and float(mse) > self.mse):
            return False
        return True

    def run(self):
        print("Golden: ", self.goldenXML)
        print("New: ", self.newXML)
        golden = self.getMetadata(self.goldenXML)
        new = self.getMetadata(self.newXML)

        bReturnValue = (golden == new)
        for index, image in enumerate(golden.imagePath):
            bReturnValue = True if self.compareImage(image, new.imagePath[index]) and bReturnValue else False
            print(bReturnValue)
        for index, mask in enumerate(golden.maskPath):
            bReturnValue = True if self.compareImage(mask, new.maskPath[index]) and bReturnValue else False
            print(bReturnValue)
        return bReturnValue




if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-g", "--golden",help="The Golden Metadata file", required=True)
    parser.add_argument("-n","--new", help="The New Metadata file", required=True)
    parser.add_argument("-m","--mse", help="MSE Maximum difference. Default is 1000", required=False, default=1000)

    args = parser.parse_args()

    Comparator = Comparator(args.golden, args.new, args.mse)
    comparison = Comparator.run()
    sys.exit(comparison)
