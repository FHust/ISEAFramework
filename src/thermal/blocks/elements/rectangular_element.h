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
#ifndef _RECTANGULAR_ELEMENT_
#define _RECTANGULAR_ELEMENT_

#include "thermal_element.h"
#include <cmath>


namespace thermal
{
using namespace geometry;

/// RectangularElement is a ThermalElement with a rectangular volume
template < typename T = double >
class RectangularElement : public ThermalElement< T >
{
    public:
    /**
     * @param[in] emplacementPoint One vertex of rectangular volume
     * @param[in] spanVector Delta x-, y- and z-coordinate spanning volume from emplacementPoint
     */
    RectangularElement( const Cartesian< T > &emplacementPoint, const Cartesian< T > &spanVector, T initialTemperatureValue,
                        const Material< T > *material, ::state::ThermalState< T > *thermalState = 0, T thermalStateFactor = 1.0 );
    virtual ~RectangularElement();

    private:
    const Cartesian< T > mEmplacementPoint;
    const Cartesian< T > mSpanVector;
};


template < typename T >
RectangularElement< T >::RectangularElement( const Cartesian< T > &emplacementPoint, const Cartesian< T > &spanVector,
                                             T initialTemperatureValue, const Material< T > *material,
                                             ::state::ThermalState< T > *thermalState, T thermalStateFactor )
    : mEmplacementPoint( emplacementPoint )
    , mSpanVector( spanVector )
{
    ThermalElement< T >::SetElement( initialTemperatureValue, material, emplacementPoint + ( spanVector / 2.0 ),
                                     fabs( spanVector.GetX() * spanVector.GetY() * spanVector.GetZ() ), thermalState,
                                     thermalStateFactor );
}

template < typename T >
RectangularElement< T >::~RectangularElement()
{
}
}
#endif
