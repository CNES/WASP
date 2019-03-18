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

import os
import argparse


class BaseComparison():
    WASP_TEST_ENV_VAR = "WASP_TEST"

    @staticmethod
    def createArgs(in_path, out_path, date = None, synthalf = None):
        args = argparse.Namespace()
        args.input = in_path
        args.out = out_path
        args.verbose = "True"
        args.removeTemp = "True"
        args.tempout = None
        args.version = "1.0"
        args.synthalf = synthalf
        args.date = date
        args.cog = "False"
        args.pathprevL3A = None
        args.weightaotmin = None
        args.weightaotmax = None
        args.aotmax = None
        args.coarseres = None
        args.kernelwidth = None
        args.sigmasmallcld = None
        args.sigmalargecld = None
        args.weightdatemin = None
        args.scatteringcoeffpath = None
        args.logging = "" #Disable logging
        return args

    def setupEnvironment(self):
        try:
            self.wasp_test_path = os.environ[self.WASP_TEST_ENV_VAR]
        except:
            raise ValueError("Cannot find env-variable " + self.WASP_TEST_ENV_VAR)

        self.execPath = os.getcwd()
        return
