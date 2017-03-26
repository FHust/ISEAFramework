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
/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : innerArea.h
* Creation Date : 04-05-2015
* Last Modified : Mo 12 Okt 2015 17:13:16 CEST
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _INNERAREA_
#define _INNERAREA_
/// DESCRIPTION

#include <vector>
#include <map>
#include <utility>
#include "cartesian.h"
#include "tolerance.h"
#include "plane.h"
#include "geometry2D.h"
#include "two_dim.h"

class TestAreaClasses;


namespace geometry
{
using std::vector;
using std::map;

/// Area fuses Plane and Geometry2D to offer all functionality of a surface area of a thermal element
template < typename T = double >
class InnerArea : public Plane< T >
{
    friend class ::TestAreaClasses;

    public:
    /**
     * @param[in] vertices Vertices of area in coordinates system of plane
     * @param[in] plane Base class Plane can be inserted directly
     * @param[in] arcs Marks if edge of polygon is an arc by containing its circle center, unmarked edges are straight
     * lines. If the key of a map element is i, the defined edge is the edge between points vertices[i] and
     * vertices[i%vertices.size()].
     * @param[in] arcPolygonEdgesLength Supposes a length for the edge of a polygon representing a circle
     */
    InnerArea( const vector< TwoDim< T > > &vertices, const Plane< T > &plane,
               const map< size_t, TwoDim< T > > &arcs = Geometry2D< T >::mNoArcs, T arcPolygonEdgesLength = 0.0 );
    virtual ~InnerArea();
    T GetAreaValue() const;
    /// Get vertices of this instance in global cartesian coordinates
    void GetVertices( vector< Cartesian< T > > &vertices ) const;

    private:
    vector< TwoDim< T > > mVertices;
    map< size_t, TwoDim< T > > mArcs;
    T mArcPolygonEdgesLength;

    Geometry2D< T > mGeometry2D;
};


template < typename T >
InnerArea< T >::InnerArea( const vector< TwoDim< T > > &vertices, const Plane< T > &plane,
                           const map< size_t, TwoDim< T > > &arcs, T arcPolygonEdgesLength )
    : Plane< T >( plane )
    , mVertices( vertices )
    , mArcs( arcs )
    , mArcPolygonEdgesLength( arcPolygonEdgesLength )
    , mGeometry2D( vertices, arcs, arcPolygonEdgesLength )
{
}

template < typename T >
InnerArea< T >::~InnerArea()
{
}

template < typename T >
T InnerArea< T >::GetAreaValue() const
{
    return mGeometry2D.CalculateAreaValue();
}

template < typename T >
void InnerArea< T >::GetVertices( vector< Cartesian< T > > &vertices ) const
{
    vertices.resize( mVertices.size() );

    for ( size_t i = 0; i < mVertices.size(); ++i )
        vertices[i] = Plane< T >::GlobalCoordinates( mVertices[i] );
}
}
#endif /* _INNERAREA_ */
