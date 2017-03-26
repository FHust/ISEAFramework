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
#ifndef _CYLINDRIC_ELEMENT_
#define _CYLINDRIC_ELEMENT_

#include "thermal_element.h"
#include "../../../geometry/cylindric.h"
#include "../../../exceptions/error_proto.h"


namespace thermal
{
using namespace geometry;

/// CylindricElement is a ThermalElement whose edges are parallel to the vector of a cylindric coordinate system, i.e. a
/// cylinder segment
template < typename T = double >
class CylindricElement : public ThermalElement< T >
{
    public:
    /**
     * @param[in] emplacementPoint Translation vector between global cartesian and cylindric coordinate system used in
     * this instance
     * @param[in] startVector One vertex of cylindric volume
     * @param[in] deltaVector Delta rho-, phi- and z-coordinate spanning volume from startVector
     */
    CylindricElement( const Cartesian< T > &emplacementPoint, const Cylindric< T > &startVector,
                      const Cylindric< T > &deltaVector, T initialTemperatureValue, const Material< T > *material,
                      ::state::ThermalState< T > *thermalState = 0, T thermalStateFactor = 1.0 );
    virtual ~CylindricElement();

    private:
    T MakeVolume() const;
    const Cartesian< T > mEmplacementPoint;
    const Cylindric< T > mStartVector;
    const Cylindric< T > mDeltaVector;
};


template < typename T >
CylindricElement< T >::CylindricElement( const Cartesian< T > &emplacementPoint, const Cylindric< T > &startVector,
                                         const Cylindric< T > &deltaVector, T initialTemperatureValue,
                                         const Material< T > *material, ::state::ThermalState< T > *thermalState, T thermalStateFactor )
    : mEmplacementPoint( emplacementPoint )
    , mStartVector( startVector )
    , mDeltaVector( deltaVector )
{
    if ( fabs( deltaVector.GetPhi().GetDeg() ) > 180.0 )    // ) > 180.0 + 0.001)
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "errorAngleSmaller180" );
    if ( startVector.GetRho() < 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "errorRhoGreaterEqZero" );
    if ( startVector.GetRho() + deltaVector.GetRho() < 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "errorDifferenceRhoDeltavector" );

    ThermalElement< T >::SetElement( initialTemperatureValue, material,
                                     CylToCart< T >( emplacementPoint, startVector + deltaVector / 2.0 ), MakeVolume(),
                                     thermalState, thermalStateFactor );
}

template < typename T >
CylindricElement< T >::~CylindricElement()
{
}

template < typename T >
T CylindricElement< T >::MakeVolume() const
{
    const T r_i = mStartVector.GetRho();
    const T r_a = mStartVector.GetRho() + mDeltaVector.GetRho();
    const T phi = mDeltaVector.GetPhi().GetRad();

    return fabs( ( r_a * r_a - r_i * r_i ) * 0.5 * phi * mDeltaVector.GetZ() );
}
}
#endif
