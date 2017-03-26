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
#include "TestAreaClasses.h"
#include "../thermal/exception_tester.h"
#include <string>

using namespace geometry;
static const double sDelta = 0.000001;

#ifndef __EXCEPTIONS__
bool geoExecThrown = false;
#endif


void TestAreaClasses::TestEasyPlane()
{
    Tolerance<> tolerance( 0.000001, Angle<>::Deg( 0.001 ), 0.1 );

    UnitVector<> xAxis( 1.0, 0.0, 0.0 );
    UnitVector<> yAxis( 0.0, 1.0, 0.0 );
    UnitVector<> zAxis( 0.0, 0.0, 1.0 );
    UnitVector<> xyAxis( 1.0, 1.0, 0.0 );
    UnitVector<> minus_xyAxis( -1.0, -1.0, 0.0 );
    UnitVector<> yxAxis( 1.0, 2.0, 0.0 );
    Cartesian<> emplacementZero( 0.0, 0.0, 0.0 );
    Cartesian<> emplacementXY( 2.0, 2.0, -3.0 );
    Cartesian<> emplacementDelta( 1.0, 5.0, 2.0 );

    Plane<> xyPlane( emplacementXY, xAxis, yAxis );
    Plane<> xzPlane( emplacementXY, xAxis, zAxis );
    Plane<> yzPlane( emplacementXY, yAxis, zAxis );
    Plane<> verticalPlane( emplacementXY, xyAxis, zAxis );


    TS_ASSERT( xyPlane.GetNormalVector().Equals( UnitVector<>( 0.0, 0.0, 1.0 ), 0.001 ) );
    TS_ASSERT_EQUALS( xyPlane.mPlaneType, Plane<>::HORIZONTAL_EASY_PLANE );
    TS_ASSERT_EQUALS( xzPlane.mPlaneType, Plane<>::VERTICAL_EASY_PLANE );
    TS_ASSERT_EQUALS( yzPlane.mPlaneType, Plane<>::VERTICAL_EASY_PLANE );
    TS_ASSERT_EQUALS( verticalPlane.mPlaneType, Plane<>::VERTICAL_EASY_PLANE );


    Plane<> verticalPlaneOrigin( emplacementZero, xyAxis, zAxis );
    Plane<> verticalPlaneMinusDir( emplacementXY, minus_xyAxis, zAxis );
    Plane<> verticalPlaneDEmpl( emplacementDelta, minus_xyAxis, zAxis );
    Plane<> verticalPlaneFail( emplacementXY, yxAxis, zAxis );

    TS_ASSERT( verticalPlane.IsParallelTo( verticalPlaneOrigin, tolerance ) );
    TS_ASSERT( verticalPlane.IsParallelTo( verticalPlaneMinusDir, tolerance ) );
    TS_ASSERT( verticalPlane.IsParallelTo( verticalPlaneDEmpl, tolerance ) );
    TS_ASSERT_EQUALS( verticalPlane.IsParallelTo( verticalPlaneFail, tolerance ), false );
    TS_ASSERT_EQUALS( verticalPlane.IsParallelTo( xyPlane, tolerance ), false );
    TS_ASSERT( verticalPlane.IsParallelTo( UnitVector<>( 1.0, -1.0, 0.0 ), tolerance ) );
    TS_ASSERT( verticalPlane.IsParallelTo( UnitVector<>( -1.0, 1.0, 0.0 ), tolerance ) );
    TS_ASSERT_EQUALS( verticalPlane.IsParallelTo( UnitVector<>( 0.99, -1.01, 0.0 ), tolerance ), false );
    TS_ASSERT( xyPlane.IsHorizontal( tolerance ) );
    TS_ASSERT_EQUALS( verticalPlane.IsHorizontal( tolerance ), false );

    TS_ASSERT_DELTA( verticalPlane.DistanceToEmplacementVectorOf( verticalPlaneOrigin ), 0.0, sDelta );
    TS_ASSERT_DELTA( verticalPlane.DistanceToEmplacementVectorOf( verticalPlaneMinusDir ), 0.0, sDelta );
    TS_ASSERT_DELTA( verticalPlane.DistanceToEmplacementVectorOf( verticalPlaneDEmpl ), 2.0 * sqrt( 2.0 ), sDelta );

    TS_ASSERT( verticalPlane.IsCoplanarTo( verticalPlaneOrigin, tolerance ) );
    TS_ASSERT( verticalPlane.IsCoplanarTo( verticalPlaneMinusDir, tolerance ) );
    TS_ASSERT_EQUALS( verticalPlane.IsCoplanarTo( verticalPlaneDEmpl, tolerance ), false );


    Cartesian<> coordinates = verticalPlane.GlobalCoordinates( TwoDim<>( 1.0, -2.0 ) );
    TS_ASSERT_DELTA( coordinates.GetX(), 1.0 / sqrt( 2.0 ) + 2.0, sDelta );
    TS_ASSERT_DELTA( coordinates.GetY(), 1.0 / sqrt( 2.0 ) + 2.0, sDelta );
    TS_ASSERT_DELTA( coordinates.GetZ(), -2.0 - 3.0, sDelta );


    Plane<>::Transformation transformation = verticalPlane.CoordinateTransformationData( verticalPlaneOrigin, tolerance );
    TS_ASSERT_DELTA( transformation.m1_1, 1.0, sDelta );
    TS_ASSERT_DELTA( transformation.m1_2, 0.0, sDelta );
    TS_ASSERT_DELTA( transformation.m2_1, 0.0, sDelta );
    TS_ASSERT_DELTA( transformation.m2_2, 1.0, sDelta );
    TS_ASSERT_DELTA( transformation.mDelta1, -2.0 / ( 1.0 / sqrt( 2.0 ) ), sDelta );
    TS_ASSERT_DELTA( transformation.mDelta2, -( -3.0 ), sDelta );

    transformation = verticalPlane.CoordinateTransformationData( verticalPlaneMinusDir, tolerance );
    TS_ASSERT_DELTA( transformation.m1_1, -1.0, sDelta );
    TS_ASSERT_DELTA( transformation.m1_2, 0.0, sDelta );
    TS_ASSERT_DELTA( transformation.m2_1, 0.0, sDelta );
    TS_ASSERT_DELTA( transformation.m2_2, 1.0, sDelta );
    TS_ASSERT_DELTA( transformation.mDelta1, 0.0, sDelta );
    TS_ASSERT_DELTA( transformation.mDelta2, 0.0, sDelta );
}

