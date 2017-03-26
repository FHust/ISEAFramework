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
* File Name : dae_sys.h
* Creation Date : 03-11-2012
* Last Modified : Mo 06 Jun 2016 13:29:07 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _DIFFERNTIALALGEBRAICSYSTEM_
#define _DIFFERNTIALALGEBRAICSYSTEM_

// STD
#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif /* __EXCEPTIONS__ */

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../misc/matrixInclude.h"
#include "../misc/fast_copy_matrix.h"
#include "stateSystemGroup.h"
#include "generalizedsystem.h"

/// Namespace for system objects
namespace systm
{
/// Class for creating a Differential Algebraic System from a electric circuit
template < typename T = myMatrixType >
class DifferentialAlgebraicSystem : public GeneralizedSystem< T >
{
    public:
    DifferentialAlgebraicSystem( StateSystemGroup< T >* stateSystemGroup );
    ~DifferentialAlgebraicSystem(){};

    const T& GetA() const { return mMatrixA; }    ///< Get MatrixA
    const T& GetC() const { return mVectorC; }    ///< Get VectorC

    void PrepareEquationSystem();
    void operator()( const T& x, T& dxdt, const double /* t */ );
    void operator()( const std::vector< double >& x, std::vector< double >& dxdt, const double /* t */ );

    void CopyDGLResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount, const T& sourceMatrix,
                                          const T& sourceVector );
    void CopyALGResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount, const T& sourceMatrix,
                                          const T& sourceVector, const T& invertedMat );

    virtual const char* GetName() const;

    bool FoundNonZeros( const T& matrix ) const;
    void ThrowIfMatrixIsNonZero( const T& matrix ) const;

    private:
    void CalculateInitialState();    ///< output x vector with all DGL values set to zero
    //        void ODEifyEquations(); ///<  make ode equations out of the linear ones (alg1,alg2)
    //        void PrepareStateVector();

    T mMatrixA;
    T mVectorC;
    StateSystemGroup< T >* mStateSystemGroup;
    StateSystem< T >* mDglStateSystem;
    StateSystem< T >* mAlgStateSystem;
};

template < typename T >
DifferentialAlgebraicSystem< T >::DifferentialAlgebraicSystem( StateSystemGroup< T >* stateSystemGroup )
    : GeneralizedSystem< T >()
    , mMatrixA( T() )
    , mStateSystemGroup( stateSystemGroup )
    , mDglStateSystem( &stateSystemGroup->mDglStateSystem )
    , mAlgStateSystem( &stateSystemGroup->mAlgStateSystem )
{
    const size_t stateCount = mStateSystemGroup->GetStateCount();
    mMatrixA.zeros( stateCount, stateCount );    //< Initialize the size and set matrix A to 0
    mVectorC.zeros( stateCount, 1 );             //< Initialize the size and set vector C to 0
    CalculateInitialState();
}

