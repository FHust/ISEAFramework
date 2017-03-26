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
#include "cylindric.h"

namespace geometry
{

template class geometry::Cylindric<double>;
template const Cartesian<double> CylToCart(const Cartesian<double> &emplacementVector, const Cylindric<double> &cyl);

template const Cylindric<double> operator+(const Cylindric<double> &lhs, const Cylindric<double> &rhs);
template const Cylindric<double> operator/(const Cylindric<double> &lhs, double rhs);

}
