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
#ifndef COOLINGFACTORYBUILDER_H_
#define COOLINGFACTORYBUILDER_H_

#include "coolingclasswrapper.h"


namespace factory
{

/// Builds a Factory for ::thermal::Cooling
template < typename T >
Factory< thermal::Cooling< T > > *BuildCoolingFactory( Factory< object::Object< T >, ArgumentTypeObject< T > > *objectFactory )
{
    Factory< thermal::Cooling< T > > *coolingFactory = new Factory< thermal::Cooling< T > >();

    coolingFactory->AddWrapper( new CoolingClassWrapper< T, thermal::DirichletBoundaryCondition >( objectFactory ),
                                "DirichletBoundaryCondition" );
    coolingFactory->AddWrapper( new CoolingClassWrapper< T, thermal::CoolingByLookUp >( objectFactory ),
                                "CoolingByLookUp" );
    coolingFactory->AddWrapper( new CoolingClassWrapper< T, thermal::CoolingByConstantValue >( objectFactory ),
                                "CoolingByConstantValue" );
    coolingFactory->AddWrapper( new CoolingClassWrapperThermalIsolation< T >( objectFactory ), "ThermalIsolation" );

    return coolingFactory;
}
}

#endif
