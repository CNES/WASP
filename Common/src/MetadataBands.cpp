/*
 * Copyright (C) 2018-2019, Centre National d'Etudes Spatiales (CNES)
 * All rights reserved
 *
 * This file is part of Weighted Average Synthesis Processor (WASP)
 *
 * Authors:
 * - Peter KETTIG <peter.kettig@cnes.fr>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * See the LICENSE.md file for more details.
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

#include "MetadataBands.h"

using namespace ts;

int Resolution::getBandIndexByType(const std::string &type){
	for(size_t band = 0; band < m_bands.size(); band++){
		if(m_bands[band].getType() == type) return (int)band;
	}
	return -1;
}

bool Resolution::doesBandTypeExist(const std::string &type){
	if(getBandIndexByType(type) >= 0) return true;
	return false;
}


MultiResolution::MultiResolution(){
	m_resolutions = {};
}

void MultiResolution::addResolution(const Resolution &res){
	m_resolutions.emplace_back(res);
}

size_t MultiResolution::getNumberOfResolutions(){
	return m_resolutions.size();
}

size_t MultiResolution::getTotalNumberOfBands(){
	size_t total = 0;
	for(auto resolution : m_resolutions){
		total += resolution.getBands().size();
	}
	return total;
}

productReturnType MultiResolution::getNthResolutionFilenames(const size_t &n = 0){
	if(n > m_resolutions.size()){
		return {};
	}
	productReturnType filenames;
	for(auto band : m_resolutions[n].getBands()){
		filenames.emplace_back(band.getPath());
	}
	return filenames;
}

bool MultiResolution::doesBandTypeExist(const std::string &type) const{
	for(auto resolution : m_resolutions){
		for(auto band : resolution.getBands()){
			if(band.getType() == type) return true;
		}
	}
	return false;
}

std::string MultiResolution::getSpecificBandTypeFilename(const std::string &type){
	for(auto resolution : m_resolutions){
		for(auto band : resolution.getBands()){
			if(band.getType() == type) return band.getPath();
		}
	}
	return "";
}
