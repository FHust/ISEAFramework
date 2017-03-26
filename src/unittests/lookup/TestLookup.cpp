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
* File Name : TestLookup.cpp
* Creation Date : 21-02-2014
* Last Modified : Di 13 Okt 2015 18:13:20 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestLookup.h"

// STD
#include <vector>
#include "../../lookup/lookup.h"

void TestLookup::testLookup()
{
    std::vector< double > data;
    std::vector< double > measurementPoints;
    data.push_back( 0 );
    data.push_back( -1 );
    data.push_back( -2 );
    data.push_back( -4 );
    data.push_back( -5 );
    measurementPoints.push_back( 0 );
    measurementPoints.push_back( 2 );
// Check for malformed input
#ifdef __EXCEPTIONS__
    {
        TS_ASSERT_THROWS( lookup::Lookup1D< double > look( data, measurementPoints ), std::runtime_error );
    }
#endif
    measurementPoints.push_back( 3 );
    measurementPoints.push_back( 4 );
    measurementPoints.push_back( 5 );
    lookup::Lookup1D< double > look( data, measurementPoints );

    TS_ASSERT_EQUALS( measurementPoints, look.GetLookupType()->GetPoints() );
    TS_ASSERT_EQUALS( data, look.GetLookupType()->GetLookup() );

    TS_ASSERT_EQUALS( look.DoLookup( 2 ), -1 );
    TS_ASSERT_EQUALS( look.DoLookup( 0 ), 0 );
    TS_ASSERT_EQUALS( look.DoLookup( 3 ), -2 );
    TS_ASSERT_EQUALS( look.DoLookup( 4 ), -4 );
    TS_ASSERT_EQUALS( look.DoLookup( 5 ), -5 );
}
void TestLookup::testLookupNonAcendingOrder()
{
    std::vector< double > data;
    std::vector< double > measurementPoints;
    data.push_back( 0 );
    data.push_back( -1 );
    data.push_back( -5 );
    measurementPoints.push_back( 0 );
    measurementPoints.push_back( 2 );
    measurementPoints.push_back( 1 );
#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS( lookup::Lookup1D< double > look( data, measurementPoints ), std::runtime_error );
#endif
}
void TestLookup::testLookupInterpolate()
{
    std::vector< double > data;
    std::vector< double > measurementPoints;
    data.push_back( 0 );
    data.push_back( -1 );
    data.push_back( -2 );
    measurementPoints.push_back( 0 );
    measurementPoints.push_back( 1 );
    measurementPoints.push_back( 2 );
    lookup::Lookup1D< double > look( data, measurementPoints, lookup::LINEAR_INTERPOLATION );
    TS_ASSERT_DELTA( look.DoLookup( 1.2 ), -1.2, 0.01 );
    TS_ASSERT_DELTA( look.DoLookup( 1.122 ), -1.122, 0.01 );
    TS_ASSERT_DELTA( look.DoLookup( 0 ), 0, 0.01 );
    TS_ASSERT_DELTA( look.DoLookup( 2 ), -2, 0.01 );
    TS_ASSERT_DELTA( look.DoLookup( 1 ), -1, 0.01 );
}

void TestLookup::test1DLookupConstantUntilThreshold()
{
    // Input data
    size_t rowSize = 4;
    std::vector< double > measurementPointsRow( rowSize, 0 );
    std::vector< double > lookupData( rowSize, 0 );

    measurementPointsRow.at( 0 ) = -10;
    measurementPointsRow.at( 1 ) = 0;
    measurementPointsRow.at( 2 ) = 10;
    measurementPointsRow.at( 3 ) = 20;

    lookupData.at( 0 ) = -3.1;
    lookupData.at( 1 ) = -2.7;
    lookupData.at( 2 ) = 3.6;
    lookupData.at( 3 ) = 5.9;

    // Create object
    lookup::ConstantUntilThreshold1D< double > myLookup( lookupData, measurementPointsRow );

    TS_ASSERT_DELTA( myLookup.DoLookup( -15 ), -3.1, 0.001 );    // Lower boundary
    TS_ASSERT_DELTA( myLookup.DoLookup( -10 ), -3.1, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -5 ), -3.1, 0.001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( 0 ), -2.7, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 4 ), -2.7, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 6 ), -2.7, 0.001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( 10 ), 3.6, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 18.9 ), 3.6, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 19.0 ), 3.6, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 19.1 ), 3.6, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 19.1 ), 3.6, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 19.9 ), 3.6, 0.001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( 20 ), 5.9, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25 ), 5.9, 0.001 );    // Upper boundary

    TS_ASSERT_DELTA( myLookup.DoLookup( 20 ), 5.9, 0.001 );    // Going back
    TS_ASSERT_DELTA( myLookup.DoLookup( 15 ), 3.6, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10 ), 3.6, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 5 ), -2.7, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 0 ), -2.7, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -5 ), -3.1, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -10 ), -3.1, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -15 ), -3.1, 0.001 );
}

// Spline interpolation

std::vector< double > CreateMeasurementPoints()
{
    std::vector< double > measurementPoints;
    measurementPoints.push_back( -10.5 );
    measurementPoints.push_back( 0.5 );
    measurementPoints.push_back( 10.5 );
    measurementPoints.push_back( 20.5 );
    return measurementPoints;
}

std::vector< double > CreateData()
{
    std::vector< double > data;
    data.push_back( -10.5 );
    data.push_back( 10.5 );
    data.push_back( -20.5 );
    data.push_back( 20.5 );
    return data;
}

