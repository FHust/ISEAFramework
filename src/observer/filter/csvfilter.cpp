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
* File Name : csvfilter.cpp
* Creation Date : 18-11-2012
* Last Modified : Fr 04 Sep 2015 10:26:03 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "csvfilter.h"


namespace observer
{
#ifndef __NO_STRING__
template class CsvFilterBase< myMatrixType, electrical::TwoPort, PreparationType >;
template class CsvFilterBase< double, thermal::ThermalElement, ThermalPreperation >;
#endif /* __NO_STRING__ */
} /*namespace*/
