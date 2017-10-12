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
/*
 * electricalclasswrapper.cpp
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#include "electricalclasswrapper.h"

#include "../../misc/matrixInclude.h"

namespace factory
{

template class Factory< electrical::TwoPort< myMatrixType >, ArgumentTypeElectrical >;

template class ElectricalClassWrapper< myMatrixType, electrical::Capacity >;
template class ElectricalClassWrapper< myMatrixType, electrical::OhmicResistance >;
template class ElectricalClassWrapper< myMatrixType, electrical::VoltageSource >;
template class ElectricalClassWrapper< myMatrixType, electrical::ParallelRC >;
template class ElectricalClassWrapper< myMatrixType, electrical::ParallelRCAlg >;
template class ElectricalClassWrapper< myMatrixType, electrical::Cellelement >;
template class ElectricalClassWrapper< myMatrixType, electrical::Zarc >;
// template class ElectricalClassWrapper< myMatrixType, electrical::ZarcAlg >;
template class ElectricalClassWrapper< myMatrixType, electrical::ParallelTwoPort >;
template class ElectricalClassWrapper< myMatrixType, electrical::SerialTwoPort >;
template class ElectricalClassWrapper< myMatrixType, electrical::WarburgTanh >;
template class ElectricalClassWrapper< myMatrixType, electrical::WarburgCotanh >;
}