void TestAreaClasses::TestComplicatedPlane()
{
    //#if defined(__EXCEPTIONS__)
    {
        TS_ASSERT_THROWS_EQUALS( Plane<>( Cartesian<>( 0.0, 0.0, 0.0 ), UnitVector<>( 1.0, 2.0, 0.0 ), UnitVector<>( 2.0, 1.0, 0.0 ) ),
                                 const std::runtime_error &e, strcmp( e.what(), "Not yet implemented." ), 0 );
    }
    //#endif

    Plane<> plane( Cartesian<>( 0.0, 0.0, 0.0 ), UnitVector<>( 1.0, 0.0, 0.0 ), UnitVector<>( 0.0, 1.0, 0.0 ) );

    // A =(0.5 0.3), (0.7 0.8), (0.5 0.5); x = 1, 10; b = 3.5, 8.7, 5.5
    Plane<>::Matrix A( 3, 2 );
    A( 0, 0 ) = 0.5 + 0.000001;
    A( 0, 1 ) = 0.3 - 0.000002;
    A( 1, 0 ) = 0.7 + 0.0000005;
    A( 1, 1 ) = 0.8 + 0.000003;
    A( 2, 0 ) = 0.5 - 0.0000015;
    A( 2, 1 ) = 0.5 - 0.000001;
    Plane<>::Vector b( 3 );
    b( 0 ) = 3.5 + 0.000003;
    b( 1 ) = 8.7 + 0.0000015;
    b( 2 ) = 5.5 - 0.0000005;
    double x1 = 0, x2 = 0;
    plane.SolveEquationOverdetermindByOne( A, b, x1, x2 );

    TS_ASSERT_DELTA( x1, 1.0, 0.001 );
    TS_ASSERT_DELTA( x2, 10.0, 0.001 );
}

void TestAreaClasses::TestGeometry2D()
{
    //#if defined(__EXCEPTIONS__)
    {
        vector< TwoDim<> > vertices( 4 );
        vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
        vertices.at( 1 ) = TwoDim<>( 0.0, 4.0 );
        vertices.at( 2 ) = TwoDim<>( 4.0, 0.0 );
        vertices.at( 3 ) = TwoDim<>( 4.0, 4.0 );
#ifdef USING_BOOST_GEOMETRY
        TS_ASSERT_THROWS_EQUALS( Geometry2D<> geometry1( vertices ), const std::runtime_error &e,
                                 strcmp( e.what(), "mPolygon is not not allowed to be self intersecting." ), 0 );
#endif

        vertices.pop_back();
        vertices.pop_back();
        vertices.pop_back();
        TS_ASSERT_THROWS_EQUALS( Geometry2D<> geometry2( vertices ), const std::runtime_error &e,
                                 strcmp( e.what(),
                                         "Vertices must have at least two elements to form an area or a line." ),
                                 0 );
    }
    //#endif


    vector< TwoDim<> > vertices( 4 );

    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 0.0, 4.0 );
    vertices.at( 2 ) = TwoDim<>( 6.0, 4.0 );
    vertices.at( 3 ) = TwoDim<>( 6.0, 0.0 );
    Geometry2D<> big( vertices );

    vertices.at( 0 ) = TwoDim<>( 3.0, 1.0 );
    vertices.at( 1 ) = TwoDim<>( 5.0, 1.0 );
    vertices.at( 2 ) = TwoDim<>( 5.0, 3.0 );
    vertices.at( 3 ) = TwoDim<>( 3.0, 3.0 );
    Geometry2D<> small( vertices );

    vertices.resize( 8 );
    vertices.at( 0 ) = TwoDim<>( 0.0, 2.0 );
    vertices.at( 1 ) = TwoDim<>( 0.0, 6.0 );
    vertices.at( 2 ) = TwoDim<>( 7.0, 6.0 );
    vertices.at( 3 ) = TwoDim<>( 7.0, 2.0 );
    vertices.at( 4 ) = TwoDim<>( 4.0, 2.0 );
    vertices.at( 5 ) = TwoDim<>( 4.0, 5.0 );
    vertices.at( 6 ) = TwoDim<>( 2.0, 5.0 );
    vertices.at( 7 ) = TwoDim<>( 2.0, 2.0 );
    Geometry2D<> wound( vertices );


    TS_ASSERT_DELTA( small.CalculateAreaValue(), 4.0, sDelta );
    TS_ASSERT_DELTA( big.CalculateAreaValue(), 24.0, sDelta );
    TS_ASSERT_DELTA( wound.CalculateAreaValue(), 22.0, sDelta );
