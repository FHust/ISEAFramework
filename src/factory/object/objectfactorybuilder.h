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
 * objectfactorybuilder.h
 *
 *  Created on: 07.10.2013
 *      Author: chris
 */

#ifndef OBJECTFACTORYBUILDER_H_
#define OBJECTFACTORYBUILDER_H_

#include "objectclasswrapper.h"

namespace factory
{
/// Builds a Factory for ::object
template < typename ValueT >
Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > > *
BuildObjectFactory( Factory< ::state::Dgl_state, ArgumentTypeState > *stateFactory )
{
    Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > > *objectFactory =
     new Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >();

    objectFactory->AddWrapper( new ObjectClassWrapper< ValueT, object::ConstObj >( stateFactory ), "ConstObj" );
    objectFactory->AddWrapper( new ObjectClassWrapper< ValueT, object::LookupObj1D >( stateFactory ), "LookupObj1D" );
    objectFactory->AddWrapper( new ObjectClassWrapper< ValueT, object::LookupObj2D >( stateFactory ), "LookupObj2D" );
    objectFactory->AddWrapper( new ObjectClassWrapper< ValueT, object::LookupObj1dWithState >( stateFactory ),
                               "LookupObj1dWithState" );
    objectFactory->AddWrapper( new ObjectClassWrapper< ValueT, object::LookupObj2dWithState >( stateFactory ),
                               "LookupObj2dWithState" );
    return objectFactory;
}

} /* namespace factory */

#endif /* OBJECTFACTORYBUILDER_H_ */
