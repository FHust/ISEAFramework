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
#ifndef _CYLINDRIC_
#define _CYLINDRIC_

#include "angle.h"
#include "cartesian.h"
#include <cmath>


namespace geometry
{

/// Cylindric is a point in cylindric coordinates translated, but not rotated to the global coordinate system
template < typename T = double >
class Cylindric
{
    template < typename U >
    friend const Cylindric< U > operator+( const Cylindric< U > &lhs, const Cylindric< U > &rhs );
    template < typename U >
    friend const Cylindric< U > operator/( const Cylindric< U > &lhs, U rhs );

    public:
    Cylindric( T rho, const Angle< T > &phi, T z );
    virtual ~Cylindric();
    T GetRho() const;
    Angle< T > GetPhi() const;
    T GetZ() const;

    private:
    const T mRho;
    const Angle< T > mPhi;
    const T mZ;
};


template < typename T >
const Cartesian< T > CylToCart( const Cartesian< T > &emplacementVector, const Cylindric< T > &cyl )
{
    const T phi = cyl.GetPhi().GetRad();
    return emplacementVector + Cartesian< T >( cyl.GetRho() * cos( phi ), cyl.GetRho() * sin( phi ), cyl.GetZ() );
}

template < typename T >
Cylindric< T >::Cylindric( T rho, const Angle< T > &phi, T z )
    : mRho( rho )
    , mPhi( phi )
    , mZ( z )
{
}

template < typename T >
Cylindric< T >::~Cylindric()
{
}

template < typename T >
T Cylindric< T >::GetRho() const
{
    return mRho;
}

template < typename T >
Angle< T > Cylindric< T >::GetPhi() const
{
    return mPhi;
}

template < typename T >
T Cylindric< T >::GetZ() const
{
    return mZ;
}


template < typename T >
const Cylindric< T > operator+( const Cylindric< T > &lhs, const Cylindric< T > &rhs )
{
    return Cylindric< T >( lhs.mRho + rhs.mRho, lhs.mPhi + rhs.mPhi, lhs.mZ + rhs.mZ );
}

template < typename T >
const Cylindric< T > operator/( const Cylindric< T > &lhs, T rhs )
{
    return Cylindric< T >( lhs.mRho / rhs, lhs.mPhi / rhs, lhs.mZ / rhs );
}
}
#endif