#ifdef USING_BOOST_GEOMETRY
    small.PrintToSVGFile( "small.svg" );
    big.PrintToSVGFile( "big.svg" );
    wound.PrintToSVGFile( "wound.svg" );
#endif

    vector< Geometry2D<> > intersection;

    small.Intersection( big, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT_DELTA( intersection.at( 0 ).CalculateAreaValue(), 4.0, sDelta );
#ifdef USING_BOOST_GEOMETRY
    intersection.at( 0 ).PrintToSVGFile( "smallbig.svg" );
#endif

    big.Intersection( small, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT_DELTA( intersection.at( 0 ).CalculateAreaValue(), 4.0, sDelta );

    small.Intersection( wound, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT_DELTA( intersection.at( 0 ).CalculateAreaValue(), 1.0, sDelta );
#ifdef USING_BOOST_GEOMETRY
    intersection.at( 0 ).PrintToSVGFile( "smallwound.svg" );
#endif

    wound.Intersection( small, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT_DELTA( intersection.at( 0 ).CalculateAreaValue(), 1.0, sDelta );

    big.Intersection( wound, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 2 );
    TS_ASSERT_DELTA( intersection.at( 0 ).CalculateAreaValue() + intersection.at( 1 ).CalculateAreaValue(), 8.0, sDelta );
#ifdef USING_BOOST_GEOMETRY
    intersection.at( 0 ).PrintToSVGFile( "woundbig1.svg" );
    intersection.at( 1 ).PrintToSVGFile( "woundbig2.svg" );
#endif

    wound.Intersection( big, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 2 );
    TS_ASSERT_DELTA( intersection.at( 0 ).CalculateAreaValue() + intersection.at( 1 ).CalculateAreaValue(), 8.0, sDelta );


    Tolerance<> tolerance( 0.15, geometry::Angle<>::Deg( 5.0 ), 3.0 );

    vertices.resize( 4 );
    vertices.at( 0 ) = TwoDim<>( 5.9, 1.0 );
    vertices.at( 1 ) = TwoDim<>( 5.9, 3.0 );
    vertices.at( 2 ) = TwoDim<>( 8.0, 3.0 );
    vertices.at( 3 ) = TwoDim<>( 8.0, 1.0 );
    Geometry2D<> adjacent1( vertices );

    vertices.at( 0 ) = TwoDim<>( 6.1, 1.0 );
    vertices.at( 1 ) = TwoDim<>( 6.1, 3.0 );
    vertices.at( 2 ) = TwoDim<>( 8.0, 3.0 );
    vertices.at( 3 ) = TwoDim<>( 8.0, 1.0 );
    Geometry2D<> adjacent2( vertices );

    vertices.at( 0 ) = TwoDim<>( 6.2, 1.0 );
    vertices.at( 1 ) = TwoDim<>( 6.2, 3.0 );
    vertices.at( 2 ) = TwoDim<>( 8.0, 3.0 );
    vertices.at( 3 ) = TwoDim<>( 8.0, 1.0 );
    Geometry2D<> nonAdjacent( vertices );

    vertices.resize( 5 );
    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 4.0, 0.0 );
    vertices.at( 2 ) = TwoDim<>( 3.0, 3.0 );
    vertices.at( 3 ) = TwoDim<>( 2.0, 0.1 );
    vertices.at( 2 ) = TwoDim<>( 1.0, 3.0 );
    Geometry2D<> notNeglactable( vertices );

    TS_ASSERT( big.IsAdjacentTo( adjacent1, tolerance ) );
    TS_ASSERT( big.IsAdjacentTo( adjacent2, tolerance ) );
    TS_ASSERT_EQUALS( big.IsAdjacentTo( nonAdjacent, tolerance ), false );

    big.Intersection( adjacent1, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT( intersection.at( 0 ).IsNeglectable( tolerance ) );

    big.Intersection( adjacent1, intersection, tolerance );
    TS_ASSERT( intersection.empty() );

    TS_ASSERT_EQUALS( notNeglactable.IsNeglectable( tolerance ), false );


    vertices.resize( 4 );
    vertices.at( 0 ) = TwoDim<>( 1.0, 1.0 );
    vertices.at( 1 ) = TwoDim<>( -1.0, 3.0 );
    vertices.at( 2 ) = TwoDim<>( 1.0, 5.0 );
    vertices.at( 3 ) = TwoDim<>( 3.0, 3.0 );
    Geometry2D<> crookedRhombus( vertices );

    TwoDim<> centroid( 1.0, 3.0 );
    TwoDim<> lowerEdge, higherEdge;
    crookedRhombus.Envelope( lowerEdge, higherEdge );
#ifdef USING_BOOST_GEOMETRY
    TS_ASSERT_DELTA( centroid.DistanceTo( crookedRhombus.Centroid() ), 0.0, sDelta );
#endif
    TS_ASSERT_DELTA( lowerEdge.DistanceTo( TwoDim<>( -1.0, 1.0 ) ), 0.0, sDelta );
    TS_ASSERT_DELTA( higherEdge.DistanceTo( TwoDim<>( 3.0, 5.0 ) ), 0.0, sDelta );


    vertices.pop_back();
    vertices.pop_back();
    Geometry2D<> line( vertices );
    TS_ASSERT_EQUALS( line.CalculateAreaValue(), 0.0 );

    line.GetPoints( vertices );
    TS_ASSERT_EQUALS( vertices.size(), 2 );
    TS_ASSERT_DELTA( vertices.at( 0 ).Get1(), 1.0, sDelta );
    TS_ASSERT_DELTA( vertices.at( 0 ).Get2(), 1.0, sDelta );
    TS_ASSERT_DELTA( vertices.at( 1 ).Get1(), -1.0, sDelta );
    TS_ASSERT_DELTA( vertices.at( 1 ).Get2(), 3.0, sDelta );


    // To test new algorithm for IsAdjacentTo() and IsTangentTo()
    vector< TwoDim<> > base( 4 );
    base.at( 0 ) = TwoDim<>( -1.0, 0.0 );
    base.at( 1 ) = TwoDim<>( -1.0, 2.0 );
    base.at( 2 ) = TwoDim<>( 1.0, 2.0 );
    base.at( 3 ) = TwoDim<>( 1.0, 0.0 );
    Geometry2D<> square( base );

    vertices.resize( 7 );
    vertices.at( 0 ) = TwoDim<>( -2.0, 1.0 );
    vertices.at( 1 ) = TwoDim<>( -1.0, 2.0 );
    vertices.at( 2 ) = TwoDim<>( 0.0, 3.0 );
    vertices.at( 3 ) = TwoDim<>( 1.0, 2.0 );
    vertices.at( 4 ) = TwoDim<>( 2.0, 1.0 );
    vertices.at( 5 ) = TwoDim<>( 2.0, 5.0 );
    vertices.at( 6 ) = TwoDim<>( -2.0, 5.0 );
    Geometry2D<> compareWithSquare1( vertices );

    vertices.clear();
    vertices.push_back( TwoDim<>( -3.0, 2.0 ) );
    vertices.push_back( TwoDim<>( -1.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 1.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 3.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 0.0, 3.0 ) );
    Geometry2D<> compareWithSquare2( vertices );

    vertices.clear();
    vertices.push_back( TwoDim<>( -2.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 0.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 2.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 0.0, 3.0 ) );
    Geometry2D<> compareWithSquare3( vertices );

    vertices.clear();
    vertices.push_back( TwoDim<>( 0.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 2.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 1.0, 4.0 ) );
    Geometry2D<> compareWithSquare4( vertices );

    vertices.clear();
    vertices.push_back( TwoDim<>( 1.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 3.0, 2.0 ) );
    vertices.push_back( TwoDim<>( 2.0, 4.0 ) );
    Geometry2D<> compareWithSquare5( vertices );

    TS_ASSERT( square.IsAdjacentTo( compareWithSquare1, tolerance ) == false );
    TS_ASSERT( square.IsAdjacentTo( compareWithSquare2, tolerance ) );
    TS_ASSERT( square.IsAdjacentTo( compareWithSquare3, tolerance ) );
    TS_ASSERT( square.IsAdjacentTo( compareWithSquare4, tolerance ) );
    TS_ASSERT( square.IsAdjacentTo( compareWithSquare5, tolerance ) == false );


    vertices.resize( 3 );
    vertices.at( 0 ) = TwoDim<>( 1.13, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 2.0, 0.0 );
    vertices.at( 2 ) = TwoDim<>( 2.0, 1.0 );
    Geometry2D<> tangentToSquare( vertices );

    vertices.at( 0 ) = TwoDim<>( 1.18, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 2.0, 0.0 );
    vertices.at( 2 ) = TwoDim<>( 2.0, 1.0 );
    Geometry2D<> notTangentToSquare( vertices );

    TS_ASSERT( square.IsTangentTo( tangentToSquare, tolerance ) );
    TS_ASSERT( square.IsTangentTo( notTangentToSquare, tolerance ) == false );


    vertices.resize( 4 );
    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 2.0, 0.0 );
    vertices.at( 2 ) = TwoDim<>( 1.0, 0.1 );
    vertices.at( 3 ) = TwoDim<>( 0.0, 1.0 );
    Geometry2D<> selfAdjacent1( vertices );

    vertices.resize( 6 );
    vertices.at( 0 ) = TwoDim<>( 2.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 2 ) = TwoDim<>( 0.0, 2.0 );
    vertices.at( 3 ) = TwoDim<>( 1.0, 0.1 );
    vertices.at( 4 ) = TwoDim<>( 3.0, 0.05 );
    vertices.at( 5 ) = TwoDim<>( 3.0, -1.0 );
    Geometry2D<> selfAdjacent2( vertices );

    vertices.erase( vertices.begin() + 2 );
    Geometry2D<> selfAdjacent3( vertices );

    TS_ASSERT( selfAdjacent1.IsSelfAdjacent( tolerance ) );
    TS_ASSERT( selfAdjacent2.IsSelfAdjacent( tolerance ) );
    TS_ASSERT( selfAdjacent3.IsSelfAdjacent( tolerance ) );

    TS_ASSERT( big.IsSelfAdjacent( tolerance ) == false );
    TS_ASSERT( small.IsSelfAdjacent( tolerance ) == false );
    TS_ASSERT( wound.IsSelfAdjacent( tolerance ) == false );
    TS_ASSERT( adjacent1.IsSelfAdjacent( tolerance ) == false );
    TS_ASSERT( crookedRhombus.IsSelfAdjacent( tolerance ) == false );
    TS_ASSERT( tangentToSquare.IsSelfAdjacent( tolerance ) == false );
}

void TestAreaClasses::TestGeometry2DWithArcs()
{
    //#if defined(__EXCEPTIONS__)
    {
        vector< TwoDim<> > vertices( 3 );
        map< size_t, TwoDim<> > arcs;
        arcs[1] = TwoDim<>( 0.0, 0.0 );
        vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
        vertices.at( 1 ) = TwoDim<>( 0.0, 2.05 );
        vertices.at( 2 ) = TwoDim<>( 2.05, 0.0 );
        TS_ASSERT_THROWS_EQUALS( Geometry2D<> geometry1( vertices, arcs, 0.0 ), const std::runtime_error &e,
                                 strcmp( e.what(),
                                         "If there are arcs, arcPolygonEdgesLength must be bigger than zero." ),
                                 0 );

        arcs[3] = TwoDim<>( 0.0, 0.0 );
        TS_ASSERT_THROWS_EQUALS(
         Geometry2D<> geometry2( vertices, arcs, 1.0 ), const std::runtime_error &e,
         strcmp( e.what(), "If there are arcs, every key of arcs must be assignable to an element of vertices." ), 0 );
    }
    //#endif


    vector< TwoDim<> > vertices( 3 );
    map< size_t, TwoDim<> > arcs;
    arcs[1] = TwoDim<>( 0.0, 0.0 );


    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 0.0, 2.05 );
    vertices.at( 2 ) = TwoDim<>( 2.05, 0.0 );
    Geometry2D<> innerArea( vertices, arcs, 0.1 );
#ifdef USING_BOOST_GEOMETRY
    innerArea.PrintToSVGFile( "innerRound.svg" );
#endif

    vertices.resize( 4 );
    arcs[3] = TwoDim<>( 0.0, 0.0 );
    vertices.at( 0 ) = TwoDim<>( 2.0 / sqrt( 2.0 ), 2.0 / sqrt( 2.0 ) );
    vertices.at( 1 ) = TwoDim<>( 4.0 / sqrt( 2.0 ), 4.0 / sqrt( 2.0 ) );
    vertices.at( 2 ) = TwoDim<>( 4.0 / sqrt( 2.0 ), -4.0 / sqrt( 2.0 ) );
    vertices.at( 3 ) = TwoDim<>( 2.0 / sqrt( 2.0 ), -2.0 / sqrt( 2.0 ) );
    Geometry2D<> outerArea( vertices, arcs, 0.136 );
#ifdef USING_BOOST_GEOMETRY
    outerArea.PrintToSVGFile( "outerRound.svg" );
#endif

    vertices.at( 0 ) = TwoDim<>( 0.0, 1.9 );
    vertices.at( 1 ) = TwoDim<>( 0.0, 2.1 );
    vertices.at( 2 ) = TwoDim<>( 2.1, 0.0 );
    vertices.at( 3 ) = TwoDim<>( 1.9, 0.0 );
    Geometry2D<> middleArea( vertices, arcs, 0.091 );
#ifdef USING_BOOST_GEOMETRY
    middleArea.PrintToSVGFile( "middleRound.svg" );
#endif


    TS_ASSERT_DELTA( innerArea.CalculateAreaValue(), 2.05 * 2.05 * Angle<>::pi / 4.0, sDelta );
    TS_ASSERT_DELTA( middleArea.CalculateAreaValue(), ( 2.1 * 2.1 - 1.9 * 1.9 ) * Angle<>::pi / 4.0, sDelta );
    TS_ASSERT_DELTA( outerArea.CalculateAreaValue(), ( 4.0 * 4.0 - 2.0 * 2.0 ) * Angle<>::pi / 4.0, sDelta );


    Tolerance<> tolerance( 0.0, Angle<>::Rad( 0 ), 0.1 );
    vector< Geometry2D<> > intersection;
    innerArea.Intersection( outerArea, intersection, tolerance );
    //#ifdef USING_BOOST_GEOMETRY
    //    BOOST_FOREACH(const Geometry2D<> &geometry2D, intersection)
    //        TS_ASSERT(geometry2D.IsArcIntersectionRest(TwoDim<>(0.0, 0.0), tolerance));
    //#endif

    middleArea.Intersection( innerArea, intersection, tolerance );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    //#ifdef USING_BOOST_GEOMETRY
    //    TS_ASSERT_EQUALS(intersection.at(0).IsArcIntersectionRest(TwoDim<>(0.0, 0.0), tolerance), false);
    //#endif

    middleArea.Intersection( outerArea, intersection, tolerance );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    //#ifdef USING_BOOST_GEOMETRY
    //    TS_ASSERT_EQUALS(intersection.at(0).IsArcIntersectionRest(TwoDim<>(0.0, 0.0), tolerance), false);
    //#endif
}

void TestAreaClasses::TestArea()
{
    //#if defined(__EXCEPTIONS__)
    {
        Tolerance<> tolerance( 0.000001, Angle<>::Deg( 0.001 ), 0.1 );
        vector< TwoDim<> > vertices( 4 );
        vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
        vertices.at( 1 ) = TwoDim<>( 1.5, 0.0 );
        vertices.at( 2 ) = TwoDim<>( 1.5, 2.0 );
        vertices.at( 3 ) = TwoDim<>( 0.0, 2.0 );

        TS_ASSERT_THROWS_EQUALS( Area<> area1( vertices, Plane<>( Cartesian<>( 0.0, 0.0, 0.0 ), UnitVector<>( 1.0, 0.0, 0.0 ),
                                                                  UnitVector<>( 0.0, 1.0, 0.0 ) ),
                                               -0.1, 0.2, TOP ),
                                 const std::runtime_error &e,
                                 strcmp( e.what(), "distanceToGridVertex must be zero or bigger than zero." ), 0 );

        TS_ASSERT_THROWS_EQUALS( Area<> area2( vertices, Plane<>( Cartesian<>( 0.0, 0.0, 0.0 ), UnitVector<>( 1.0, 0.0, 0.0 ),
                                                                  UnitVector<>( 0.0, 1.0, 0.0 ) ),
                                               0.1, -0.2, BOTTOM ),
                                 const std::runtime_error &e,
                                 strcmp( e.what(), "Conductivity must be zero or bigger than zero." ), 0 );
    }
    //#endif


    Tolerance<> tolerance( 0.000001, Angle<>::Deg( 0.001 ), 0.1 );

    vector< TwoDim<> > vertices( 4 );
    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 1.5, 0.0 );
    vertices.at( 2 ) = TwoDim<>( 1.5, 2.0 );
    vertices.at( 3 ) = TwoDim<>( 0.0, 2.0 );

    Area<> area( vertices, Plane<>( Cartesian<>( 0.0, 0.0, 0.0 ), UnitVector<>( 1.0, 0.0, 0.0 ), UnitVector<>( 0.0, 1.0, 0.0 ) ),
                 0.1, 0.2, TOP );
    TS_ASSERT_EQUALS( area.GetDistanceToGridVertex(), 0.1 );
    TS_ASSERT_EQUALS( area.GetConductivity(), 0.2 );
    TS_ASSERT_EQUALS( area.GetLocation(), TOP );
    TS_ASSERT_EQUALS( area.GetAreaValue(), 3.0 );

    vector< Cartesian<> > globalVertices;
    area.GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.at( 0 ).GetX(), 0.0 );
    TS_ASSERT_EQUALS( globalVertices.at( 0 ).GetY(), 0.0 );
    TS_ASSERT_EQUALS( globalVertices.at( 1 ).GetX(), 1.5 );
    TS_ASSERT_EQUALS( globalVertices.at( 1 ).GetY(), 0.0 );
    TS_ASSERT_EQUALS( globalVertices.at( 2 ).GetX(), 1.5 );
    TS_ASSERT_EQUALS( globalVertices.at( 2 ).GetY(), 2.0 );
    TS_ASSERT_EQUALS( globalVertices.at( 3 ).GetX(), 0.0 );
    TS_ASSERT_EQUALS( globalVertices.at( 3 ).GetY(), 2.0 );

    Area<> areaBottom( vertices,
                       Plane<>( Cartesian<>( 1.0, 1.0, 0.0 ), UnitVector<>( 1.0, 0.0, 0.0 ), UnitVector<>( 0.0, 1.0, 0.0 ) ),
                       0.1, 0.2, BOTTOM );
    TS_ASSERT_EQUALS( areaBottom.GetLocation(), BOTTOM );


    Area<> areaNoOverlap( vertices,
                          Plane<>( Cartesian<>( -1.0, -1.0, 1.0 ), UnitVector<>( 1.0, 0.0, 0.0 ), UnitVector<>( 0.0, 1.0, 0.0 ) ),
                          0.1, 0.2, TOP );
    TS_ASSERT_DELTA( area.OverlapWith( areaBottom, tolerance ), 0.5, 0.0001 );
    TS_ASSERT_EQUALS( area.OverlapWith( areaNoOverlap, tolerance ), 0.0 );
    TS_ASSERT_EQUALS( areaNoOverlap.GetLocation(), TOP );


    area.TransformToThisCoordinateSystem( areaBottom, vertices, tolerance );
    TS_ASSERT_EQUALS( vertices.at( 0 ).Get1(), 1.0 );
    TS_ASSERT_EQUALS( vertices.at( 0 ).Get2(), 1.0 );
    TS_ASSERT_EQUALS( vertices.at( 1 ).Get1(), 2.5 );
    TS_ASSERT_EQUALS( vertices.at( 1 ).Get2(), 1.0 );
    TS_ASSERT_EQUALS( vertices.at( 2 ).Get1(), 2.5 );
    TS_ASSERT_EQUALS( vertices.at( 2 ).Get2(), 3.0 );
    TS_ASSERT_EQUALS( vertices.at( 3 ).Get1(), 1.0 );
    TS_ASSERT_EQUALS( vertices.at( 3 ).Get2(), 3.0 );
    Area<> areaNotCoplanar( vertices,
                            Plane<>( Cartesian<>( 0.0, 0.0, 0.0 ), UnitVector<>( 1.0, 0.0, 0.0 ), UnitVector<>( 0.0, 0.0, 1.0 ) ),
                            0.1, 0.2, SIDE );
    //#if defined(__EXCEPTIONS__)
    TS_ASSERT_THROWS_EQUALS(
     area.TransformToThisCoordinateSystem( areaNotCoplanar, vertices, tolerance ), const std::runtime_error &e,
     strcmp( e.what(), "No transformation is possible, because this instance and rhsArea are not coplanar." ), 0 );
    TS_ASSERT_THROWS_EQUALS(
     area.TransformToThisCoordinateSystem( areaNoOverlap, vertices, tolerance ), const std::runtime_error &e,
     strcmp( e.what(), "No transformation is possible, because this instance and rhsArea are not coplanar." ), 0 );
    //#endif


    set< Area<> * > areaSet;
    areaSet.insert( &area );
    areaSet.insert( &areaBottom );
    areaSet.insert( &areaNoOverlap );
    areaSet.insert( &areaNotCoplanar );
    TwoDim<> lowestEdge, highestEdge;
    map< Area<> *, TwoDim<> > centroids;

    area.Aggregate( areaSet, lowestEdge, highestEdge, centroids, tolerance );

    TS_ASSERT_DELTA( lowestEdge.DistanceTo( TwoDim<>( 0.0, 0.0 ) ), 0.0, sDelta );
    TS_ASSERT_DELTA( highestEdge.DistanceTo( TwoDim<>( 2.5, 3.0 ) ), 0.0, sDelta );
