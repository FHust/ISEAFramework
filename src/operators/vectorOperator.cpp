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
* File Name : vectorOperator.cpp
* Creation Date : 11-11-2012
* Last Modified : Sun 11 Nov 2012 01:37:28 AM CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "vectorOperator.h"

template std::vector<double> operator*( std::vector<double> vec,  const double scalar);
template std::vector< std::vector< double > > operator*( std::vector< std::vector< double > > vec,  const double scalar);