// Test coefficients b, c, d
void TestLookup::testSplineInterpolation1D_testAuxiliaryVariables()
{
    std::vector< double > data = CreateData();
    std::vector< double > measurementPoints = CreateMeasurementPoints();

    lookup::SplineInterpolation1D< double > look( data, measurementPoints );

    // alpha
    TS_ASSERT_DELTA( look.mAuxiliaryAlpha.size(), measurementPoints.size() - 1, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryAlpha.at( 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryAlpha.at( 1 ), -15.0273, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryAlpha.at( 2 ), 21.6000, 0.001 );

    // beta
    TS_ASSERT_DELTA( look.mAuxiliaryBeta.size(), measurementPoints.size() - 1, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryBeta.at( 0 ), 11, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryBeta.at( 1 ), 10, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryBeta.at( 2 ), 10, 0.001 );

    // gamma
    TS_ASSERT_DELTA( look.mAuxiliaryGamma.size(), measurementPoints.size(), 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryGamma.at( 0 ), 1.0000, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryGamma.at( 1 ), 42.0000, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryGamma.at( 2 ), 37.6190, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryGamma.at( 3 ), 1, 0.001 );

    // delta
    TS_ASSERT_DELTA( look.mAuxiliaryDelta.size(), measurementPoints.size(), 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryDelta.at( 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryDelta.at( 1 ), 0.2381, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryDelta.at( 2 ), 0.2658, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryDelta.at( 3 ), 0, 0.001 );

    // epsilon
    TS_ASSERT_DELTA( look.mAuxiliaryEpsilon.size(), measurementPoints.size(), 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryEpsilon.at( 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryEpsilon.at( 1 ), -0.3578, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryEpsilon.at( 2 ), 0.6693, 0.001 );
    TS_ASSERT_DELTA( look.mAuxiliaryEpsilon.at( 3 ), 0, 0.001 );
}

// Test coefficients b, c, d
void TestLookup::testSplineInterpolation1D_testCoefficients()
{
    std::vector< double > data = CreateData();
    std::vector< double > measurementPoints = CreateMeasurementPoints();

    lookup::SplineInterpolation1D< double > look( data, measurementPoints );

    // b
    TS_ASSERT_DELTA( look.mCoefficientB.size(), measurementPoints.size() - 1, 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientB.at( 0 ), 3.8053, 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientB.at( 1 ), -1.8833, 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientB.at( 2 ), -0.3619, 0.001 );

    // c
    TS_ASSERT_DELTA( look.mCoefficientC.size(), measurementPoints.size(), 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientC.at( 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientC.at( 1 ), -0.5171, 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientC.at( 2 ), 0.6693, 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientC.at( 3 ), 0, 0.001 );

    // d
    TS_ASSERT_DELTA( look.mCoefficientD.size(), measurementPoints.size() - 1, 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientD.at( 0 ), -0.0157, 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientD.at( 1 ), 0.0395, 0.001 );
    TS_ASSERT_DELTA( look.mCoefficientD.at( 2 ), -0.0223, 0.001 );
}

// Test results
void TestLookup::testSplineInterpolation1D_testResults()
{
    std::vector< double > data = CreateData();
    std::vector< double > measurementPoints = CreateMeasurementPoints();

    lookup::SplineInterpolation1D< double > look( data, measurementPoints );

    TS_ASSERT_DELTA( look.DoLookup( -20.5 ), -10.5, 0.001 );    // Lower boundary
    TS_ASSERT_DELTA( look.DoLookup( -10.5 ), -10.5, 0.001 );
    TS_ASSERT_DELTA( look.DoLookup( -1.5 ), 12.3234, 0.001 );    // Spline 1/3
    TS_ASSERT_DELTA( look.DoLookup( 0.5 ), 10.5, 0.001 );
    TS_ASSERT_DELTA( look.DoLookup( 6.6 ), -11.2546, 0.001 );    // Spline 2/3
    TS_ASSERT_DELTA( look.DoLookup( 10.5 ), -20.5, 0.001 );
    TS_ASSERT_DELTA( look.DoLookup( 17.4 ), 1.5387, 0.001 );    // Spline 3/3
    TS_ASSERT_DELTA( look.DoLookup( 20.5 ), 20.5, 0.001 );
    TS_ASSERT_DELTA( look.DoLookup( 25.5 ), 20.5, 0.001 );    // Upper boundary
}

void TestLookup::test2DLookup()
{
    std::vector< std::vector< double > > data;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.at( 0 ).push_back( 0 );
    data.at( 0 ).push_back( 1 );
    data.at( 0 ).push_back( 2 );

    data.at( 1 ).push_back( 3 );
    data.at( 1 ).push_back( 4 );
    data.at( 1 ).push_back( 5 );

    data.at( 2 ).push_back( 6 );
    data.at( 2 ).push_back( 7 );
    data.at( 2 ).push_back( 8 );

    data.at( 3 ).push_back( 9 );
    data.at( 3 ).push_back( 0 );
    data.at( 3 ).push_back( -1 );

    measurementPointsRow.push_back( 10 );
    measurementPointsRow.push_back( 20 );
    measurementPointsRow.push_back( 30 );

    measurementPointsColumn.push_back( 100 );
    measurementPointsColumn.push_back( 200 );
    measurementPointsColumn.push_back( 300 );
    measurementPointsColumn.push_back( 400 );

    lookup::Lookup2D< double > look( data, measurementPointsRow, measurementPointsColumn );
    look.DoLookup( 30.0, 400.0 );

    TS_ASSERT_EQUALS( measurementPointsRow, look.GetLookupType()->GetPointsRow() );
    TS_ASSERT_EQUALS( measurementPointsColumn, look.GetLookupType()->GetPointsCol() );
    TS_ASSERT_EQUALS( data, look.GetLookupType()->GetLookup() );

    TS_ASSERT_EQUALS( look.DoLookup( 10.0, 100.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 30.0, 400.0 ), -1.0 );
}

void TestLookup::test2DLookupInterpolate()
{
    std::vector< std::vector< double > > data;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.at( 0 ).push_back( 0 );
    data.at( 0 ).push_back( 1 );
    data.at( 0 ).push_back( 2 );

    data.at( 1 ).push_back( 3 );
    data.at( 1 ).push_back( 4 );
    data.at( 1 ).push_back( 5 );

    data.at( 2 ).push_back( 6 );
    data.at( 2 ).push_back( 7 );
    data.at( 2 ).push_back( 8 );

    data.at( 3 ).push_back( 9 );
    data.at( 3 ).push_back( 0 );
    data.at( 3 ).push_back( -1 );

    measurementPointsRow.push_back( 10 );
    measurementPointsRow.push_back( 20 );
    measurementPointsRow.push_back( 30 );

    measurementPointsColumn.push_back( 100 );
    measurementPointsColumn.push_back( 200 );
    measurementPointsColumn.push_back( 300 );
    measurementPointsColumn.push_back( 400 );

    lookup::Lookup2D< double > look( data, measurementPointsRow, measurementPointsColumn, lookup::LINEAR_INTERPOLATION );
    look.DoLookup( 30.0, 400.0 );

    TS_ASSERT_EQUALS( look.DoLookup( 10.0, 100.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 30.0, 400.0 ), -1.0 );
}

void TestLookup::test2DLookupInterpolateCorner1()
{
    std::vector< std::vector< double > > data;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.at( 0 ).push_back( 0 );
    data.at( 0 ).push_back( 0 );

    data.at( 1 ).push_back( 0 );
    data.at( 1 ).push_back( 1 );

    measurementPointsRow.push_back( 0 );
    measurementPointsRow.push_back( 1 );

    measurementPointsColumn.push_back( 0 );
    measurementPointsColumn.push_back( 1 );

    lookup::Lookup2D< double > look( data, measurementPointsRow, measurementPointsColumn, lookup::LINEAR_INTERPOLATION );

    TS_ASSERT_EQUALS( look.DoLookup( 0.0, 0.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 0.0, 1.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 1.0, 0.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 1.0, 1.0 ), 1.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 0.5, 0.5 ), 0.25 );
}
void TestLookup::test2DLookupInterpolateCorner2()
{
    std::vector< std::vector< double > > data;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.at( 0 ).push_back( 0 );
    data.at( 0 ).push_back( 0 );

    data.at( 1 ).push_back( 1 );
    data.at( 1 ).push_back( 0 );

    measurementPointsRow.push_back( 0 );
    measurementPointsRow.push_back( 1 );

    measurementPointsColumn.push_back( 0 );
    measurementPointsColumn.push_back( 1 );

    lookup::Lookup2D< double > look( data, measurementPointsRow, measurementPointsColumn, lookup::LINEAR_INTERPOLATION );

    TS_ASSERT_EQUALS( look.DoLookup( 0.0, 0.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 0.0, 1.0 ), 1.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 1.0, 0.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 1.0, 1.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 0.5, 0.5 ), 0.25 );
}

void TestLookup::test2DLookupInterpolateCorner3()
{
    std::vector< std::vector< double > > data;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.at( 0 ).push_back( 0 );
    data.at( 0 ).push_back( 1 );

    data.at( 1 ).push_back( 0 );
    data.at( 1 ).push_back( 0 );

    measurementPointsRow.push_back( 0 );
    measurementPointsRow.push_back( 1 );

    measurementPointsColumn.push_back( 0 );
    measurementPointsColumn.push_back( 1 );

    lookup::Lookup2D< double > look( data, measurementPointsRow, measurementPointsColumn, lookup::LINEAR_INTERPOLATION );

    TS_ASSERT_EQUALS( look.DoLookup( 0.0, 0.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 0.0, 1.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 1.0, 0.0 ), 1.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 1.0, 1.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 0.5, 0.5 ), 0.25 );
}
void TestLookup::test2DLookupInterpolateCorner4()
{
    std::vector< std::vector< double > > data;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.at( 0 ).push_back( 1 );
    data.at( 0 ).push_back( 0 );

    data.at( 1 ).push_back( 0 );
    data.at( 1 ).push_back( 0 );

    measurementPointsRow.push_back( 0 );
    measurementPointsRow.push_back( 1 );

    measurementPointsColumn.push_back( 0 );
    measurementPointsColumn.push_back( 1 );

    lookup::Lookup2D< double > look( data, measurementPointsRow, measurementPointsColumn, lookup::LINEAR_INTERPOLATION );

    TS_ASSERT_EQUALS( look.DoLookup( 0.0, 0.0 ), 1.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 0.0, 1.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 1.0, 0.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 1.0, 1.0 ), 0.0 );
    TS_ASSERT_EQUALS( look.DoLookup( 0.5, 0.5 ), 0.25 );
}
void TestLookup::test2DLookupInterpolate1d()
{
    std::vector< std::vector< double > > data;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.at( 0 ).push_back( 0 );
    data.at( 0 ).push_back( 1 );

    data.at( 1 ).push_back( 0 );
    data.at( 1 ).push_back( 0 );

    measurementPointsRow.push_back( 0 );
    measurementPointsRow.push_back( 1 );

    measurementPointsColumn.push_back( 0 );
    measurementPointsColumn.push_back( 1 );

    lookup::Lookup2D< double > look( data, measurementPointsRow, measurementPointsColumn, lookup::LINEAR_INTERPOLATION );
    double dpoint = 0.0;
    // for (double dpoint = 0.0; dpoint >1.0; dpoint += 0.1);
    while ( dpoint < 1.0 )
    {
        TS_ASSERT_EQUALS( look.DoLookup( dpoint, 0.0 ), dpoint );
        dpoint += 0.1;
    }
}

