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
#include "TestCoolingFunctions.h"
#include "exception_tester.h"

using namespace thermal;
static const double sDelta = 0.000001;

bool IsEqualByTolerance( const TwoDim<> &lhs, const TwoDim<> &rhs )
{
    return fabs( lhs.Get1() - rhs.Get1() ) < sDelta && fabs( lhs.Get2() - rhs.Get2() ) < sDelta;
}


void TestCoolingFunctions::TestConvectionByFormula()
{
    //#if defined(__EXCEPTIONS__)
    {
        TS_ASSERT_THROWS_EQUALS( ConvectionByFormula<> top( -0.1 ), std::runtime_error & e,
                                 strcmp( e.what(), "preFactor must be bigger than zero." ), 0 );
    }
    //#endif


    ConvectionByFormula<> top( 0.71 );
    ConvectionByFormula<> side( 0.548 );
    Linearization< double > offsetSlope( 0.0, 0.0 );

    offsetSlope = top.GetOffsetSlope( 23.0, 0.1, 25.0 );
    TS_ASSERT_EQUALS( offsetSlope.mOffset, 0.0 );
    TS_ASSERT_EQUALS( offsetSlope.mSlope, 0.0 );


    double airTemperature = 25.0;
    double surfaceTemperature = 27.0;
    double characteristicLength = 0.1;

    double pr_Air = 0.73135;
    double lambda_Air = 0.0262;
    double ny_Air = 0.0000175;
    double g = 9.81;

    double mHelp_SameForAll = pow( pr_Air * g / ( ny_Air * ny_Air ), 0.25 ) * lambda_Air;
    double help_SameForOffsetSlope = mHelp_SameForAll * pow( ( airTemperature + 273.15 ) * characteristicLength, -0.25 );
    double deltaTemperature = surfaceTemperature - airTemperature;

    double offset = help_SameForOffsetSlope * pow( deltaTemperature, 1.25 );
    double slope = help_SameForOffsetSlope * 1.25 * pow( deltaTemperature, 0.25 );


    offsetSlope = top.GetOffsetSlope( 27.0, 0.1, 25.0 );
    TS_ASSERT_DELTA( offsetSlope.mOffset, 0.71 * offset, sDelta );
    TS_ASSERT_DELTA( offsetSlope.mSlope, 0.71 * slope, sDelta );

    offsetSlope = side.GetOffsetSlope( 27.0, 0.1, 25.0 );
    TS_ASSERT_DELTA( offsetSlope.mOffset, 0.548 * offset, sDelta );
    TS_ASSERT_DELTA( offsetSlope.mSlope, 0.548 * slope, sDelta );
}

void TestCoolingFunctions::TestConvectionByLookUp()
{
    vector< double > measRow;
    vector< double > measCol;
    vector< vector< double > > lookUpData;
    vector< double > lookUpHelp;


    measRow.push_back( 0.5 );
    measRow.push_back( 1.5 );
    measCol.push_back( 1.0 );
    measCol.push_back( 2.0 );
    lookUpHelp.push_back( 1000.0 );
    lookUpHelp.push_back( 2000.0 );
    lookUpData.push_back( lookUpHelp );
    lookUpHelp.clear();
    lookUpHelp.push_back( 3000.0 );
    lookUpHelp.push_back( 4000.0 );
    lookUpData.push_back( lookUpHelp );
    shared_ptr< object::Object< double > > offset( new object::LookupObj2D< double >( lookUpData, measRow, measCol ) );


    measRow.clear();
    measCol.clear();
    lookUpHelp.clear();
    lookUpData.clear();
    measRow.push_back( 0.5 );
    measRow.push_back( 1.5 );
    measCol.push_back( 1.0 );
    measCol.push_back( 2.0 );
    lookUpHelp.push_back( 100.0 );
    lookUpHelp.push_back( 200.0 );
    lookUpData.push_back( lookUpHelp );
    lookUpHelp.clear();
    lookUpHelp.push_back( 300.0 );
    lookUpHelp.push_back( 400.0 );
    lookUpData.push_back( lookUpHelp );
    shared_ptr< object::Object< double > > slope( new object::LookupObj2D< double >( lookUpData, measRow, measCol ) );

    ConvectionByLookUp<> convection( offset, slope );


    TS_ASSERT_EQUALS( convection.GetOffsetSlope( 0.5, 1.0, 25.0 ).mOffset, 1000.0 );
    TS_ASSERT_EQUALS( convection.GetOffsetSlope( 0.5, 1.0, 25.0 ).mSlope, 100.0 );
    TS_ASSERT_EQUALS( convection.GetOffsetSlope( 1.5, 1.0, 25.0 ).mOffset, 2000.0 );
    TS_ASSERT_EQUALS( convection.GetOffsetSlope( 1.5, 1.0, 25.0 ).mSlope, 200.0 );

    TS_ASSERT_EQUALS( convection.GetOffsetSlope( 0.5, 2.0, 25.0 ).mOffset, 3000.0 );
    TS_ASSERT_EQUALS( convection.GetOffsetSlope( 0.5, 2.0, 25.0 ).mSlope, 300.0 );
    TS_ASSERT_EQUALS( convection.GetOffsetSlope( 1.5, 2.0, 25.0 ).mOffset, 4000.0 );
    TS_ASSERT_EQUALS( convection.GetOffsetSlope( 1.5, 2.0, 25.0 ).mSlope, 400.0 );
}

