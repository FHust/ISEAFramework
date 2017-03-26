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
* File Name : srcmatrixInclude.cpp
* Creation Date : 11-02-2015
* Last Modified : Mi 11 Feb 2015 18:54:24 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "matrixInclude.h"

#ifdef _EIGEN_
template <>
Eigen::SparseMatrix<double, RowMajor>& CopyTopToMatrixLeft(Eigen::SparseMatrix<double, RowMajor> &target, const Eigen::SparseMatrix<double, RowMajor> &source, size_t  length)
{
    target.topRows(length) = source;
    return target;
}
#endif
