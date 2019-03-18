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

class FR_LUS_20180315(unittest.TestCase, BaseComparison):
    test_name = "test_VNS_FR-LUS_4_20180207"
    out_path = os.path.join("VENUS-XS_20180207-000000-000_L3A_FR-LUS_C_V1-0",
                                "VENUS-XS_20180207-000000-000_L3A_FR-LUS_C_V1-0_MTD_ALL.xml")
    inputs = [os.path.join("VENUS-XS_20180212-105550-000_L2A_FR-LUS_C_V1-0","VENUS-XS_20180212-105550-000_L2A_FR-LUS_C_V1-0_MTD_ALL.xml"),
                os.path.join("VENUS-XS_20180210-105551-000_L2A_FR-LUS_C_V1-0","VENUS-XS_20180210-105551-000_L2A_FR-LUS_C_V1-0_MTD_ALL.xml"),
                os.path.join("VENUS-XS_20180208-105552-000_L2A_FR-LUS_C_V1-0","VENUS-XS_20180208-105552-000_L2A_FR-LUS_C_V1-0_MTD_ALL.xml"),
                os.path.join("VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0","VENUS-XS_20180202-105554-000_L2A_FR-LUS_C_V1-0_MTD_ALL.xml")]

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
        date = "20180207"
        synthalf = 5
        ts = WASP.TemporalSynthesis(self.createArgs(input_path, out, date, synthalf))
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
