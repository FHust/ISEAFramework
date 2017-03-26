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
 * eigen_wrapper.h
 *
 *  Created on: 16.02.2014
 *      Author: chris
 */

#ifndef EIGEN_WRAPPER_H_
#define EIGEN_WRAPPER_H_

#ifdef _EIGEN_


#include <eigen3/Eigen/Sparse>

namespace Eigen
{

double ScalarMul( const SparseMatrix< double > &rowMatrix, const SparseMatrix< double > &colMatrix );

SparseMatrix< double > solve( const SparseMatrix< double > &matrixA, const SparseMatrix< double > &vectorC );

} /* namespace Eigen */


#endif /* _EIGEN_ */

#endif /* EIGEN_WRAPPER_H_ */
