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
#ifndef _CARTESIAN_
#define _CARTESIAN_

#include <cmath>
#ifndef __NO_STRING__
#include <fstream>
#endif


/// geometry contains geometrical information of thermal part of the model
namespace geometry
{

/// Cartesian is a point or vector in global cartesian coordinates
template < typename T = double >
class Cartesian
{
    template < typename U >
    friend const Cartesian< U > operator+( const Cartesian< U > &lhs, const Cartesian< U > &rhs );
    template < typename U >
    friend const Cartesian< U > operator-( const Cartesian< U > &lhs, const Cartesian< U > &rhs );
    template < typename U >
    friend const Cartesian< U > operator*( const Cartesian< U > &lhs, U rhs );
    template < typename U >
    friend const Cartesian< U > operator/( const Cartesian< U > &lhs, U rhs );

    public:
    Cartesian();
    Cartesian( T x, T y, T z );
    virtual ~Cartesian();
    const Cartesian< T > &operator=( const Cartesian< T > &rhs );
    /// Returns true if X, Y and Z coordinates of this instance and rhs are equal by tolerance (distance between them is
    /// smaller than tolerance), otherwise false
    bool Equals( const Cartesian< T > &rhs, T tolerance ) const;
    Cartesian< T > CrossProduct( const Cartesian< T > &rhs ) const;
    T DotProduct( const Cartesian< T > &rhs ) const;
    /// Distance
    T Distance( const Cartesian< T > &rhs ) const;
    /// Returns the normed vector of this instance interpreted as a vector, i.e. the same direction with length of 1
    Cartesian< T > Normed() const;
    T GetX() const;
    T GetY() const;
    T GetZ() const;

    private:
    T mX;
    T mY;
    T mZ;
};

#ifndef __NO_STRING__
template < typename T >
std::ostream &operator<<( std::ostream &os, const Cartesian< T > &rhs )
{
    os << rhs.GetX() << ", " << rhs.GetY() << ", " << rhs.GetZ();
    return os;
}
#endif


template < typename T >
Cartesian< T >::Cartesian()
    : mX( 0.0 )
    , mY( 0.0 )
    , mZ( 0.0 )
{
}

template < typename T >
Cartesian< T >::Cartesian( T x, T y, T z )
    : mX( x )
    , mY( y )
    , mZ( z )
{
}

template < typename T >
Cartesian< T >::~Cartesian()
{
}

template < typename T >
const Cartesian< T > &Cartesian< T >::operator=( const Cartesian< T > &rhs )
{
    if ( this != &rhs )
    {
        mX = rhs.mX;
        mY = rhs.mY;
        mZ = rhs.mZ;
    }

    return *this;
}

template < typename T >
T Cartesian< T >::Distance( const Cartesian< T > &rhs ) const
{
    return sqrt( ( mX - rhs.mX ) * ( mX - rhs.mX ) + ( mY - rhs.mY ) * ( mY - rhs.mY ) + ( mZ - rhs.mZ ) * ( mZ - rhs.mZ ) );
}

template < typename T >
bool Cartesian< T >::Equals( const Cartesian< T > &rhs, T tolerance ) const
{
    return sqrt( ( mX - rhs.mX ) * ( mX - rhs.mX ) + ( mY - rhs.mY ) * ( mY - rhs.mY ) + ( mZ - rhs.mZ ) * ( mZ - rhs.mZ ) ) < tolerance;
}

template < typename T >
Cartesian< T > Cartesian< T >::CrossProduct( const Cartesian< T > &rhs ) const
{
    return Cartesian< T >( mY * rhs.mZ - mZ * rhs.mY, mZ * rhs.mX - mX * rhs.mZ, mX * rhs.mY - mY * rhs.mX );
}

template < typename T >
T Cartesian< T >::DotProduct( const Cartesian< T > &rhs ) const
{
    return mX * rhs.mX + mY * rhs.mY + mZ * rhs.mZ;
}

template < typename T >
Cartesian< T > Cartesian< T >::Normed() const
{
    return *this / sqrt( mX * mX + mY * mY + mZ * mZ );
}

template < typename T >
T Cartesian< T >::GetX() const
{
    return mX;
}

template < typename T >
T Cartesian< T >::GetY() const
{
    return mY;
}

template < typename T >
T Cartesian< T >::GetZ() const
{
    return mZ;
}


template < typename T >
const Cartesian< T > operator+( const Cartesian< T > &lhs, const Cartesian< T > &rhs )
{
    return Cartesian< T >( lhs.mX + rhs.mX, lhs.mY + rhs.mY, lhs.mZ + rhs.mZ );
}

template < typename T >
const Cartesian< T > operator-( const Cartesian< T > &lhs, const Cartesian< T > &rhs )
{
    return Cartesian< T >( lhs.mX - rhs.mX, lhs.mY - rhs.mY, lhs.mZ - rhs.mZ );
}

template < typename T >
const Cartesian< T > operator*( const Cartesian< T > &lhs, T rhs )
{
    return Cartesian< T >( lhs.mX * rhs, lhs.mY * rhs, lhs.mZ * rhs );
}

template < typename T >
const Cartesian< T > operator/( const Cartesian< T > &lhs, T rhs )
{
    return Cartesian< T >( lhs.mX / rhs, lhs.mY / rhs, lhs.mZ / rhs );
}
}
#endif
