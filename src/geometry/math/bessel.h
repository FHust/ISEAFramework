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
#ifndef _BESSEL_
#define _BESSEL_


namespace geometry
{
/// Bessel function of first kind with index 0 with valid domain in [0, 2.4048]
double Bessel0( double x );

/// Bessel function of first kind with index 2 with valid domain in [0, 5.1356]
double Bessel2( double x );

extern const double bessel0ZP;     ///<First zero point of bessel function of first kind with index 0
extern const double bessel2ZP;     ///<First zero point of bessel function of first kind with index 2
extern const double bessel2Max;    ///<First maximum point of bessel function of first kind with index 2
}
#endif
