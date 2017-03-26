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
 * matrixInclude.h
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#ifndef MATRIXINCLUDE_H_
#define MATRIXINCLUDE_H_

#ifdef _ARMADILLO_
#include <armadillo>
using namespace arma;
#include "../container/armadillo_wrapper.h"

#ifdef SPARSE_MATRIX_FORMAT
typedef arma::SpMat< double > myMatrixType;
#else
typedef arma::Mat< double > myMatrixType;
#endif
#elif _EIGEN_
#include <armadillo>
#include "../container/armadillo_wrapper.h"
#include <eigen3/Eigen/Sparse>
#include "../container/eigen_wrapper.h"
#include <eigen3/Eigen/Cholesky>
#include <eigen3/Eigen/LU>
#include <eigen3/Eigen/SparseLU>
using namespace Eigen;
typedef Eigen::SparseMatrix< double, RowMajor > myMatrixType;
#elif _SYMBOLIC_
#include "../stub/armadillo.h"
#include "symbolic.h"
typedef symbolic::SymbolicMatrix myMatrixType;
using namespace symbolic;
#else
#Error Either _ARMADILLO_ or _EIGEN_ must be defined
#endif

template < typename MatrixType >
MatrixType &CopyTopToMatrixLeft( MatrixType &target, const MatrixType &source, size_t length )
{
    for ( size_t i = 0; i < length; ++i )
    {
        target( i, 0 ) = source( i, 0 );
    }
    return target;
}

#ifdef _EIGEN_
template <>
Eigen::SparseMatrix< double, RowMajor > &CopyTopToMatrixLeft( Eigen::SparseMatrix< double, RowMajor > &target,
                                                              const Eigen::SparseMatrix< double, RowMajor > &source, size_t length );
#endif


#endif /* MATRIXINCLUDE_H_ */
