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
 * constantstepdglsystemsolver.h
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

#ifndef CONSTANTSTEPDGLSYSTEMSOLVER_H_
#define CONSTANTSTEPDGLSYSTEMSOLVER_H_

#include "systemSolver.h"
#include "dae_sys.h"
#include <boost/scoped_ptr.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta4.hpp>

namespace systm
{

/// Solver with constant dt and euler integration
template < typename T >
class ConstantStepDglSystemSolver : public SystemSolver< T >
{
    public:
    ConstantStepDglSystemSolver( StateSystemGroup< T >* stateSystemGroup, double dt );

    virtual ~ConstantStepDglSystemSolver() {}

    virtual double Solve();

    private:
    T mDxDt;
    boost::scoped_ptr< systm::DifferentialAlgebraicSystem< T > > daeSystem;
};

/// Solver with constant dt and euler integration
template < typename T >
ConstantStepDglSystemSolver< T >::ConstantStepDglSystemSolver( StateSystemGroup< T >* stateSystemGroup, double dt )
    : SystemSolver< T >( stateSystemGroup, dt )
    , daeSystem( new systm::DifferentialAlgebraicSystem< T >( stateSystemGroup ) )
{
}

template < typename T >
double ConstantStepDglSystemSolver< T >::Solve()
{
    daeSystem->PrepareEquationSystem();
    daeSystem->operator()( this->mStateVector.submat( 0, 0, this->mStateVector.n_rows - 2, 0 ), mDxDt,
                           0 );    // TODO: FHU mStateVector topRows wrappen
    this->mStateVector.submat( 0, 0, this->mStateVector.n_rows - 2, 0 ) += mDxDt * this->mDt;
    this->mTime += this->mDt;

    this->ResetStateSystemGroup();

    return this->mTime;
}

#ifdef _EIGEN_
template <>
double ConstantStepDglSystemSolver< SparseMatrix< double, RowMajor > >::Solve();
#endif /* _EIGEN_ */

#ifdef _ARMADILLO_

/// Solver with constant dt and euler integration
template <>
class ConstantStepDglSystemSolver< arma::Mat< double > > : public SystemSolver< arma::Mat< double > >
{
    public:
    ConstantStepDglSystemSolver( StateSystemGroup< arma::Mat< double > >* stateSystemGroup, double dt );

    virtual ~ConstantStepDglSystemSolver() {}

    virtual double Solve();

    private:
    boost::scoped_ptr< systm::DifferentialAlgebraicSystem< arma::Mat< double > > > daeSystem;
    boost::numeric::odeint::runge_kutta4< arma::Mat< double > > mStepper;
};

#endif /* _ARMADILLO_ */

} /* namespace systm */

#endif /* CONSTANTSTEPDGLSYSTEMSOLVER_H_ */
