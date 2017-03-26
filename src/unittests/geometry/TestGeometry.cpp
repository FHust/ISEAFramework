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
#include "TestGeometry.h"
//#include "../thermal/exception_tester.h"
#include <sstream>
#include <string>

using namespace geometry;
static const double sDelta = 0.000001;


void TestGeometry::TestAngle()
{
    TS_ASSERT_DELTA( Angle<>::pi, 3.1416, 0.001 );
    TS_ASSERT_DELTA( Angle<>::circle.GetRad(), 6.2832, 0.002 );


    Angle<> angle = Angle<>::Rad( 1.2 );
    TS_ASSERT_EQUALS( angle.GetRad(), 1.2 );
    TS_ASSERT_DELTA( angle.GetDeg(), 1.2 / Angle<>::pi * 180.0, sDelta );

    Angle<> angle2( Angle<>::Deg( 180.0 ) );
    TS_ASSERT_EQUALS( angle2.GetDeg(), 180.0 );
    TS_ASSERT_DELTA( angle2.GetRad(), Angle<>::pi, sDelta );


    TS_ASSERT_DELTA( ( angle + angle2 ).GetRad(), 1.2 + Angle<>::pi, sDelta );
    TS_ASSERT_DELTA( ( angle - angle2 ).GetRad(), 1.2 - Angle<>::pi, sDelta );
    TS_ASSERT_DELTA( ( angle / 3.0 ).GetRad(), 0.4, sDelta );


    Angle<> bigAngle = Angle<>::Rad( 5 * Angle<>::pi );
    TS_ASSERT_EQUALS( angle.AngleTo( angle ).GetRad(), 0.0 );
    TS_ASSERT_DELTA( angle2.AngleTo( angle ).GetRad(), 1.2 - Angle<>::pi + 2 * Angle<>::pi, sDelta );
    TS_ASSERT_DELTA( angle.AngleTo( angle2 ).GetRad(), Angle<>::pi - 1.2, sDelta );
    TS_ASSERT_DELTA( bigAngle.AngleTo( angle ).GetRad(), 1.2 - Angle<>::pi + 2 * Angle<>::pi, sDelta );
}

void TestGeometry::TestCartesian()
{
    Cartesian<> zero;
    TS_ASSERT_EQUALS( zero.GetX(), 0.0 );
    TS_ASSERT_EQUALS( zero.GetY(), 0.0 );
    TS_ASSERT_EQUALS( zero.GetZ(), 0.0 );

    Cartesian<> cartesian( 1.2, 2.0, 3.0 );
    Cartesian<> cart2( 9.0, 5.0, 6.0 );

    TS_ASSERT_EQUALS( cartesian.GetX(), 1.2 );
    TS_ASSERT_EQUALS( cartesian.GetY(), 2.0 );
    TS_ASSERT_EQUALS( cartesian.GetZ(), 3.0 );

    // TS_ASSERT_EQUALS(cartesian.Distance(cart2), sqrt(7.8 * 7.8 + 9.0 + 9.0));

    TS_ASSERT_DELTA( ( cartesian + cart2 ).GetX(), 10.2, sDelta );
    TS_ASSERT_DELTA( ( cartesian + cart2 ).GetY(), 7.0, sDelta );
    TS_ASSERT_DELTA( ( cartesian + cart2 ).GetZ(), 9.0, sDelta );

    TS_ASSERT_DELTA( ( cartesian - cart2 ).GetX(), -7.8, sDelta );
    TS_ASSERT_DELTA( ( cartesian - cart2 ).GetY(), -3.0, sDelta );
    TS_ASSERT_DELTA( ( cartesian - cart2 ).GetZ(), -3.0, sDelta );

    TS_ASSERT_DELTA( ( cartesian * 2.0 ).GetX(), 2.4, sDelta );
    TS_ASSERT_DELTA( ( cartesian * 2.0 ).GetY(), 4.0, sDelta );
    TS_ASSERT_DELTA( ( cartesian * 2.0 ).GetZ(), 6.0, sDelta );

    TS_ASSERT_DELTA( ( cart2 / 2.0 ).GetX(), 4.5, sDelta );
    TS_ASSERT_DELTA( ( cart2 / 2.0 ).GetY(), 2.5, sDelta );
    TS_ASSERT_DELTA( ( cart2 / 2.0 ).GetZ(), 3.0, sDelta );

    Cartesian<> crossProd( cartesian.CrossProduct( cart2 ) );
    TS_ASSERT_DELTA( crossProd.GetX(), 2.0 * 6.0 - 3.0 * 5.0, sDelta );
    TS_ASSERT_DELTA( crossProd.GetY(), 3.0 * 9.0 - 1.2 * 6.0, sDelta );
    TS_ASSERT_DELTA( crossProd.GetZ(), 1.2 * 5.0 - 2.0 * 9.0, sDelta );

    TS_ASSERT_DELTA( cartesian.DotProduct( cart2 ), 10.8 + 10.0 + 18.0, sDelta );

    cart2 = cartesian;
    TS_ASSERT_EQUALS( cart2.GetX(), 1.2 );
    TS_ASSERT_EQUALS( cart2.GetY(), 2.0 );
    TS_ASSERT_EQUALS( cart2.GetZ(), 3.0 );

    TS_ASSERT_DELTA( cartesian.Normed().GetX(), 1.2 / sqrt( 14.44 ), sDelta );
    TS_ASSERT_DELTA( cartesian.Normed().GetY(), 2.0 / sqrt( 14.44 ), sDelta );
    TS_ASSERT_DELTA( cartesian.Normed().GetZ(), 3.0 / sqrt( 14.44 ), sDelta );

    Cartesian<> compare( 1.1, 2.1, 2.8 );    // sqrt(0.01^2+0.1^2+0.2^2) ~=0.245
    TS_ASSERT_EQUALS( cartesian.Equals( compare, 0.3 ), true );
    TS_ASSERT_EQUALS( cartesian.Equals( compare, 0.2 ), false );


#ifndef __NO_STRING__
    std::stringstream testStream;
    testStream << Cartesian<>( 1.0, -2.3, 9.1 );
    std::string testString = testStream.str();
    const char *charPointer = testString.c_str();
    TS_ASSERT_DELTA( atof( charPointer ), 1.0, sDelta );

    charPointer = strchr( charPointer, ',' );
    ++charPointer;
    TS_ASSERT_DELTA( atof( charPointer ), -2.3, sDelta );

    charPointer = strchr( charPointer, ',' );
    ++charPointer;
    TS_ASSERT_DELTA( atof( charPointer ), 9.1, sDelta );
#endif
}