template < typename T >
void DifferentialAlgebraicSystem< T >::PrepareEquationSystem()    ///<  make ode equations out of the linear ones (alg1,alg2)
{
#ifndef _SYMBOLIC_
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( dglUIDCount == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

    const T& dglMatrixA = mStateSystemGroup->mDglStateSystem.GetEquationSystemAMatrix();
    const T& dglVectorC = mDglStateSystem->GetEquationSystemCVector();

    CopyDGLResultsToMatrixAndVector( dglUIDCount, algUIDCount, dglMatrixA, dglVectorC );

    if ( algUIDCount == 0 )
        return;

    const T& algVectorC = mAlgStateSystem->GetEquationSystemCVector();
    const T alg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().submat( 0, 0, algUIDCount - 1, dglUIDCount - 1 );
    const T alg2MatrixA =
     mAlgStateSystem->GetEquationSystemAMatrix().submat( 0, dglUIDCount, algUIDCount - 1, dglUIDCount + algUIDCount - 1 );

    // CalculateInitialStateFromCurrentState
    T& stateVector = mStateSystemGroup->mStateVector;
    T invertedMat = solve( alg2MatrixA, -1 * algVectorC - alg1MatrixA * ( stateVector.submat( 0, 0, dglUIDCount - 1, 0 ) ) );
    //.....................................

    const T invAlg2MinusAlg1 = inv( alg2MatrixA ) * ( -1.0 * alg1MatrixA );
    const T DGLifiedA = invAlg2MinusAlg1 * dglMatrixA;
    const T DGLifiedC = invAlg2MinusAlg1 * dglVectorC;

    CopyALGResultsToMatrixAndVector( dglUIDCount, algUIDCount, DGLifiedA, DGLifiedC, invertedMat );
#endif
}

template < typename T >
void DifferentialAlgebraicSystem< T >::operator()( const T& x, T& dxdt, const double /* t */ )
{
    dxdt = mMatrixA * x + mVectorC;
}

template < typename T >
void DifferentialAlgebraicSystem< T >::operator()( const std::vector< double >& x, std::vector< double >& dxdt, const double /* t */ )
{
#ifndef _SYMBOLIC_
    const size_t stateCount = mStateSystemGroup->GetStateCount();

    // Store x into temporary states
    T tmpX( stateCount, 1 );
    misc::FastCopyMatrix( tmpX, &x[0], stateCount );

    // Calculate dxdt
    T tmpDxdt( stateCount, 1 );
    operator()( tmpX, tmpDxdt, 0.0 );

    // Store result back into dxdt
    dxdt.resize( stateCount );
    misc::FastCopyMatrix( &dxdt[0], tmpDxdt, stateCount );
#endif
}

template < typename T >
void DifferentialAlgebraicSystem< T >::CalculateInitialState()    ///< output x vector with all DGL values set to zero
{
#ifndef _SYMBOLIC_
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( algUIDCount == 0 )
        return;

    const T& algVectorC = mAlgStateSystem->GetEquationSystemCVector();
    const T alg2MatrixA =
     mAlgStateSystem->GetEquationSystemAMatrix().submat( 0, dglUIDCount, algUIDCount - 1, dglUIDCount + algUIDCount - 1 );
    ThrowIfMatrixIsNonZero( alg2MatrixA );


    T& stateVector = mStateSystemGroup->mStateVector;

    T invertedMat = solve( alg2MatrixA, algVectorC );    // -1 is beeing put later
    for ( size_t i = 0; i < algUIDCount; ++i )           // is faster than memcpy ...?
        stateVector( dglUIDCount + i, 0 ) = -invertedMat( i, 0 );
#endif
}

template < typename T >
void DifferentialAlgebraicSystem< T >::ThrowIfMatrixIsNonZero( const T& matrix ) const
{
    if ( !FoundNonZeros( matrix ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlgNotInvertable" );
}

template < typename T >
bool DifferentialAlgebraicSystem< T >::FoundNonZeros( const T& matrix ) const
{
    for ( size_t x = 0; x < matrix.n_rows; ++x )
    {
        for ( size_t y = 0; y < matrix.n_cols; ++y )
        {
            if ( matrix( x, y ) != 0.0 )
                return true;
        }
    }
    return false;
}


template < typename T >
void DifferentialAlgebraicSystem< T >::CopyDGLResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                        const T& sourceMatrix, const T& sourceVector )
{
    for ( size_t i = 0; i < dglUIDCount; ++i )
    {
        for ( size_t j = 0; j < algUIDCount + dglUIDCount; ++j )
            mMatrixA( i, j ) = sourceMatrix( i, j );

        mVectorC( i, 0 ) = sourceVector( i, 0 );
    }
}

template < typename T >
void DifferentialAlgebraicSystem< T >::CopyALGResultsToMatrixAndVector( const size_t dglUIDCount,
                                                                        const size_t algUIDCount, const T& sourceMatrix,
                                                                        const T& sourceVector, const T& invertedMat )
{
    T& stateVector = mStateSystemGroup->mStateVector;
    for ( size_t i = 0; i < algUIDCount; ++i )
    {
        stateVector( dglUIDCount + i, 0 ) = invertedMat( i, 0 );

        for ( size_t j = 0; j < algUIDCount + dglUIDCount; ++j )
            mMatrixA( i + dglUIDCount, j ) = sourceMatrix( i, j );

        mVectorC( i + dglUIDCount, 0 ) = sourceVector( i, 0 );
    }
}

template < typename T >
const char* DifferentialAlgebraicSystem< T >::GetName() const
{
    return "DifferentialAlgebraicSystem";
}

#ifdef _ARMADILLO_

template <>
void DifferentialAlgebraicSystem< Mat< double > >::CopyALGResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                                    const Mat< double >& sourceMatrix,
                                                                                    const Mat< double >& sourceVector,
                                                                                    const Mat< double >& invertedMat );

template <>
void DifferentialAlgebraicSystem< Mat< double > >::CopyDGLResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                                    const Mat< double >& sourceMatrix,
                                                                                    const Mat< double >& sourceVector );


