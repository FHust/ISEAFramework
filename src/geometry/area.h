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
#ifndef _AREA_
#define _AREA_

#include <vector>
#include <map>
#include <set>
#include <utility>
#include "cartesian.h"
#include "tolerance.h"
#include "plane.h"
#include "geometry2D.h"
#include "two_dim.h"


// erase later
namespace thermal
{
extern const double globalMaxValue;
extern const double globalMinValue;
}

class TestAreaClasses;


namespace geometry
{
using std::vector;
using std::map;
using std::set;
using std::pair;

/// Location enumerates the assigning of the location of a surface area to a thermal element/block
enum Location
{
    TOP,
    SIDE,
    BOTTOM
};

/// Area fuses Plane and Geometry2D to offer all functionality of a surface area of a thermal element
template < typename T = double >
class Area : public Plane< T >
{
    friend class ::TestAreaClasses;

    public:
    /**
     * @param[in] vertices Vertices of area in coordinates system of plane
     * @param[in] plane Base class Plane can be inserted directly
     * @param[in] distanceToGridVertex Distance to grid vertex of a thermal element
     * @param[in] conductivity conductivity between the grid vertex of a thermla element and this area
     * @param[in] location The location of the area, can be TOP, BOTTOM or SIDE
     * @param[in] arcs Marks if edge of polygon is an arc by containing its circle center, unmarked edges are straight
     * lines. If the key of a map element is i, the defined edge is the edge between points vertices[i] and
     * vertices[i%vertices.size()].
     * @param[in] arcPolygonEdgesLength Supposes a length for the edge of a polygon representing a circle
     */
    Area( const vector< TwoDim< T > > &vertices, const Plane< T > &plane, T distanceToGridVertex, T conductivity,
          Location location, const map< size_t, TwoDim< T > > &arcs = Geometry2D< T >::mNoArcs, T arcPolygonEdgesLength = 0.0 );
    virtual ~Area();
    /// Returns area value of overlap
    T OverlapWith( const Area< T > &rhs, const Tolerance< T > &tolerance ) const;
    /**
     * Aggregates several areas to determine bounding box and centroids of aggregated areas in the coordinate system of
     * this instance
     * @param[in] areasToAggregate Set of pointers to Area that are to be aggregated, areas that are not coplanar to
     * this instance are neglected
     * @param[out] lowestEdge Lowest edge of bounding box
     * @param[out] highestEdge Highest edge of bounding box
     * @param[out] centroids Centroids of areas, mapped by pointer to Area
     */
    void Aggregate( const set< Area< T > * > &areasToAggregate, TwoDim< T > &lowestEdge, TwoDim< T > &highestEdge,
                    map< Area< T > *, TwoDim< T > > &centroids, const Tolerance< T > &tolerance = Tolerance< T >() ) const;
    /// Returns distance to grid vertex passed to constructor
    T GetDistanceToGridVertex() const;
    /// Returns conductivity passed to constructor
    T GetConductivity() const;
    T GetAreaValue() const;
    /// Get vertices of this instance in global cartesian coordinates
    void GetVertices( vector< Cartesian< T > > &vertices ) const;
    /// Returns Location of this instance
    Location GetLocation() const { return mLocation; }

    private:
    /**
     * Gets another Area as input and returns the vertices of Geometry2D<T> of that area in the coordinate system of
     * this instance. If areas are not coplanar, an exceptions is thrown.
     * @param[in] rhsArea The other area
     * @param[out] vertices Vertices of Geometry2D<T> of rhsArea transformed into coordinate system of this instance
     * @throw Throws an exception if this instance and rhsArea are not coplanar
     */
    void TransformToThisCoordinateSystem( const Area< T > &rhsArea, vector< TwoDim< T > > &vertices,
                                          const Tolerance< T > &tolerance ) const;
    vector< TwoDim< T > > mVertices;
    map< size_t, TwoDim< T > > mArcs;
    T mArcPolygonEdgesLength;