#ifdef USING_BOOST_GEOMETRY
    TS_ASSERT_EQUALS( centroids.size(), 2 );
    TS_ASSERT_EQUALS( centroids.count( &area ), 1 );
    TS_ASSERT_EQUALS( centroids.count( &areaBottom ), 1 );
    TS_ASSERT_DELTA( centroids[&area].DistanceTo( TwoDim<>( 0.75, 1.0 ) ), 0.0, sDelta );
    TS_ASSERT_DELTA( centroids[&areaBottom].DistanceTo( TwoDim<>( 1.75, 2.0 ) ), 0.0, sDelta );
#endif
}

void TestAreaClasses::TestAreaWithArcs()
{
    TS_ASSERT( true );    // Yet to do
}

void TestAreaClasses::TestInnerAreas()
{
    vector< TwoDim<> > vertices( 4 );
    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 1.5, 0.0 );
    vertices.at( 2 ) = TwoDim<>( 1.5, 2.0 );
    vertices.at( 3 ) = TwoDim<>( 0.0, 2.0 );

    InnerArea<> innerArea( vertices, Plane<>( Cartesian<>( 1.0, 2.0, 3.0 ), UnitVector<>( 1.0, 0.0, 0.0 ),
                                              UnitVector<>( 0.0, 1.0, 0.0 ) ) );

    vector< Cartesian<> > globalVertices;
    innerArea.GetVertices( globalVertices );
    TS_ASSERT_DELTA( globalVertices.at( 0 ).GetX(), 1.0, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 0 ).GetY(), 2.0, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 0 ).GetZ(), 3.0, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 1 ).GetX(), 2.5, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 1 ).GetY(), 2.0, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 1 ).GetZ(), 3.0, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 2 ).GetX(), 2.5, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 2 ).GetY(), 4.0, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 2 ).GetZ(), 3.0, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 3 ).GetX(), 1.0, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 3 ).GetY(), 4.0, sDelta );
    TS_ASSERT_DELTA( globalVertices.at( 3 ).GetZ(), 3.0, sDelta );

    TS_ASSERT_DELTA( innerArea.GetAreaValue(), 3.0, sDelta );
}

