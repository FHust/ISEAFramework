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

#define USING_BOOST_GEOMETRY


#if defined( _MSC_VER )
#pragma warning( push )
#pragma warning( disable : 4005 )
#pragma warning( disable : 4100 )
#endif

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#if defined( _MSC_VER )
#pragma warning( pop )
#endif


#include <cmath>
#include <vector>
#include <map>
#include <boost/foreach.hpp>
#include "angle.h"
#include "two_dim.h"
#include "unit_vector.h"
#include "tolerance.h"
#include "../exceptions/error_proto.h"

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

    typedef boost::geometry::model::d2::point_xy< T > Point;
    typedef boost::geometry::model::box< Point > Box;
    typedef boost::geometry::model::segment< Point > Segment;
    typedef boost::geometry::model::linestring< Point > Linestring;
    typedef boost::geometry::model::ring< Point, false, false > Ring;
    typedef boost::geometry::model::polygon< Point, false, false > Polygon;

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
    /// Returns true if any lines of the polygon of this instance are adjacent to each other. ATTENTION: Only works
    /// correctly if this instance was constructed without arcs.
    bool IsSelfAdjacent( const Tolerance< T > tolerance ) const;
    /**Returns true if this instance is an unintended rest created during intersection of two polygonized arcs lieing on
     * the same circle, otherwise false
     * @param[in] centerPoint Center point of circle on which both intersecting arcs lied on
     * @param[in] tolerance Determines maximum expansion in radius dimension around centerPoint of this instance
     * @note This function is not in use at the moment. It could be used to take into account polygonization while
     * cutting areas confined by arcs from other areas.
     */
    // bool IsArcIntersectionRest(const TwoDim<T> &centerPoint, const Tolerance<T> tolerance) const;
    /// Returns true of this instance and rhs are adjacent by tolerance, returns false if they are not adjacent,
    /// undefinined behaviour if they overlap. ATTENTION: Only works correctly if this instance was constructed without
    /// arcs.
    bool IsAdjacentTo( const Geometry2D< T > &rhs, const Tolerance< T > tolerance ) const;
    /// Returns true of this instance and rhs are tangent by tolerance, returns false if they are not tangent,
    /// undefinined behaviour if they overlap. ATTENTION: Only works correctly if this instance was constructed without
    /// arcs.
    bool IsTangentTo( const Geometry2D< T > &rhs, const Tolerance< T > tolerance ) const;
    /// Return bounding box of this instance: the smallest possible box that surrounds this polygon and whose edges are
    /// parallel to the coordinate axis
    void Envelope( TwoDim< T > &lowerEdge, TwoDim< T > &higherEdge ) const;
    ///@note This function is not in use at the moment. It could be used to calculate of the distance between centre of
    /// an external area and the leading edge of a fluid flow cooling that external area
    TwoDim< T > Centroid() const;
    T CalculateAreaValue() const;
    void PrintToSVGFile( const char *fileName ) const;
    /// Returns the vertices of this instance as a parameter
    void GetPoints( vector< TwoDim< T > > &points ) const;
    static const map< size_t, TwoDim< T > > mNoArcs;

    private:
    explicit Geometry2D( const Polygon &polygon );
    Polygon mPolygon;
};


