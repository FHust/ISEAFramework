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
#include "thermal_structs.h"

namespace thermal
{

template struct TaylorData<double>;
template struct IndexedValue<double>;
template IndexedValue<double> MakeIndexedValue<double>(const size_t &index, const double &value);
template struct IndexedArea<double>;
template struct Linearization<double>;

template struct IndexIs<double>;

const double globalMaxValue = 1000000000.0;
const double globalMinValue = - globalMaxValue;

}
