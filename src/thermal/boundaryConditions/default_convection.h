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
#ifndef _DEFAULT_CONVECTION_
#define _DEFAULT_CONVECTION_

#include <boost/shared_ptr.hpp>
#include "../thermal_structs.h"
#include "../../object/lookup_obj2d.h"
#include "../../exceptions/error_proto.h"


namespace thermal
{
using boost::shared_ptr;

/// DefaultConvection is interface class for the default convective cooling for external areas
template < typename T = double >
class DefaultConvection
{
    public:
    DefaultConvection(){};
    virtual ~DefaultConvection(){};
    /// Returns offset and slope for first order Taylor approximation of power per area cooled
    virtual Linearization< T > GetOffsetSlope( T surfaceTemperature, T characteristicLength, T airTemperature ) = 0;
};


/// ConvectionByFormula returns the cooling of an area by convection with Heiko's proposed formula
template < typename T = double >
class ConvectionByFormula : public DefaultConvection< T >
{
    public:
    explicit ConvectionByFormula( T preFactor );
    virtual ~ConvectionByFormula(){};
    Linearization< T > GetOffsetSlope( T surfaceTemperature, T characteristicLength, T airTemperature );

    private:
    T mHelp_SameForAll;
    T mAbsoluteZeroInC;
};


template < typename T >
ConvectionByFormula< T >::ConvectionByFormula( T preFactor )
    : mAbsoluteZeroInC( -273.15 )
{
    if ( preFactor <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "preFactorNegative" );

    const T pr_Air = 0.73135;
    const T lambda_Air = 0.0262;
    const T ny_Air = 0.0000175;
    const T g = 9.81;
    mHelp_SameForAll = preFactor * pow( pr_Air * g / ( ny_Air * ny_Air ), 0.25 ) * lambda_Air;
}

template < typename T >
Linearization< T > ConvectionByFormula< T >::GetOffsetSlope( T surfaceTemperature, T characteristicLength, T airTemperature )
{
    const T deltaTemperature = surfaceTemperature - airTemperature;
    if ( deltaTemperature < 0.0 )
        return Linearization< T >( 0.0, 0.0 );

    const T help_SameForOffsetSlope =
     mHelp_SameForAll * pow( ( airTemperature - mAbsoluteZeroInC ) * characteristicLength, -0.25 );
    return Linearization< T >( help_SameForOffsetSlope * pow( deltaTemperature, 1.25 ),
                               help_SameForOffsetSlope * 1.25 * pow( deltaTemperature, 0.25 ) );
}


/// ConvectionByLookUp returns the convective cooling of an area defined by a look up table
template < typename T = double >
class ConvectionByLookUp : public DefaultConvection< T >
{
    public:
    ConvectionByLookUp( shared_ptr< object::Object< T > > offsetLookUp, shared_ptr< object::Object< T > > slopeLookUp );
    virtual ~ConvectionByLookUp(){};
    virtual Linearization< T > GetOffsetSlope( T surfaceTemperature, T characteristicLength, T airTemperature );

    private:
    const boost::shared_ptr< object::Object< T > > mOffsetLookUp;
    const shared_ptr< object::Object< T > > mSlopeLookUp;
};

template < typename T >
ConvectionByLookUp< T >::ConvectionByLookUp( shared_ptr< object::Object< T > > offsetLookUp, shared_ptr< object::Object< T > > slopeLookUp )
    : mOffsetLookUp( offsetLookUp )
    , mSlopeLookUp( slopeLookUp )
{
}

template < typename T >
Linearization< T > ConvectionByLookUp< T >::GetOffsetSlope( T surfaceTemperature, T characteristicLength, T )
{
    return Linearization< T >( mOffsetLookUp->GetValue( surfaceTemperature, characteristicLength ),
                               mSlopeLookUp->GetValue( surfaceTemperature, characteristicLength ) );
}
}
#endif