template < typename T >
Geometry2D< T >::Geometry2D( const vector< TwoDim< T > > &vertices, const map< size_t, TwoDim< T > > &arcs, T arcPolygonEdgesLength )
{
    if ( vertices.size() < 2 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "verticesTwoElements" );
    //    InitErrorImpl( __FUNCTION__, "vertices must have at least two elements to form an area or a line" );

    if ( !arcs.empty() )
    {
        if ( arcPolygonEdgesLength <= 0.0 )
            // InitErrorImpl( __FUNCTION__, "If there are arcs, arcPolygonEdgesLength must be bigger than zero" );
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "arcPolygonEdgesLength" );
        // InitErrorImpl( __FUNCTION__, "If there are arcs, arcPolygonEdgesLength must be bigger than zero" )
        if ( arcs.rbegin()->first > vertices.size() - 1 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "arcKey" );
        // InitErrorImpl( __FUNCTION__,
        //               "If there are arcs, every key of arcs must be assignable to an element of vertices" );
    }


    vector< Point > polygonVertices;
    polygonVertices.reserve( vertices.size() );

    for ( size_t i = 0; i < vertices.size(); ++i )
    {
        polygonVertices.push_back( Point( vertices[i].Get1(), vertices[i].Get2() ) );

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
            const size_t n_min = std::max( static_cast< size_t >( fabs( phi.GetRad() ) * 8.0 / Angle< T >::pi + 1.0 ),
                                           static_cast< size_t >( 2 ) );
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
                polygonVertices.push_back( Point( arc.Get1() + radius * cos( phi_i.GetRad() + ( 0.5 + i ) * dPhi.GetRad() ),
                                                  arc.Get2() + radius * sin( phi_i.GetRad() + ( 0.5 + i ) * dPhi.GetRad() ) ) );
        }
    }


    Ring ring( polygonVertices.begin(), polygonVertices.end() );
    boost::geometry::assign( mPolygon, ring );

    if ( boost::geometry::intersects( mPolygon ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "PolygonSelfIntersecting" );
    // InitErrorImpl( __FUNCTION__,
    //               "mPolygon is not not allowed to be self intersecting" );    // comming up soon: is_valid()

    boost::geometry::correct( mPolygon );
}

template < typename T >
Geometry2D< T >::~Geometry2D()
{
}

