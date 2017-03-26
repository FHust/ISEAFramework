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
#ifndef _TRIANGULAR_ELEMENT_
#define _TRIANGULAR_ELEMENT_

#include "thermal_element.h"
#include <cmath>
#include "../../../exceptions/error_proto.h"


namespace thermal
{
using namespace geometry;

/// TriangularElement is a ThermalElement with a triangular base in a plane parallel to the xy-plane and a height in
/// z-direction
template < typename T = double >
class TriangularElement : public ThermalElement< T >
{
    public:
    /**
     * @param[in] emplacementPoint Origin of construction
     * @param[in] vertex1,vertex2,vertex3 Vertices 1-3 define the base triangle of the triangular element as two
     * dimensional coordinates in a plane parallel to the xy-plane with the emplacement point as origin
     * @param[in] height Extension in z-direction
     */
    TriangularElement( const Cartesian< T > &emplacementPoint, const TwoDim< T > &vertex1, const TwoDim< T > &vertex2,
                       const TwoDim< T > &vertex3, T height, T initialTemperatureValue, const Material< T > *material,
                       ::state::ThermalState< T > *thermalState = 0, T thermalStateFactor = 1.0 );
    virtual ~TriangularElement();

    private:
    /// Tests if vertices 1-3 form a valid triangle
    bool CheckForLineError( const TwoDim< T > &vertex1, const TwoDim< T > &vertex2, const TwoDim< T > &vertex3 ) const;
    const Cartesian< T > mEmplacementPoint;
    const TwoDim< T > mVertex1;
    const TwoDim< T > mVertex2;
    const TwoDim< T > mVertex3;
    const T mHeight;
};


template < typename T >
TriangularElement< T >::TriangularElement( const Cartesian< T > &emplacementPoint, const TwoDim< T > &vertex1,
                                           const TwoDim< T > &vertex2, const TwoDim< T > &vertex3, T height,
                                           T initialTemperatureValue, const Material< T > *material,
                                           ::state::ThermalState< T > *thermalState, T thermalStateFactor )
    : mEmplacementPoint( emplacementPoint )
    , mVertex1( vertex1 )
    , mVertex2( vertex2 )
    , mVertex3( vertex3 )
    , mHeight( height )
{
    if ( CheckForLineError( vertex1, vertex2, vertex3 ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "VerticesDoesNotFormArea" );

    ThermalElement< T >::SetElement( initialTemperatureValue, material,
                                     emplacementPoint +
                                      Cartesian< T >( ( vertex1.Get1() + vertex2.Get1() + vertex3.Get1() ) / 3.0,
                                                      ( vertex1.Get2() + vertex2.Get2() + vertex3.Get2() ) / 3.0, height / 2.0 ),
                                     height * 0.5 *
                                      fabs( ( vertex1.Get2() + vertex2.Get2() ) * ( vertex1.Get1() - vertex2.Get1() ) +
                                            ( vertex2.Get2() + vertex3.Get2() ) * ( vertex2.Get1() - vertex3.Get1() ) +
                                            ( vertex3.Get2() + vertex1.Get2() ) * ( vertex3.Get1() - vertex1.Get1() ) ),
                                     thermalState, thermalStateFactor );
}

template < typename T >
TriangularElement< T >::~TriangularElement()
{
}

template < typename T >
bool TriangularElement< T >::CheckForLineError( const TwoDim< T > &vertex1, const TwoDim< T > &vertex2, const TwoDim< T > &vertex3 ) const
{
    // At least two points are equal
    if ( vertex1.DistanceTo( vertex2 ) == 0.0 || vertex1.DistanceTo( vertex3 ) == 0.0 || vertex2.DistanceTo( vertex3 ) == 0.0 )
        return true;

    // All three points on a line
    TwoDim< T > deltaVertex = vertex1 - vertex2;
    if ( ( vertex3.Get1() - vertex1.Get1() ) / ( vertex2.Get1() - vertex1.Get1() ) ==
         ( vertex3.Get2() - vertex1.Get2() ) / ( vertex2.Get2() - vertex1.Get2() ) )
        return true;

    return false;
}
}
#endif
