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
#include "blockclasswrapper.h"

namespace factory
{

template class Factory< thermal::ThermalBlock<double>, ArgumentTypeThermalBlock<double> >;

template class BlockClassWrapper< double, thermal::RectangularBlock >;
template class BlockClassWrapper< double, thermal::QuadraticCellBlock >;
template class BlockClassWrapper< double, thermal::Supercap >;

}