template < typename T >
void Geometry2D< T >::Intersection( const Geometry2D< T > &rhs, vector< Geometry2D< T > > &geometry2DVector,
                                    const Tolerance< T > &tolerance ) const
{
    vector< Polygon > polygonVector;
    boost::geometry::intersection( mPolygon, rhs.mPolygon, polygonVector );


    list< Geometry2D< T > > geometry2DList;
    BOOST_FOREACH ( Polygon &polygon, polygonVector )
    {
        boost::geometry::correct( polygon );
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
    if ( 2.0 * CalculateAreaValue() / boost::geometry::perimeter( mPolygon ) < tolerance.mLength )
        return true;
    else
        return false;
}

template < typename T >
bool Geometry2D< T >::IsSelfAdjacent( const Tolerance< T > tolerance ) const
{
    vector< Point > points = mPolygon.outer();

    // If the two ends of a line of one polygon are both adjacent to one line of the other polygon
    for ( size_t i = 0; i < points.size(); ++i )
        for ( size_t j = 0; j < points.size(); ++j )
        {
            if ( i != j )
            {
                size_t jpp = ( j + 1 ) % points.size();

                if ( boost::geometry::distance( points[i], Segment( points[j], points[jpp] ) ) < tolerance.mLength )
                    if ( boost::geometry::distance( points[( i + 1 ) % points.size()], Segment( points[j], points[jpp] ) ) <
                         tolerance.mLength )
                        return true;
            }
        }


    // If any two lines of one and the other polygon are collinear and one end is adjacent to a line of the other
    // polygon
    for ( size_t i = 0; i < points.size(); ++i )
        for ( size_t j = 0; j < points.size(); ++j )
        {
            if ( i != j && i != ( j + 1 ) % points.size() && i != ( j - 1 ) % points.size() &&
                 i != ( j + 2 ) % points.size() && i != ( j - 2 ) % points.size() )
            {
                size_t ipp = ( i + 1 ) % points.size();
                Segment segment( points[i], points[ipp] );

                if ( boost::geometry::distance( points[j], segment ) < tolerance.mLength )
                {
                    size_t jpp = ( j + 1 ) % points.size();
                    size_t jmm = ( j - 1 ) % points.size();
                    UnitVector< T > line( points[i].x() - points[ipp].x(), points[i].y() - points[ipp].y(), 0.0 );

                    UnitVector< T > rhsLine( points[j].x() - points[jpp].x(), points[j].y() - points[jpp].y(), 0.0 );
                    if ( line.IsEqualDirection( rhsLine, tolerance.mAngle ) || line.IsOppositeDirection( rhsLine, tolerance.mAngle ) )
                    {
                        T lengthRhsLine = boost::geometry::distance( points[j], points[jpp] ) - tolerance.mLength;
                        if ( ( boost::geometry::distance( points[j], points[i] ) < lengthRhsLine &&
                               boost::geometry::distance( points[jpp], points[i] ) < lengthRhsLine ) ||
                             ( boost::geometry::distance( points[j], points[ipp] ) < lengthRhsLine &&
                               boost::geometry::distance( points[jpp], points[ipp] ) < lengthRhsLine ) )
                            return true;
                    }

                    rhsLine = UnitVector< T >( points[j].x() - points[jmm].x(), points[j].y() - points[jmm].y(), 0.0 );
                    if ( line.IsEqualDirection( rhsLine, tolerance.mAngle ) || line.IsOppositeDirection( rhsLine, tolerance.mAngle ) )
                    {
                        T lengthRhsLine = boost::geometry::distance( points[j], points[jmm] ) - tolerance.mLength;
                        if ( ( boost::geometry::distance( points[j], points[i] ) < lengthRhsLine &&
                               boost::geometry::distance( points[jmm], points[i] ) < lengthRhsLine ) ||
                             ( boost::geometry::distance( points[j], points[ipp] ) < lengthRhsLine &&
                               boost::geometry::distance( points[jmm], points[ipp] ) < lengthRhsLine ) )
                            return true;
                    }
                }
            }
        }


    return false;
}
/*
template<typename T>
bool Geometry2D<T>::IsArcIntersectionRest(const TwoDim<T> &centerPoint, const Tolerance<T> tolerance) const
{
    const Point center(centerPoint.Get1(), centerPoint.Get2());

    //Max and Min distance of all vertices of polygon from center point
    T minDistance = globalMaxValue;
    T maxDistance = globalMinValue;

    BOOST_FOREACH(const Point &polygonPoint, mPolygon.outer())
    {
        const T distance = boost::geometry::distance(center, polygonPoint);
        if(minDistance > distance)
            minDistance = distance;
        if(maxDistance < distance)
            maxDistance = distance;
    }

    if(maxDistance - minDistance < tolerance.mArcLinearisation)
        return true;
    else
        return false;
}
*/
template < typename T >
bool Geometry2D< T >::IsAdjacentTo( const Geometry2D< T > &rhs, const Tolerance< T > tolerance ) const
{
    vector< Point > points = mPolygon.outer();
    vector< Point > rhsPoints = rhs.mPolygon.outer();

    // If the two ends of a line of one polygon are both adjacent to one line of the other polygon
    for ( size_t i = 0; i < points.size(); ++i )
        for ( size_t j = 0; j < rhsPoints.size(); ++j )
        {
            size_t jpp = ( j + 1 ) % rhsPoints.size();

            if ( boost::geometry::distance( points[i], Segment( rhsPoints[j], rhsPoints[jpp] ) ) < tolerance.mLength )
                if ( boost::geometry::distance( points[( i + 1 ) % points.size()],
                                                Segment( rhsPoints[j], rhsPoints[jpp] ) ) < tolerance.mLength )
                    return true;
        }
    for ( size_t i = 0; i < rhsPoints.size(); ++i )
        for ( size_t j = 0; j < points.size(); ++j )
        {
            size_t jpp = ( j + 1 ) % points.size();

            if ( boost::geometry::distance( rhsPoints[i], Segment( points[j], points[jpp] ) ) < tolerance.mLength )
                if ( boost::geometry::distance( rhsPoints[( i + 1 ) % rhsPoints.size()],
                                                Segment( points[j], points[jpp] ) ) < tolerance.mLength )
                    return true;
        }


    // If any two lines of one and the other polygon are collinear and one end is adjacent to a line of the other
    // polygon
    for ( size_t i = 0; i < points.size(); ++i )
        for ( size_t j = 0; j < rhsPoints.size(); ++j )
        {
            size_t ipp = ( i + 1 ) % points.size();
            Segment segment( points[i], points[ipp] );

            if ( boost::geometry::distance( rhsPoints[j], segment ) < tolerance.mLength )
            {
                size_t jpp = ( j + 1 ) % rhsPoints.size();
                size_t jmm = ( j == 0 ) ? ( rhsPoints.size() - 1 ) : ( j - 1 );
                UnitVector< T > line( points[i].x() - points[ipp].x(), points[i].y() - points[ipp].y(), 0.0 );

                UnitVector< T > rhsLine( rhsPoints[j].x() - rhsPoints[jpp].x(), rhsPoints[j].y() - rhsPoints[jpp].y(), 0.0 );
                if ( line.IsEqualDirection( rhsLine, tolerance.mAngle ) || line.IsOppositeDirection( rhsLine, tolerance.mAngle ) )
                {
                    T lengthRhsLine = boost::geometry::distance( rhsPoints[j], rhsPoints[jpp] ) - tolerance.mLength;
                    if ( ( boost::geometry::distance( rhsPoints[j], points[i] ) < lengthRhsLine &&
                           boost::geometry::distance( rhsPoints[jpp], points[i] ) < lengthRhsLine ) ||
                         ( boost::geometry::distance( rhsPoints[j], points[ipp] ) < lengthRhsLine &&
                           boost::geometry::distance( rhsPoints[jpp], points[ipp] ) < lengthRhsLine ) )
                        return true;
                }

                rhsLine = UnitVector< T >( rhsPoints[j].x() - rhsPoints[jmm].x(), rhsPoints[j].y() - rhsPoints[jmm].y(), 0.0 );
                if ( line.IsEqualDirection( rhsLine, tolerance.mAngle ) || line.IsOppositeDirection( rhsLine, tolerance.mAngle ) )
                {
                    T lengthRhsLine = boost::geometry::distance( rhsPoints[j], rhsPoints[jmm] ) - tolerance.mLength;
                    if ( ( boost::geometry::distance( rhsPoints[j], points[i] ) < lengthRhsLine &&
                           boost::geometry::distance( rhsPoints[jmm], points[i] ) < lengthRhsLine ) ||
                         ( boost::geometry::distance( rhsPoints[j], points[ipp] ) < lengthRhsLine &&
                           boost::geometry::distance( rhsPoints[jmm], points[ipp] ) < lengthRhsLine ) )
                        return true;
                }
            }
        }


    return false;
}

template < typename T >
bool Geometry2D< T >::IsTangentTo( const Geometry2D< T > &rhs, const Tolerance< T > tolerance ) const
{
    // TODO: Boost geometry 1.56 has a distance function to compare two polygons. This reduces the code of this methode
    // to one line! But it could be slower than the existing code?

    // Check if one of the vertices of this polygen is tangent to rhs polygon by tolerance. If this is the case, both
    // are tangent.
    BOOST_FOREACH ( const Point &polygonVertex, mPolygon.outer() )
        if ( boost::geometry::distance( polygonVertex, rhs.mPolygon ) < tolerance.mLength )
            return true;

    // Check if one of the vertices of rhs polygen is tangent to this polygon by tolerance. If this is the case, both
    // are tangent.
    BOOST_FOREACH ( const Point &polygonVertex, rhs.mPolygon.outer() )
        if ( boost::geometry::distance( polygonVertex, mPolygon ) < tolerance.mLength )
            return true;

    return false;
}

// TODO: This outcommented code has no function at the moment, delete or implement
/*
template<typename T>
vector<Geometry2D<T> > Geometry2D<T>::SubstractedBy(const Geometry2D<T> &rhs) const
{
    vector<Polygon> polygonVector;
    boost::geometry::difference(mPolygon, rhs.mPolygon, polygonVector);

    vector<Geometry2D<T> > geometry2DVector;
    geometry2DVector.reserve(polygonVector.size());

    BOOST_FOREACH(const Polygon &polygon, polygonVector)
        geometry2DVector.push_back(Geometry2D<T>(polygon));

    return geometry2DVector;
}*/
/*
template<typename T>
bool Geometry2D<T>::Surrounds(const Geometry2D<T> &rhs) const
{
    vector<Point> points = rhs.mPolygon.outer();

    BOOST_FORREACH(const Point &point, points)
        if(!boost::geometry::within(point, mPolygon));
            return false;

    return true;
}*/
/*
template<typename T>
bool Geometry2D<T>::IsSurroundedBy(const Geometry2D<T> &rhs) const
{
    vector<Point> points = mPolygon.outer();

    BOOST_FORREACH(const Point &point, points)
        if(!boost::geometry::within(point, rhs.mPolygon));
            return false;

    return true;
}*/

template < typename T >
void Geometry2D< T >::Envelope( TwoDim< T > &lowerEdge, TwoDim< T > &higherEdge ) const
{
    Box envelopeBox;
    boost::geometry::envelope( mPolygon, envelopeBox );

    lowerEdge = TwoDim< T >( envelopeBox.min_corner().x(), envelopeBox.min_corner().y() );
    higherEdge = TwoDim< T >( envelopeBox.max_corner().x(), envelopeBox.max_corner().y() );
}

template < typename T >
TwoDim< T > Geometry2D< T >::Centroid() const
{
    Point centroidPoint( 0.0, 0.0 );
    boost::geometry::centroid( mPolygon, centroidPoint );

    return TwoDim< T >( centroidPoint.x(), centroidPoint.y() );
}

template < typename T >
T Geometry2D< T >::CalculateAreaValue() const
{
    return boost::geometry::area( mPolygon );
}

template < typename T >
void Geometry2D< T >::PrintToSVGFile( const char *
#ifdef _BOOSTGEOMETRY_HAS_SVGMAPPER_
                                       fileName
#endif
                                      ) const
{
#ifdef _BOOSTGEOMETRY_HAS_SVGMAPPER_
    std::ofstream svgFile( fileName );
    boost::geometry::svg_mapper< Point > mapper( svgFile, 400, 400 );
    mapper.add( mPolygon );

    Linestring xAxisBig;
    xAxisBig.push_back( Point( 0.0, 0.0 ) );
    xAxisBig.push_back( Point( 10.0, 0.0 ) );
    mapper.add( xAxisBig );

    Linestring yAxisBig;
    yAxisBig.push_back( Point( 0.0, 0.0 ) );
    yAxisBig.push_back( Point( 0.0, 10.0 ) );
    mapper.add( yAxisBig );

    Linestring xAxisSmall;
    xAxisSmall.push_back( Point( 0.0, 0.0 ) );
    xAxisSmall.push_back( Point( 1.0, 0.0 ) );
    mapper.add( xAxisSmall );

    Linestring yAxisSmall;
    yAxisSmall.push_back( Point( 0.0, 0.0 ) );
    yAxisSmall.push_back( Point( 0.0, 1.0 ) );
    mapper.add( yAxisSmall );

    mapper.map( mPolygon, "fill-opacity:0.3;fill:rgb(51,51,153);stroke:rgb(51,51,153);stroke-width:2" );
    mapper.map( xAxisBig, "opacity:0.4;fill:none;stroke:rgb(100,100,100);stroke-width:5", 5 );
    mapper.map( yAxisBig, "opacity:0.4;fill:none;stroke:rgb(100,100,100);stroke-width:5", 5 );
    mapper.map( xAxisSmall, "opacity:0.4;fill:none;stroke:rgb(250,250,250);stroke-width:5", 5 );
    mapper.map( yAxisSmall, "opacity:0.4;fill:none;stroke:rgb(250,250,250);stroke-width:5", 5 );
#endif
}

template < typename T >
void Geometry2D< T >::GetPoints( vector< TwoDim< T > > &points ) const
{
    points.clear();
    points.reserve( mPolygon.outer().size() );

    BOOST_FOREACH ( const Point &point, mPolygon.outer() )
        points.push_back( TwoDim< T >( point.x(), point.y() ) );
}

template < typename T >
Geometry2D< T >::Geometry2D( const Polygon &polygon )
    : mPolygon( polygon )
{
}

template < typename T >
const map< size_t, TwoDim< T > > Geometry2D< T >::mNoArcs;
}
#endif
