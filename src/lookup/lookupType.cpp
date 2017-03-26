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
#include "lookupType.h"

template class lookup::LookupType1D<double>;
template class lookup::NearestNeighbour1D<double>;
template class lookup::Linear1DInterpolation<double>;
template class lookup::LookupType2D<double>;
template class lookup::NearestNeighbour2D<double>;
template class lookup::Linear2DInterpolation<double>;