void TestLookup::test2DLookupInterpolatePreCalc()
{
    std::vector< std::vector< double > > data;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.at( 0 ).push_back( 1 );
    data.at( 0 ).push_back( 2 );

    data.at( 1 ).push_back( 3 );
    data.at( 1 ).push_back( 4 );

    measurementPointsRow.push_back( 0 );
    measurementPointsRow.push_back( 1 );

    measurementPointsColumn.push_back( 0 );
    measurementPointsColumn.push_back( 1 );

    lookup::Lookup2D< double > look( data, measurementPointsRow, measurementPointsColumn, lookup::LINEAR_INTERPOLATION );

    double array[11] = {1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0};
    for ( unsigned i = 0; i < 10; ++i )
    {
        TS_ASSERT_DELTA( look.DoLookup( 0.0, i * 0.1 ), array[i], 0.001 );
    }


    double array1[11] = {1, 1.1000, 1.2000, 1.3000, 1.4000, 1.5000, 1.6000, 1.7000, 1.8000, 1.9000, 2};
    for ( unsigned i = 0; i < 10; ++i )
    {
        TS_ASSERT_DELTA( look.DoLookup( i * 0.1, 0.0 ), array1[i], 0.001 );
    }
}

void TestLookup::test2DLookupInterpolate3Point()
{
    std::vector< std::vector< double > > data;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.push_back( std::vector< double >() );
    data.at( 0 ).push_back( 1 );
    data.at( 0 ).push_back( 2 );
    data.at( 0 ).push_back( 3 );

    data.at( 1 ).push_back( 4 );
    data.at( 1 ).push_back( 5 );
    data.at( 1 ).push_back( 6 );

    data.at( 2 ).push_back( 7 );
    data.at( 2 ).push_back( 8 );
    data.at( 2 ).push_back( 9 );

    measurementPointsRow.push_back( 0 );
    measurementPointsRow.push_back( 1 );
    measurementPointsRow.push_back( 2 );

    measurementPointsColumn.push_back( 0 );
    measurementPointsColumn.push_back( 1 );
    measurementPointsColumn.push_back( 2 );

    lookup::Lookup2D< double > look( data, measurementPointsRow, measurementPointsColumn, lookup::LINEAR_INTERPOLATION );


    double array[21] = {1,      1.1000, 1.2000, 1.3000, 1.4000, 1.5000, 1.6000, 1.7000, 1.8000, 1.9000, 2,
                        2.1000, 2.2000, 2.3000, 2.4000, 2.5000, 2.6000, 2.7000, 2.8000, 2.9000, 3};
    for ( unsigned i = 0; i < 20; ++i )
    {
        TS_ASSERT_DELTA( look.DoLookup( i * 0.1, 0 ), array[i], 0.001 );
    }
    double array1[21] = {1,      1.3000, 1.6000, 1.9000, 2.2000, 2.5000, 2.8000, 3.1000, 3.4000, 3.7000, 4,
                         4.3000, 4.6000, 4.9000, 5.2000, 5.5000, 5.8000, 6.1000, 6.4000, 6.7000, 7};
    for ( unsigned i = 0; i < 20; ++i )
    {
        TS_ASSERT_DELTA( look.DoLookup( 0.0, i * 0.1 ), array1[i], 0.001 );
    }
    double array2[21] = {1,      1.4000, 1.8000, 2.2000, 2.6000, 3,      3.4000, 3.8000, 4.2000, 4.6000, 5,
                         5.4000, 5.8000, 6.2000, 6.6000, 7,      7.4000, 7.8000, 8.2000, 8.6000, 9};
    for ( unsigned i = 0; i < 20; ++i )
    {
        TS_ASSERT_DELTA( look.DoLookup( i * 0.1, i * 0.1 ), array2[i], 0.001 );
    }
    double array3[21] = {7,      6.8000, 6.6000, 6.4000, 6.2000, 6,      5.8000, 5.6000, 5.4000, 5.2000, 5,
                         4.8000, 4.6000, 4.4000, 4.2000, 4,      3.8000, 3.6000, 3.4000, 3.2000, 3};
    for ( unsigned i = 0; i < 20; ++i )
    {
        TS_ASSERT_DELTA( look.DoLookup( i * 0.1, 2 - i * 0.1 ), array3[i], 0.001 );
    }
    double array4[21] = {3,      3.2000, 3.4000, 3.6000, 3.8000, 4,      4.2000, 4.4000, 4.6000, 4.8000, 5,
                         5.2000, 5.4000, 5.6000, 5.8000, 6,      6.2000, 6.4000, 6.6000, 6.8000, 7};
    for ( unsigned i = 0; i < 20; ++i )
    {
        TS_ASSERT_DELTA( look.DoLookup( 2 - i * 0.1, i * 0.1 ), array4[i], 0.001 );
    }
}

