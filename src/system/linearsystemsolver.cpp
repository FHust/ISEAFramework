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
/*
 * linearsystemsolver.cpp
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

#include "linearsystemsolver.h"

template class systm::LinearSystemSolver< myMatrixType>;

namespace systm
{

#ifdef _EIGEN_
template< >
double LinearSystemSolver< Eigen::SparseMatrix<double, RowMajor> >::Solve()
{
    if(linAlgSystem->GetEquationCount())
    {
        Eigen::SparseMatrix<double, RowMajor> realStat = this->mStateVector.topRows( this->mStateVector.n_rows - 1);
        linAlgSystem->SolveForStateVector( realStat );
        this->mStateVector.topRows( this->mStateVector.n_rows - 1) = realStat;
    }
    this->mTime += this->mDt;

    this->ResetStateSystemGroup();

    return this->mTime;
}
#endif /* _EIGEN_ */
}
