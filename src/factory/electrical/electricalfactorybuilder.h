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
 * electricalfactorybuilder.h
 *
 *  Created on: 07.10.2013
 *      Author: chris
 */

#ifndef ELECTRICALFACTORYBUILDER_H_
#define ELECTRICALFACTORYBUILDER_H_

#include "electricalclasswrapper.h"
#include "../../misc/matrixInclude.h"

namespace factory
{
/// Builds a Factory for ::electrical
template < typename MatrixT, typename ValueT >
Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *
BuildElectricalFactory( Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > > *objectFactory,
                        Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
{
    Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory =
     new Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical >();

    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ParallelRC >( electricalFactory,
                                                                                                  objectFactory, stateFactory ),
                                   "ParallelRC" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ParallelTwoPort >( electricalFactory, objectFactory,
                                                                                                       stateFactory ),
                                   "ParallelTwoPort" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::SerialTwoPort >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "SerialTwoPort" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Cellelement >( electricalFactory,
                                                                                                   objectFactory, stateFactory ),
                                   "CellElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Zarc >( electricalFactory, objectFactory, stateFactory ),
                                   "ZarcElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ZarcAlg >( electricalFactory, objectFactory, stateFactory ),
                                   "ZarcAlgElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::OhmicResistance >( electricalFactory, objectFactory,
                                                                                                       stateFactory ),
                                   "OhmicResistance" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Capacity >( electricalFactory, objectFactory, stateFactory ),
                                   "Capacity" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::VoltageSource >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "VoltageSource" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::WarburgTanh >( electricalFactory,
                                                                                                   objectFactory, stateFactory ),
                                   "WarburgTanh" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::WarburgCotanh >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "WarburgCotanh" );

    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::SphericalDiffusion >( electricalFactory, objectFactory,
                                                                                                          stateFactory ),
                                   "SphericalDiffusion" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Rmphn >( electricalFactory, objectFactory, stateFactory ),
                                   "Rmphn" );


    // UNSUPPORTED
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ConstantPhaseElement >( electricalFactory, objectFactory,
                                                                                                            stateFactory ),
                                   "ConstantPhaseElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Inductance >( electricalFactory,
                                                                                                  objectFactory, stateFactory ),
                                   "Inductance" );


    return electricalFactory;
}

} /* namespace factory */

#endif /* ELECTRICALFACTORYBUILDER_H_ */