void TestLookup::testLookupTestMinMax()
{
    // Lookup1D Test
    std::vector< double > data;
    std::vector< double > measurementPoints;
    data.push_back( 0 );
    data.push_back( -1 );
    data.push_back( -2 );
    data.push_back( -4 );
    data.push_back( -5 );
    measurementPoints.push_back( 0 );
    measurementPoints.push_back( 2 );
    measurementPoints.push_back( 3 );
    measurementPoints.push_back( 4 );
    measurementPoints.push_back( 5 );
    lookup::Lookup1D< double > look( data, measurementPoints );
    TS_ASSERT_DELTA( look.GetMinValueOfLookup(), -5, 0.00001 );
    TS_ASSERT_DELTA( look.GetMaxValueOfLookup(), 0, 0.00001 );
}
void TestLookup::test2DLookupTestMinMax()
{
    // Lookup2D Test
    std::vector< std::vector< double > > data2D;
    std::vector< double > measurementPointsRow;
    std::vector< double > measurementPointsColumn;
    data2D.push_back( std::vector< double >() );
    data2D.push_back( std::vector< double >() );
    data2D.push_back( std::vector< double >() );
    data2D.push_back( std::vector< double >() );
    data2D.at( 0 ).push_back( 0 );
    data2D.at( 0 ).push_back( 1 );
    data2D.at( 0 ).push_back( 2 );

    data2D.at( 1 ).push_back( 3 );
    data2D.at( 1 ).push_back( 4 );
    data2D.at( 1 ).push_back( 5 );

    data2D.at( 2 ).push_back( 6 );
    data2D.at( 2 ).push_back( 7 );
    data2D.at( 2 ).push_back( 8 );

    data2D.at( 3 ).push_back( 9 );
    data2D.at( 3 ).push_back( 0 );
    data2D.at( 3 ).push_back( -1 );

    measurementPointsRow.push_back( 10 );
    measurementPointsRow.push_back( 20 );
    measurementPointsRow.push_back( 30 );

    measurementPointsColumn.push_back( 100 );
    measurementPointsColumn.push_back( 200 );
    measurementPointsColumn.push_back( 300 );
    measurementPointsColumn.push_back( 400 );

    lookup::Lookup2D< double > look2d( data2D, measurementPointsRow, measurementPointsColumn );
    TS_ASSERT_DELTA( look2d.GetMinValueOfLookup(), -1, 0.00001 );
    TS_ASSERT_DELTA( look2d.GetMaxValueOfLookup(), 9, 0.00001 );
}

