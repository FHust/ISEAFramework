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
 * constantstepdglsystemsolver.cpp
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

#include "constantstepdglsystemsolver.h"

namespace systm
{

template class systm::ConstantStepDglSystemSolver< myMatrixType >;


#ifdef _EIGEN_
template <>
double ConstantStepDglSystemSolver< SparseMatrix< double, RowMajor > >::Solve()
{
    daeSystem->PrepareEquationSystem();

    size_t realStates = this->mStateVector.n_rows - 1;

    SparseMatrix< double, RowMajor > dxdt( realStates, 1 );
    daeSystem->operator()( this->mStateVector.topRows( realStates ), dxdt, 0 );

    this->mStateVector.topRows( realStates ) += dxdt * this->mDt;
    this->mStateVector( realStates, 0 ) = 1;

    this->mTime += this->mDt;
    this->ResetStateSystemGroup();

    return this->mTime;
}
#endif /* _EIGEN_ */


#ifdef _ARMADILLO_

ConstantStepDglSystemSolver< arma::Mat< double > >::ConstantStepDglSystemSolver( StateSystemGroup< arma::Mat< double > >* stateSystemGroup,
                                                                                 double dt )
    : SystemSolver< arma::Mat< double > >( stateSystemGroup, dt )
    , daeSystem( new systm::DifferentialAlgebraicSystem< arma::Mat< double > >( stateSystemGroup ) )
{
}

double ConstantStepDglSystemSolver< arma::Mat< double > >::Solve()
{
    daeSystem->PrepareEquationSystem();
    arma::Mat< double > states = this->mStateVector.submat( 0, 0, this->mStateVector.n_rows - 2, 0 );
    mStepper.do_step( *daeSystem, states, 0, this->mDt );
    this->mStateVector.submat( 0, 0, this->mStateVector.n_rows - 2, 0 ) = states;

    this->mTime += this->mDt;

    this->ResetStateSystemGroup();

    return this->mTime;
}

#endif /* _ARMADILLO_ */

} /* namespace systm */
