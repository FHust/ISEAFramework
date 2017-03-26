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
#ifndef _TWO_DIM_
#define _TWO_DIM_

#include <cmath>
#include "angle.h"


namespace geometry
{

/// TwoDim can be a point or vector in any two dimensional coordinate system
template < typename T = double >
class TwoDim
{
    template < typename U >
    friend const TwoDim< U > operator+( const TwoDim< U > &lhs, const TwoDim< U > &rhs );
    template < typename U >
    friend const TwoDim< U > operator-( const TwoDim< U > &lhs, const TwoDim< U > &rhs );
    template < typename U >
    friend const TwoDim< U > operator*( const TwoDim< U > &lhs, U rhs );
    template < typename U >
    friend const TwoDim< U > operator/( const TwoDim< U > &lhs, U rhs );

    public:
    TwoDim();
    TwoDim( T _1, T _2 );
    /// If interpreted as point in (X,Y)-coordinates, constructs an instance by absolute value and phase/angle
    TwoDim( T absolute, const Angle< T > &phase );
    virtual ~TwoDim();
    const TwoDim< T > &operator=( const TwoDim< T > &rhs );
    /// If interpreted as point in (X,Y)-coordinates, returns angle of this instance in xy-Plane
    Angle< T > AngleInXY() const;
    /// Returns length of this instance interpreted as a vector
    T Length() const;
    /// Returns distance between this instance and other
    T DistanceTo( const TwoDim< T > &other ) const;
    /// Returns first coordinate
    T Get1() const;
    /// Returns second coordinate
    T Get2() const;

    private:
    T m1;
    T m2;
};


template < typename T >
TwoDim< T >::TwoDim()
    : m1( 0.0 )
    , m2( 0.0 )
{
}

template < typename T >
TwoDim< T >::TwoDim( T _1, T _2 )
    : m1( _1 )
    , m2( _2 )
{
}

template < typename T >
TwoDim< T >::TwoDim( T radius, const Angle< T > &angle )
    : m1( radius * cos( angle.GetRad() ) )
    , m2( radius * sin( angle.GetRad() ) )
{
}

template < typename T >
TwoDim< T >::~TwoDim()
{
}

template < typename T >
const TwoDim< T > &TwoDim< T >::operator=( const TwoDim< T > &rhs )
{
    if ( this != &rhs )
    {
        m1 = rhs.m1;
        m2 = rhs.m2;
    }

    return *this;
}

template < typename T >
Angle< T > TwoDim< T >::AngleInXY() const
{
    return Angle< T >::Rad( atan2( m2, m1 ) );
}

template < typename T >
T TwoDim< T >::Length() const
{
    return sqrt( m1 * m1 + m2 * m2 );
}

template < typename T >
T TwoDim< T >::DistanceTo( const TwoDim< T > &other ) const
{
    return ( *this - other ).Length();
}

template < typename T >
T TwoDim< T >::Get1() const
{
    return m1;
}

template < typename T >
T TwoDim< T >::Get2() const
{
    return m2;
}


template < typename T >
const TwoDim< T > operator+( const TwoDim< T > &lhs, const TwoDim< T > &rhs )
{
    return TwoDim< T >( lhs.m1 + rhs.m1, lhs.m2 + rhs.m2 );
}

template < typename T >
const TwoDim< T > operator-( const TwoDim< T > &lhs, const TwoDim< T > &rhs )
{
    return TwoDim< T >( lhs.m1 - rhs.m1, lhs.m2 - rhs.m2 );
}

template < typename T >
const TwoDim< T > operator*( const TwoDim< T > &lhs, T rhs )
{
    return TwoDim< T >( lhs.m1 * rhs, lhs.m2 * rhs );
}

template < typename T >
const TwoDim< T > operator/( const TwoDim< T > &lhs, T rhs )
{
    return TwoDim< T >( lhs.m1 / rhs, lhs.m2 / rhs );
}
}
#endif
