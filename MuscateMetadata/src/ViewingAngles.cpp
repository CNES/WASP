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

#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>

#include "ViewingAngles.hpp"
#include "MetadataUtil.hpp"
static void checkDimensions(std::string expectedColumnUnit,
                            std::string expectedColumnStep,
                            std::string expectedRowUnit,
                            std::string expectedRowStep,
                            size_t expectedHeight,
                            size_t expectedWidth,
                            const MuscateAngleList &grid)
{
    if (grid.ColumnUnit != expectedColumnUnit) {
        throw std::runtime_error("The angle grids must have the same column unit");
    }
    if (grid.ColumnStep != expectedColumnStep) {
        throw std::runtime_error("The angle grids must have the same column step");
    }
    if (grid.RowUnit != expectedRowUnit) {
        throw std::runtime_error("The angle grids must have the same row unit");
    }
    if (grid.RowStep != expectedRowStep) {
        throw std::runtime_error("The angle grids must have the same row step");
    }

    if (grid.Values.size() != expectedHeight) {
        throw std::runtime_error("The angle grids must have the same height");
    }

    for (const auto &row : grid.Values) {
        if (row.size() != expectedWidth) {
            throw std::runtime_error("The angle grids must have the same width");
        }
    }
}

static std::vector<std::vector<double> > makeGrid(size_t height, size_t width)
{
    std::vector<std::vector<double> > r(height);

    for (auto &row : r) {
        row.resize(width);
    }

    return r;
}

std::vector<MuscateBandViewingAnglesGrid> ComputeViewingAngles(const std::vector<MuscateViewingAnglesGrid> &angleGrids) {
    if (angleGrids.empty() || angleGrids.front().ViewIncidenceAnglesGrid.Zenith.Values.empty()) {
        return {};
    }

    const auto &firstGrid = angleGrids.front().ViewIncidenceAnglesGrid.Zenith;
    auto columnUnit = firstGrid.ColumnUnit;
    auto columnStep = firstGrid.ColumnStep;
    auto rowUnit = firstGrid.RowUnit;
    auto rowStep = firstGrid.RowStep;
    auto width = firstGrid.Values.front().size();
    auto height = firstGrid.Values.size();
    std::map<std::string, int> bandPos;
    std::map<std::string, MuscateBandViewingAnglesGrid> resultGrids;
    auto endBandPos = std::end(bandPos);
    auto endResultGrids = std::end(resultGrids);
    for (const auto &grid : angleGrids) {
        checkDimensions(columnUnit, columnStep, rowUnit, rowStep, height, width, grid.ViewIncidenceAnglesGrid.Zenith);
        checkDimensions(columnUnit, columnStep, rowUnit, rowStep, height, width, grid.ViewIncidenceAnglesGrid.Azimuth);
        auto itBandPos = bandPos.find(grid.bandID);
        if (itBandPos == endBandPos) {
            bandPos.emplace(grid.bandID, bandPos.size());
        }

        auto it = resultGrids.find(grid.bandID);
        if (it == endResultGrids) {
            resultGrids.emplace(
                grid.bandID,
                MuscateBandViewingAnglesGrid{ grid.bandID,
                                            { { columnUnit, columnStep, rowUnit, rowStep, makeGrid(height, width) },
                                              { columnUnit, columnStep, rowUnit, rowStep, makeGrid(height, width) } } });
        }
    }

    for (auto &resultGrid : resultGrids) {
        for (size_t j = 0; j < height; j++) {
            for (size_t i = 0; i < width; i++) {
                auto zenith = std::numeric_limits<double>::quiet_NaN();
                auto azimuth = std::numeric_limits<double>::quiet_NaN();

                for (const auto &grid : angleGrids) {
                    if (grid.bandID == resultGrid.first) {
                        auto z = grid.ViewIncidenceAnglesGrid.Zenith.Values[j][i];
                        auto a = grid.ViewIncidenceAnglesGrid.Azimuth.Values[j][i];

                        if (!std::isnan(z)) {
                            zenith = z;
                        }
                        if (!std::isnan(a)) {
                            azimuth = a;
                        }
                    }
                }

                resultGrid.second.Angles.Zenith.Values[j][i] = zenith;
                resultGrid.second.Angles.Azimuth.Values[j][i] = azimuth;
            }
        }
    }

    std::vector<MuscateBandViewingAnglesGrid> result(resultGrids.size());
    for (const auto &grid : resultGrids) {
        result[bandPos[grid.first]] = grid.second;
    }
    return result;
}