// This unittest can be deleted when there is a new implementation for 2d lookups
void TestLookup::test2DLookupCreateExtendedData()
{
    // Input data
    size_t rowSize = 3, columnSize = 2;
    std::vector< double > measurementPointsRow( rowSize, 0 );
    std::vector< double > measurementPointsColumn( columnSize, 0 );
    std::vector< std::vector< double > > lookupData( columnSize, std::vector< double >( rowSize, 0 ) );

    measurementPointsRow.at( 0 ) = -5;
    measurementPointsRow.at( 1 ) = 10;
    measurementPointsRow.at( 2 ) = 25;

    measurementPointsColumn.at( 0 ) = -10;
    measurementPointsColumn.at( 1 ) = 20;

    lookupData.at( 0 ).at( 0 ) = 3.3;
    lookupData.at( 0 ).at( 1 ) = 4.5;
    lookupData.at( 0 ).at( 2 ) = 1.8;

    lookupData.at( 1 ).at( 0 ) = 6.8;
    lookupData.at( 1 ).at( 1 ) = 5.7;
    lookupData.at( 1 ).at( 2 ) = 9.5;

    // Create object
    lookup::Linear2DInterpolation< double > my2dLookup( lookupData, measurementPointsRow, measurementPointsColumn );

    // mExtendedMeasurementPointsRow
    TS_ASSERT_LESS_THAN( my2dLookup.mExtendedMeasurementPointsRow.at( 0 ), -1000000 );    // value < -1000000
    TS_ASSERT_DELTA( my2dLookup.mExtendedMeasurementPointsRow.at( 1 ), -5, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedMeasurementPointsRow.at( 2 ), 10, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedMeasurementPointsRow.at( 3 ), 25, 0.000001 );
    TS_ASSERT_LESS_THAN( 1000000, my2dLookup.mExtendedMeasurementPointsRow.at( 4 ) );    // value > 1000000

    // mExtendedMeasurementPointsColumn
    TS_ASSERT_LESS_THAN( my2dLookup.mExtendedMeasurementPointsColumn.at( 0 ), -10000000 );    // value < -1000000
    TS_ASSERT_DELTA( my2dLookup.mExtendedMeasurementPointsColumn.at( 1 ), -10, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedMeasurementPointsColumn.at( 2 ), 20, 0.000001 );
    TS_ASSERT_LESS_THAN( 1000000, my2dLookup.mExtendedMeasurementPointsColumn.at( 3 ) );    // value > 1000000

    // mExtendedLookupData
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 0 ).at( 0 ), 3.3, 0.000001 );    // 1st row
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 0 ).at( 1 ), 3.3, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 0 ).at( 2 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 0 ).at( 3 ), 1.8, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 0 ).at( 4 ), 1.8, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 1 ).at( 0 ), 3.3, 0.000001 );    // 2nd row
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 1 ).at( 1 ), 3.3, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 1 ).at( 2 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 1 ).at( 3 ), 1.8, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 1 ).at( 4 ), 1.8, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 2 ).at( 0 ), 6.8, 0.000001 );    // 3rd row
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 2 ).at( 1 ), 6.8, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 2 ).at( 2 ), 5.7, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 2 ).at( 3 ), 9.5, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 2 ).at( 3 ), 9.5, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 3 ).at( 0 ), 6.8, 0.000001 );    // 4th row
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 3 ).at( 1 ), 6.8, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 3 ).at( 2 ), 5.7, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 3 ).at( 3 ), 9.5, 0.000001 );
    TS_ASSERT_DELTA( my2dLookup.mExtendedLookupData.at( 3 ).at( 3 ), 9.5, 0.000001 );

    // mRowSlopes
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 0 ).at( 0 ), 0.0, 0.001 );    // 1st row
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 0 ).at( 1 ), 0.08, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 0 ).at( 2 ), -0.18, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 0 ).at( 3 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 1 ).at( 0 ), 0.0, 0.001 );    // 2nd row
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 1 ).at( 1 ), 0.08, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 1 ).at( 2 ), -0.18, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 1 ).at( 3 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 2 ).at( 0 ), 0.0, 0.001 );    // 3rd row
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 2 ).at( 1 ), -0.073, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 2 ).at( 2 ), 0.253, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 2 ).at( 3 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 3 ).at( 0 ), 0.0, 0.001 );    // 4th row
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 3 ).at( 1 ), -0.073, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 3 ).at( 2 ), 0.253, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mRowSlopes.at( 3 ).at( 3 ), 0.0, 0.001 );

    // mColumnSlopes
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 0 ).at( 0 ), 0.0, 0.001 );    // 1st row
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 0 ).at( 1 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 0 ).at( 2 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 0 ).at( 3 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 0 ).at( 4 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 1 ).at( 0 ), 0.116, 0.001 );    // 2nd row
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 1 ).at( 1 ), 0.116, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 1 ).at( 2 ), 0.04, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 1 ).at( 3 ), 0.256, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 1 ).at( 4 ), 0.256, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 2 ).at( 0 ), 0.0, 0.001 );    // 3rd row
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 2 ).at( 1 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 2 ).at( 2 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 2 ).at( 3 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mColumnSlopes.at( 2 ).at( 4 ), 0.0, 0.001 );

    // mDifferenceOfQuotientOfSlopes
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 0 ).at( 0 ), 0.0, 0.001 );    // 1st row
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 0 ).at( 1 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 0 ).at( 2 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 0 ).at( 3 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 1 ).at( 0 ), 0.0, 0.001 );    // 2nd row
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 1 ).at( 1 ), -0.0051, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 1 ).at( 2 ), 0.0144, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 1 ).at( 3 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 2 ).at( 0 ), 0.0, 0.001 );    // 3rd row
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 2 ).at( 1 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 2 ).at( 2 ), 0.0, 0.001 );
    TS_ASSERT_DELTA( my2dLookup.mDifferenceOfQuotientOfSlopes.at( 2 ).at( 3 ), 0.0, 0.001 );
}