    T mDistanceToGridVertex;
    T mConductivity;
    Geometry2D< T > mGeometry2D;
    Location mLocation;
};


template < typename T >
Area< T >::Area( const vector< TwoDim< T > > &vertices, const Plane< T > &plane, T distanceToGridVertex, T conductivity,
                 Location location, const map< size_t, TwoDim< T > > &arcs, T arcPolygonEdgesLength )
    : Plane< T >( plane )
    , mVertices( vertices )
    , mArcs( arcs )
    , mArcPolygonEdgesLength( arcPolygonEdgesLength )
    , mDistanceToGridVertex( distanceToGridVertex )
    , mConductivity( conductivity )
    , mGeometry2D( vertices, arcs, arcPolygonEdgesLength )
    , mLocation( location )
{
    if ( distanceToGridVertex < 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "distanceToGridVertex" );
    if ( conductivity < 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ConductivityLessThanZero" );
}

template < typename T >
Area< T >::~Area()
{
}

template < typename T >
T Area< T >::OverlapWith( const Area< T > &rhs, const Tolerance< T > &tolerance ) const
{
    if ( !Plane< T >::IsCoplanarTo( rhs, tolerance ) )
        return 0.0;

    // Get vertices and arcs of rhs in coordinate system of this instance, then intersect and calulate area value of
    // intersection
    vector< TwoDim< T > > rhsVertices;
    TransformToThisCoordinateSystem( rhs, rhsVertices, tolerance );

    Geometry2D< T > transformedRhsPolygon( rhsVertices );
    vector< Geometry2D< T > > intersection;
    mGeometry2D.Intersection( transformedRhsPolygon, intersection, tolerance );

    T totalArea = 0.0;
    BOOST_FOREACH ( const Geometry2D< T > &oneOfIntersection, intersection )
    {
        totalArea += oneOfIntersection.CalculateAreaValue();
    }

    return totalArea;
}

template < typename T >
void Area< T >::Aggregate( const set< Area< T > * > &areasToAggregate, TwoDim< T > &lowestEdge, TwoDim< T > &highestEdge,
                           map< Area< T > *, TwoDim< T > > &centroids, const Tolerance< T > &tolerance ) const
{
    centroids.clear();
    // Lowest and highest edges of bounding boxes of all areas
    T lowest1 = globalMaxValue, lowest2 = globalMaxValue;
    T highest1 = globalMinValue, highest2 = globalMinValue;

    BOOST_FOREACH ( Area< T > *areaPointer, areasToAggregate )
    {
        // neglect areas that are not coplanar to this instance
        if ( !Plane< T >::IsCoplanarTo( *areaPointer, tolerance ) )
            continue;

        vector< TwoDim< T > > vertices;
        TransformToThisCoordinateSystem( *areaPointer, vertices, tolerance );

        Geometry2D< T > geometry2D( vertices );
#ifdef USING_BOOST_GEOMETRY
        centroids.insert( pair< Area< T > *, TwoDim< T > >( areaPointer, geometry2D.Centroid() ) );
#endif
        TwoDim< T > lowerEdge, higherEdge;
        geometry2D.Envelope( lowerEdge, higherEdge );

        if ( lowest1 > lowerEdge.Get1() )
            lowest1 = lowerEdge.Get1();
        if ( lowest2 > lowerEdge.Get2() )
            lowest2 = lowerEdge.Get2();
        if ( highest1 < higherEdge.Get1() )
            highest1 = higherEdge.Get1();
        if ( highest2 < higherEdge.Get2() )
            highest2 = higherEdge.Get2();
    }

    lowestEdge = TwoDim< T >( lowest1, lowest2 );
    highestEdge = TwoDim< T >( highest1, highest2 );
}

template < typename T >
T Area< T >::GetDistanceToGridVertex() const
{
    return mDistanceToGridVertex;
}

template < typename T >
T Area< T >::GetConductivity() const
{
    return mConductivity;
}

template < typename T >
T Area< T >::GetAreaValue() const
{
    return mGeometry2D.CalculateAreaValue();
}

template < typename T >
void Area< T >::GetVertices( vector< Cartesian< T > > &vertices ) const
{
    vertices.resize( mVertices.size() );

    for ( size_t i = 0; i < mVertices.size(); ++i )
        vertices[i] = Plane< T >::GlobalCoordinates( mVertices[i] );
}

template < typename T >
void Area< T >::TransformToThisCoordinateSystem( const Area< T > &rhsArea, vector< TwoDim< T > > &vertices,
                                                 const Tolerance< T > &tolerance ) const
{

    // No transformation is possible if this instance and rhsArea are not coplanar
    if ( !Plane< T >::IsCoplanarTo( rhsArea, tolerance ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoTransformationPossible" );

    typename Plane< T >::Transformation transformation = Plane< T >::CoordinateTransformationData( rhsArea, tolerance );

    rhsArea.mGeometry2D.GetPoints( vertices );
    BOOST_FOREACH ( TwoDim< T > &vertex, vertices )
    {
        const T v1 = vertex.Get1();
        const T v2 = vertex.Get2();
        vertex = TwoDim< T >( transformation.m1_1 * v1 + transformation.m1_2 * v2 + transformation.mDelta1,
                              transformation.m2_1 * v1 + transformation.m2_2 * v2 + transformation.mDelta2 );
    }
}
}
#endif
