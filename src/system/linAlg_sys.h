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
* File Name : linAlg_sys.h
* Creation Date : 04-09-2014
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SYMBOLIC_

#ifndef _LINEARALGEBRAISCHESSYSTEM_
#define _LINEARALGEBRAISCHESSYSTEM_

// BOOST
#include <boost/shared_ptr.hpp>

#include "../misc/matrixInclude.h"
#include "stateSystemGroup.h"
#include "generalizedsystem.h"


namespace systm
{

template < typename MatrixT >
class LinAlgSystem : public GeneralizedSystem< MatrixT >
{
    public:
    LinAlgSystem( StateSystemGroup< MatrixT > *stateSystemGroup );
    ~LinAlgSystem(){};

    void PrepareEquationSystem();
    void operator()( const MatrixT &x, MatrixT &dxdt, const double /* t */ );
    void operator()( const std::vector< double > &x, std::vector< double > &dxdt, const double /* t */ );
    void SolveForStateVector( MatrixT &x );

    size_t GetEquationCount();
    virtual const char *GetName() const;

    private:
    //        MatrixT mDxDt;
    StateSystemGroup< MatrixT > *mStateSystemGroup;
    StateSystem< MatrixT > &mAlgStateSystem;
};

template < typename MatrixT >
LinAlgSystem< MatrixT >::LinAlgSystem( StateSystemGroup< MatrixT > *stateSystemGroup )
    : GeneralizedSystem< MatrixT >()
    , mStateSystemGroup( stateSystemGroup )
    , mAlgStateSystem( stateSystemGroup->mAlgStateSystem )
//  , mDxDt( MatrixT())
{
    // const size_t stateCount = stateSystemGroup->GetStateCount();
    // mDxDt.zeros(stateCount ,1); //< Initialize the size and set matrix mDxDt to zero
}

template < typename MatrixT >
void LinAlgSystem< MatrixT >::operator()( const MatrixT & /* x */, MatrixT & /* dxdt */, const double /* t */ )
{
}

template < typename MatrixT >
void LinAlgSystem< MatrixT >::
operator()( const std::vector< double > & /* x */, std::vector< double > & /* dxdt */, const double /* t */ )
{
}

template < typename MatrixT >
void LinAlgSystem< MatrixT >::PrepareEquationSystem()
{
    if ( mAlgStateSystem.GetEquationCount() > 0 )
    {
        const size_t equationCount = GetEquationCount();
        const MatrixT &tmpMatrix =
         solve( mAlgStateSystem.GetEquationSystemAMatrix(), -1 * mAlgStateSystem.GetEquationSystemCVector() );
        CopyTopToMatrixLeft( mStateSystemGroup->mStateVector, tmpMatrix, equationCount );
        mStateSystemGroup->mStateVector( mStateSystemGroup->mStateVector.n_rows - 1, 0 ) = 1;
    }
}

template < typename MatrixT >
size_t LinAlgSystem< MatrixT >::GetEquationCount()
{
    return mAlgStateSystem.GetEquationCount();
};


template < typename MatrixT >
void LinAlgSystem< MatrixT >::SolveForStateVector( MatrixT &x )
{
    if ( mAlgStateSystem.GetEquationCount() > 0 )
        x = solve( mAlgStateSystem.GetEquationSystemAMatrix(), -1 * mAlgStateSystem.GetEquationSystemCVector() );
}

template < typename MatrixT >
const char *LinAlgSystem< MatrixT >::GetName() const
{
    return "LinAlgSystem";
}

} /* systm */

#endif /* _LINEARALGEBRAISCHESSYSTEM_ */

#endif    // _SYMBOLIC_