void TestGeometry::TestUnitVector()
{
    //#if defined(__EXCEPTIONS__)
    {
        TS_ASSERT_THROWS_EQUALS( UnitVector<> unitVector( 0.0, 0.0, 0.0 ), const std::runtime_error &e,
                                 strcmp( e.what(), "(X,Y,Z) == (0,0,0), i.e. unit vector has no direction." ), 0 );
    }
    //#endif


    UnitVector<> unitVector( 1.2, 2.0, 3.0 );
    TS_ASSERT_DELTA( unitVector.GetX(), 1.2 / sqrt( 14.44 ), sDelta );
    TS_ASSERT_DELTA( unitVector.GetY(), 2.0 / sqrt( 14.44 ), sDelta );
    TS_ASSERT_DELTA( unitVector.GetZ(), 3.0 / sqrt( 14.44 ), sDelta );

    UnitVector<> cartesianConstructor( Cartesian<>( 1.2, 2.0, 3.0 ) );
    TS_ASSERT_DELTA( cartesianConstructor.GetX(), 1.2 / sqrt( 14.44 ), sDelta );
    TS_ASSERT_DELTA( cartesianConstructor.GetY(), 2.0 / sqrt( 14.44 ), sDelta );
    TS_ASSERT_DELTA( cartesianConstructor.GetZ(), 3.0 / sqrt( 14.44 ), sDelta );

    UnitVector<> minus( -unitVector );
    TS_ASSERT_DELTA( minus.GetX(), -1.2 / sqrt( 14.44 ), sDelta );
    TS_ASSERT_DELTA( minus.GetY(), -2.0 / sqrt( 14.44 ), sDelta );
    TS_ASSERT_DELTA( minus.GetZ(), -3.0 / sqrt( 14.44 ), sDelta );

    UnitVector<> compare( 1.1, 2.1, 2.8 );    // dAng ~= 3.53 degree
    TS_ASSERT_EQUALS( unitVector.IsEqualDirection( compare, Angle<>::Deg( 4.0 ) ), true );
    TS_ASSERT_EQUALS( unitVector.IsEqualDirection( compare, Angle<>::Deg( 3.0 ) ), false );

    // dAng ~= 3.53 degree
    TS_ASSERT_EQUALS( unitVector.IsOppositeDirection( -compare, Angle<>::Deg( 4.0 ) ), true );
    TS_ASSERT_EQUALS( unitVector.IsOppositeDirection( -compare, Angle<>::Deg( 3.0 ) ), false );
}

