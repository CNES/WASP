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

class T33UUU_20150727(unittest.TestCase, BaseComparison):
    test_name = "test_S2A_T33UUU_8_20150727"
    out_path = os.path.join("SENTINEL2A_20150727-102256-960_L3A_T33UUU_C_V1-0",
                                "SENTINEL2A_20150727-102256-960_L3A_T33UUU_C_V1-0_MTD_ALL.xml")
    inputs = [os.path.join("SENTINEL2A_20150627-102531-456_L2A_T33UUU_D_V1-0", "SENTINEL2A_20150627-102531-456_L2A_T33UUU_D_V1-0_MTD_ALL.xml"),
                os.path.join("SENTINEL2A_20150704-101337-922_L2A_T33UUU_D_V1-0", "SENTINEL2A_20150704-101337-922_L2A_T33UUU_D_V1-0_MTD_ALL.xml"),
                os.path.join("SENTINEL2A_20150724-101008-461_L2A_T33UUU_D_V1-0", "SENTINEL2A_20150724-101008-461_L2A_T33UUU_D_V1-0_MTD_ALL.xml"),
                os.path.join("SENTINEL2A_20150806-102012-461_L2A_T33UUU_D_V1-0", "SENTINEL2A_20150806-102012-461_L2A_T33UUU_D_V1-0_MTD_ALL.xml"),
                os.path.join("SENTINEL2A_20150813-101020-457_L2A_T33UUU_D_V1-0", "SENTINEL2A_20150813-101020-457_L2A_T33UUU_D_V1-0_MTD_ALL.xml"),
                os.path.join("SENTINEL2A_20150816-102023-459_L2A_T33UUU_D_V1-0", "SENTINEL2A_20150816-102023-459_L2A_T33UUU_D_V1-0_MTD_ALL.xml"),
                os.path.join("SENTINEL2A_20150823-101019-462_L2A_T33UUU_D_V1-0", "SENTINEL2A_20150823-101019-462_L2A_T33UUU_D_V1-0_MTD_ALL.xml"),
                os.path.join("SENTINEL2A_20150826-102022-464_L2A_T33UUU_D_V1-0", "SENTINEL2A_20150826-102022-464_L2A_T33UUU_D_V1-0_MTD_ALL.xml")]

    def setUp(self):
        self.setupEnvironment()

    def test_run(self):
        input_path = [os.path.join(self.wasp_test_path,
                                  self.test_name, "INPUTS", i) for i in self.inputs]
        print('[%s]' % ', '.join(map(str, input_path)))
        for xml in input_path:
            if(not self.assertTrue(os.path.exists(xml))):
                print("Cannot find %s".format(xml))
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