void TestCoolingFunctions::TestDirichletBoundaryCondition()
{
    DirichletBoundaryCondition<> cooling( 7.0 );
    TS_ASSERT( cooling.IsDirichletBoundaryCondition() );
    Linearization< double > offsetSlope = cooling.GetOffsetSlope( 23.0, 25.0 );
    TS_ASSERT_EQUALS( offsetSlope.mOffset, 7.0 );
    TS_ASSERT_EQUALS( offsetSlope.mSlope, 7.0 );
}

void TestCoolingFunctions::TestRadiation()
{
    Radiation<> radiation;
    TS_ASSERT( !radiation.IsDirichletBoundaryCondition() );

    Linearization< double > offsetSlope = radiation.GetOffsetSlope( 27.0, 25.0 );
    const double t25 = 25.0 + 273.15;
    const double t27 = 27.0 + 273.15;
    TS_ASSERT_DELTA( offsetSlope.mOffset, 0.0000000567 * 1.0 * ( t27 * t27 * t27 * t27 - t25 * t25 * t25 * t25 ), sDelta );
    TS_ASSERT_DELTA( offsetSlope.mSlope, 0.0000000567 * 1.0 * 4.0 * t27 * t27 * t27, sDelta );

    offsetSlope = radiation.GetOffsetSlope( 23.0, 25.0 );
    TS_ASSERT_EQUALS( offsetSlope.mOffset, 0.0 );
    TS_ASSERT_EQUALS( offsetSlope.mSlope, 0.0 );
}

void TestCoolingFunctions::TestCoolingByLookUp()
{
    vector< double > lookUpData( 2 );
    vector< double > points( 2 );
    points.at( 0 ) = 25.0;
    points.at( 1 ) = 27.0;

    lookUpData.at( 0 ) = 1000.0;
    lookUpData.at( 1 ) = 2000.0;
    boost::shared_ptr< object::Object< double > > offset( new object::LookupObj1D< double >( lookUpData, points ) );

    lookUpData.at( 0 ) = 3000.0;
    lookUpData.at( 1 ) = 4000.0;
    boost::shared_ptr< object::Object< double > > slope( new object::LookupObj1D< double >( lookUpData, points ) );

    CoolingByLookUp<> cooling( offset, slope );


    Linearization< double > offsetSlope = cooling.GetOffsetSlope( 25.0, 26.0 );
    TS_ASSERT_DELTA( offsetSlope.mOffset, 1000.0, sDelta );
    TS_ASSERT_DELTA( offsetSlope.mSlope, 3000.0, sDelta );

    offsetSlope = cooling.GetOffsetSlope( 25.0, 27.0 );
    TS_ASSERT_DELTA( offsetSlope.mOffset, 1000.0, sDelta );
    TS_ASSERT_DELTA( offsetSlope.mSlope, 3000.0, sDelta );

    offsetSlope = cooling.GetOffsetSlope( 27.0, 27.0 );
    TS_ASSERT_DELTA( offsetSlope.mOffset, 2000.0, sDelta );
    TS_ASSERT_DELTA( offsetSlope.mSlope, 4000.0, sDelta );

    TS_ASSERT_EQUALS( cooling.GetOffsetLookUp().get(), offset.get() );
    TS_ASSERT_EQUALS( cooling.GetSlopeLookUp().get(), slope.get() );
}

