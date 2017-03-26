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
 * chooseElectricSystemType.h
 *
 *  Created on: 14.09.2013
 *      Author: chris
 */

#ifndef _CHOOSEELECTRICSYSTEMTYPE_H
#define _CHOOSEELECTRICSYSTEMTYPE_H


// BOOST
#include <boost/shared_ptr.hpp>

//#include "stateSystemGroup.h"
#include "../../system/generalizedsystem.h"
#include "../../system/linAlg_sys.h"
#include "../../system/dae_sys.h"

namespace factory
{

template < typename MatrixT >
boost::shared_ptr< systm::GeneralizedSystem< MatrixT > >
ChooseElectricEquationSystemType( systm::StateSystemGroup< MatrixT >* stateSystemGroup )
{
#ifndef _SYMBOLIC_
    if ( stateSystemGroup->mDglStateSystem.GetEquationCount() < 1 )
    {
        return boost::shared_ptr< systm::GeneralizedSystem< MatrixT > >( new systm::LinAlgSystem< MatrixT >( stateSystemGroup ) );
    }
#endif
    return boost::shared_ptr< systm::GeneralizedSystem< MatrixT > >(
     new systm::DifferentialAlgebraicSystem< MatrixT >( stateSystemGroup ) );
}


}    // namespace factory

#endif /*_CHOOSEELECTRICSYSTEMTYPE_H*/
