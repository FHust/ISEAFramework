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
#ifndef _ANGLE_
#define _ANGLE_

#include <cmath>


namespace geometry
{

/// Angle encapsulates an angle with useful functionality
template < typename T = double >
class Angle
{
    template < typename U >
    friend const Angle< U > operator+( const Angle< U > &lhs, const Angle< U > &rhs );
    template < typename U >
    friend const Angle< U > operator-( const Angle< U > &lhs, const Angle< U > &rhs );
    template < typename U >
    friend const Angle< U > operator/( const Angle< U > &lhs, U div );

    public:
    Angle();
    virtual ~Angle();
    const Angle< T > &operator=( const Angle< T > &rhs );
    /// Returns angle difference from this instance to rhs in xy-Plane in positive mathematical direction, returned
    /// angle is between 0 degree and 360 degree
    const Angle< T > AngleTo( const Angle< T > &rhs ) const;
    T GetRad() const;    ///<Returns value of angle in radiants
    T GetDeg() const;    ///<Returns value of angle in degrees

    static Angle< T > Rad( T radiants );    ///<Constructs Angle instance with radiants as input parameter
    static Angle< T > Deg( T degrees );     ///<Constructs Angle instance with degrees as input parameter
    const static T pi;                      ///<Is the value of pi as template type T
    const static Angle< T > circle;         ///<Is an Angle instance representing a full circle, i.e. 360 degree
    private:
    explicit Angle( T rad );    ///<Is private, as Rad or Deg should be used to construct new instance
    T mValueInRadiants;
};


template < typename T >
Angle< T >::Angle()
    : mValueInRadiants( 0.0 )
{
}

template < typename T >
Angle< T >::Angle( T radiants )
    : mValueInRadiants( radiants )
{
}

template < typename T >
Angle< T >::~Angle()
{
}

template < typename T >
const Angle< T > &Angle< T >::operator=( const Angle< T > &rhs )
{
    if ( &rhs != this )
    {
        mValueInRadiants = rhs.mValueInRadiants;
    }
    return *this;
}

template < typename T >
const Angle< T > Angle< T >::AngleTo( const Angle< T > &rhs ) const
{
    T distance = rhs.mValueInRadiants - mValueInRadiants;

    // Periodicity of angle taken into account
    for ( ; distance >= 2 * pi; distance -= 2 * pi )
        ;
    for ( ; distance < 0.0; distance += 2 * pi )
        ;

    return Angle< T >( distance );
}

template < typename T >
T Angle< T >::GetRad() const
{
    return mValueInRadiants;
}

template < typename T >
T Angle< T >::GetDeg() const
{
    return mValueInRadiants * 180.0 / pi;
}

template < typename T >
Angle< T > Angle< T >::Rad( T radiants )
{
    return Angle< T >( radiants );
}

template < typename T >
Angle< T > Angle< T >::Deg( T degrees )
{
    return Angle< T >( degrees / 180.0 * pi );
}

template < typename T >    // log_10(2) * 112 Bits = 34, so 35 digits to make complete use of quadruple precision float
const T Angle< T >::pi( 3.1415926535897932384626433832795028 );

template < typename T >
const Angle< T > Angle< T >::circle( 2.0 * 3.1415926535897932384626433832795028 );


template < typename T >
const Angle< T > operator+( const Angle< T > &lhs, const Angle< T > &rhs )
{
    return Angle< T >( lhs.mValueInRadiants + rhs.mValueInRadiants );
}

template < typename T >
const Angle< T > operator-( const Angle< T > &lhs, const Angle< T > &rhs )
{
    return Angle< T >( lhs.mValueInRadiants - rhs.mValueInRadiants );
}

template < typename T >
const Angle< T > operator/( const Angle< T > &lhs, T rhs )
{
    return Angle< T >( lhs.mValueInRadiants / rhs );
}
}
#endif