void TestCoolingFunctions::TestCoolingByConstantValue()
{
    CoolingByConstantValue<> cooling( 25.6 );

    Linearization< double > offsetSlope = cooling.GetOffsetSlope( 25.0, 26.0 );
    TS_ASSERT_EQUALS( offsetSlope.mOffset, 25.6 );
    TS_ASSERT_EQUALS( offsetSlope.mSlope, 0.0 );

    TS_ASSERT_EQUALS( cooling.GetValue(), 25.6 );
}

void TestCoolingFunctions::TestCoolingHorizontalPlane()
{
    //#if defined(__EXCEPTIONS__)
    {
        shared_ptr< Cooling<> > cooling( new CoolingByConstantValue<>( 25.6 ) );
        vector< TwoDim<> > vertices( 2 );
        vertices.at( 0 ) = TwoDim<>( -1.0, 1.0 );
        vertices.at( 1 ) = TwoDim<>( 2.0, 5.0 );
        TS_ASSERT_THROWS_EQUALS( CoolingHorizontalPlane<> coolingBlock( "", vertices, 1.0, cooling ), std::runtime_error & e,
                                 strcmp( e.what(), "Vertices must have more than two elements to form an area." ), 0 );
    }
    //#endif

    shared_ptr< Cooling<> > cooling( new CoolingByConstantValue<>( 25.6 ) );

    vector< TwoDim<> > vertices( 3 );
    vertices.at( 0 ) = TwoDim<>( -1.0, 1.0 );
    vertices.at( 1 ) = TwoDim<>( 2.0, 5.0 );
    vertices.at( 2 ) = TwoDim<>( 3.0, 3.0 );

    CoolingHorizontalPlane<> coolingBlock( "descriptionCooling", vertices, 1.0, cooling );

    vector< Area<> > coolingAreas;
    shared_ptr< BlockGeometry<> > blockGeometry;
    coolingBlock.GetCoolingAreas( coolingAreas );
    coolingBlock.GetBlockGeometry( blockGeometry );


    TS_ASSERT_EQUALS( coolingBlock.GetCooling().get(), cooling.get() );
    TS_ASSERT_EQUALS( strcmp( blockGeometry->GetDescription(), "descriptionCooling" ), 0 );


    double zLower, zUpper;
    vector< TwoDim<> > returnedVertices;
    blockGeometry->GetInternalData( zLower, zUpper )->GetPoints( returnedVertices );

    TS_ASSERT_DELTA( zLower, 1.0, sDelta );
    TS_ASSERT_DELTA( zUpper, 1.0, sDelta );

    TS_ASSERT_EQUALS( returnedVertices.size(), 3 );
    TS_ASSERT( boost::algorithm::is_permutation( vertices.begin(), vertices.end(), returnedVertices.begin(), IsEqualByTolerance ) );


    vector< Cartesian<> > globalVertices;
    TS_ASSERT_EQUALS( coolingAreas.size(), 2 );

    TS_ASSERT_EQUALS( coolingAreas.front().GetLocation(), BOTTOM );
    coolingAreas.front().GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.size(), 3 );
    TS_ASSERT( globalVertices.at( 0 ).Equals( Cartesian<>( -1.0, 1.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 1 ).Equals( Cartesian<>( 2.0, 5.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 2 ).Equals( Cartesian<>( 3.0, 3.0, 1.0 ), sDelta ) );

    TS_ASSERT_EQUALS( coolingAreas.back().GetLocation(), TOP );
    coolingAreas.back().GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.size(), 3 );
    TS_ASSERT( globalVertices.at( 0 ).Equals( Cartesian<>( -1.0, 1.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 1 ).Equals( Cartesian<>( 2.0, 5.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 2 ).Equals( Cartesian<>( 3.0, 3.0, 1.0 ), sDelta ) );
}

void TestCoolingFunctions::TestCoolingVerticalPlane()
{
    //#if defined(__EXCEPTIONS__)
    {
        shared_ptr< Cooling<> > cooling( new CoolingByConstantValue<>( 25.6 ) );
        vector< TwoDim<> > vertices( 3 );
        vertices.at( 0 ) = TwoDim<>( -1.0, 1.0 );
        vertices.at( 1 ) = TwoDim<>( 2.0, 5.0 );
        vertices.at( 2 ) = TwoDim<>( 3.0, 3.0 );
        TS_ASSERT_THROWS_EQUALS( CoolingVerticalPlane<> coolingBlock( "", vertices, 1.0, 1.0, cooling ), std::runtime_error & e,
                                 strcmp( e.what(), "UpperZCoordinate must be bigger than LowerZCoordinate." ), 0 );
    }
    //#endif

    shared_ptr< Cooling<> > cooling( new CoolingByConstantValue<>( 25.6 ) );

    vector< TwoDim<> > vertices( 3 );
    vertices.at( 0 ) = TwoDim<>( -1.0, 1.0 );
    vertices.at( 1 ) = TwoDim<>( 2.0, 5.0 );
    vertices.at( 2 ) = TwoDim<>( 3.0, 3.0 );

    CoolingVerticalPlane<> coolingBlock( "descriptionCooling", vertices, 1.0, 2.0, cooling );

    vector< Area<> > coolingAreas;
    shared_ptr< BlockGeometry<> > blockGeometry;
    coolingBlock.GetCoolingAreas( coolingAreas );
    coolingBlock.GetBlockGeometry( blockGeometry );


    TS_ASSERT_EQUALS( coolingBlock.GetCooling().get(), cooling.get() );
    TS_ASSERT_EQUALS( strcmp( blockGeometry->GetDescription(), "descriptionCooling" ), 0 );


    double zLower, zUpper;
    vector< TwoDim<> > returnedVertices;
    blockGeometry->GetInternalData( zLower, zUpper )->GetPoints( returnedVertices );

    TS_ASSERT_DELTA( zLower, 1.0, sDelta );
    TS_ASSERT_DELTA( zUpper, 2.0, sDelta );

    TS_ASSERT_EQUALS( returnedVertices.size(), 3 );
    TS_ASSERT( boost::algorithm::is_permutation( vertices.begin(), vertices.end(), returnedVertices.begin(), IsEqualByTolerance ) );


    vector< Cartesian<> > globalVertices;
    TS_ASSERT_EQUALS( coolingAreas.size(), 2 );

    coolingAreas.front().GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.size(), 4 );
    TS_ASSERT( globalVertices.at( 0 ).Equals( Cartesian<>( -1.0, 1.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 1 ).Equals( Cartesian<>( -1.0, 1.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 2 ).Equals( Cartesian<>( 2.0, 5.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 3 ).Equals( Cartesian<>( 2.0, 5.0, 1.0 ), sDelta ) );

    coolingAreas.back().GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.size(), 4 );
    TS_ASSERT( globalVertices.at( 0 ).Equals( Cartesian<>( 2.0, 5.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 1 ).Equals( Cartesian<>( 2.0, 5.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 2 ).Equals( Cartesian<>( 3.0, 3.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 3 ).Equals( Cartesian<>( 3.0, 3.0, 1.0 ), sDelta ) );
}

void TestCoolingFunctions::TestCoolingPrismatic()
{
    //#if defined(__EXCEPTIONS__)
    {
        shared_ptr< Cooling<> > cooling( new CoolingByConstantValue<>( 25.6 ) );
        vector< TwoDim<> > vertices( 3 );
        vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
        vertices.at( 1 ) = TwoDim<>( 1.0, 1.0 );
        vertices.at( 2 ) = TwoDim<>( 2.0, 0.0 );
        TS_ASSERT_THROWS_EQUALS( CoolingPrismatic<> coolingBlock( "", vertices, 1.0, 1.0, cooling ), std::runtime_error & e,
                                 strcmp( e.what(), "UpperZCoordinate must be bigger than LowerZCoordinate." ), 0 );
        vertices.pop_back();
        TS_ASSERT_THROWS_EQUALS( CoolingPrismatic<> coolingBlock( "", vertices, 1.0, 2.0, cooling ), std::runtime_error & e,
                                 strcmp( e.what(), "Vertices must have more than two elements to form an area." ), 0 );
#if defined( __EXCEPTIONS__ )
        vertices.push_back( TwoDim<>( 0.0, 1.0 ) );
        vertices.push_back( TwoDim<>( 1.0, 0.0 ) );
        TS_ASSERT_THROWS_EQUALS( CoolingPrismatic<> coolingBlock( "", vertices, 1.0, 2.0, cooling ),
                                 std::runtime_error & e, strcmp( e.what(), "Vertices create an overlapping polygon." ), 0 );
#endif
    }
    //#endif

    shared_ptr< Cooling<> > cooling( new CoolingByConstantValue<>( 25.6 ) );

    vector< TwoDim<> > vertices( 3 );
    vertices.at( 0 ) = TwoDim<>( -1.0, 1.0 );
    vertices.at( 1 ) = TwoDim<>( 2.0, 5.0 );
    vertices.at( 2 ) = TwoDim<>( 3.0, 3.0 );

    CoolingPrismatic<> coolingBlock( "descriptionCooling", vertices, 1.0, 2.0, cooling );

    vector< Area<> > coolingAreas;
    shared_ptr< BlockGeometry<> > blockGeometry;
    coolingBlock.GetCoolingAreas( coolingAreas );
    coolingBlock.GetBlockGeometry( blockGeometry );


    TS_ASSERT_EQUALS( coolingBlock.GetCooling().get(), cooling.get() );
    TS_ASSERT_EQUALS( strcmp( blockGeometry->GetDescription(), "descriptionCooling" ), 0 );


    double zLower, zUpper;
    vector< TwoDim<> > returnedVertices;
    blockGeometry->GetInternalData( zLower, zUpper )->GetPoints( returnedVertices );

    TS_ASSERT_DELTA( zLower, 1.0, sDelta );
    TS_ASSERT_DELTA( zUpper, 2.0, sDelta );

    TS_ASSERT_EQUALS( vertices.size(), 3 );
    TS_ASSERT( boost::algorithm::is_permutation( vertices.begin(), vertices.end(), returnedVertices.begin(), IsEqualByTolerance ) );


    vector< Cartesian<> > globalVertices;
    TS_ASSERT_EQUALS( coolingAreas.size(), 5 );
    vector< Area<> >::const_iterator it = coolingAreas.begin();

    it->GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.size(), 3 );
    TS_ASSERT( globalVertices.at( 0 ).Equals( Cartesian<>( -1.0, 1.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 1 ).Equals( Cartesian<>( 2.0, 5.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 2 ).Equals( Cartesian<>( 3.0, 3.0, 1.0 ), sDelta ) );
    TS_ASSERT_EQUALS( it->GetLocation(), BOTTOM );

    ++it;
    it->GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.size(), 3 );
    TS_ASSERT( globalVertices.at( 0 ).Equals( Cartesian<>( -1.0, 1.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 1 ).Equals( Cartesian<>( 2.0, 5.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 2 ).Equals( Cartesian<>( 3.0, 3.0, 2.0 ), sDelta ) );
    TS_ASSERT_EQUALS( it->GetLocation(), TOP );

    ++it;
    it->GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.size(), 4 );
    TS_ASSERT( globalVertices.at( 0 ).Equals( Cartesian<>( -1.0, 1.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 1 ).Equals( Cartesian<>( -1.0, 1.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 2 ).Equals( Cartesian<>( 2.0, 5.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 3 ).Equals( Cartesian<>( 2.0, 5.0, 1.0 ), sDelta ) );
    TS_ASSERT_EQUALS( it->GetLocation(), SIDE );

    ++it;
    it->GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.size(), 4 );
    TS_ASSERT( globalVertices.at( 0 ).Equals( Cartesian<>( 2.0, 5.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 1 ).Equals( Cartesian<>( 2.0, 5.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 2 ).Equals( Cartesian<>( 3.0, 3.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 3 ).Equals( Cartesian<>( 3.0, 3.0, 1.0 ), sDelta ) );
    TS_ASSERT_EQUALS( it->GetLocation(), SIDE );

    ++it;
    it->GetVertices( globalVertices );
    TS_ASSERT_EQUALS( globalVertices.size(), 4 );
    TS_ASSERT( globalVertices.at( 0 ).Equals( Cartesian<>( 3.0, 3.0, 1.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 1 ).Equals( Cartesian<>( 3.0, 3.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 2 ).Equals( Cartesian<>( -1.0, 1.0, 2.0 ), sDelta ) );
    TS_ASSERT( globalVertices.at( 3 ).Equals( Cartesian<>( -1.0, 1.0, 1.0 ), sDelta ) );
    TS_ASSERT_EQUALS( it->GetLocation(), SIDE );
}