void TestLookup::test2DLookupConstantUntilThreshold()
{
    // Input data
    size_t rowSize = 4, columnSize = 3;
    std::vector< double > measurementPointsRow( rowSize, 0 );
    std::vector< double > measurementPointsColumn( columnSize, 0 );
    std::vector< std::vector< double > > lookupData( columnSize, std::vector< double >( rowSize, 0 ) );

    measurementPointsRow.at( 0 ) = -5;
    measurementPointsRow.at( 1 ) = 10;
    measurementPointsRow.at( 2 ) = 25;
    measurementPointsRow.at( 3 ) = 35;

    measurementPointsColumn.at( 0 ) = -10;
    measurementPointsColumn.at( 1 ) = 0;
    measurementPointsColumn.at( 2 ) = 10;

    lookupData.at( 0 ).at( 0 ) = 3.3;
    lookupData.at( 0 ).at( 1 ) = 4.5;
    lookupData.at( 0 ).at( 2 ) = 1.8;
    lookupData.at( 0 ).at( 3 ) = 2.8;

    lookupData.at( 1 ).at( 0 ) = 6.8;
    lookupData.at( 1 ).at( 1 ) = 5.7;
    lookupData.at( 1 ).at( 2 ) = 9.5;
    lookupData.at( 1 ).at( 3 ) = 6.5;

    lookupData.at( 2 ).at( 0 ) = 5.8;
    lookupData.at( 2 ).at( 1 ) = 3.7;
    lookupData.at( 2 ).at( 2 ) = 6.5;
    lookupData.at( 2 ).at( 3 ) = 4.5;

    // Create object
    lookup::ConstantUntilThreshold2D< double > myLookup( lookupData, measurementPointsRow, measurementPointsColumn );

    // Changing rows
    TS_ASSERT_DELTA( myLookup.DoLookup( -7.5, -10 ), 3.3, 0.000001 );    // 1st row
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, -10 ), 3.3, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 2.5, -10 ), 3.3, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, -10 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 17.5, -10 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, -10 ), 1.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 30, -10 ), 1.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, -10 ), 2.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 40, -10 ), 2.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, -10 ), 2.8, 0.000001 );    // Going back
    TS_ASSERT_DELTA( myLookup.DoLookup( 30, -10 ), 1.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, -10 ), 1.8, 0.000001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( -7.5, 0 ), 6.8, 0.000001 );    // 2nd row
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, 0 ), 6.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 2.5, 0 ), 6.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 0 ), 5.7, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 17.5, 0 ), 5.7, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 0 ), 9.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 30, 0 ), 9.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 0 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 40, 0 ), 6.5, 0.000001 );    // Going back
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 0 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 30, 0 ), 9.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 0 ), 9.5, 0.000001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( -7.5, 10 ), 5.8, 0.000001 );    // 3rd row
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, 10 ), 5.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 2.5, 10 ), 5.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 10 ), 3.7, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 17.5, 10 ), 3.7, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 10 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 30, 10 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 10 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 40, 10 ), 4.5, 0.000001 );    // Going back
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 10 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 30, 10 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 10 ), 6.5, 0.000001 );

    // Changing columns
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, -20 ), 3.3, 0.000001 );    // 1st column
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, -10 ), 3.3, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, -5 ), 3.3, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, 0 ), 6.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, 5 ), 6.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, 10 ), 5.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, 20 ), 5.8, 0.000001 );    // Going back
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, 10 ), 5.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, 5 ), 6.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, 0 ), 6.8, 0.000001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( 10, -20 ), 4.5, 0.000001 );    // 2nd column
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, -10 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, -5 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 0 ), 5.7, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 5 ), 5.7, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 10 ), 3.7, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 20 ), 3.7, 0.000001 );    // Going back
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 10 ), 3.7, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 5 ), 5.7, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 0 ), 5.7, 0.000001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( 25, -20 ), 1.8, 0.000001 );    // 3rd column
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, -10 ), 1.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, -5 ), 1.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 0 ), 9.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 5 ), 9.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 10 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 20 ), 6.5, 0.000001 );    // Going back
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 10 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 5 ), 9.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 25, 0 ), 9.5, 0.000001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( 35, -20 ), 2.8, 0.000001 );    // 4th column
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, -10 ), 2.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, -5 ), 2.8, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 0 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 5 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 10 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 20 ), 4.5, 0.000001 );    // Going back
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 10 ), 4.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 5 ), 6.5, 0.000001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 35, 0 ), 6.5, 0.000001 );

    // Check boundaries
    TS_ASSERT_DELTA( myLookup.DoLookup( -10, -20 ), 3.3, 0.000001 );    // lower left
    TS_ASSERT_DELTA( myLookup.DoLookup( -10, 20 ), 5.8, 0.000001 );     // upper left
    TS_ASSERT_DELTA( myLookup.DoLookup( 40, -20 ), 2.8, 0.000001 );     // lower right
    TS_ASSERT_DELTA( myLookup.DoLookup( 40, 20 ), 4.5, 0.000001 );      // upper right
}

