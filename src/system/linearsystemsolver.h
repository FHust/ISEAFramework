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
 * linearsystemsolver.h
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

#ifndef LINEARSYSTEMSOLVER_H_
#define LINEARSYSTEMSOLVER_H_

// STD
#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif /* __EXCEPTIONS__ */

// BOOST
#include <boost/scoped_ptr.hpp>

// ETC
#include "systemSolver.h"
#include "linAlg_sys.h"
#include "../exceptions/error_proto.h"

namespace systm
{

/// Solver for linear systems using gauss.
template < typename T >
class LinearSystemSolver : public SystemSolver< T >
{
    public:
    LinearSystemSolver( StateSystemGroup< T >* stateSystemGroup, double dt );

    virtual ~LinearSystemSolver() {}

    virtual double Solve();

    private:
    boost::scoped_ptr< systm::LinAlgSystem< T > > linAlgSystem;
};

template < typename T >
LinearSystemSolver< T >::LinearSystemSolver( StateSystemGroup< T >* stateSystemGroup, double dt )
    : SystemSolver< T >( stateSystemGroup, dt )
    , linAlgSystem( new systm::LinAlgSystem< T >( stateSystemGroup ) )
{
    if ( dt == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DtIsZero" );
}

template < typename T >
double LinearSystemSolver< T >::Solve()
{
    if ( linAlgSystem->GetEquationCount() )
    {
        T realStat = this->mStateVector.submat( 0, 0, this->mStateVector.n_rows - 1, 0 );
        linAlgSystem->SolveForStateVector( realStat );

        for ( size_t i = 0; i < realStat.n_rows; ++i )
            this->mStateVector( i, 0 ) = realStat( i, 0 );
    }
    this->mTime += this->mDt;

    this->ResetStateSystemGroup();

    return this->mTime;
}

#ifdef _EIGEN_
template <>
double LinearSystemSolver< Eigen::SparseMatrix< double, RowMajor > >::Solve();
#endif /* _EIGEN_ */


} /* namespace systm */

#endif /* LINEARSYSTEMSOLVER_H_ */
