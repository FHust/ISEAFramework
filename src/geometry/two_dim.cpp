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
#include "two_dim.h"

namespace geometry
{

template class geometry::TwoDim<double>;

template const TwoDim<double> operator+(const TwoDim<double> &lhs, const TwoDim<double> &rhs);
template const TwoDim<double> operator-(const TwoDim<double> &lhs, const TwoDim<double> &rhs);
template const TwoDim<double> operator/(const TwoDim<double> &lhs, double rhs);

}