std::vector< double > CreateMeasurementPointsRow()
{
    std::vector< double > measurementPointsRow( 4, 0 );
    measurementPointsRow.at( 0 ) = -10;
    measurementPointsRow.at( 1 ) = 0;
    measurementPointsRow.at( 2 ) = 10;
    measurementPointsRow.at( 3 ) = 20;
    return measurementPointsRow;
}
std::vector< double > CreateMeasurementPointsColumn()
{
    std::vector< double > measurementPointsColumn( 3, 0 );
    measurementPointsColumn.at( 0 ) = -10;
    measurementPointsColumn.at( 1 ) = 0;
    measurementPointsColumn.at( 2 ) = 10;
    return measurementPointsColumn;
}
std::vector< std::vector< double > > CreateLookupData2D()
{
    std::vector< std::vector< double > > lookupData( 3, std::vector< double >( 4, 0 ) );
    lookupData.at( 0 ).at( 0 ) = 2.5;
    lookupData.at( 0 ).at( 1 ) = -3.5;
    lookupData.at( 0 ).at( 2 ) = 5;
    lookupData.at( 0 ).at( 3 ) = -7;

    lookupData.at( 1 ).at( 0 ) = 11.5;
    lookupData.at( 1 ).at( 1 ) = -13.5;
    lookupData.at( 1 ).at( 2 ) = 17;
    lookupData.at( 1 ).at( 3 ) = -19;

    lookupData.at( 2 ).at( 0 ) = 23.5;
    lookupData.at( 2 ).at( 1 ) = -27.5;
    lookupData.at( 2 ).at( 2 ) = 31;
    lookupData.at( 2 ).at( 3 ) = -37;

    return lookupData;
}

void TestLookup::test2DCubicInterpolationTestBeta()
{
    // Create Data
    std::vector< std::vector< double > > lookupData = CreateLookupData2D();
    std::vector< double > measurementPointsRow = CreateMeasurementPointsRow();
    std::vector< double > measurementPointsColumn = CreateMeasurementPointsColumn();

    // Create lookup object
    lookup::BicubicInterpolation2D< double > myLookup( lookupData, measurementPointsRow, measurementPointsColumn );

    // Beta
    std::vector< double > myVectorBeta = myLookup.GetKnownVectorBeta( 1, 2 );
    TS_ASSERT_DELTA( myVectorBeta.at( 0 ), 17, 0.001 );
    TS_ASSERT_DELTA( myVectorBeta.at( 1 ), -19, 0.001 );
    TS_ASSERT_DELTA( myVectorBeta.at( 2 ), -37, 0.001 );
    TS_ASSERT_DELTA( myVectorBeta.at( 3 ), 31, 0.001 );
}

