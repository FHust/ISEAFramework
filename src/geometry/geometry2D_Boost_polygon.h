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
#ifndef _GEOMETRY2D_
#define _GEOMETRY2D_

#define USING_BOOST_POLYGON
#define GEOMETRICAL_DOUBLE_INT_RATIO 100000000.0
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4244 )
#endif

#include <boost/polygon/polygon.hpp>

#include <vector>
#include <list>
#include <map>
#include <boost/foreach.hpp>
#include "two_dim.h"
#include "tolerance.h"
#include "../thermal/exceptions/initialization_error.h"

// erase later
namespace thermal
{
extern const double globalMaxValue;
extern const double globalMinValue;
}

#ifndef __NO_STRING__
#include <fstream>
#endif


class TestAreaClasses;


namespace geometry
{
using namespace thermal;
using std::vector;
using std::list;
using std::map;

/// Geometry2D wraps the boost geometry library and represents a polygon in a two dimensional coordinate system
template < typename T = double >
class Geometry2D
{
    friend class ::TestAreaClasses;

    typedef boost::polygon::point_data< int > Point;
    typedef boost::polygon::rectangle_data< int > Rectangle;
    typedef boost::polygon::segment_data< int > Segment;
    typedef boost::polygon::polygon_data< int > Polygon;
    // typedef boost::polygon::polygon_traits<Polygon>::point_type Point;

    public:
    /**
     * @param[in] vertices Vertices of polygon in two dimensional coordinate system
     * @param[in] arcs Marks if edge of polygon is an arc, unmarked edges are straight lines, see \ref ArcConcept
     * @param[in] arcPolygonEdgesLength Supposes a length for the edge of a polygon representing a circle
     */
    explicit Geometry2D( const vector< TwoDim< T > > &vertices, const map< size_t, TwoDim< T > > &arcs = mNoArcs,
                         T arcPolygonEdgesLength = T() );
    virtual ~Geometry2D();
    /**
     * Intersection is the union of two geometric entities, in this case the overlap of two areas as a set of areas
     * @param[in] rhs Other Geometry2D instance to create the union
     * @param[out] geometry2DVector All polygons that together form intersection
     * @param[in] tolerance If one of intersection polygons is smaller than supposed by tolerance, it is neglected
     */
    void Intersection( const Geometry2D< T > &rhs, vector< Geometry2D< T > > &geometry2DVector,
                       const Tolerance< T > &tolerance = Tolerance< T >() ) const;
    /// Returns true if this instance is a polygon that is smaller than tolerance in one of its dimensions, otherwise
    /// false
    bool IsNeglectable( const Tolerance< T > tolerance ) const;
    /// Returns true of this instance and rhs are tangent by tolerance, returns false if they are not tangent,
    /// undefinined behaviour if they overlap
    bool IsAdjacentTo( const Geometry2D< T > &rhs, const Tolerance< T > tolerance ) const;
    /// Return bounding box of this instance: the smallest possible box that surrounds this polygon and whose edges are
    /// parallel to coordinate axis
    void Envelope( TwoDim< T > &lowerEdge, TwoDim< T > &higherEdge ) const;
    // TwoDim<T> Centroid() const;
    T CalculateAreaValue() const;
    /// Returns the vertices of this instance as a parameter
    void GetPoints( vector< TwoDim< T > > &points ) const;    // UnitTest only
    static const map< size_t, TwoDim< T > > mNoArcs;