/// Class for creating a Differential Algebraic System from a electric circuit
template <>
class DifferentialAlgebraicSystem< arma::SpMat< double > > : public GeneralizedSystem< arma::SpMat< double > >
{
    public:
    DifferentialAlgebraicSystem( StateSystemGroup< arma::SpMat< double > >* stateSystemGroup );
    ~DifferentialAlgebraicSystem(){};

    const arma::SpMat< double > GetA() const;    ///< Get MatrixA
    const arma::SpMat< double > GetC() const;    ///< Get VectorC
    void PrepareEquationSystem();
    void operator()( const arma::SpMat< double >& x, arma::SpMat< double >& dxdt, const double /* t */ );
    void operator()( const arma::Mat< double >& x, arma::Mat< double >& dxdt, const double /* t */ );
    void operator()( const std::vector< double >& x, std::vector< double >& dxdt, const double /* t */ );

    virtual const char* GetName() const;

    private:
    void CalculateInitialState();    ///< output x vector with all DGL values set to zero
    //        void ODEifyEquations(); ///<  make ode equations out of the linear ones (alg1,alg2)

    arma::Mat< double > mMatrixA;
    arma::Mat< double > mVectorC;
    StateSystemGroup< arma::SpMat< double > >* mStateSystemGroup;
    StateSystem< arma::SpMat< double > >* mDglStateSystem;
    StateSystem< arma::SpMat< double > >* mAlgStateSystem;
};

#endif /* _ARMADILLO_ */


#ifdef _EIGEN_

/// Class for creating a Differential Algebraic System from a electric circuit
template <>
class DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > > : public GeneralizedSystem< SparseMatrix< double, RowMajor > >
{
    typedef SparseMatrix< double, RowMajor > MatrixType;

    public:
    DifferentialAlgebraicSystem( StateSystemGroup< MatrixType >* stateSystemGroup );
    ~DifferentialAlgebraicSystem(){};

    void PrepareEquationSystem();
    void operator()( const MatrixType& x, MatrixType& dxdt, const double /* t */ );
    void operator()( const MatrixType& x, MatrixXd& dxdt, const double /* t */ );
    void operator()( const arma::Mat< double >& x, arma::Mat< double >& dxdt, const double /* t */ );
    void operator()( const std::vector< double >& x, std::vector< double >& dxdt, const double /* t */ );

    const MatrixType GetA() const;    ///< Get MatrixA
    const MatrixType GetC() const;    ///< Get VectorC

    virtual const char* GetName() const;

    private:
    void CalculateInitialState();    ///< output x vector with all DGL values set to zero
                                     //        void PrepareStateVector();

    MatrixType mAlg1MatrixA;
    MatrixType mAlg2MatrixA;

    StateSystemGroup< MatrixType >* mStateSystemGroup;
    StateSystem< MatrixType >* mDglStateSystem;
    StateSystem< MatrixType >* mAlgStateSystem;
};

#endif /* _EIGEN_ */

} /* END NAMESPACE */

#endif /* _DIFFERNTIALALGEBRAICSYSTEM_ */
