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
 * variablestepdglsystemsolver.cpp
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

// STD
#ifdef __EXCEPTIONS__
#include <stdexcept>
#include <string>
#endif /* __EXCEPTIONS__ */

// ETC
#include "variablestepdglsystemsolver.h"
#include "../exceptions/error_proto.h"
namespace systm
{
#ifdef _ARMADILLO_

VariableStepDglSystemSolver< arma::Mat< double > >::VariableStepDglSystemSolver( StateSystemGroup< arma::Mat< double > >* stateSystemGroup )
    : SystemSolver< arma::Mat< double > >( stateSystemGroup, 0.0001 )
    , daeSystem( new systm::DifferentialAlgebraicSystem< arma::Mat< double > >( stateSystemGroup ) )
    , mStepper( make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< arma::Mat< double > >() ) )
{
}

double VariableStepDglSystemSolver< arma::Mat< double > >::Solve()
{
    bool successfull = false;

    daeSystem->PrepareEquationSystem();

    for ( size_t tries = SOLVER_TRIES; tries > 0 && !successfull; --tries )
    {
        arma::Mat< double > states = this->mStateVector.submat( 0, 0, this->mStateVector.n_rows - 2, 0 );
        successfull = ( mStepper.try_step( boost::ref( *daeSystem ), states, mTime, mDt ) == boost::numeric::odeint::success );
        this->mStateVector.submat( 0, 0, this->mStateVector.n_rows - 2, 0 ) = states;
    }

    if ( !successfull )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorStep" );

    this->ResetStateSystemGroup();

    return mTime;
}

VariableStepDglSystemSolver< arma::SpMat< double > >::VariableStepDglSystemSolver( StateSystemGroup< arma::SpMat< double > >* stateSystemGroup )
    : SystemSolver< arma::SpMat< double > >( stateSystemGroup, 0.0001 )
    , daeSystem( new systm::DifferentialAlgebraicSystem< arma::SpMat< double > >( stateSystemGroup ) )
    , mStepper( make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< arma::Mat< double > >() ) )
{
    mDenseStateVector.zeros( stateSystemGroup->GetStateCount(), 1 );
}

double VariableStepDglSystemSolver< arma::SpMat< double > >::Solve()
{
    bool successfull = false;

    daeSystem->PrepareEquationSystem();

    for ( size_t row = 0; row < mDenseStateVector.n_rows; ++row )
        mDenseStateVector( row, 0 ) = this->mStateVector( row, 0 );

    for ( size_t tries = SOLVER_TRIES; tries > 0 && !successfull; --tries )
    {
        successfull =
         ( mStepper.try_step( boost::ref( *daeSystem ), mDenseStateVector, mTime, mDt ) == boost::numeric::odeint::success );
    }

    for ( size_t row = 0; row < mDenseStateVector.n_rows; ++row )
        this->mStateVector( row, 0 ) = mDenseStateVector( row, 0 );

    if ( !successfull )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorStep" );

    this->ResetStateSystemGroup();

    return mTime;
}

#endif /* _ARMADILLO_ */

#ifdef _EIGEN_

VariableStepDglSystemSolver< SparseMatrix< double, RowMajor > >::VariableStepDglSystemSolver(
 StateSystemGroup< SparseMatrix< double, RowMajor > >* stateSystemGroup )
    : SystemSolver< SparseMatrix< double, RowMajor > >( stateSystemGroup, 0.0001 )
    , daeSystem( new systm::DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >( stateSystemGroup ) )
    , mStepper( make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< arma::Mat< double > >() ) )
{
}

double VariableStepDglSystemSolver< SparseMatrix< double, RowMajor > >::Solve()
{
    bool successfull = false;

    daeSystem->PrepareEquationSystem();

    size_t realStates = this->mStateVector.rows() - 1;    // TODO: Uncomment when one is concatenated to state vector

    //    MatrixXd x = this->mStateVector.topRows(realStates);
    arma::Mat< double > x( realStates, 1 );
    for ( size_t i = 0; i < realStates; ++i )
        x( i, 0 ) = this->mStateVector.coeff( i, 0 );

    for ( size_t tries = SOLVER_TRIES; tries > 0 && !successfull; --tries )
    {
        successfull = ( mStepper.try_step( boost::ref( *daeSystem ), x, mTime, mDt ) == boost::numeric::odeint::success );
    }

    for ( size_t i = 0; i < realStates; ++i )
        this->mStateVector.coeffRef( i, 0 ) = x( i, 0 );

    this->mStateVector.coeffRef( realStates, 0 ) = 1;    // TODO: Uncomment when one is concatenated to state vector

    if ( !successfull )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorStep" );

    this->ResetStateSystemGroup();

    return mTime;
}

#endif /* _EIGEN_ */

} /* namespace systm */
