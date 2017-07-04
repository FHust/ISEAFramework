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
* File Name : dae_sys.cpp
* Creation Date : 20-03-2013
* Last Modified : So 04 Jun 2017 23:34:39 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "dae_sys.h"
#include <cstring>

/// Namespace for system objects
namespace systm
{

#ifdef _ARMADILLO_

template <>
void DifferentialAlgebraicSystem< Mat< double > >::CopyALGResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                                    const Mat< double > &sourceMatrix,
                                                                                    const Mat< double > &sourceVector,
                                                                                    const Mat< double > &invertedMat )
{
    mStateSystemGroup->mStateVector.submat( dglUIDCount, 0, dglUIDCount + algUIDCount - 1, 0 ) = invertedMat;
    mMatrixA.submat( dglUIDCount, 0, dglUIDCount + algUIDCount - 1, algUIDCount + dglUIDCount - 1 ) = sourceMatrix;
    mVectorC.submat( dglUIDCount, 0, dglUIDCount + algUIDCount - 1, 0 ) = sourceVector;
}

template <>
void DifferentialAlgebraicSystem< Mat< double > >::CopyDGLResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                                    const Mat< double > &sourceMatrix,
                                                                                    const Mat< double > &sourceVector )
{
    mMatrixA.submat( 0, 0, dglUIDCount - 1, algUIDCount + dglUIDCount - 1 ) = sourceMatrix;
    mVectorC.submat( 0, 0, dglUIDCount - 1, 0 ) = sourceVector;
}

DifferentialAlgebraicSystem< arma::SpMat< double > >::DifferentialAlgebraicSystem( StateSystemGroup< arma::SpMat< double > > *stateSystemGroup )
    : GeneralizedSystem< arma::SpMat< double > >()
    , mMatrixA( arma::Mat< double >() )
    , mStateSystemGroup( stateSystemGroup )
    , mDglStateSystem( &stateSystemGroup->mDglStateSystem )
    , mAlgStateSystem( &stateSystemGroup->mAlgStateSystem )
{
    const size_t stateCount = mStateSystemGroup->GetStateCount();
    mMatrixA.zeros( stateCount, stateCount );
    mVectorC.zeros( stateCount, 1 );
    CalculateInitialState();
}

const arma::SpMat< double > DifferentialAlgebraicSystem< arma::SpMat< double > >::GetA() const    ///< Get MatrixA
{
    return arma::SpMat< double >( mMatrixA );
}

const arma::SpMat< double > DifferentialAlgebraicSystem< arma::SpMat< double > >::GetC() const    ///< Get VectorC
{
    return arma::SpMat< double >( mVectorC );
}

