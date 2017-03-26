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
#ifndef _UNIT_VECTOR_
#define _UNIT_VECTOR_

#include <cmath>
#include "angle.h"
#include "cartesian.h"
#include "../exceptions/error_proto.h"
//#include "../thermal/exceptions/initialization_error.h"


namespace geometry
{

/// UnitVector is a unit vector in cartesian coordinates
template < typename T = double >
class UnitVector : public Cartesian< T >
{
    template < typename U >
    friend const UnitVector< U > operator-( const UnitVector< U > &rhs );

    public:
    /// x, y and z determine the direction of the unit vector and can be chosen arbitrarily (e.g. x=1, y=1, z=2)
    UnitVector( T x, T y, T z );
    UnitVector( const Cartesian< T > &cartesian );
    virtual ~UnitVector();
    /// Returns true if this instance and rhs point into the same direction by tolerance, otherwise false
    bool IsEqualDirection( const UnitVector< T > &rhs, const Angle< T > &tolerance ) const;
    /// Returns true if this instance and rhs point into the opposite direction by tolerance, otherwise false
    bool IsOppositeDirection( const UnitVector< T > &rhs, const Angle< T > &tolerance ) const;
};


template < typename T >
UnitVector< T >::UnitVector( T x, T y, T z )
    : Cartesian< T >( Cartesian< T >( x, y, z ).Normed() )
{
    if ( x == 0.0 && y == 0.0 && z == 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoDirection" );
}

template < typename T >
UnitVector< T >::UnitVector( const Cartesian< T > &cartesian )
    : Cartesian< T >( cartesian.Normed() )
{
}

template < typename T >
UnitVector< T >::~UnitVector()
{
}

template < typename T >
bool UnitVector< T >::IsEqualDirection( const UnitVector< T > &rhs, const Angle< T > &tolerance ) const
{
    const T dotProduct = Cartesian< T >::DotProduct( rhs );
    return ( dotProduct >= 1.0 ) ? true : ( acos( dotProduct ) < tolerance.GetRad() );
}

template < typename T >
bool UnitVector< T >::IsOppositeDirection( const UnitVector< T > &rhs, const Angle< T > &tolerance ) const
{
    const T dotProduct = Cartesian< T >::DotProduct( rhs );
    return ( dotProduct <= -1.0 ) ? true : ( acos( dotProduct ) > Angle<>::pi - tolerance.GetRad() );
}


template < typename T >
const UnitVector< T > operator-( const UnitVector< T > &rhs )
{
    return UnitVector< T >( -rhs.GetX(), -rhs.GetY(), -rhs.GetZ() );
}
}
#endif
