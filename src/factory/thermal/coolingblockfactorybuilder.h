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
#ifndef COOLINGBLOCKFACTORYBUILDER_H_
#define COOLINGBLOCKFACTORYBUILDER_H_

#include "coolingblockclasswrapper.h"


namespace factory
{

/// Builds a Factory for ::thermal::CoolingBlock
template < typename T >
Factory< thermal::CoolingBlock< T >, ArgumentTypeCoolingBlock< T > > *
BuildCoolingBlockFactory( Factory< thermal::Cooling< T > > *coolingFactory )
{
    Factory< thermal::CoolingBlock< T >, ArgumentTypeCoolingBlock< T > > *coolingBlockFactory =
     new Factory< thermal::CoolingBlock< T >, ArgumentTypeCoolingBlock< T > >();

    coolingBlockFactory->AddWrapper( new CoolingBlockClassWrapper< T, thermal::CoolingHorizontalPlane >( coolingFactory ),
                                     "CoolingHorizontalPlane" );
    coolingBlockFactory->AddWrapper( new CoolingBlockClassWrapper< T, thermal::CoolingVerticalPlane >( coolingFactory ),
                                     "CoolingVerticalPlane" );
    coolingBlockFactory->AddWrapper( new CoolingBlockClassWrapper< T, thermal::CoolingPrismatic >( coolingFactory ),
                                     "CoolingPrismatic" );

    return coolingBlockFactory;
}
}

#endif
