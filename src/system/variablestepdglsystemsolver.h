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
 * variablestepdglsystemsolver.h
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

#ifndef VARIABLESTEPDGLSYSTEMSOLVER_H_
#define VARIABLESTEPDGLSYSTEMSOLVER_H_
#include "systemSolver.h"
#include "dae_sys.h"
#include <boost/scoped_ptr.hpp>


#ifdef _EIGEN_
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>
#include <boost/ref.hpp>
#endif /* _EIGEN_ */

#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif /* __EXCEPTIONS__ */

#define SOLVER_TRIES 10

namespace systm
{

/// Solver with variable dt
template < typename T >
class VariableStepDglSystemSolver : public SystemSolver< T >
{
    public:
    VariableStepDglSystemSolver( StateSystemGroup< T >* stateSystemGroup );

    virtual ~VariableStepDglSystemSolver() {}

    virtual double Solve() { return 0; }

    private:
    boost::scoped_ptr< systm::DifferentialAlgebraicSystem< T > > daeSystem;
};

/// Solver with variable dt. This class will throw a not implemented exception.
template < typename T >
VariableStepDglSystemSolver< T >::VariableStepDglSystemSolver( StateSystemGroup< T >* stateSystemGroup )
    : SystemSolver< T >( stateSystemGroup, 0 )
    , daeSystem( new systm::DifferentialAlgebraicSystem< T >( stateSystemGroup ) )
{
    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OnlyArmadillo" );
}

#ifdef _ARMADILLO_

/// Solver with variable dt
template <>
class VariableStepDglSystemSolver< arma::Mat< double > > : public SystemSolver< arma::Mat< double > >
{
    public:
    VariableStepDglSystemSolver( StateSystemGroup< arma::Mat< double > >* stateSystemGroup );

    virtual ~VariableStepDglSystemSolver() {}

    virtual double Solve();

    private:
    boost::scoped_ptr< systm::DifferentialAlgebraicSystem< arma::Mat< double > > > daeSystem;
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< arma::Mat< double > > >::type mStepper;
};

/// Solver with variable dt
template <>
class VariableStepDglSystemSolver< arma::SpMat< double > > : public SystemSolver< arma::SpMat< double > >
{
    public:
    VariableStepDglSystemSolver( StateSystemGroup< arma::SpMat< double > >* stateSystemGroup );

    virtual ~VariableStepDglSystemSolver() {}

    virtual double Solve();

    private:
    boost::scoped_ptr< systm::DifferentialAlgebraicSystem< arma::SpMat< double > > > daeSystem;
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< arma::Mat< double > > >::type mStepper;
    arma::Mat< double > mDenseStateVector;
};

#endif /* _ARMADILLO_ */

#ifdef _EIGEN_
template <>
class VariableStepDglSystemSolver< SparseMatrix< double, RowMajor > > : public SystemSolver< SparseMatrix< double, RowMajor > >
{
    public:
    VariableStepDglSystemSolver( StateSystemGroup< SparseMatrix< double, RowMajor > >* stateSystemGroup );

    virtual ~VariableStepDglSystemSolver() {}

    virtual double Solve();

    private:
    boost::scoped_ptr< systm::DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > > > daeSystem;
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< arma::Mat< double > > >::type mStepper;
};
#endif /* _EIGEN_ */

} /* namespace systm */
#endif /* VARIABLESTEPDGLSYSTEMSOLVER_H_ */