void TestLookup::test2DCubicInterpolationTestAlpha()
{
    // Create Data
    std::vector< std::vector< double > > lookupData = CreateLookupData2D();
    std::vector< double > measurementPointsRow = CreateMeasurementPointsRow();
    std::vector< double > measurementPointsColumn = CreateMeasurementPointsColumn();

    // Create lookup object
    lookup::BicubicInterpolation2D< double > myLookup( lookupData, measurementPointsRow, measurementPointsColumn );

    // Beta
    std::vector< double > myVectorBeta = myLookup.GetKnownVectorBeta( 0, 0 );
    std::vector< double > myVectorAlpha = myLookup.GetNeededCoefficientsAlpha( myVectorBeta );

    TS_ASSERT_DELTA( myVectorAlpha.at( 0 ), 2.5, 0.001 );
    TS_ASSERT_DELTA( myVectorAlpha.at( 1 ), 27, 0.001 );
    TS_ASSERT_DELTA( myVectorAlpha.at( 2 ), -18, 0.001 );

    TS_ASSERT_DELTA( myVectorAlpha.at( 3 ), -18, 0.001 );
    TS_ASSERT_DELTA( myVectorAlpha.at( 4 ), -171, 0.001 );
    TS_ASSERT_DELTA( myVectorAlpha.at( 5 ), 114, 0.001 );

    TS_ASSERT_DELTA( myVectorAlpha.at( 6 ), 12, 0.001 );
    TS_ASSERT_DELTA( myVectorAlpha.at( 7 ), 114, 0.001 );
    TS_ASSERT_DELTA( myVectorAlpha.at( 8 ), -76, 0.001 );
}

void TestLookup::test2DCubicInterpolationTestLookup()
{
    // Create Data
    std::vector< std::vector< double > > lookupData = CreateLookupData2D();
    std::vector< double > measurementPointsRow = CreateMeasurementPointsRow();
    std::vector< double > measurementPointsColumn = CreateMeasurementPointsColumn();

    // Create lookup object
    lookup::BicubicInterpolation2D< double > myLookup( lookupData, measurementPointsRow, measurementPointsColumn );

    // Beta
    std::vector< double > myVectorBeta = myLookup.GetKnownVectorBeta( 0, 0 );

    // Alpha
    std::vector< double > myVectorAlpha = myLookup.GetNeededCoefficientsAlpha( myVectorBeta );

    // Points exactly on the measurement points (= points on grid)
    TS_ASSERT_DELTA( myLookup.DoLookup( -10, -20 ), 2.5, 0.001 );    // Lower row boundary
    TS_ASSERT_DELTA( myLookup.DoLookup( 0, -20 ), -3.5, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, -20 ), 5, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 20, -20 ), -7, 0.001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( -10, -10 ), 2.5, 0.001 );    // 1st row
    TS_ASSERT_DELTA( myLookup.DoLookup( 0, -10 ), -3.5, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, -10 ), 5, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 20, -10 ), -7, 0.001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( -10, 0 ), 11.5, 0.001 );    // 2nd row
    TS_ASSERT_DELTA( myLookup.DoLookup( 0, 0 ), -13.5, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 0 ), 17, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 20, 0 ), -19, 0.001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( -10, 10 ), 23.5, 0.001 );    // 3rd row
    TS_ASSERT_DELTA( myLookup.DoLookup( 0, 10 ), -27.5, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 10 ), 31, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 20, 10 ), -37, 0.001 );

    TS_ASSERT_DELTA( myLookup.DoLookup( -10, 20 ), 23.5, 0.001 );    // Upper row boundary
    TS_ASSERT_DELTA( myLookup.DoLookup( 0, 20 ), -27.5, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 10, 20 ), 31, 0.001 );
    TS_ASSERT_DELTA( myLookup.DoLookup( 20, 20 ), -37, 0.001 );

    // Interpolated points within the measurement points
    TS_ASSERT_DELTA( myLookup.DoLookup( -5, -6 ), -0.676, 0.01 );    // row0, column0
    TS_ASSERT_DELTA( myLookup.DoLookup( -4, 3 ), -5.747, 0.01 );     // row0, column1

    TS_ASSERT_DELTA( myLookup.DoLookup( 5, -4 ), 1.398, 0.01 );    // row1, column0
    TS_ASSERT_DELTA( myLookup.DoLookup( 3, 2 ), -7.739, 0.01 );    // row1, column1

    TS_ASSERT_DELTA( myLookup.DoLookup( 16, -7 ), -3.543, 0.01 );    // row2, column0
    TS_ASSERT_DELTA( myLookup.DoLookup( 15, 5 ), -2, 0.01 );         // row2, column1

    // Interpolated points outside the measurement points -> Use linear interpolation
    lookup::Linear2DInterpolation< double > myLinear2DInterpolation( lookupData, measurementPointsRow, measurementPointsColumn );

    TS_ASSERT_DELTA( myLookup.DoLookup( -10, -20 ), myLinear2DInterpolation.DoLookup( -10, -20 ),
                     0.01 );    // Outside column boundaries
    TS_ASSERT_DELTA( myLookup.DoLookup( -20, -20 ), myLinear2DInterpolation.DoLookup( -10, -20 ),
                     0.01 );    // Outside column and row boundaries
    TS_ASSERT_DELTA( myLookup.DoLookup( -20, -10 ), myLinear2DInterpolation.DoLookup( -10, -20 ),
                     0.01 );    // Outside row boundaries
}
