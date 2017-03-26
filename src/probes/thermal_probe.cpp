/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
*/
/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : thermal_probe.cpp
* Creation Date : 04-03-2015
* Last Modified : Do 05 Mär 2015 12:12:41 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "thermal_probe.h"

namespace probe
{

ThermalProbe::ThermalProbe(const std::vector<double> &vec)
    :
        mProbeCoordinates(vec.at(0), vec.at(1), vec.at(2) )
{ }

ThermalProbe::ThermalProbe(const double x, const double y, const double z)
    :
        mProbeCoordinates(x, y, z )
{ }

} /* namespace probe */