void TestAreaClasses::TestBlockGeometry()
{
    //#if defined(__EXCEPTIONS__)
    {
        vector< TwoDim<> > vertices( 4 );
        vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
        vertices.at( 1 ) = TwoDim<>( 0.0, 1.0 );
        vertices.at( 2 ) = TwoDim<>( 1.0, 1.0 );
        vertices.at( 3 ) = TwoDim<>( 1.0, 0.0 );
        TS_ASSERT_THROWS_EQUALS( BlockGeometry<> blockGeometry( Geometry2D<>( vertices ), 1.0, 0.99, "" ),
                                 const std::runtime_error &e,
                                 strcmp( e.what(), "zLower must not be bigger than zUpper." ), 0 );

        TS_ASSERT_THROWS_NOTHING( BlockGeometry<> blockGeometry( Geometry2D<>( vertices ), 1.0, 1.0, "" ) );
    }
    //#endif


    Tolerance<> tolerance( 0.1 );

    vector< TwoDim<> > vertices( 4 );
    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 0.0, 1.0 );
    vertices.at( 2 ) = TwoDim<>( 1.0, 1.0 );
    vertices.at( 3 ) = TwoDim<>( 1.0, 0.0 );
    BlockGeometry<> blockGeometry( Geometry2D<>( vertices ), 0.0, 1.0, "" );
    BlockGeometry<> toHigh( Geometry2D<>( vertices ), 2.0, 3.0, "" );
    BlockGeometry<> heightAdjacent1( Geometry2D<>( vertices ), 1.05, 2.0, "" );
    BlockGeometry<> heightMoreAdjacent1( Geometry2D<>( vertices ), 0.95, 2.0, "" );
    BlockGeometry<> heightCollide1( Geometry2D<>( vertices ), 0.85, 1.0, "" );
    BlockGeometry<> heightAdjacent2( Geometry2D<>( vertices ), -1.0, -0.05, "" );
    BlockGeometry<> heightMoreAdjacent2( Geometry2D<>( vertices ), -1.0, 0.05, "" );
    BlockGeometry<> heightCollide2( Geometry2D<>( vertices ), -1.0, 0.15, "" );

    vertices.at( 0 ) = TwoDim<>( 2.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 2.0, 1.0 );
    vertices.at( 2 ) = TwoDim<>( 3.0, 1.0 );
    vertices.at( 3 ) = TwoDim<>( 3.0, 0.0 );
    BlockGeometry<> away( Geometry2D<>( vertices ), 0.0, 1.0, "" );

    vertices.at( 2 ) = TwoDim<>( 2.0, 1.0 );
    vertices.at( 3 ) = TwoDim<>( 2.0, 0.0 );
    vertices.at( 0 ) = TwoDim<>( 1.1, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 1.15, 15.0 );
    BlockGeometry<> notAdjacent( Geometry2D<>( vertices ), 0.0, 1.0, "" );
    vertices.at( 0 ) = TwoDim<>( 0.95, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 0.95, 1.0 );
    BlockGeometry<> adjacent( Geometry2D<>( vertices ), 0.0, 1.0, "" );
    vertices.at( 0 ) = TwoDim<>( 0.8, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 0.8, 1.0 );
    BlockGeometry<> colliding( Geometry2D<>( vertices ), 0.0, 1.0, "" );
    vertices.resize( 3 );
    vertices.at( 0 ) = TwoDim<>( 0.5, 0.95 );
    vertices.at( 1 ) = TwoDim<>( 0.0, 2.0 );
    vertices.at( 2 ) = TwoDim<>( 1.0, 2.0 );
    BlockGeometry<> tangentSideToSide1( Geometry2D<>( vertices ), -0.5, 1.5, "" );
    vertices.at( 0 ) = TwoDim<>( 0.5, 1.07 );
    BlockGeometry<> tangentSideToSide2( Geometry2D<>( vertices ), 0.5, 0.8, "" );
    vertices.at( 0 ) = TwoDim<>( 0.5, 1.11 );
    BlockGeometry<> notTangentSideToSide( Geometry2D<>( vertices ), 0.5, 0.8, "" );

    TS_ASSERT( blockGeometry.CollidesWith( blockGeometry, tolerance ) );

    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( toHigh, tolerance ), NOT_ADJACENT );
    TS_ASSERT( !blockGeometry.CollidesWith( toHigh, tolerance ) );
    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( heightAdjacent1, tolerance ), ADJACENT_TOP_BOTTOM );
    TS_ASSERT( !blockGeometry.CollidesWith( heightAdjacent1, tolerance ) );
    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( heightMoreAdjacent1, tolerance ), ADJACENT_TOP_BOTTOM );
    TS_ASSERT( !blockGeometry.CollidesWith( heightMoreAdjacent1, tolerance ) );
    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( heightAdjacent2, tolerance ), ADJACENT_BOTTOM_TOP );
    TS_ASSERT( !blockGeometry.CollidesWith( heightAdjacent2, tolerance ) );
    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( heightMoreAdjacent2, tolerance ), ADJACENT_BOTTOM_TOP );
    TS_ASSERT( !blockGeometry.CollidesWith( heightMoreAdjacent2, tolerance ) );
    TS_ASSERT( blockGeometry.CollidesWith( heightCollide1, tolerance ) );
    TS_ASSERT( blockGeometry.CollidesWith( heightCollide2, tolerance ) );

    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( away, tolerance ), NOT_ADJACENT );
    TS_ASSERT( !blockGeometry.CollidesWith( away, tolerance ) );

    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( notAdjacent, tolerance ), NOT_ADJACENT );
    TS_ASSERT( !blockGeometry.CollidesWith( notAdjacent, tolerance ) );
    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( adjacent, tolerance ), ADJACENT_SIDE_SIDE );
    TS_ASSERT( !blockGeometry.CollidesWith( adjacent, tolerance ) );
    TS_ASSERT( blockGeometry.CollidesWith( colliding, tolerance ) );

    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( tangentSideToSide1, tolerance ), NOT_ADJACENT_BUT_TANGENT_SIDE_SIDE );
    TS_ASSERT( !blockGeometry.CollidesWith( tangentSideToSide1, tolerance ) );
    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( tangentSideToSide2, tolerance ), NOT_ADJACENT_BUT_TANGENT_SIDE_SIDE );
    TS_ASSERT( !blockGeometry.CollidesWith( tangentSideToSide2, tolerance ) );
    TS_ASSERT_EQUALS( blockGeometry.IsAdjacentTo( notTangentSideToSide, tolerance ), NOT_ADJACENT );


    double zLower, zUpper;
    vertices.resize( 2 );
    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 1.0, 1.0 );
    BlockGeometry<> testBlockGeometry( Geometry2D<>( vertices ), 1.0, 2.0, "descriptionText" );
    vertices.clear();
    testBlockGeometry.GetInternalData( zLower, zUpper )->GetPoints( vertices );

    TS_ASSERT_DELTA( zLower, 1.0, sDelta );
    TS_ASSERT_DELTA( zUpper, 2.0, sDelta );

    TS_ASSERT_EQUALS( vertices.size(), 2 );
    TS_ASSERT_DELTA( vertices.at( 0 ).Get1(), 0.0, sDelta );
    TS_ASSERT_DELTA( vertices.at( 0 ).Get2(), 0.0, sDelta );
    TS_ASSERT_DELTA( vertices.at( 1 ).Get1(), 1.0, sDelta );
    TS_ASSERT_DELTA( vertices.at( 1 ).Get2(), 1.0, sDelta );

    TS_ASSERT_EQUALS( strcmp( testBlockGeometry.GetDescription(), "descriptionText" ), 0 );


    Tolerance<> tolInsuffDim( Tolerance<>( 0.001, Angle<>::Deg( 0.1 ), 1.0 ) );
    TS_ASSERT( !blockGeometry.HasInsufficientDimension( tolInsuffDim ) );

    vertices.resize( 4 );
    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 0.0, 1.0 );
    vertices.at( 2 ) = TwoDim<>( 1.0, 1.0 );
    vertices.at( 3 ) = TwoDim<>( 1.0, 0.0 );
    BlockGeometry<> twoSmall1( Geometry2D<>( vertices ), 0.0, 0.000001, "" );
    TS_ASSERT( twoSmall1.HasInsufficientDimension( tolInsuffDim ) );

    vertices.at( 1 ) = TwoDim<>( 0.0, 0.000001 );
    vertices.at( 2 ) = TwoDim<>( 1.0, 0.000001 );
    BlockGeometry<> twoSmall2( Geometry2D<>( vertices ), 0.0, 1.0, "" );
    TS_ASSERT( twoSmall2.HasInsufficientDimension( tolInsuffDim ) );
}
