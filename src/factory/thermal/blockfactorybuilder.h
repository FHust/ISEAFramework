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
#ifndef BLOCKFACTORYBUILDER_H_
#define BLOCKFACTORYBUILDER_H_

#include "blockclasswrapper.h"


namespace factory
{

/// Builds a Factory for ::thermal::ThermalBlock
template < typename T >
Factory< thermal::ThermalBlock< T >, ArgumentTypeThermalBlock< T > > *
BuildBlockFactory( Factory< thermal::Material< T > > *materialFactory )
{
    Factory< thermal::ThermalBlock< T >, ArgumentTypeThermalBlock< T > > *blockFactory =
     new Factory< thermal::ThermalBlock< T >, ArgumentTypeThermalBlock< T > >();

    blockFactory->AddWrapper( new BlockClassWrapper< T, thermal::RectangularBlock >( materialFactory ),
                              "RectangularBlock" );
    blockFactory->AddWrapper( new BlockClassWrapper< T, thermal::QuadraticCellBlock >( materialFactory ),
                              "QuadraticCellBlock" );
    blockFactory->AddWrapper( new BlockClassWrapper< T, thermal::Supercap >( materialFactory ), "Supercap" );
    blockFactory->AddWrapper( new BlockClassWrapper< T, thermal::TriangularPrismBlock >( materialFactory ),
                              "TriangularPrismBlock" );
    blockFactory->AddWrapper( new BlockClassWrapper< T, thermal::HexagonalCellBlock >( materialFactory ),
                              "HexagonalCellBlock" );

    return blockFactory;
}
}

#endif
