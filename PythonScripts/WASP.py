#!/usr/bin/env python
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

import os, sys
import logging
import argparse
from lxml import etree
from timeit import default_timer as timer
import subprocess
import datetime as dt
import numpy as np


class OTBApplicationError(Exception):
        def __init__(self, name, returnCode):

            # Call the base class constructor with the parameters it needs
            super(OTBApplicationError, self).__init__("OTB App {0} failed with error code {1}".format(name, returnCode))

class TemporalSynthesis():
    """
    @brief The Class to run the Level-3A Temporal Synthesis chain for Sentinel-2A/B products
           The run() method will run the full-Atillery ;)
    """

    # =========================================================================
    #   Program specific parameters:
    # =========================================================================
    ExeVersion = "1.0"
    scatteringCoeffBasePath = "scattering_coeffs_"
    scatteringCoeffsVersion = "1.0"

    defRemoveTemp = True
    defVerbose = logging.DEBUG
    defCoG = False
    #Default GIP-Parameters:
    ParameterVersion = "1.1"
    defS2Syntperiod = int(23)
    defVnsSyntperiod = int(11)
    defWeightAOTMin = float(0.33)
    defWeightAOTMax = float(1)
    defAOTMax = float(0.8)
    defCoarseRes = int(240)
    defKernelwidth = int(801)
    defSigmaSmallCLD = float(2)
    defSigmaLargeCLD = float(10)
    defWeightDateMin = float(0.5)

    venusPlatform = "VENUS"
    s2Platform = "SENTINEL2"
    # =========================================================================
    # Environment Variables
    # =========================================================================
    # GDAL RAM usage
    GDAL_CACHEMAX=16384
    # Set the number of available CPUs for each OTB-App
    ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS = 8

    def __init__(self, defArgs):
        self.initLoggers(filepath = defArgs.logging)

        self.execPath = os.path.dirname(os.path.realpath(__file__))
        self.setupEnvironmentVariable(variableName="PATH", path=os.path.join(self.execPath))
        self.setupEnvironmentVariable(variableName="LD_LIBRARY_PATH", path=os.path.join(self.execPath, "../lib"))
        self.setupEnvironmentVariable(variableName="OTB_APPLICATION_PATH", path=os.path.join(self.execPath, "../lib/otb/applications"))
        self.platform = self.getPlatformIdentifier(defArgs.input)
        if(self.platform not in [self.venusPlatform, self.s2Platform]):
            raise ValueError("Unknown platform found: {0}".format(self.platform))
        logging.info("Platform found: {0}".format(self.platform))
        self.args = self.getParameters(defArgs)

        self.setupEnvironmentVariable(variableName="GDAL_CACHEMAX", path=str(self.GDAL_CACHEMAX), reset = True, ignoreWarning=True)
        self.setupEnvironmentVariable(variableName="ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS", path=str(self.args.nthreads), reset = True, ignoreWarning=True)

        try:
            self.checkApplicationAvailability()
        except OSError:
            raise OSError("Cannot find otbApplicationLauncherCommandLine executable")

    def initLoggers(self, msgLevel = logging.DEBUG, filepath = ""):
        """
        @brief Init a file and a stdout logger
        @param msgLevel Standard msgLevel for both loggers. Default is DEBUG
        @param filepath The path to save the log-file to. If none, the current directory is used"
        """
        filename = "TS-INFO.log"

        #Create default path or get the pathname without the extension, if there is one

        logger=logging.getLogger()
        logger.handlers=[] #Remove the standard handler again - Bug in logging module

        logger.setLevel(msgLevel)
        formatter = logging.Formatter("%(asctime)s [%(levelname)-5.5s] %(message)s")

        consoleHandler = logging.StreamHandler(sys.stdout)
        consoleHandler.setFormatter(formatter)
        logger.addHandler(consoleHandler)

        if(filepath == ""):
            logging.warning("No logging-path provided. No log-file will be created!")
        else:
            strDbgLogFullPath = os.path.join(filepath, filename)
            print("Logging to file " + strDbgLogFullPath)
            self.createDirectory(filepath)
            fileHandler=logging.FileHandler(strDbgLogFullPath)
            fileHandler.setFormatter(formatter)
            logger.addHandler(fileHandler)
        return logger

    def setupEnvironmentVariable(self, variableName, path, reset = True, ignoreWarning = False):
        """
        @brief Export an environment variable
        @variableName The variable to be appended or exportet
        @path the Path to be appended to the given variable
        """
        #Export environment variables:
        envVar = str(variableName)
        if(reset):
            os.environ[envVar] = path
        else:
            try:
                os.environ[envVar] = os.environ[envVar] + os.pathsep + path
            except:
                if(not ignoreWarning): logging.warning("Cannot find existing {0}, setting it anyway".format(envVar))
                os.environ[envVar] = path
        logging.info(envVar + " " + os.environ[envVar])
        return

    def checkApplicationAvailability(self):
        """
        @brief Check if all TemporalSynthesis Applications can be found
        @return none, but throws exception if App is missing.
        """
        availableApplications = self.runOTBApplication("test", [], testRun = True)[1]

        if(not availableApplications):
            raise NameError("List of available OTB applications is empty.")

        for item in ["CompositePreprocessing", "WeightOnClouds", "WeightAOT", "TotalWeight", "UpdateSynthesis", "ProductFormatter"]:
            if(item not in availableApplications):
                raise NameError("Cannot find {0} in the list of OTB Apps".format(item))
        return

    def getAcquisitionPeriod(self, xmllist, synthalf = None, date = None):
        """
        @brief Calculate Min, Max and Middle-datetimes from a Muscate-XML list
        @param xmllist the list of filenames containing an XML
        @return The oldest and newest datetime, as well as the date between those two as list,
                as well as the mean interval between middle and min/max in days
        """

        if(self.platform == self.venusPlatform):
            MIN_DATE_INTERVAL = 7
            MAX_DATE_INTERVAL = 15
        else:
            MIN_DATE_INTERVAL = 20
            MAX_DATE_INTERVAL = 23
        xpath = "/Muscate_Metadata_Document/Product_Characteristics/ACQUISITION_DATE"
        dates = []

        for xml in xmllist:
            try:
                tree = etree.parse(str(xml))
                acqDate = str(tree.xpath(xpath)[0].text)
                dates.append(self.stringToDatetime(acqDate))
            except:
                logging.warning("Cannot open XML {0}".format(xml))
        if(not dates):
            logging.error("Dates list empty. Are all XMLs readable?")
            exit(1)

        # Filter date that are not within synthalf range
        if synthalf is not None and date is not None:
            date = self.stringToDatetime(args.date, short = True)
            logging.info("Filtering dates outside of target date {} +/- {} days".format(date,synthalf))
            synthalf_delta = dt.timedelta(days = synthalf)
            
            filtered_dates = []
            filtered_xmllist = []
            for xml,d in zip(xmllist,dates):
                if abs(d-date) <= synthalf_delta:
                    filtered_dates.append(d)
                    filtered_xmllist.append(xml)
            dates = filtered_dates
            xmllist = filtered_xmllist
            logging.info("{} dates kept for synthesis".format(len(dates)))

        dates.sort()

        minDate = min(dates)
        maxDate = max(dates)
        midDate = minDate + (maxDate - minDate)/2
        dateIntervals = [(midDate - minDate).days, (maxDate - midDate).days]
        if(dateIntervals[0] <= MIN_DATE_INTERVAL or dateIntervals[1] <= MIN_DATE_INTERVAL):
            logging.warning("XML-Input dates interval is smaller than " + str(MIN_DATE_INTERVAL*2) + " days!")
        elif(dateIntervals[0] > MAX_DATE_INTERVAL or dateIntervals[1] > MAX_DATE_INTERVAL):
            logging.warning("XML-Input dates interval is bigger than " + str(MAX_DATE_INTERVAL*2 - 1) + " days!")

        return [minDate, midDate, maxDate],  np.mean(dateIntervals), dates, xmllist

    def datetimeToString(self, d, short = False):
        """
        @brief Parse datetime to string of format "YYYY-MM-DDTHH-mm-SS.fffZ"
        @param d Datetime struct
        @return String with the datetime in format "YYYY-MM-DDTHH-mm-SS.fffZ"
        """
        if(short):
            return str(d.strftime("%Y%m%d"))
        return str(d.strftime("%Y-%m-%dT%H:%M:%S.%f")[:-3] + "Z")

    def stringToDatetime(self, d, short = False):
        """
        @brief Parse string of format "YYYY-MM-DDTHH-mm-SS.fff" or YYYYMMDD to datetime
        @param d String in format "YYYY-MM-DDTHH-mm-SS.fffZ" or YYYYMMDD
        @return Datetime struct
        """
        if(short):
            return dt.datetime.strptime(d, "%Y%m%d")
        if(d[-1:] == "Z"):
            return dt.datetime.strptime(d[:-1], "%Y-%m-%dT%H:%M:%S.%f")

        return dt.datetime.strptime(d, "%Y-%m-%dT%H:%M:%S.%f")

    def str2bool(self, v):
        """
        @brief Returns a boolean following a string input
        @param v String to be tested if it"s containing a True/False value
        @return bool True, if string contains ("yes", "true", "t", "y", "1"), false if not
        """
        if v.lower() in ("yes", "true", "t", "y", "1"):
            return True
        elif v.lower() in ("no", "false", "f", "n", "0"):
            return False
        else:
            raise argparse.ArgumentTypeError("Boolean value expected.")
        return

    def getParameters(self, args):
        """
        @brief Get the Command line arguments and set them to the desired or default value respectively
        @note There is an ongoing bug in the argparse module which makes it impossible to parse Booleans directly.
              It is therefore parsed as string and then converted to a Boolean.
        """

        if(args.removeTemp):
            args.removeTemp = self.str2bool(args.removeTemp)
        else:
            args.removeTemp = self.defRemoveTemp

        if(args.verbose):
            verbosity = logging.DEBUG if self.str2bool(args.verbose) else logging.INFO
        else:
            verbosity = self.defVerbose
        logging.getLogger().setLevel(verbosity)
        logging.info("Logging set to " + logging.getLevelName(logging.getLogger().getEffectiveLevel()))

        if(args.cog):
            args.cog = self.str2bool(args.cog)
        else:
            args.cog = self.defCoG
        if(args.tempout == None):
            args.tempout = args.out
        if(args.weightaotmin == None):
            args.weightaotmin = self.defWeightAOTMin
        if(args.weightaotmax == None):
            args.weightaotmax = self.defWeightAOTMax
        if(args.aotmax == None):
            args.aotmax = self.defAOTMax
        if(args.coarseres == None):
            args.coarseres = self.defCoarseRes
        if(args.sigmalargecld == None):
            args.sigmalargecld = self.defSigmaLargeCLD
        if(args.sigmasmallcld == None):
            args.sigmasmallcld = self.defSigmaSmallCLD
        if(args.kernelwidth == None):
            args.kernelwidth = self.defKernelwidth
        if(args.weightdatemin == None):
            args.weightdatemin = self.defWeightDateMin
        if(args.synthalf == None):
            if(self.platform == self.s2Platform):
                args.synthalf = self.defS2Syntperiod
            else:
                args.synthalf = self.defVnsSyntperiod
        if(args.nthreads == None):
            args.nthreads = self.ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS
        if(args.pathprevL3A == None):
            args.pathprevL3A = ""
        if(args.scatteringcoeffpath == None):
            #Search for existence of the scattering-coeff files in the App-path:
            applicationPath = os.environ["OTB_APPLICATION_PATH"].split(":")
            for path in applicationPath:
                searchPath = os.path.join(path, "../../../", "share")
                scatteringCoeffs = [self.scatteringCoeffBasePath + str(res) + "m.txt" for res in [10, 20]]
                #Check if both scatteringcoeff-files exist:
                if(all(os.path.exists(os.path.join(searchPath, scatteringCoeff)) == True
                       for scatteringCoeff in scatteringCoeffs)):
                    args.scatteringcoeffpath = searchPath
                    break
            #If the files are still not found, raise an Error
            if(args.scatteringcoeffpath == None):
                raise OSError("Cannot find scattering-coefficients file. Please verify your OTB_APPLICATION_PATH")

        #Check if Geo-zone is unique:
        if(not self.checkTileUnique(args.input)):
            logging.error("WASP can only be executed on a single tile. Exiting now...")
            sys.exit(1)

        #Set Parameter version
        if(args.version):
            self.setParameterVersion(args.version)

        #Calculate temporal parameters
        acqPeriod, synthalf, datesAsDatetime, xmllist = self.getAcquisitionPeriod(args.input, args.synthalf, args.date)
        args.input = xmllist

        #Set temporal parameters to either default values or to user-defined ones
        if(args.date == None):
            args.date = self.datetimeToString(acqPeriod[1])
            args.mindate = self.datetimeToString(acqPeriod[0])
            args.maxdate = self.datetimeToString(acqPeriod[2])
            pass
        else:
            #Remove dashes in case the string is entered as YYYY-MM-DD
            if(len(args.date) != 8):
                args.date = args.date.replace("-", "")
            date = self.stringToDatetime(args.date, short = True)
            args.date = self.datetimeToString(date)
            args.mindate = self.datetimeToString(date - dt.timedelta(days=args.synthalf))
            args.maxdate = self.datetimeToString(date + dt.timedelta(days=args.synthalf))
        #Print warning to indicate wrongly set synthesis parameters
        if(abs(args.synthalf - synthalf) > 2):
            logging.warning("The given Half synthesis period of " + str(args.synthalf) +
                            " days differs from the one in the datelist with " + str(synthalf) + " days.")
        #Standard info message about synthesis date and period
        logging.info("Synthesis date is: " + args.date)
        logging.info("Half synthesis period in days is: " + str(args.synthalf))

        return args

    def setParameterVersion(self, version):
        """
        @brief Set the parameter version by checking the format of the string first
               If the format is not correct, return and use the default
        @param version The version string in the format "x.x"
        """
        versions = version.split(".")
        if(versions is None or len(versions) != 2):
            return
        if(versions[0].isdigit() and versions[1].isdigit()):
            self.ParameterVersion = version
        return

    def checkMetadataFieldsEqual(self, xpath, xmllist):
        """
        @brief Check, whether all field of each XML are the same
        @return True if a single string is found, False if not
                If True, the value of the field is returned as well, None if not
        """
        fieldList = []
        for xml in xmllist:
            if(not xml):
                fieldList.append("")
            try:
                tree = etree.parse(str(xml))
            except:
                raise IOError("Cannot open XML " + xml)
            fieldList.append(tree.xpath(xpath)[0].text)
        #Check if all items are identical
        if(fieldList.count(fieldList[0]) == len(fieldList)):
            return True, fieldList[0]

        logging.warning("Found multiple items of {0}: ".format(xpath) + ", ".join([tile for tile in fieldList]))
        return False, None

    def checkTileUnique(self, xmllist):
        """
        @brief Check, whether all tiles of each XML are the same
        @return True if a single tile is found, False if not
        """
        xPathToTile = "/Muscate_Metadata_Document/Dataset_Identification/GEOGRAPHICAL_ZONE"
        return self.checkMetadataFieldsEqual(xPathToTile, xmllist)[0]

    def getPlatformIdentifier(self, xmllist):
        """
        @brief Check, whether all tiles of each XML are the same
        @return True if a single tile is found, False if not
        """
        xPathToPlatform = "/Muscate_Metadata_Document/Dataset_Identification/PROJECT"
        allEqual, platform = self.checkMetadataFieldsEqual(xPathToPlatform, xmllist)
        if(not allEqual):
            raise ValueError("Multiple platforms found in L2A XML!")
        return platform

    def getL3AProductPath(self, xml):
        """
        @brief find finished Level-3A masks and rasters to be used for the manual-mode
        @TODO Find a way to feed the rasters into UpdateSynthesis. Modify Parameters maybe?
        """
        xmlBase = os.path.dirname(xml)
        if(not xml):
            return []
        try:
            tree = etree.parse(str(xml))
        except:
            raise IOError("Cannot open XML " + xml)

        l3aProductArguments = []
        xPathBase = "/Muscate_Metadata_Document/Product_Organisation/Muscate_Product"
        xPathImgs = os.path.join(xPathBase, "Image_List/Image/Image_File_List/IMAGE_FILE")
        rastersL3A = [img.text for img in tree.xpath(xPathImgs)]
        nSentinelBands = 10
        if(len(rastersL3A) != nSentinelBands):
            raise ValueError("Number of Bands in existing L3 product is not equal to the number of Sentinel-2 bands of" + str(nSentinelBands))

        masksNature = {"Weighted_Average_Dates" : " -prevl3datesr", "Pixel_Weight" : " -prevl3weightsr", "Pixel_Status_Flag" : " -prevl3flagsr"}
        xPathMaskBase = os.path.join(xPathBase, "Mask_List/Mask")
        for mask in tree.xpath(xPathMaskBase):
            currentNature =  mask.xpath("Mask_Properties/NATURE")[0].text
            currentPaths = mask.xpath("Mask_File_List/MASK_FILE")
            resolutions = ["1" if(maskFile.attrib["group_id"] == "R1") else "2" for maskFile in currentPaths]
            if(currentNature in masksNature):
                [l3aProductArguments.append(masksNature[currentNature] + resolutions[i] + " " + os.path.join(xmlBase, currentPaths[i].text)) for i in range(len(currentPaths))]

        return []

    def createDirectory(self, path):
        """
        @brief Creates a Directory with the given path. Throws are warning if it's already existing and an error
               if a file with the same name already exists
        @param path The full path to the new directory
        """
        if os.path.exists(path) and not os.path.isdir(path):
            logging.error("Cannot create the output directory because there is a file with the same name. Remove: " + path)
            exit(1)

        else:
            try:
                os.makedirs(path)
            except:
                logging.debug("Cannot create directory " + path)
        return

    def writeGIPP(self, args, path):
        """
        @brief Write the given GIPPs to a temporary XML file
        @param args The argument list, containing all necessary parameters for the processing chain
        @path The path where the GIPP shall be written to
        @return Writing the file TS_GIPP_x-x.xml to the desired folder
        """

        paramsFilenameXML= path + "/TS_GIPP_v"+ self.ParameterVersion.replace(".","-") +".xml"
        with open(paramsFilenameXML, "wb") as xmlfile:
            logging.info("Writing GIPP-File to: " + paramsFilenameXML)
            root = etree.Element("Weighted_Average_Synthesis_Processor")
            etree.SubElement(root, "PROGRAM_VERSION").text = str(self.ExeVersion)
            etree.SubElement(root, "PARAMETER_VERSION").text = str(self.ParameterVersion)
            gipps = etree.SubElement(root, "Image_Processing_Parameters")
            dirCorr = etree.SubElement(gipps, "Directional_Correction")
            etree.SubElement(dirCorr, "SCATTERING_COEFFICIENTS").text = str(self.scatteringCoeffsVersion)
            wAOT = etree.SubElement(gipps, "Weight_AOT")
            etree.SubElement(wAOT, "WEIGHT_AOT_MIN").text = str(args.weightaotmin)
            etree.SubElement(wAOT, "WEIGHT_AOT_MAX").text = str(args.weightaotmax)
            etree.SubElement(wAOT, "AOT_MAX").text = str(args.aotmax)
            wCLD = etree.SubElement(gipps, "Weight_On_Clouds")
            etree.SubElement(wCLD, "COARSE_RES").text = str(args.coarseres)
            etree.SubElement(wCLD, "SIGMA_SMALL_CLD").text = str(args.sigmasmallcld)
            etree.SubElement(wCLD, "SIGMA_LARGE_CLD").text = str(args.sigmalargecld)
            wDATE = etree.SubElement(gipps, "Weight_On_Date")
            etree.SubElement(wDATE, "WEIGHT_DATE_MIN").text = str(args.weightdatemin)
            etree.SubElement(wDATE, "SYNTHESIS_DATE").text = str(args.date)
            etree.SubElement(wDATE, "SYNTHESIS_PERIOD_MIN").text = str(args.mindate)
            etree.SubElement(wDATE, "SYNTHESIS_PERIOD_MAX").text = str(args.maxdate)
            etree.SubElement(wDATE, "HALF_SYNTHESIS").text = str(args.synthalf)
            usedXMLs = etree.SubElement(gipps, "XML_INPUTS")
            for index, xml in enumerate(args.input):
                XMLElement = etree.SubElement(usedXMLs, "XML")
                XMLElement.text = os.path.basename(xml)
                XMLElement.set("id", str(index))
            et = etree.ElementTree(root)
            et.write(xmlfile, xml_declaration=True, encoding="UTF-8", pretty_print=True)
        return paramsFilenameXML

    def runOTBApplication(self, name, args, testRun = False):
        """
        @brief Run an OTB app using the otbApplicationLauncherCommandLine
        @param name the Name of the application
        @param args The list of arguments to run the app with
        @param testRun True, if only a list of current apps shall be displayed, False otherwise
        @return The return code of the App, as it is being spawned using subprocess.Popen()
        """

        fullArgs = ["otbApplicationLauncherCommandLine", name] + args + ["-progress", str(1)]
        if(not testRun):
            logging.info(" ".join(a for a in fullArgs))
            fullArgs = fullArgs #Prepend other programs here
        start = timer()
        proc = subprocess.Popen(fullArgs, shell=False, bufsize=1, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        output = []
        while (True):
            # Read line from stdout, break if EOF reached, append line to output
            line = proc.stdout.readline().decode('utf-8')
            #Poll(): Used to get the return code at the end of the execution
            if (line == "") and proc.poll() is not None:
                break
            if(not testRun):
                logging.debug(line[:-1])
            else:
                #Extract the name of each Application, which is after \t
                start = line.find("\t")
                if(start >= 0): output.append(str(line[start+1:-1]))
        end = timer()
        returnCode = proc.returncode
        #If this is not the testRun, raise an Error:
        if(not testRun and returnCode != 0):
            raise OTBApplicationError(name, returnCode)
        #Print total execution time for the App:
        if(not testRun): logging.info("OTB App {0} took: {1}s".format(name, end - start))
        return returnCode, output

    def compositePreprocessing(self, platform, xml, scatteringcoeffpath, out, outcld, outwat, outsnw, outaot):
        """
        @brief Run the compositePreprocessing-App
        """

        scatteringcoeffs = [os.path.join(scatteringcoeffpath, self.scatteringCoeffBasePath + str(res) + "m.txt") for res in [10, 20]]
        appName = "CompositePreprocessing"

        args = ["-xml", str(xml),
                "-outcld", str(outcld),
                "-outwat", str(outwat),
                "-outsnw", str(outsnw),
                "-outaot", str(outaot),
                "-outr1", str(out[0]),]

        if(platform == self.s2Platform):
              args += ["-outr2", str(out[1])]
              args += ["-scatteringcoeffsr1", str(scatteringcoeffs[0]),
                           "-scatteringcoeffsr2", str(scatteringcoeffs[1])]
        self.runOTBApplication(appName, args)
        return

    def weightOnClouds(self, cldpath, coarseres, sigmasmallcld, sigmalargecld, kernelwidth, out, cut):
        """
        @brief Run the WeightOnClouds-App
        """

        appName = "WeightOnClouds"
        args = ["-incldmsk", str(cldpath),
                "-coarseres", str(coarseres),
                "-sigmasmallcld", str(sigmasmallcld),
                "-sigmalargecld", str(sigmalargecld),
                "-kernelwidth", str(kernelwidth),
                "-out", str(out),
                "-cut", str(cut)]

        self.runOTBApplication(appName, args)
        return

    def weightAot(self, aotmsk, xmlInput, weightAot, waotmin, waotmax, aotmax):
        """
        @brief Run the WeightAOT-App
        """

        appName = "WeightAOT"
        args = ["-in", str(aotmsk),
                "-xml", str(xmlInput),
                "-out", str(weightAot),
                "-waotmin", str(waotmin),
                "-waotmax", str(waotmax),
                "-aotmax", str(aotmax)]
        self.runOTBApplication(appName, args)
        return

    def totalWeight(self, xmlInput, weightAot, weightClouds, l3adate, halfsynthesis, wdatemin, out):
        """
        @brief Run the TotalWeight-App
        """

        appName = "TotalWeight"
        args = ["-xml", str(xmlInput),
                "-waotfile", str(weightAot),
                "-wcldfile", str(weightClouds),
                "-l3adate", str(l3adate),
                "-halfsynthesis", str(halfsynthesis),
                "-wdatemin", str(wdatemin),
                "-out", out]
        self.runOTBApplication(appName, args)
        return

    def updateSynthesis(self, platform, reflsIn, xmlInput, cldmsk, watmsk, snwmsk, weightl2a, previousL3Product, finishedL3Product, out):
        """
        @brief Run the UpdateSynthesis-App
        """

        appName = "UpdateSynthesis"
        args = ["-inr1", str(reflsIn[0]),
                "-xml", str(xmlInput),
                "-cld", str(cldmsk),
                "-wat", str(watmsk),
                "-snw", str(snwmsk),
                "-weightl2a", str(weightl2a),
                "-outr1", str(out[0])]

        if(previousL3Product):
                args += ["-prevproductr1", previousL3Product[0]]
        elif(finishedL3Product):
                args += ["-prevl3weightsr1", previousL3Product[0],
                         "-prevl3datesr1", previousL3Product[1],
                         "-prevl3reflr1", finishedL3Product[2],
                         "-prevl3flagsr1", finishedL3Product[3]]
        if(platform == self.s2Platform):
            args += ["-inr2", str(reflsIn[1]),
                         "-outr2", str(out[1])]
            if(previousL3Product):
                args += ["-prevproductr2", previousL3Product[1]]
            elif(finishedL3Product):
                args += [
                         "-prevl3weightsr2", finishedL3Product[4],
                         "-prevl3datesr2", finishedL3Product[5],
                         "-prevl3reflr2", finishedL3Product[6],
                         "-prevl3flagsr2", finishedL3Product[7]]
        self.runOTBApplication(appName, args)
        return

    def productFormatter(self, dirrCorr, platform, destination, syntdate, begin, end, xmllist, vcurrent, gipp, cog, cogtemp):
        """
        @brief Run the ProductFormatter-App
        """

        appName = "ProductFormatter"
        args = ["-products", str(dirrCorr[0]), "" if platform == self.venusPlatform else str(dirrCorr[1]),
                "-platform", str(platform),
                "-destination", str(destination),
                "-syntdate", str(syntdate),
                "-begin", str(begin),
                "-end", str(end),
                "-vcurrent", str(vcurrent),
                "-gipp", gipp,
                "-cog", str(1 if cog == True else 0),
                "-cogtemp", str(cogtemp),
                "-xml"] + xmllist

        self.runOTBApplication(appName, args)
        return

    def getFilepath(self, folder, name, index, resolution = None):
        """
        @brief Get the path to a file
        @param folder the folder, where the file is
        @param name The name to the file
        @param index Index that will be prepended
        @param resolution List that can contain multiple resolution-indices
        @return If resolutions are given, a list with filenames is returned, otherwise just a string
        """

        splitname = os.path.splitext(name)
        if(resolution):
            return [(os.path.join(folder, str(index) + "_" + str(splitname[0]) + str(res) + str(splitname[1]))) for res in resolution]
        return os.path.join(folder, str(index) + "_" + str(name))

    def removeFile(self, filename):
        """
        @brief Removes a file
        @param filename the name of the file
        @return None, and no OSError is thrown if the file didn't exist.
        """
        try:
            os.remove(filename)
        except OSError:
            pass
        return

    def run(self):
        """
        @brief Run the whole synthesis. This is the function to run WASP completely:
                - Creates the output directories
                - Writes the GIPP file
                - Runs the Apps CompositePreprocessing, WeightOnClouds, WeightAOT,
                    TotalWeight and UpdateSynthis for each input product as a loop
                - Runs the ProductFormatter after all products have been looped over
                - Prints the total execution time for the whole synthesis.
        @note Currently, no Level-3 product can be given as an init.
        """

        totalTimeStart = timer()
        logging.info("================= This is WASP Version " + str(self.ExeVersion) + " =================")

        # =====================================================================
        # Output directory creation
        # =====================================================================
        self.createDirectory(self.args.out)
        self.createDirectory(self.args.tempout)
        gippPath = self.writeGIPP(self.args, self.args.tempout)
        previousL3AProduct = []
        finishedL3AProduct = self.getL3AProductPath(self.args.pathprevL3A)
        for index, xmlInput in enumerate(self.args.input):
            if(self.platform == self.s2Platform):
                dirrCorr = self.getFilepath(self.args.tempout, "CP_R.tif", index, resolution = [1,2])
            else:
                dirrCorr = self.getFilepath(self.args.tempout, "CP_XS.tif", index, resolution = [1])
            cldmsk = self.getFilepath(self.args.tempout, "cld10.tif", index)
            watmsk = self.getFilepath(self.args.tempout, "wat10.tif", index)
            snwmsk = self.getFilepath(self.args.tempout, "snw10.tif", index)
            aotmsk = self.getFilepath(self.args.tempout, "aot10.tif", index)

            self.compositePreprocessing(self.platform, xmlInput, self.args.scatteringcoeffpath, dirrCorr, cldmsk, watmsk, snwmsk, aotmsk)

            coarseres = self.args.coarseres
            sigmasmallcld = self.args.sigmasmallcld
            sigmalargecld = self.args.sigmalargecld
            kernelwidth = self.args.kernelwidth
            weightClouds = self.getFilepath(self.args.tempout, "WeightOnCloud.tif", index)
            cut = 1 if self.platform == self.s2Platform else 0
            self.weightOnClouds(cldmsk, coarseres, sigmasmallcld, sigmalargecld, kernelwidth, weightClouds, cut)

            waotmin = self.args.weightaotmin
            waotmax = self.args.weightaotmax
            aotmax = self.args.aotmax
            weightAot = self.getFilepath(self.args.tempout, "WeightAot.tif", index)

            self.weightAot(aotmsk, xmlInput, weightAot, waotmin, waotmax, aotmax)

            l3adate = self.datetimeToString(self.stringToDatetime(self.args.date), short=True) #Convert to short datetime
            halfsynthesis = self.args.synthalf
            wdatemin = self.args.weightdatemin
            weightTotal = self.getFilepath(self.args.tempout, "WeightTotal.tif", index)

            self.totalWeight(xmlInput, weightAot, weightClouds, l3adate, halfsynthesis, wdatemin, weightTotal)

            if(self.platform == self.s2Platform):
                updateSynthesis = self.getFilepath(self.args.tempout, "UpdateSynthesis_R.tif", index, resolution = [1,2])
            else:
                updateSynthesis = self.getFilepath(self.args.tempout, "UpdateSynthesis_XS.tif", index, resolution = [1])

            self.updateSynthesis(self.platform, dirrCorr, xmlInput, cldmsk, watmsk, snwmsk, weightTotal, previousL3AProduct, finishedL3AProduct, updateSynthesis)

            if(self.args.removeTemp):
                [self.removeFile(filename) for filename in dirrCorr]
                self.removeFile(cldmsk)
                self.removeFile(watmsk)
                self.removeFile(snwmsk)
                self.removeFile(aotmsk)
                self.removeFile(weightClouds)
                self.removeFile(weightAot)
                self.removeFile(weightTotal)
                [self.removeFile(filename) for filename in previousL3AProduct]

            previousL3AProduct = updateSynthesis
            pass

        platform = self.platform
        destination = self.args.out
        syntdate = self.args.date
        begin = self.args.mindate
        end = self.args.maxdate
        xmllist = self.args.input
        vcurrent = self.ParameterVersion.replace(".", "-")
        gipp = gippPath
        cog = self.args.cog
        cogtemp = self.args.tempout

        logging.info("COG: {0} Temp: {1}".format(cog ,self.args.tempout))
        self.productFormatter(updateSynthesis, platform, destination, syntdate, begin, end, xmllist, vcurrent, gipp, cog, cogtemp)
        if(self.args.removeTemp):
            [self.removeFile(filename) for filename in updateSynthesis]
            self.removeFile(gippPath)

        totalTimeEnd = timer()

        logging.info("================= WASP synthesis of " + str(len(self.args.input)) + " inputs finished in " + str(totalTimeEnd - totalTimeStart) + "s =================")
        return

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input",help="The Metadata input products in MUSCATE format. Required.", nargs="+", required=True, type=str)
    parser.add_argument("--verbose", help="Verbose output of the processing. Default is True", default="True", type=str)
    parser.add_argument("-o", "--out", help="Output directory. Required.", required=True, type=str)
    parser.add_argument("-t", "--tempout", help="Temporary output directory. If none is given, it is set to the value of --out", required=False, type=str)
    parser.add_argument("-v", "--version", help="Parameter version. Default is 1.0", required = False, type=str)
    parser.add_argument("-log", "--logging", help="Path to log-file. Default is in the current directory. If none is given, no log-file will be created.", required = False, default="", type=str)
    parser.add_argument("-d" ,"--date", help="L3A synthesis date in the format 'YYYYMMDD'. If none, then the middle date between all products is used", required=False, type=str)
    parser.add_argument("--synthalf", help="Half synthesis period in days. Default for S2 is 23, for Venus is 9", required=False, type=int)
    parser.add_argument("--pathprevL3A", help="Path to the previous L3A product folder. Does not have to be set.", required=False, type=str)
    parser.add_argument("-r", "--removeTemp", help="Removes the temporary created files after use. Default is true", required=False)
    parser.add_argument("--cog", help="Write the product conform to the CloudOptimized-Geotiff format. Default is false", required=False)
    parser.add_argument("--weightaotmin", help="AOT minimum weight. Default is 0.33", required=False, type=float)
    parser.add_argument("--weightaotmax", help="AOT maximum weight. Default is 1", required=False, type=float)
    parser.add_argument("--aotmax", help="AOT Maximum value. Default is 0.8", required=False, type=float)
    parser.add_argument("--coarseres", help="Resolution for Cloud weight resampling. Default is 240" , required=False, type=int)
    parser.add_argument("--kernelwidth", help="Kernel width for the Cloud Weight Calculation. Default is 801", required=False, type=int)
    parser.add_argument("--sigmasmallcld", help="Sigma for small Clouds. Default is 2", required=False, type=float)
    parser.add_argument("--sigmalargecld",  help="Sigma for large Clouds. Default is 10", required=False, type=float)
    parser.add_argument("--weightdatemin", help="Minimum Weight for Dates. Default is 0.5", required=False, type=float)
    parser.add_argument("--nthreads", help="Number of threads to be used for running the chain. Default is 8.", required=False, type=int)
    parser.add_argument("--scatteringcoeffpath", help="Path to the scattering coefficients files. If none, it will be searched for using the OTB-App path. Only has to be set for testing-purposes", required=False, type=str)

    args = parser.parse_args()
    TemporalSynthesisApplication = TemporalSynthesis(args)
    TemporalSynthesisApplication.run()