void TestGeometry::TestTwoDim()
{
    TwoDim<> twoDimZero;
    TwoDim<> twoDim( 1.1, 2.0 );
    TwoDim<> twoDim2( 0.6, 0.5 );
    TwoDim<> twoDimAngle( 2.3, Angle<>::Rad( 0.4 ) );

    TS_ASSERT_EQUALS( twoDimZero.Get1(), 0.0 );
    TS_ASSERT_EQUALS( twoDimZero.Get2(), 0.0 );
    TS_ASSERT_EQUALS( twoDim.Get1(), 1.1 );
    TS_ASSERT_EQUALS( twoDim.Get2(), 2.0 );
    TS_ASSERT_EQUALS( twoDimAngle.Get1(), 2.3 * cos( 0.4 ) );
    TS_ASSERT_EQUALS( twoDimAngle.Get2(), 2.3 * sin( 0.4 ) );

    TS_ASSERT_DELTA( ( twoDim + twoDim2 ).Get1(), 1.7, sDelta );
    TS_ASSERT_DELTA( ( twoDim + twoDim2 ).Get2(), 2.5, sDelta );
    TS_ASSERT_DELTA( ( twoDim - twoDim2 ).Get1(), 0.5, sDelta );
    TS_ASSERT_DELTA( ( twoDim - twoDim2 ).Get2(), 1.5, sDelta );
    TS_ASSERT_DELTA( ( twoDim / 2.0 ).Get1(), 0.55, sDelta );
    TS_ASSERT_DELTA( ( twoDim / 4.0 ).Get2(), 0.5, sDelta );
    TS_ASSERT_DELTA( ( twoDim * 2.0 ).Get1(), 2.2, sDelta );
    TS_ASSERT_DELTA( ( twoDim * 1.6 ).Get2(), 3.2, sDelta );

    twoDimZero = twoDim;
    TS_ASSERT_EQUALS( twoDimZero.Get1(), 1.1 );
    TS_ASSERT_EQUALS( twoDimZero.Get2(), 2.0 );

    TS_ASSERT_DELTA( twoDim.DistanceTo( twoDim2 ), sqrt( 0.5 * 0.5 + 1.5 * 1.5 ), sDelta );
    TS_ASSERT_DELTA( twoDim.Length(), sqrt( 1.1 * 1.1 + 2.0 * 2.0 ), sDelta );

    Angle<> angle = twoDim.AngleInXY();
    TS_ASSERT_DELTA( atan2( 2.0, 1.1 ), angle.GetRad(), sDelta );
    angle = twoDim2.AngleInXY();
    TS_ASSERT_DELTA( atan2( 0.5, 0.6 ), angle.GetRad(), sDelta );
}

void TestGeometry::TestCylindric()
{
    geometry::Cylindric<> cylindric( 1.2, Angle<>::Rad( 2.0 ), 3.0 );
    geometry::Cylindric<> cyl2( 11.0, Angle<>::Rad( 0.5 ), -3.1 );

    TS_ASSERT_EQUALS( cylindric.GetRho(), 1.2 );
    TS_ASSERT_DELTA( cylindric.GetPhi().GetRad(), 2.0, sDelta );
    TS_ASSERT_EQUALS( cylindric.GetZ(), 3.0 );

    TS_ASSERT_DELTA( ( cylindric / 2.0 ).GetRho(), 0.6, sDelta );
    TS_ASSERT_DELTA( ( cylindric / 4.0 ).GetPhi().GetRad(), 0.5, sDelta );
    TS_ASSERT_DELTA( ( cylindric / 2.0 ).GetZ(), 1.5, sDelta );

    TS_ASSERT_DELTA( ( cylindric + cyl2 ).GetRho(), 12.2, sDelta );
    TS_ASSERT_DELTA( ( cylindric + cyl2 ).GetPhi().GetRad(), 2.5, sDelta );
    TS_ASSERT_DELTA( ( cylindric + cyl2 ).GetZ(), -0.1, sDelta );


    Cartesian<> converted = CylToCart( Cartesian<>( 1.1, 1.3, -0.6 ), cylindric );
    TS_ASSERT_DELTA( converted.GetX(), 1.1 + 1.2 * cos( Angle<>::Rad( 2.0 ).GetRad() ), sDelta );
    TS_ASSERT_DELTA( converted.GetY(), 1.3 + 1.2 * sin( Angle<>::Rad( 2.0 ).GetRad() ), sDelta );
    TS_ASSERT_DELTA( converted.GetZ(), 3.0 - 0.6, sDelta );
}

void TestGeometry::TestTolerance()
{
    //#if defined(__EXCEPTIONS__)
    {
        TS_ASSERT_THROWS_EQUALS( Tolerance<> tolerance( -0.1, Angle<>::Rad( 0.2 ), 0.3 ), const std::runtime_error &e,
                                 strcmp( e.what(), "Length must be zero or bigger than zero." ), 0 );
        TS_ASSERT_THROWS_EQUALS( Tolerance<> tolerance( 0.1, Angle<>::Rad( -0.2 ), 0.3 ), const std::runtime_error &e,
                                 strcmp( e.what(), "Angle must be zero or bigger than zero." ), 0 );
        TS_ASSERT_THROWS_EQUALS( Tolerance<> tolerance( 0.1, Angle<>::Rad( 0.2 ), -0.3 ), const std::runtime_error &e,
                                 strcmp( e.what(), "percentOfQuantity must be zero or bigger than zero." ), 0 );
    }
    //#endif


    Tolerance<> toleranceDefault;
    TS_ASSERT_EQUALS( toleranceDefault.mLength, 0.0 );
    TS_ASSERT_EQUALS( toleranceDefault.mAngle.GetRad(), 0.0 );
    TS_ASSERT_EQUALS( toleranceDefault.mPercentOfQuantity, 0.0 );

    Tolerance<> tolerance( 0.1, Angle<>::Rad( 0.2 ), 3.0 );
    TS_ASSERT_EQUALS( tolerance.mLength, 0.1 );
    TS_ASSERT_DELTA( tolerance.mAngle.GetRad(), 0.2, sDelta );
    TS_ASSERT_EQUALS( tolerance.mPercentOfQuantity, 3.0 );
}
