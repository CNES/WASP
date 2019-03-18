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

#ifndef DIRECTIONAL_MODEL_H
#define DIRECTIONAL_MODEL_H

/**
 * @brief The TemporalSynthesis namespace, covering all needed functions to execute this processing chain
 */
namespace ts {

/**
 * @brief Implementation of the directional model used
 */
class DirectionalModel {
public:
	/**
	 * @brief Constructor, initializing the four angles theta_(s,v) and phi_(s,v)
	 * @param theta_s
	 * @param phi_s
	 * @param theta_v
	 * @param phi_v
	 */
    DirectionalModel(double theta_s, double phi_s, double theta_v, double phi_v);
    double delta();
    double masse();
    double cos_xsi();
    double sin_xsi();
    double xsi();
    double cos_t();
    double sin_t();
    double t();
    double FV();
    double FR();
    double dir_mod(double kV,double kR);

private:
    double m_theta_s;
    double m_theta_v;
    double m_phi;

    // These variables are for optimizing the calculations to avoid repetitive operations.
    double m_sin_phi;
    double m_cos_phi;

    double m_sin_theta_s;
    double m_cos_theta_s;
    double m_tan_theta_s;

    double m_sin_theta_v;
    double m_cos_theta_v;
    double m_tan_theta_v;

    double m_delta_Val;
    double m_masse_Val;
    double m_cos_xsi_Val;
    double m_sin_xsi_Val;
    double m_xsi_Val;
    double m_cos_t_Val;
    double m_sin_t_Val;
    double m_t_Val;
    double m_FV_Val;
    double m_FR_Val;
};
} //namespace ts
#endif // DIRECTIONAL_MODEL