void DifferentialAlgebraicSystem< arma::SpMat< double > >::PrepareEquationSystem()    ///<  make ode equations out of the linear ones (alg1,alg2)
{
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( dglUIDCount == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

    mMatrixA.zeros();
    mVectorC.zeros();

    const arma::SpMat< double > &dglMatrixA = mStateSystemGroup->mDglStateSystem.GetEquationSystemAMatrix();
    const arma::SpMat< double > &dglVectorC = mDglStateSystem->GetEquationSystemCVector();

    for ( arma::SpMat< double >::const_iterator it = dglMatrixA.begin(); it != dglMatrixA.end(); it++ )
        mMatrixA( it.row(), it.col() ) = *it;

    for ( arma::SpMat< double >::const_iterator it = dglVectorC.begin(); it != dglVectorC.end(); it++ )
        mVectorC( it.row(), 0 ) = *it;

    if ( algUIDCount == 0 )
        return;

    const arma::SpMat< double > &algVectorC = mAlgStateSystem->GetEquationSystemCVector();
    const arma::SpMat< double > mAlg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().cols( 0, dglUIDCount - 1 );
    const arma::SpMat< double > mAlg2MatrixA =
     mAlgStateSystem->GetEquationSystemAMatrix().cols( dglUIDCount, dglUIDCount + algUIDCount - 1 );

    // CalculateInitialStateFromCurrentState
    arma::SpMat< double > &stateVector = mStateSystemGroup->mStateVector;
    arma::SpMat< double > invertedMat =
     solve( mAlg2MatrixA, -1 * algVectorC - mAlg1MatrixA * ( stateVector.rows( 0, dglUIDCount - 1 ) ) );
    for ( size_t i = 0; i < algUIDCount; ++i )
        stateVector( dglUIDCount + i, 0 ) = invertedMat( i, 0 );
    //.....................................

    const arma::SpMat< double > invAlg2MinusAlg1 = inv( mAlg2MatrixA ) * ( -1.0 * mAlg1MatrixA );
    const arma::SpMat< double > DGLifiedA = invAlg2MinusAlg1 * dglMatrixA;
    const arma::SpMat< double > DGLifiedC = invAlg2MinusAlg1 * dglVectorC;

    for ( arma::SpMat< double >::const_iterator it = DGLifiedA.begin(); it != DGLifiedA.end(); it++ )
        mMatrixA( it.row() + dglUIDCount, it.col() ) = *it;

    for ( arma::SpMat< double >::const_iterator it = DGLifiedC.begin(); it != DGLifiedC.end(); it++ )
        mVectorC( it.row() + dglUIDCount, 0 ) = *it;
}

void DifferentialAlgebraicSystem< arma::SpMat< double > >::
operator()( const arma::SpMat< double > &x, arma::SpMat< double > &dxdt, const double /* t */ )
{
    dxdt = mMatrixA * x + mVectorC;
}

void DifferentialAlgebraicSystem< arma::SpMat< double > >::
operator()( const arma::Mat< double > &x, arma::Mat< double > &dxdt, const double /* t */ )
{
    dxdt = mMatrixA * x + mVectorC;
}

void DifferentialAlgebraicSystem< arma::SpMat< double > >::
operator()( const std::vector< double > &x, std::vector< double > &dxdt, const double /* t */ )
{
    const size_t stateCount = mStateSystemGroup->GetStateCount();

    // Store x into temporary states
    arma::SpMat< double > tmpX( stateCount, 1 );
    misc::FastCopyMatrix( tmpX, &x[0], stateCount );

    // Calculate dxdt
    arma::SpMat< double > tmpDxdt( stateCount, 1 );
    operator()( tmpX, tmpDxdt, 0.0 );

    // Store result back into dxdt
    dxdt.resize( x.size() );
    misc::FastCopyMatrix( &dxdt[0], tmpDxdt, stateCount );
}

void DifferentialAlgebraicSystem< arma::SpMat< double > >::CalculateInitialState()    ///< output x vector with all DGL values set to zero
{
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( algUIDCount == 0 )
        return;

    const arma::SpMat< double > &algVectorC = mAlgStateSystem->GetEquationSystemCVector();
    const arma::SpMat< double > &mAlg2MatrixA =
     mAlgStateSystem->GetEquationSystemAMatrix().cols( dglUIDCount, dglUIDCount + algUIDCount - 1 );

    if ( mAlg2MatrixA.n_nonzero == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlgNotInvertable" );

    arma::SpMat< double > &stateVector = mStateSystemGroup->mStateVector;

    arma::SpMat< double > invertedMat = solve( mAlg2MatrixA, algVectorC );    // -1 is beeing put later
    for ( size_t i = 0; i < algUIDCount; ++i )                                // is faster than memcpy ...?
        stateVector( dglUIDCount + i, 0 ) = -invertedMat( i, 0 );
}

const char *DifferentialAlgebraicSystem< arma::SpMat< double > >::GetName() const
{
    return "DifferentialAlgebraicSystem";
}

#endif /* _ARMADILLO_ */

#ifdef _EIGEN_

DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::DifferentialAlgebraicSystem( StateSystemGroup< MatrixType > *stateSystemGroup )
    : GeneralizedSystem< SparseMatrix< double, RowMajor > >()
    , mStateSystemGroup( stateSystemGroup )
    , mDglStateSystem( &stateSystemGroup->mDglStateSystem )
    , mAlgStateSystem( &stateSystemGroup->mAlgStateSystem )
{
    CalculateInitialState();
}

void DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::PrepareEquationSystem()    ///<  make ode equations out of the linear ones (alg1,alg2)
{
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( dglUIDCount == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

    mAlg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().leftCols( dglUIDCount );
    mAlg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );

    if ( algUIDCount == 0 )
        return;

    // CalculateInitialStateFromCurrentState
    const MatrixType &xDgl = mStateSystemGroup->mStateVector.topRows( dglUIDCount );
    SparseMatrix< double, ColMajor > b = ( mAlg1MatrixA * xDgl );
    b += SparseMatrix< double, ColMajor >( mAlgStateSystem->GetEquationSystemCVector() );
    b *= -1;

    b.makeCompressed();
    mAlg2MatrixA.makeCompressed();

    SparseLU< SparseMatrix< double, ColMajor > > solver;

    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( mAlg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( mAlg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );

    SparseMatrix< double, ColMajor > ret = solver.solve( b );

    mStateSystemGroup->mStateVector.middleRows( dglUIDCount, algUIDCount ) = ret;
}

void DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::
operator()( const MatrixType &x, MatrixType &dxdt, const double /* t */ )
{
    dxdt.setZero();

    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();
    const MatrixType &dglMatrixA = mDglStateSystem->GetEquationSystemAMatrix();
    const MatrixType &dglVectorC = mDglStateSystem->GetEquationSystemCVector();

    MatrixType dxdt_dgl = dglMatrixA * x;
    dxdt_dgl += dglVectorC;

    dxdt.topRows( dglUIDCount ) = dxdt_dgl;

    if ( algUIDCount == 0 )
        return;

    SparseMatrix< double, ColMajor > b = -mAlg1MatrixA * dxdt_dgl;
    b.makeCompressed();

    SparseLU< SparseMatrix< double, ColMajor > > solver;

    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( mAlg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( mAlg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );

    SparseMatrix< double, ColMajor > ret = solver.solve( b );

    dxdt.bottomRows( algUIDCount ) = ret;
}

void DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::operator()( const MatrixType &x, MatrixXd &dxdt, const double /* t */ )
{
    dxdt.setZero();

    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();
    const MatrixType &dglMatrixA = mDglStateSystem->GetEquationSystemAMatrix();
    const MatrixType &dglVectorC = mDglStateSystem->GetEquationSystemCVector();

    MatrixType dxdt_dgl = dglMatrixA * x;
    dxdt_dgl += dglVectorC;

    dxdt.topRows( dglUIDCount ) = dxdt_dgl;

    if ( algUIDCount == 0 )
        return;

    SparseMatrix< double, ColMajor > b = -mAlg1MatrixA * dxdt_dgl;
    b.makeCompressed();

    SparseLU< SparseMatrix< double, ColMajor > > solver;

    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( mAlg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( mAlg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );

    SparseMatrix< double, ColMajor > ret = solver.solve( b );

    dxdt.bottomRows( algUIDCount ) = ret;
}


void DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::
operator()( const arma::Mat< double > &x, arma::Mat< double > &dxdt, const double /* t */ )
{
    // Read from arma matrix
    // TODO:Use memcpy in future
    MatrixType x2( x.n_rows, x.n_cols );
    for ( size_t i = 0; i < x.n_rows; ++i )
        x2( i, 0 ) = x( i, 0 );

    dxdt.resize( x.n_rows, x.n_cols );
    MatrixXd dxdt2( dxdt.n_rows, dxdt.n_cols );

    // Do the job
    operator()( x2, dxdt2, 0 );

    // Write back to arma matrix
    for ( size_t i = 0; i < dxdt.n_rows; ++i )
        dxdt( i, 0 ) = dxdt2( i, 0 );
}

void DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::
operator()( const std::vector< double > &x, std::vector< double > &dxdt, const double /* t */ )
{
    const size_t stateCount = mStateSystemGroup->GetStateCount();

    // Store x into temporary states
    SparseMatrix< double, RowMajor > tmpX( stateCount, 1 );
    misc::FastCopyMatrix( tmpX, &x[0], stateCount );

    // Calculate dxdt
    SparseMatrix< double, RowMajor > tmpDxdt( stateCount, 1 );
    operator()( tmpX, tmpDxdt, 0.0 );

    // Store result back into dxdt
    dxdt.resize( x.size() );
    misc::FastCopyMatrix( &dxdt[0], tmpDxdt, stateCount );
}

void DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::CalculateInitialState()    ///< output x vector with all DGL values set to zero
{
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( algUIDCount == 0 )
        return;

    mAlg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );
    if ( mAlg2MatrixA.nonZeros() == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlgNotInvertable" );

    SparseMatrix< double, ColMajor > b = mAlgStateSystem->GetEquationSystemCVector();
    b.makeCompressed();

    SparseLU< SparseMatrix< double, ColMajor > > solver;
    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( mAlg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( mAlg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );

    SparseMatrix< double, ColMajor > ret = solver.solve( b );

    mStateSystemGroup->mStateVector.middleRows( dglUIDCount, algUIDCount ) = -ret;
}

const char *DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::GetName() const
{
    return "DifferentialAlgebraicSystem";
}
#endif /* _EIGEN_ */


template class DifferentialAlgebraicSystem< myMatrixType >;

} /* END NAMESPACE */
