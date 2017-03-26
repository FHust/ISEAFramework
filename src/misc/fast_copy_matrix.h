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
#ifndef _FAST_COPY_MATRIX_
#define _FAST_COPY_MATRIX_

#include "../misc/matrixInclude.h"
typedef double real_T;


namespace misc
{

/// Copies a matrix in Form of a column vector into a real_T array
template < typename Matrix >
void FastCopyMatrix( real_T *array, const Matrix &matrix, unsigned int numberOfElements )
{
    for ( unsigned int i = 0; i < numberOfElements; ++i )
        array[i] = matrix( i, 0 );
}

/// Copies a real_T array into a matrix in Form of a column vector
template < typename Matrix >
void FastCopyMatrix( Matrix &matrix, const real_T *array, unsigned int numberOfElements )
{
    for ( unsigned int i = 0; i < numberOfElements; ++i )
        matrix( i, 0 ) = array[i];
}

/// Fuses a matrix in Form of a colomn vector and a real_T array into a matrix in Form of a column vector
template < typename Matrix >
void FastCopyMatrix( Matrix &matrix, const Matrix &matrixToCopyFrom, unsigned int numberOfElementsMatrix,
                     const real_T *array, unsigned int numberOfElementsArray )
{
    for ( unsigned int i = 0; i < numberOfElementsMatrix; ++i )
        matrix( i, 0 ) = matrixToCopyFrom( i, 0 );
    for ( unsigned int i = 0; i < numberOfElementsArray; ++i )
        matrix( numberOfElementsMatrix + i, 0 ) = array[i];
}

#if defined( _ARMADILLO_ ) || defined( _EIGEN_ )
template <>
void FastCopyMatrix< myMatrixType >( real_T *array, const myMatrixType &matrix, unsigned int numberOfElements );
template <>
void FastCopyMatrix< myMatrixType >( myMatrixType &matrix, const real_T *array, unsigned int numberOfElements );
template <>
void FastCopyMatrix< myMatrixType >( myMatrixType &matrix, const myMatrixType &matrixToCopyFrom, unsigned int numberOfElementsMatrix,
                                     const real_T *array, unsigned int numberOfElementsArray );
#endif


}    // namespace misc
#endif