    private:
    explicit Geometry2D( const Polygon &polygon );
    bool IsPointCloseToLine( const Point &point, const Point &lineStart, const Point &lineEnd, const Tolerance< T > tolerance ) const;
    Point CreatePointFromT( T x, T y ) const;
    TwoDim< T > PointToTwoDim( const Point &point ) const;
    Polygon mPolygon;
};


template < typename T >
Geometry2D< T >::Geometry2D( const vector< TwoDim< T > > &vertices, const map< size_t, TwoDim< T > > &arcs, T arcPolygonEdgesLength )
{
    if ( vertices.size() < 2 )
        InitErrorImpl( __FUNCTION__, "vertices must have at least two elements to form an area or a line" );

    if ( !arcs.empty() )
    {
        if ( arcPolygonEdgesLength <= 0.0 )
            InitErrorImpl( __FUNCTION__, "If there are arcs, arcPolygonEdgesLength must be bigger than zero" );
        if ( arcs.rbegin()->first > vertices.size() - 1 )
            InitErrorImpl( __FUNCTION__,
                           "If there are arcs, every key of arcs must be assignable to an element of vertices" );
    }


    vector< Point > polygonVertices;
    polygonVertices.reserve( vertices.size() );

    for ( size_t i = 0; i < vertices.size(); ++i )
    {
        polygonVertices.push_back( CreatePointFromT( vertices[i].Get1(), vertices[i].Get2() ) );

        // Create arc as line string, a chain of straight lines
        if ( arcs.count( i ) == 1 )
        {
            // Calculate arc angle and radius
            const size_t j = ( i + 1 ) % vertices.size();
            const TwoDim< T > arc = arcs.find( i )->second;
            const Angle< T > phi_i = ( vertices[i] - arc ).AngleInXY();
            const Angle< T > phi_j = ( vertices[j] - arc ).AngleInXY();
            Angle< T > phi = phi_i.AngleTo( phi_j );
            if ( phi.GetRad() > Angle< T >::pi )
                phi = phi - Angle< T >::circle;
            else if ( phi.GetRad() < -Angle< T >::pi )
                phi = phi + Angle< T >::circle;
            T radius = vertices[i].DistanceTo( arc );

            // At least one line for every 45°, otherwise try to make edges as long as proposed by arcPolygonEdgesLength
            size_t n = static_cast< size_t >( fabs( phi.GetRad() ) / asin( arcPolygonEdgesLength / radius ) + 1.0 );
            const size_t n_min = static_cast< size_t >( fabs( phi.GetRad() ) * 4.0 / Angle< T >::pi + 1.0 );
            if ( n < n_min )
                n = n_min;

            // This formula takes care that the polygonized arcs do not change the area value of the polygon
            const T phiVal = fabs( phi.GetRad() );
            const T sinVal = sin( 0.5 * phiVal / n );
            const T denominator = 2 * cos( 0.5 * phiVal / n ) * sin( 0.5 * phiVal / n ) * ( n - 1 );
            const T ratio = ( sqrt( sinVal * sinVal + phiVal * denominator ) - sinVal ) / denominator;
            radius *= fabs( ratio );
            // radius *= sqrt(2 * Angle<T>::pi / static_cast<double>(n) / sin(2 * Angle<T>::pi / static_cast<double>(n))
            // );//doesn't work exactly -> change that!!!

            // Create line chain
            const Angle< T > dPhi = phi / static_cast< double >( n );
            for ( size_t i = 0; i < n; ++i )
                polygonVertices.push_back(
                 CreatePointFromT( arc.Get1() + radius * cos( phi_i.GetRad() + ( 0.5 + i ) * dPhi.GetRad() ),
                                   arc.Get2() + radius * sin( phi_i.GetRad() + ( 0.5 + i ) * dPhi.GetRad() ) ) );
        }
    }


    boost::polygon::set_points( mPolygon, polygonVertices.begin(), polygonVertices.end() );

    // TODO: This outcommented code has no equivalent in Boost/Polygon, delete or implement
    // if(boost::geometry::intersects(mPolygon))
    //    InitErrorImpl(__FUNCTION__, "mPolygon is not not allowed to be self intersecting");//comming up soon:
    //    is_valid()

    // boost::geometry::correct(mPolygon);
}

template < typename T >
Geometry2D< T >::~Geometry2D()
{
}

template < typename T >
void Geometry2D< T >::Intersection( const Geometry2D< T > &rhs, vector< Geometry2D< T > > &geometry2DVector,
                                    const Tolerance< T > &tolerance ) const
{
    using namespace boost::polygon::operators;
    vector< Polygon > polygonVector;
    boost::polygon::assign( polygonVector, vector< Polygon >( 1, mPolygon ) & vector< Polygon >( 1, rhs.mPolygon ) );


    list< Geometry2D< T > > geometry2DList;
    BOOST_FOREACH ( Polygon &polygon, polygonVector )
    {
        // TODO: This outcommented code has no equivalent in Boost/Polygon, delete or implement
        // boost::geometry::correct(polygon);
        Geometry2D< T > geometry2D( polygon );

        if ( !geometry2D.IsNeglectable( tolerance ) )
            geometry2DList.push_back( geometry2D );
    }


    geometry2DVector.clear();
    geometry2DVector.insert( geometry2DVector.end(), geometry2DList.begin(), geometry2DList.end() );
}

template < typename T >
bool Geometry2D< T >::IsNeglectable( const Tolerance< T > tolerance ) const
{
    if ( tolerance.mLength == 0.0 )
        return false;

    // General way two determine smallest dimension of area, if that dimension is smaller than tolerance, this instance
    // is neglectable
    if ( 2.0 * CalculateAreaValue() * GEOMETRICAL_DOUBLE_INT_RATIO / boost::polygon::perimeter( mPolygon ) < tolerance.mLength )
        return true;
    else
        return false;
}

template < typename T >
bool Geometry2D< T >::IsAdjacentTo( const Geometry2D< T > &rhs, const Tolerance< T > tolerance ) const
{
    vector< Point > thisPoints( mPolygon.begin(), mPolygon.end() );
    vector< Point > rhsPoints( rhs.mPolygon.begin(), rhs.mPolygon.end() );

    for ( size_t i = 0; i < thisPoints.size(); ++i )
    {
        Segment thisSegment( thisPoints[i], thisPoints[( i + 1 ) % thisPoints.size()] );
        for ( size_t j = 0; j < rhsPoints.size(); ++j )
        {
            Segment rhsSegment( rhsPoints[j], rhsPoints[( j + 1 ) % rhsPoints.size()] );
            if ( boost::polygon::euclidean_distance( thisSegment, rhsSegment ) / GEOMETRICAL_DOUBLE_INT_RATIO < tolerance.mLength )
                return true;
        }
    }

    return false;
}

template < typename T >
void Geometry2D< T >::Envelope( TwoDim< T > &lowerEdge, TwoDim< T > &higherEdge ) const
{
    Rectangle envelopeBox;
    boost::polygon::extents( envelopeBox, mPolygon );

    lowerEdge = PointToTwoDim( boost::polygon::ll( envelopeBox ) );
    higherEdge = PointToTwoDim( boost::polygon::ur( envelopeBox ) );
}

// TODO: This outcommented code has no equivalent in Boost/Polygon, delete or implement
/*template<typename T>
TwoDim<T> Geometry2D<T>::Centroid() const
{
    Point centroidPoint;
    boost::polygon::center(centroidPoint, mPolygon);

    return PointToTwoDim(centroidPoint);
}*/

template < typename T >
T Geometry2D< T >::CalculateAreaValue() const
{
    return boost::polygon::area( mPolygon ) / ( GEOMETRICAL_DOUBLE_INT_RATIO * GEOMETRICAL_DOUBLE_INT_RATIO );
}

template < typename T >
void Geometry2D< T >::GetPoints( vector< TwoDim< T > > &points ) const
{
    points.clear();
    points.reserve( mPolygon.size() );

    for ( typename Polygon::iterator_type it = mPolygon.begin(); it != mPolygon.end(); ++it )
        points.push_back( PointToTwoDim( *it ) );
}

template < typename T >
Geometry2D< T >::Geometry2D( const Polygon &polygon )
    : mPolygon( polygon )
{
}

template < typename T >
bool Geometry2D< T >::IsPointCloseToLine( const Point &point, const Point &lineStart, const Point &lineEnd,
                                          const Tolerance< T > tolerance ) const
{
    Segment segment( lineStart, lineEnd );
    T length = boost::polygon::length( segment );

    if ( boost::polygon::euclidean_distance( segment, point ) / GEOMETRICAL_DOUBLE_INT_RATIO < tolerance.mLength )
        if ( boost::polygon::euclidean_distance( lineStart, point ) / GEOMETRICAL_DOUBLE_INT_RATIO < length - tolerance.mLength &&
             boost::polygon::euclidean_distance( lineEnd, point ) / GEOMETRICAL_DOUBLE_INT_RATIO < length - tolerance.mLength )
            return true;

    return false;
}

template < typename T >
typename Geometry2D< T >::Point Geometry2D< T >::CreatePointFromT( T x, T y ) const
{
    return Point( x * GEOMETRICAL_DOUBLE_INT_RATIO, y * GEOMETRICAL_DOUBLE_INT_RATIO );
}

template < typename T >
TwoDim< T > Geometry2D< T >::PointToTwoDim( const Point &point ) const
{
    return TwoDim< T >( point.x() / GEOMETRICAL_DOUBLE_INT_RATIO, point.y() / GEOMETRICAL_DOUBLE_INT_RATIO );
}

template < typename T >
const map< size_t, TwoDim< T > > Geometry2D< T >::mNoArcs;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
