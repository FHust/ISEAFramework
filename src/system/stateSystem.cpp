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
 * systemlinearequations.cpp
 *
 *  Created on: 03.11.2013
 *      Author: chris
 */

#include "stateSystem.h"

namespace systm
{

template class systm::StateSystem< myMatrixType >;


#ifdef _EIGEN_
template <>
void StateSystem< SparseMatrix< double, RowMajor > >::AddEquations( size_t equationNumber,
                                                                    const SparseMatrix< double, RowMajor > &matrix )
{
    mMatrixA.middleRows( equationNumber, matrix.n_rows ) = matrix.leftCols( mMatrixA.n_cols );
    mVectorC.middleRows( equationNumber, matrix.n_rows ) = matrix.rightCols( 1 );
}

template <>
void StateSystem< SparseMatrix< double, RowMajor > >::AddEquations( size_t equationNumber,
                                                                    const SparseMatrix< double, RowMajor > &matrix,
                                                                    const SparseMatrix< double, RowMajor > &vectorC )
{
    mMatrixA.middleRows( equationNumber, matrix.n_rows ) = matrix;
    mVectorC.middleRows( equationNumber, matrix.n_rows ) = vectorC;
}

template <>
void StateSystem< SparseMatrix< double, RowMajor > >::ResetSystem()
{
    mMatrixA.setZero();
    mVectorC.setZero();
}
#endif /* _EIGEN_ */


#ifdef _ARMADILLO_


/// Standard constructor for empty StateSystem
StateSystem< arma::SpMat< double > >::StateSystem()
    : mEquationCount( 0 )
    , mStateCount( 0 )
    , mIsInitialized( false )
    , mMatrixANeedsUpdate( true )
    , mMatrixCNeedsUpdate( true )
{
    ResetSystem();
}

/// Constructs a system with a given matrix's data
StateSystem< arma::SpMat< double > >::StateSystem( const arma::SpMat< double > &matrixA, const arma::SpMat< double > &vectorC )
    : mMatrixA( matrixA )
    , mVectorC( vectorC )
    , mEquationCount( matrixA.n_rows )
    , mStateCount( matrixA.n_cols )
    , mIsInitialized( true )
    , mMatrixANeedsUpdate( true )
    , mMatrixCNeedsUpdate( true )
{
}

void StateSystem< arma::SpMat< double > >::AddPartition( size_t &rowOffset, size_t startRow, size_t endRow )
{
    const size_t rowSpan = endRow - startRow;
    const size_t partitionIndex = mSubMatrixARawDataVector.size();

    if ( !rowSpan )
        return;

    rowOffset += rowSpan;

    std::vector< size_t > tmp( rowSpan, partitionIndex );
    mPartitionLookupVector.insert( mPartitionLookupVector.end(), tmp.begin(), tmp.end() );

    mSubMatrixARawDataVector.push_back( RawSparseData() );
    mSubVectorCRawDataVector.push_back( RawSparseData() );
    mMatrixANeedsUpdate = true;
    mMatrixCNeedsUpdate = true;
}

void StateSystem< arma::SpMat< double > >::PartitionSystem( std::vector< std::pair< size_t, size_t > > partitions )    // index = workerid ^ value = rows
{
    size_t rowOffset = 0;

    mMatrixANeedsUpdate = true;
    mMatrixCNeedsUpdate = true;

    mPartitionLookupVector.clear();
    mSubMatrixARawDataVector.clear();
    mSubVectorCRawDataVector.clear();

    for ( size_t i = 0; i < partitions.size(); ++i )
    {
        const size_t startRow = partitions.at( i ).first;
        const size_t endRow = partitions.at( i ).second;

        if ( startRow > rowOffset )
        {
            AddPartition( rowOffset, rowOffset, startRow );
        }

        AddPartition( rowOffset, startRow, endRow );
    }

    AddPartition( rowOffset, rowOffset, mEquationCount );
}

/// Set one or more lines in the matrix
void StateSystem< arma::SpMat< double > >::AddEquations( size_t equationNumber, const arma::SpMat< double > &matrix )
{
    size_t partitionIndex = mPartitionLookupVector.at( equationNumber );
    mSubMatrixARawDataVector.at( partitionIndex ).Add( matrix.cols( 0, matrix.n_cols - 2 ), equationNumber );
    mSubVectorCRawDataVector.at( partitionIndex ).Add( matrix.col( matrix.n_cols - 1 ), equationNumber );

    mMatrixANeedsUpdate = true;
    mMatrixCNeedsUpdate = true;
}

/// Set one or more lines in the matrix
void StateSystem< arma::SpMat< double > >::AddEquations( size_t equationNumber, const arma::SpMat< double > &matrixA,
                                                         const arma::SpMat< double > &vectorC )
{
    size_t partitionIndex = mPartitionLookupVector.at( equationNumber );
    mSubMatrixARawDataVector.at( partitionIndex ).Add( matrixA, equationNumber );
    mSubVectorCRawDataVector.at( partitionIndex ).Add( vectorC, equationNumber );

    mMatrixANeedsUpdate = true;
    mMatrixCNeedsUpdate = true;
}

/// Reset the matrix to zeros (used for sparse only)
void StateSystem< arma::SpMat< double > >::ResetSystem()
{
    for ( size_t i = 0; i < mSubMatrixARawDataVector.size(); ++i )
    {
        mSubMatrixARawDataVector.at( i ).Clear();
        mSubVectorCRawDataVector.at( i ).Clear();
    }
    mMatrixA.zeros();
    mVectorC.zeros();
}

/// After all equations have been registered, this method will set the data sizes and calls itself the reset method
void StateSystem< arma::SpMat< double > >::Initialize( size_t stateCount )
{
    if ( mIsInitialized )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlreadyInitialized", "StateSystem" );

    mStateCount = stateCount;
    mMatrixA.zeros( mEquationCount, mStateCount );
    mVectorC.zeros( mEquationCount, 1 );

    mPartitionLookupVector = std::vector< size_t >( mEquationCount + 1, 0 );
    mSubMatrixARawDataVector.push_back( RawSparseData() );
    mSubVectorCRawDataVector.push_back( RawSparseData() );

    mIsInitialized = true;
}

/// Returns the constant part "C" of the matrix
const arma::SpMat< double > &StateSystem< arma::SpMat< double > >::GetEquationSystemCVector()
{
    if ( !mMatrixCNeedsUpdate )
        return mVectorC;
    mMatrixCNeedsUpdate = true;

    RawSparseData rawCData( mSubVectorCRawDataVector );
    rawCData.FillMatrix( mVectorC, mEquationCount, 1 );

    return mVectorC;
}

/// Returns the matrix part "A" of the matrix
const arma::SpMat< double > &StateSystem< arma::SpMat< double > >::GetEquationSystemAMatrix()
{
    if ( !mMatrixANeedsUpdate )
        return mMatrixA;
    mMatrixANeedsUpdate = true;

    RawSparseData rawAData( mSubMatrixARawDataVector );
    rawAData.FillMatrix( mMatrixA, mEquationCount, mStateCount );

    return mMatrixA;
}

/// Register a new equation and get its rownumber
size_t StateSystem< arma::SpMat< double > >::GetNewEquation()
{
    if ( mIsInitialized )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                             "CannotRegisterEquationAfterInitializationException" );

    return ++mEquationCount - 1;
}

/// The number of registered equations
size_t StateSystem< arma::SpMat< double > >::GetEquationCount() { return mEquationCount; }

#endif /* _ARMADILLO_ */

} /* namespace systm */
