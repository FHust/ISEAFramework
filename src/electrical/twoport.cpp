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
* File Name : twoport.cpp
* Creation Date : 13-03-2015
* Last Modified : Fr 14 Aug 2015 14:10:28 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "twoport.h"
namespace electrical
{
#ifdef _EIGEN_
template <>
double ReturnFirstElement( Eigen::SparseMatrix< double, RowMajor > &mat )
{
    return mat.coeff( 0, 0 );
}
#endif /* _EIGEN_ */
}
