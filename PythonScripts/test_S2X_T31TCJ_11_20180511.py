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

import unittest
import os
import WASP
import CompareL3AProducts
from base_comparison import BaseComparison

class T31TCJ_11_20180511(unittest.TestCase, BaseComparison):
    test_name = "test_S2X_T31TCJ_11_20180511"
    out_path = os.path.join("SENTINEL2X_20180510-224307-416_L3A_T31TCJ_C_V1-0",
                                "SENTINEL2X_20180510-224307-416_L3A_T31TCJ_C_V1-0_MTD_ALL.xml")
    inputs = [os.path.join("SENTINEL2A_20180418-104512-083_L2A_T31TCJ_D_V1-7", "SENTINEL2A_20180418-104512-083_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2A_20180421-105629-233_L2A_T31TCJ_D_V1-7", "SENTINEL2A_20180421-105629-233_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2A_20180424-110609-057_L2A_T31TCJ_D_V1-7", "SENTINEL2A_20180424-110609-057_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2A_20180511-105804-037_L2A_T31TCJ_D_V1-7", "SENTINEL2A_20180511-105804-037_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2A_20180521-105702-711_L2A_T31TCJ_D_V1-7", "SENTINEL2A_20180521-105702-711_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2B_20180419-110127-730_L2A_T31TCJ_D_V1-7", "SENTINEL2B_20180419-110127-730_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2B_20180423-104701-920_L2A_T31TCJ_D_V1-7", "SENTINEL2B_20180423-104701-920_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2B_20180426-105202-871_L2A_T31TCJ_D_V1-7", "SENTINEL2B_20180426-105202-871_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2B_20180506-105423-569_L2A_T31TCJ_D_V1-7", "SENTINEL2B_20180506-105423-569_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2B_20180519-110334-001_L2A_T31TCJ_D_V1-7", "SENTINEL2B_20180519-110334-001_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2B_20180523-104124-396_L2A_T31TCJ_D_V1-7", "SENTINEL2B_20180523-104124-396_L2A_T31TCJ_D_V1-7_MTD_ALL.xml"),
                    os.path.join("SENTINEL2B_20180602-104102-749_L2A_T31TCJ_D_V1-7", "SENTINEL2B_20180602-104102-749_L2A_T31TCJ_D_V1-7_MTD_ALL.xml")]
    def setUp(self):
        self.setupEnvironment()

    def test_run(self):
        input_path = [os.path.join(self.wasp_test_path,
                                  self.test_name, "INPUTS", i) for i in self.inputs]
        for xml in input_path:
            try:
                self.assertTrue(os.path.exists(xml))
            except AssertionError as e:
                print("Cannot find {0}".format(xml))
                exit(1)
        out = self.execPath
        ts = WASP.TemporalSynthesis(self.createArgs(input_path, out))
        ts.run()
        out_new = os.path.join(out, self.out_path)
        out_golden = os.path.join(self.wasp_test_path, self.test_name,
                                  self.out_path)
        self.assertTrue(os.path.exists(out_new))
        self.assertTrue(os.path.exists(out_golden))

        comparator = CompareL3AProducts.Comparator(out_golden, out_new)
        self.assertTrue(comparator.run())
        return


if __name__ == '__main__':
    unittest.main()
