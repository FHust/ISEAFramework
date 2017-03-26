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
#include "TestBlocks.h"
#include "exception_tester.h"

using namespace geometry;
using namespace thermal;
static const double sDelta = 0.000001;


bool TwoDimEqual( const TwoDim<> &lhs, const TwoDim<> &rhs ) { return lhs.DistanceTo( rhs ) < sDelta; }


void TestBlocks::TestRectangularBlock()
{
    //#if defined(__EXCEPTIONS__)
    {
        Material<> mMaterial( 250.0, 1000.0, 50.0, 40.0, 20.0 );

        TS_ASSERT_THROWS_EQUALS( RectangularBlock<> block1( "", geometry::Cartesian<>( 0.0, 0.0, 0.0 ), 0.0, 1.0, 0.3,
                                                            2, 5, 1, &mMaterial, 50.0 ),
                                 std::runtime_error & e,
                                 !strstr( e.what(), "Every element of Dimensions must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( RectangularBlock<> block1( "", geometry::Cartesian<>( 0.0, 0.0, 0.0 ), 0.2, -1.0, 0.3,
                                                            2, 5, 1, &mMaterial, 50.0 ),
                                 std::runtime_error & e,
                                 !strstr( e.what(), "Every element of Dimensions must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( RectangularBlock<> block1( "", geometry::Cartesian<>( 0.0, 0.0, 0.0 ), 0.2, 1.0, -0.3,
                                                            2, 5, 1, &mMaterial, 50.0 ),
                                 std::runtime_error & e,
                                 !strstr( e.what(), "Every element of Dimensions must be bigger than zero" ), 0 );
    }
    //#endif


    std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
    thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
    thermalStates.at( 0 )->AddPowerDissipation( 200.0, 0.0 );
    thermalStates.at( 0 )->ResetPowerDissipation();
    Material<> mMaterial( 250.0, 1000.0, 50.0, 40.0, 20.0 );
    RectangularBlock<> blockZeroHeat( "description1", Cartesian<>( 0.0, 0.0, 0.0 ), 0.2, 1.0, 0.3, 2, 5, 1, &mMaterial, 50.0 );
    RectangularBlock<> block( "description2", Cartesian<>( 1.0, 2.0, 2.0 ), 0.4, 0.3, 0.5, 4, 3, 2, &mMaterial, 25.0, thermalStates );
    RectangularBlock<> oneElement( "description3", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.1, 0.1, 1, 1, 1, &mMaterial, 25.0 );


    TS_ASSERT_EQUALS( blockZeroHeat.GetNumberOfThermalElements(), 10 );
    TS_ASSERT_EQUALS( block.GetNumberOfThermalElements(), 24 );
    TS_ASSERT_EQUALS( oneElement.GetNumberOfThermalElements(), 1 );

    TS_ASSERT_EQUALS( blockZeroHeat.GetThermalStates(), std::vector< boost::shared_ptr< ::state::ThermalState<> > >( 1 ) );
    TS_ASSERT_EQUALS( block.GetThermalStates(), thermalStates );
    TS_ASSERT_EQUALS( oneElement.GetThermalStates(), std::vector< boost::shared_ptr< ::state::ThermalState<> > >( 1 ) );

    TS_ASSERT( blockZeroHeat.GetEmplacementPoint().Equals( Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );
    TS_ASSERT( block.GetEmplacementPoint().Equals( Cartesian<>( 1.0, 2.0, 2.0 ), sDelta ) );
    TS_ASSERT( oneElement.GetEmplacementPoint().Equals( Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );

    TS_ASSERT_EQUALS( !strstr( blockZeroHeat.GetDescription(), "description1" ), 0 );
    TS_ASSERT_EQUALS( !strstr( block.GetDescription(), "description2" ), 0 );
    TS_ASSERT_EQUALS( !strstr( oneElement.GetDescription(), "description3" ), 0 );

    RectangularBlockTester blockTester;
    double zLower, zUpper;
    vector< IndexedArea< double > > surfaceAreas;


    boost::shared_ptr< BlockGeometry<> > blockGeometryZeroHeat;
    blockZeroHeat.CreateData( blockTester.mThermalElements, blockTester.mConductivityMatrix, surfaceAreas, blockGeometryZeroHeat );

    blockTester.mSurfaceAreas.insert( blockTester.mSurfaceAreas.end(), surfaceAreas.begin(), surfaceAreas.end() );
    blockTester.mNX = 2;
    blockTester.mNY = 5;
    blockTester.mNZ = 1;
    blockTester.mDimX = 0.1;
    blockTester.mDimY = 0.2;
    blockTester.mDimZ = 0.3;
    blockTester.mOrigin = Cartesian<>( 0.0, 0.0, 0.0 );
    blockTester.mMaterial = &mMaterial;
    blockTester.mTemperature = 50.0;
    blockTester.mPowerDissipation = 0.0;

    blockTester.ThermalElementsTest();
    blockTester.SurfaceAreasTest();
    blockTester.ConductivityMatrixTest();

    const Geometry2D<> *geometry2DPtr = blockGeometryZeroHeat->GetInternalData( zLower, zUpper );
    TS_ASSERT_DELTA( zLower, blockTester.mOrigin.GetZ(), sDelta );
    TS_ASSERT_DELTA( zUpper, blockTester.mOrigin.GetZ() + blockTester.mNZ * blockTester.mDimZ, sDelta );
    vector< Geometry2D<> > intersectionVector;
    geometry2DPtr->Intersection( Geometry2D<>( blockTester.GetBaseVertices() ), intersectionVector );
    TS_ASSERT_EQUALS( intersectionVector.size(), 1 );
    TS_ASSERT_DELTA( intersectionVector.at( 0 ).CalculateAreaValue() / geometry2DPtr->CalculateAreaValue(), 1.0, sDelta );


    boost::shared_ptr< BlockGeometry<> > blockGeometry;
    block.CreateData( blockTester.mThermalElements, blockTester.mConductivityMatrix, surfaceAreas, blockGeometry );

    blockTester.mSurfaceAreas.clear();
    blockTester.mSurfaceAreas.insert( blockTester.mSurfaceAreas.end(), surfaceAreas.begin(), surfaceAreas.end() );
    blockTester.mNX = 4;
    blockTester.mNY = 3;
    blockTester.mNZ = 2;
    blockTester.mDimX = 0.1;
    blockTester.mDimY = 0.1;
    blockTester.mDimZ = 0.25;
    blockTester.mOrigin = Cartesian<>( 1.0, 2.0, 2.0 );
    blockTester.mMaterial = &mMaterial;
    blockTester.mTemperature = 25.0;
    blockTester.mPowerDissipation = 200.0 / ( 4.0 * 3.0 * 2.0 );

    blockTester.ThermalElementsTest();
    blockTester.SurfaceAreasTest();
    blockTester.ConductivityMatrixTest();

    geometry2DPtr = blockGeometry->GetInternalData( zLower, zUpper );
    TS_ASSERT_DELTA( zLower, blockTester.mOrigin.GetZ(), sDelta );
    TS_ASSERT_DELTA( zUpper, blockTester.mOrigin.GetZ() + blockTester.mNZ * blockTester.mDimZ, sDelta );
    geometry2DPtr->Intersection( Geometry2D<>( blockTester.GetBaseVertices() ), intersectionVector );
    TS_ASSERT_EQUALS( intersectionVector.size(), 1 );
    TS_ASSERT_DELTA( intersectionVector.at( 0 ).CalculateAreaValue() / geometry2DPtr->CalculateAreaValue(), 1.0, sDelta );


    oneElement.CreateData( blockTester.mThermalElements, blockTester.mConductivityMatrix, surfaceAreas, blockGeometry );

    blockTester.mSurfaceAreas.clear();
    blockTester.mSurfaceAreas.insert( blockTester.mSurfaceAreas.end(), surfaceAreas.begin(), surfaceAreas.end() );
    blockTester.mNX = 1;
    blockTester.mNY = 1;
    blockTester.mNZ = 1;
    blockTester.mDimX = 0.1;
    blockTester.mDimY = 0.1;
    blockTester.mDimZ = 0.1;
    blockTester.mOrigin = Cartesian<>( 0.0, 0.0, 0.0 );
    blockTester.mMaterial = &mMaterial;
    blockTester.mTemperature = 25.0;
    blockTester.mPowerDissipation = 0.0;

    blockTester.ThermalElementsTest();
    blockTester.SurfaceAreasTest();
    blockTester.ConductivityMatrixTest();

    geometry2DPtr = blockGeometry->GetInternalData( zLower, zUpper );
    TS_ASSERT_DELTA( zLower, blockTester.mOrigin.GetZ(), sDelta );
    TS_ASSERT_DELTA( zUpper, blockTester.mOrigin.GetZ() + blockTester.mNZ * blockTester.mDimZ, sDelta );
    geometry2DPtr->Intersection( Geometry2D<>( blockTester.GetBaseVertices() ), intersectionVector );
    TS_ASSERT_EQUALS( intersectionVector.size(), 1 );
    TS_ASSERT_DELTA( intersectionVector.at( 0 ).CalculateAreaValue() / geometry2DPtr->CalculateAreaValue(), 1.0, sDelta );
}

void TestBlocks::TestQuadraticCellBlock()
{
    //#if defined(__EXCEPTIONS__)
    {
        std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
        thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
        Material<> mElectrolyte( 250.0, 1000.0, 50.0, 40.0, 20.0 );
        Material<> mRubber( 250.0, 1000.0, 50.0, 40.0, 20.0 );

        TS_ASSERT_THROWS_EQUALS( QuadraticCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.0, 0.04, 0.2, 1, 4, 1,
                                                             0.01, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(), "Length must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( QuadraticCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.0, 0.2, 1, 4, 1,
                                                             0.01, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e,
                                 !strstr( e.what(),
                                          "RadiusCell must be bigger than zero and less than half of length, in order "
                                          "that the cell fits into the block" ),
                                 0 );
        TS_ASSERT_THROWS_EQUALS( QuadraticCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.04, 0.0, 1, 4, 1,
                                                             0.01, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(), "Height must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( QuadraticCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.04, 0.2, 1, 4, 1,
                                                             0.0, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e,
                                 !strstr( e.what(), "ArcPolygonization must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( QuadraticCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.06, 0.2, 1, 4, 1,
                                                             0.01, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e,
                                 !strstr( e.what(),
                                          "RadiusCell must be bigger than zero and less than half of length, in order "
                                          "that the cell fits into the block" ),
                                 0 );
        TS_ASSERT_THROWS_EQUALS( QuadraticCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.04, 0.2, 1, 2, 1,
                                                             0.01, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(), "PhiDivision must be divisible by four" ), 0 );
    }
    //#endif


    std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
    thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
    thermalStates.at( 0 )->AddPowerDissipation( 100.0, 0.0 );
    thermalStates.at( 0 )->ResetPowerDissipation();
    Material<> electrolyte( 250.0, 1000.0, 51.0, 41.0, 21.0 );
    Material<> rubber( 250.0, 1000.0, 55.0, 45.0, 25.0 );
    QuadraticCellBlock<> block( "descriptionText", Cartesian<>( 0.01, 0.02, 0.03 ), 0.1, 0.04, 0.2, 2, 4, 2, 0.01,
                                &electrolyte, &rubber, 25.0, thermalStates );

    vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
    vector< vector< IndexedValue< double > > > conductivityMatrix;
    vector< IndexedArea< double > > surfaceAreas;
    boost::shared_ptr< BlockGeometry<> > blockGeometry;
    block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

    TS_ASSERT_EQUALS( block.GetNumberOfThermalElements(), 24 );
    TS_ASSERT_EQUALS( block.GetThermalStates(), thermalStates );
    TS_ASSERT( block.GetEmplacementPoint().Equals( Cartesian<>( 0.01, 0.02, 0.03 ), sDelta ) );
    TS_ASSERT_EQUALS( !strstr( block.GetDescription(), "descriptionText" ), 0 );


    TS_ASSERT_EQUALS( thermalElements.size(), 24 );
    TS_ASSERT_EQUALS( conductivityMatrix.size(), 24 );
    size_t maxIndex = 0;
    BOOST_FOREACH ( const IndexedArea< double > &area, surfaceAreas )
        if ( maxIndex < area.mIndex )
            maxIndex = area.mIndex;
    TS_ASSERT_LESS_THAN( maxIndex, 24 );


    for ( size_t i = 0; i < 24; ++i )
        TS_ASSERT_EQUALS( thermalElements.at( 0 )->GetTemperature(), 25.0 );

    for ( size_t i = 0; i < 8; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &electrolyte );
        TS_ASSERT_EQUALS( thermalElements.at( i + 12 )->GetMaterial(), &electrolyte );
    }
    for ( size_t i = 8; i < 12; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &rubber );
        TS_ASSERT_EQUALS( thermalElements.at( i + 12 )->GetMaterial(), &rubber );
    }

    for ( size_t i = 0; i < 4; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 2.0, 1.0 ), 100.0 / 32.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetPowerDissipation( 2.0, 1.0 ), 100.0 / 32.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), 0.02 * 0.02 * Angle<>::pi / 4.0 * 0.1, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetVolume(), 0.02 * 0.02 * Angle<>::pi / 4.0 * 0.1, sDelta );
    }
    for ( size_t i = 4; i < 8; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 2.0, 1.0 ), 100.0 * 3.0 / 32.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetPowerDissipation( 2.0, 1.0 ), 100.0 * 3.0 / 32.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), ( 0.0016 - 0.0004 ) * Angle<>::pi / 4.0 * 0.1, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetVolume(), ( 0.0016 - 0.0004 ) * Angle<>::pi / 4.0 * 0.1, sDelta );
    }
    for ( size_t i = 8; i < 12; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 2.0, 1.0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetPowerDissipation( 2.0, 1.0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), ( 0.1 * 0.1 - 0.0016 * Angle<>::pi ) / 4.0 * 0.1, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetVolume(), ( 0.1 * 0.1 - 0.0016 * Angle<>::pi ) / 4.0 * 0.1, sDelta );
    }

    for ( size_t i = 0; i < 2; ++i )
    {
        TS_ASSERT( thermalElements.at( 0 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.01, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 1 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, 0.01, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 2 + 12 * i )->GetGridVertex().Equals( Cartesian<>( -0.01, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 3 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, -0.01, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );

        TS_ASSERT( thermalElements.at( 4 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.03, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 5 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, 0.03, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 6 + 12 * i )->GetGridVertex().Equals( Cartesian<>( -0.03, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 7 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, -0.03, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );

        TS_ASSERT( thermalElements.at( 8 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.045, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 9 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, 0.045, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 10 + 12 * i )->GetGridVertex().Equals( Cartesian<>( -0.045, 0.0, 0.05 + i * 0.1 ) +
                                                                               Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                              sDelta ) );
        TS_ASSERT( thermalElements.at( 11 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, -0.045, 0.05 + i * 0.1 ) +
                                                                               Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                              sDelta ) );
    }


    double conductivityRho1 = 51.0 * 0.02 * Angle<>::pi / 2.0 * 0.1 / 0.02;
    double conductivityRho2 = 0.04 * Angle<>::pi / 2.0 * 0.1 / ( 0.01 / 51.0 + 0.005 / 55.0 );
    double conductivityPhi1 = 41.0 * 0.02 * 0.1 / ( 0.01 * Angle<>::pi / 2.0 );
    double conductivityPhi2 = 41.0 * 0.02 * 0.1 / ( 0.03 * Angle<>::pi / 2.0 );
    double conductivityPhi3 = 45.0 * sqrt( ( 0.05 * sqrt( 2.0 ) - 0.04 ) * 0.01 ) * 0.1 / ( 0.045 * Angle<>::pi / 2.0 );
    double conductivityZ1 = 21.0 * 0.02 * 0.02 * Angle<>::pi / 4.0 / 0.1;
    double conductivityZ2 = 21.0 * ( 0.0016 - 0.0004 ) * Angle<>::pi / 4.0 / 0.1;
    double conductivityZ3 = 25.0 * ( 0.1 * 0.1 - 0.0016 * Angle<>::pi ) / 4.0 / 0.1;

    vector< list< IndexedValue< double > > > checkConductivityMatrix( 24 );
    for ( size_t i = 0; i < 4; ++i )
    {
        size_t j = ( i + 1 ) % 4;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivityPhi1 ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivityPhi1 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( j + 12, conductivityPhi1 ) );
        checkConductivityMatrix.at( j + 12 ).push_back( MakeIndexedValue< double >( i + 12, conductivityPhi1 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 4, conductivityRho1 ) );
        checkConductivityMatrix.at( i + 4 ).push_back( MakeIndexedValue< double >( i, conductivityRho1 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i + 16, conductivityRho1 ) );
        checkConductivityMatrix.at( i + 16 ).push_back( MakeIndexedValue< double >( i + 12, conductivityRho1 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 12, conductivityZ1 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i, conductivityZ1 ) );
    }
    for ( size_t i = 4; i < 8; ++i )
    {
        size_t j = ( i + 1 ) % 4 + 4;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivityPhi2 ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivityPhi2 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( j + 12, conductivityPhi2 ) );
        checkConductivityMatrix.at( j + 12 ).push_back( MakeIndexedValue< double >( i + 12, conductivityPhi2 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 4, conductivityRho2 ) );
        checkConductivityMatrix.at( i + 4 ).push_back( MakeIndexedValue< double >( i, conductivityRho2 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i + 16, conductivityRho2 ) );
        checkConductivityMatrix.at( i + 16 ).push_back( MakeIndexedValue< double >( i + 12, conductivityRho2 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 12, conductivityZ2 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i, conductivityZ2 ) );
    }
    for ( size_t i = 8; i < 12; ++i )
    {
        size_t j = ( i + 1 ) % 4 + 8;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivityPhi3 ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivityPhi3 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( j + 12, conductivityPhi3 ) );
        checkConductivityMatrix.at( j + 12 ).push_back( MakeIndexedValue< double >( i + 12, conductivityPhi3 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 12, conductivityZ3 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i, conductivityZ3 ) );
    }

    for ( size_t i = 0; i < 24; ++i )
    {
        double selfConductivity = 0;
        BOOST_FOREACH ( const IndexedValue< double > &conductivity, checkConductivityMatrix.at( i ) )
            selfConductivity -= conductivity.mValue;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i, selfConductivity ) );
        checkConductivityMatrix.at( i ).sort();

        TS_ASSERT_EQUALS( checkConductivityMatrix.at( i ).size(), conductivityMatrix.at( i ).size() );
        TS_ASSERT( boost::algorithm::is_permutation( conductivityMatrix.at( i ).begin(), conductivityMatrix.at( i ).end(),
                                                     checkConductivityMatrix.at( i ).begin(),
                                                     IndexedValuesEqual< double >( sDelta ) ) );
    }


    TS_ASSERT_EQUALS( surfaceAreas.size(), 4 * ( 2 + 2 + 2 * 2 ) );

    vector< IndexedArea< double > >::const_iterator it = surfaceAreas.begin();
    for ( size_t i = 0; i < 12; ++i )
    {
        for ( size_t j = 0; j < 2; ++j )
        {
            TS_ASSERT_EQUALS( it->mIndex, i + j * 12 );
            TS_ASSERT_DELTA( it->mArea.GetDistanceToGridVertex(), 0.05, sDelta );
            if ( i % 12 < 8 )
            {
                TS_ASSERT_EQUALS( it->mArea.GetConductivity(), 21.0 );
            }
            else
            {
                TS_ASSERT_EQUALS( it->mArea.GetConductivity(), 25.0 );
            }
            if ( j == 0 )
            {
                TS_ASSERT_EQUALS( it->mArea.GetLocation(), BOTTOM );
            }
            else
            {
                TS_ASSERT_EQUALS( it->mArea.GetLocation(), TOP );
            }

            ++it;
        }
    }
    size_t indices[8] = {8, 9, 10, 11, 20, 21, 22, 23};
    for ( size_t i = 0; i < 8; ++i )
    {
        TS_ASSERT_EQUALS( it->mIndex, indices[i] );
        TS_ASSERT_DELTA( it->mArea.GetDistanceToGridVertex(), 0.005, sDelta );
        TS_ASSERT_EQUALS( it->mArea.GetConductivity(), 55.0 );
        TS_ASSERT_EQUALS( it->mArea.GetLocation(), SIDE );

        ++it;
    }

    it = surfaceAreas.begin();
    for ( size_t i = 0; i < 4; ++i )
        for ( size_t j = 0; j < 2; ++j )
        {
            vector< Cartesian<> > vertices;
            it->mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 3 );
            TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.0, 0.0, j * 0.2 ) + Cartesian<>( 0.06, 0.07, 0.03 ), sDelta ) );
            TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.02 * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.02 * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 0.02 * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.02 * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );

            ++it;
        }
    for ( size_t i = 0; i < 4; ++i )
        for ( size_t j = 0; j < 2; ++j )
        {
            vector< Cartesian<> > vertices;
            it->mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );
            TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.02 * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.02 * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.02 * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.02 * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 0.04 * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 0.04 * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );

            ++it;
        }
    for ( size_t i = 0; i < 4; ++i )
        for ( size_t j = 0; j < 2; ++j )
        {
            vector< Cartesian<> > vertices;
            it->mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );
            TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.04 * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.04 * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                              0.05 * sqrt( 2.0 ) * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                  Cartesian<>( 0.06, 0.07, 0.03 ),
                                 sDelta ) );
            TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.05 * sqrt( 2.0 ) * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );

            ++it;
        }

    for ( size_t j = 0; j < 2; ++j )
        for ( size_t i = 0; i < 4; ++i )
        {
            vector< Cartesian<> > vertices;
            it->mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );
            TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.05 * sqrt( 2.0 ) * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.1 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 1 )
                        .Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                              0.05 * sqrt( 2.0 ) * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.1 ) +
                                  Cartesian<>( 0.06, 0.07, 0.03 ),
                                 sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                              0.05 * sqrt( 2.0 ) * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), ( j + 1 ) * 0.1 ) +
                                  Cartesian<>( 0.06, 0.07, 0.03 ),
                                 sDelta ) );
            TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.05 * sqrt( 2.0 ) * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             ( j + 1 ) * 0.1 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );

            ++it;
        }


    double zLower, zUpper;
    const Geometry2D<> *geometry2DPtr = blockGeometry->GetInternalData( zLower, zUpper );
    TS_ASSERT_DELTA( zLower, 0.03, sDelta );
    TS_ASSERT_DELTA( zUpper, 0.23, sDelta );
    TS_ASSERT_DELTA( geometry2DPtr->CalculateAreaValue(), 0.01, sDelta );

    TwoDim<> lowerEdge, higherEdge;
    geometry2DPtr->Envelope( lowerEdge, higherEdge );
    TS_ASSERT_DELTA( lowerEdge.Get1(), 0.01, sDelta );
    TS_ASSERT_DELTA( lowerEdge.Get2(), 0.02, sDelta );
    TS_ASSERT_DELTA( higherEdge.Get1(), 0.11, sDelta );
    TS_ASSERT_DELTA( higherEdge.Get2(), 0.12, sDelta );

    TS_ASSERT_EQUALS( !strstr( blockGeometry->GetDescription(), "descriptionText" ), 0 );

    // test lateral areas
    vector< IndexedInnerArea< double > > innerAreas;
    block.GetInnerSurfaceAreas( innerAreas );
    TS_ASSERT_EQUALS( innerAreas.size(), 8 );
    TS_ASSERT_EQUALS( innerAreas.at( 0 ).mIndex, 4 );
    TS_ASSERT_EQUALS( innerAreas.at( 1 ).mIndex, 16 );
    TS_ASSERT_EQUALS( innerAreas.at( 2 ).mIndex, 5 );
    TS_ASSERT_EQUALS( innerAreas.at( 3 ).mIndex, 17 );
    TS_ASSERT_EQUALS( innerAreas.at( 4 ).mIndex, 6 );
    TS_ASSERT_EQUALS( innerAreas.at( 5 ).mIndex, 18 );
    TS_ASSERT_EQUALS( innerAreas.at( 6 ).mIndex, 7 );
    TS_ASSERT_EQUALS( innerAreas.at( 7 ).mIndex, 19 );

    vector< Cartesian< double > > areaVertices;
    BOOST_FOREACH ( const IndexedInnerArea< double > &indexedArea, innerAreas )
    {
        TS_ASSERT_DELTA( indexedArea.mArea.GetAreaValue(), 0.00565686, sDelta );
        indexedArea.mArea.GetVertices( areaVertices );
        BOOST_FOREACH ( const Cartesian< double > &vertex, areaVertices )
        {
            TS_ASSERT_DELTA( std::pow( ( vertex.GetX() - 0.01 - 0.05 ), 2.0 ) +
                              std::pow( ( vertex.GetY() - 0.02 - 0.05 ), 2.0 ) - std::pow( 0.04, 2.0 ),
                             0.0, sDelta );
        }
    }
}

void TestBlocks::TestQuadraticCellBlockPhiDivisionEight()
{
    std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
    thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
    thermalStates.at( 0 )->AddPowerDissipation( 100.0, 0.0 );
    thermalStates.at( 0 )->ResetPowerDissipation();
    Material<> electrolyte( 250.0, 1000.0, 51.0, 41.0, 21.0 );
    Material<> rubber( 250.0, 1000.0, 55.0, 45.0, 25.0 );
    QuadraticCellBlock<> block( "descriptionText", Cartesian<>( -0.05, -0.05, -0.05 ), 0.1, 0.04, 0.1, 1, 8, 1, 0.01,
                                &rubber, &rubber, 25.0, thermalStates );

    vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
    vector< vector< IndexedValue< double > > > conductivityMatrix;
    vector< IndexedArea< double > > surfaceAreas;
    boost::shared_ptr< BlockGeometry<> > blockGeometry;
    block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

    TS_ASSERT_EQUALS( block.GetNumberOfThermalElements(), 16 );
    TS_ASSERT_EQUALS( block.GetThermalStates(), thermalStates );
    TS_ASSERT( block.GetEmplacementPoint().Equals( Cartesian<>( -0.05, -0.05, -0.05 ), sDelta ) );
    TS_ASSERT_EQUALS( !strstr( block.GetDescription(), "descriptionText" ), 0 );


    TS_ASSERT_EQUALS( thermalElements.size(), 16 );
    TS_ASSERT_EQUALS( conductivityMatrix.size(), 16 );
    size_t maxIndex = 0;
    BOOST_FOREACH ( const IndexedArea< double > &area, surfaceAreas )
        if ( maxIndex < area.mIndex )
            maxIndex = area.mIndex;
    TS_ASSERT_EQUALS( maxIndex, 15 );


    for ( size_t i = 0; i < 16; ++i )
        TS_ASSERT_EQUALS( thermalElements.at( 0 )->GetTemperature(), 25.0 );

    for ( size_t i = 0; i < 8; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &rubber );
    }
    for ( size_t i = 8; i < 16; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &rubber );
    }

    for ( size_t i = 0; i < 8; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 1.0, 1.0 ), 100.0 / 8.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), 0.04 * 0.04 * Angle<>::pi / 8.0 * 0.1, sDelta );
    }
    for ( size_t i = 8; i < 16; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 1.0, 1.0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), ( ( 0.1 * 0.1 - 0.04 * 0.04 * Angle<>::pi ) / 8.0 ) * 0.1, sDelta );
    }

    for ( size_t i = 0; i < 8; ++i )
    {
        TS_ASSERT(
         thermalElements.at( i )->GetGridVertex().Equals( Cartesian<>( 0.02 * cos( Angle<>::pi * ( -0.125 + i * 0.25 ) ),
                                                                       0.02 * sin( Angle<>::pi * ( -0.125 + i * 0.25 ) ), 0.0 ),
                                                          sDelta ) );
    }
    TS_ASSERT( thermalElements.at( 8 )->GetGridVertex().Equals( Cartesian<>( 0.04347759, -0.02015367, 0.0 ), sDelta ) );
    TS_ASSERT( thermalElements.at( 9 )->GetGridVertex().Equals( Cartesian<>( 0.04347759, 0.02015367, 0.0 ), sDelta ) );
    TS_ASSERT( thermalElements.at( 10 )->GetGridVertex().Equals( Cartesian<>( 0.02015367, 0.04347759, 0.0 ), sDelta ) );
    TS_ASSERT( thermalElements.at( 11 )->GetGridVertex().Equals( Cartesian<>( -0.02015367, 0.04347759, 0.0 ), sDelta ) );
    TS_ASSERT( thermalElements.at( 12 )->GetGridVertex().Equals( Cartesian<>( -0.04347759, 0.02015367, 0.0 ), sDelta ) );
    TS_ASSERT( thermalElements.at( 13 )->GetGridVertex().Equals( Cartesian<>( -0.04347759, -0.02015367, 0.0 ), sDelta ) );
    TS_ASSERT( thermalElements.at( 14 )->GetGridVertex().Equals( Cartesian<>( -0.02015367, -0.04347759, 0.0 ), sDelta ) );
    TS_ASSERT( thermalElements.at( 15 )->GetGridVertex().Equals( Cartesian<>( 0.02015367, -0.04347759, 0.0 ), sDelta ) );


    const double conductivityRho = 55.0 * 0.08 * Angle<>::pi / 8.0 * 0.1 / 0.02792151;
    const double conductivityPhi1 = 45.0 * 0.04 * 0.1 / ( 0.04 * Angle<>::pi / 8.0 );
    const double conPhi2_spaceSmallerEnd = 0.04792151 * 0.43405779 * 2.0;
    const double conPhi2_spaceBiggerEnd = 0.04792151 * 0.3513403734 * 2.0;
    const double zHeight = 0.1;
    const double conPhi2_distRhoAtGridVertex = 0.00792151 + 0.006727847;
    const double conPhi2_distRhoSmallerEnd = 0.01;
    const double conPhi2_distRhoBiggerEnd = 0.05 * sqrt( 2.0 ) - 0.04;
    const double conductivityPhi2_1 =
     45.0 * pow( conPhi2_distRhoAtGridVertex * conPhi2_distRhoSmallerEnd, 0.5 ) * zHeight / conPhi2_spaceSmallerEnd;
    const double conductivityPhi2_2 =
     45.0 * pow( conPhi2_distRhoAtGridVertex * conPhi2_distRhoBiggerEnd, 0.5 ) * zHeight / conPhi2_spaceBiggerEnd;

    vector< list< IndexedValue< double > > > checkConductivityMatrix( 16 );
    for ( size_t i = 0; i < 8; ++i )
    {
        size_t j = ( i + 1 ) % 8;

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivityPhi1 ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivityPhi1 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 8, conductivityRho ) );
        checkConductivityMatrix.at( i + 8 ).push_back( MakeIndexedValue< double >( i, conductivityRho ) );
    }
    for ( size_t i = 8; i < 16; ++i )
    {
        double conductivity;
        if ( i % 2 == 0 )
            conductivity = conductivityPhi2_1;
        else
            conductivity = conductivityPhi2_2;

        size_t j = ( i + 1 ) % 8 + 8;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivity ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivity ) );
    }

    for ( size_t i = 0; i < 16; ++i )
    {
        double selfConductivity = 0;
        BOOST_FOREACH ( const IndexedValue< double > &conductivity, checkConductivityMatrix.at( i ) )
            selfConductivity -= conductivity.mValue;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i, selfConductivity ) );
        checkConductivityMatrix.at( i ).sort();

        TS_ASSERT_EQUALS( checkConductivityMatrix.at( i ).size(), conductivityMatrix.at( i ).size() );
        TS_ASSERT( boost::algorithm::is_permutation( conductivityMatrix.at( i ).begin(), conductivityMatrix.at( i ).end(),
                                                     checkConductivityMatrix.at( i ).begin(),
                                                     IndexedValuesEqual< double >( sqrt( sDelta ) ) ) );
    }


    TS_ASSERT_EQUALS( surfaceAreas.size(), 2 * ( 2 * 8 ) + 8 * 1 );

    for ( size_t i = 0; i < 32; ++i )
    {
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetConductivity(), 25.0 );
        TS_ASSERT_DELTA( surfaceAreas.at( i ).mArea.GetDistanceToGridVertex(), 0.05, sDelta );
        if ( i % 2 == 0 )
        {
            TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetLocation(), BOTTOM );
        }
        else
        {
            TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetLocation(), TOP );
        }
    }
    for ( size_t i = 32; i < 40; ++i )
    {
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetConductivity(), 55.0 );
        TS_ASSERT_DELTA( surfaceAreas.at( i ).mArea.GetDistanceToGridVertex(), 0.00672784685, 0.00001 );
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mIndex, i - 24 );
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetLocation(), SIDE );
    }

    for ( size_t i = 0; i < 8; ++i )
    {
        vector< Cartesian<> > vertices;
        surfaceAreas.at( i * 2 ).mArea.GetVertices( vertices );
        TS_ASSERT_EQUALS( vertices.size(), 3 );
        TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.0, 0.0, -0.05 ), sDelta ) );
        TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.04 * cos( ( i * 0.25 - 0.25 ) * Angle<>::pi ),
                                                         0.04 * sin( ( i * 0.25 - 0.25 ) * Angle<>::pi ), -0.05 ),
                                            sDelta ) );
        TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 0.04 * cos( ( ( i + 1 ) * 0.25 - 0.25 ) * Angle<>::pi ),
                                                         0.04 * sin( ( ( i + 1 ) * 0.25 - 0.25 ) * Angle<>::pi ), -0.05 ),
                                            sDelta ) );
    }
    for ( size_t i = 0; i < 8; ++i )
    {
        vector< Cartesian<> > vertices;
        surfaceAreas.at( 1 + i * 2 ).mArea.GetVertices( vertices );
        TS_ASSERT_EQUALS( vertices.size(), 3 );
        TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.0, 0.0, 0.05 ), sDelta ) );
        TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.04 * cos( ( i * 0.25 - 0.25 ) * Angle<>::pi ),
                                                         0.04 * sin( ( i * 0.25 - 0.25 ) * Angle<>::pi ), 0.05 ),
                                            sDelta ) );
        TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 0.04 * cos( ( ( i + 1 ) * 0.25 - 0.25 ) * Angle<>::pi ),
                                                         0.04 * sin( ( ( i + 1 ) * 0.25 - 0.25 ) * Angle<>::pi ), 0.05 ),
                                            sDelta ) );
    }

    for ( size_t i = 0; i < 8; ++i )
        for ( size_t j = 0; j < 2; ++j )
        {
            vector< Cartesian<> > vertices;
            surfaceAreas.at( 2 * i + 16 + j ).mArea.GetVertices( vertices );
            double zCoordinate = ( j == 0 ) ? -0.05 : 0.05;
            TS_ASSERT_EQUALS( vertices.size(), 4 );
            TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.04 * cos( ( i * 0.25 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( i * 0.25 - 0.25 ) * Angle<>::pi ), zCoordinate ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.04 * cos( ( ( i + 1 ) * 0.25 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( ( i + 1 ) * 0.25 - 0.25 ) * Angle<>::pi ), zCoordinate ),
                                                sDelta ) );

            double xOf2, yOf2, xOf3, yOf3;
            switch ( i )
            {
                case 0:
                    xOf3 = 0.05;
                    yOf3 = -0.05;
                    xOf2 = 0.05;
                    yOf2 = 0.0;
                    break;
                case 1:
                    xOf3 = 0.05;
                    yOf3 = 0.0;
                    xOf2 = 0.05;
                    yOf2 = 0.05;
                    break;
                case 2:
                    xOf3 = 0.05;
                    yOf3 = 0.05;
                    xOf2 = 0.0;
                    yOf2 = 0.05;
                    break;
                case 3:
                    xOf3 = 0.0;
                    yOf3 = 0.05;
                    xOf2 = -0.05;
                    yOf2 = 0.05;
                    break;
                case 4:
                    xOf3 = -0.05;
                    yOf3 = 0.05;
                    xOf2 = -0.05;
                    yOf2 = 0.0;
                    break;
                case 5:
                    xOf3 = -0.05;
                    yOf3 = 0.0;
                    xOf2 = -0.05;
                    yOf2 = -0.05;
                    break;
                case 6:
                    xOf3 = -0.05;
                    yOf3 = -0.05;
                    xOf2 = 0.0;
                    yOf2 = -0.05;
                    break;
                case 7:
                    xOf3 = 0.0;
                    yOf3 = -0.05;
                    xOf2 = 0.05;
                    yOf2 = -0.05;
                    break;
            }


            TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( xOf2, yOf2, zCoordinate ), sDelta ) );
            TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( xOf3, yOf3, zCoordinate ), sDelta ) );
        }
    // Seitenflaeche nicht getestet
}

void TestBlocks::TestSupercap()
{
    //#if defined(__EXCEPTIONS__)
    {
        std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
        thermalStates.at( 0 ).reset( new ::state::ThermalState<> );

        TS_ASSERT_THROWS_EQUALS( Supercap<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.0, 0.03, 0.04, 0.2, 4, 1, 0.01,
                                                   0, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(), "Length must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( Supercap<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.0, 0.04, 0.2, 4, 1, 0.01, 0,
                                                   0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e,
                                 !strstr( e.what(),
                                          "InnerRadiusCell must be bigger than zero and less "
                                          "than half of length, in order that the cell fits into the block" ),
                                 0 );
        TS_ASSERT_THROWS_EQUALS( Supercap<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.03, 0.04, 0.0, 4, 1, 0.01,
                                                   0, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(), "Height must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( Supercap<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.04, 0.04, 0.2, 4, 1, 0.0, 0,
                                                   0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e,
                                 !strstr( e.what(), "ArcPolygonEdgesLength must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( Supercap<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.03, 0.06, 0.2, 4, 1, 0.01,
                                                   0, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e,
                                 !strstr( e.what(),
                                          "OuterRadiusCell must be bigger than zero and less "
                                          "than half of length, in order that the cell fits into the block" ),
                                 0 );
        TS_ASSERT_THROWS_EQUALS( Supercap<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.03, 0.04, 0.2, 2, 1, 0.01,
                                                   0, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(), "PhiDivision must be divisible by four" ), 0 );
    }
    //#endif


    std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
    thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
    thermalStates.at( 0 )->AddPowerDissipation( 100.0, 0.0 );
    thermalStates.at( 0 )->ResetPowerDissipation();
    Material<> innerCellMaterial( 250.0, 1000.0, 57.0, 47.0, 27.0 );
    Material<> outerCellMaterial( 250.0, 1000.0, 53.0, 43.0, 23.0 );
    Material<> rubber( 250.0, 1000.0, 50.0, 40.0, 20.0 );
    Supercap<> block( "descriptionText", Cartesian<>( 0.01, 0.02, 0.03 ), 0.1, 0.03, 0.04, 0.2, 4, 2, 0.01,
                      &innerCellMaterial, &outerCellMaterial, &rubber, 25.0, thermalStates );

    vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
    vector< vector< IndexedValue< double > > > conductivityMatrix;
    vector< IndexedArea< double > > surfaceAreas;
    boost::shared_ptr< BlockGeometry<> > blockGeometry;
    block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

    TS_ASSERT_EQUALS( block.GetNumberOfThermalElements(), 24 );
    TS_ASSERT_EQUALS( block.GetThermalStates(), thermalStates );
    TS_ASSERT( block.GetEmplacementPoint().Equals( Cartesian<>( 0.01, 0.02, 0.03 ), sDelta ) );
    TS_ASSERT_EQUALS( !strstr( block.GetDescription(), "descriptionText" ), 0 );


    TS_ASSERT_EQUALS( thermalElements.size(), 24 );
    TS_ASSERT_EQUALS( conductivityMatrix.size(), 24 );
    size_t maxIndex = 0;
    BOOST_FOREACH ( const IndexedArea< double > &area, surfaceAreas )
        if ( maxIndex < area.mIndex )
            maxIndex = area.mIndex;
    TS_ASSERT_LESS_THAN( maxIndex, 24 );


    for ( size_t i = 0; i < 24; ++i )
        TS_ASSERT_EQUALS( thermalElements.at( 0 )->GetTemperature(), 25.0 );

    for ( size_t i = 0; i < 4; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &innerCellMaterial );
        TS_ASSERT_EQUALS( thermalElements.at( i + 12 )->GetMaterial(), &innerCellMaterial );
    }
    for ( size_t i = 4; i < 8; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &outerCellMaterial );
        TS_ASSERT_EQUALS( thermalElements.at( i + 12 )->GetMaterial(), &outerCellMaterial );
    }
    for ( size_t i = 8; i < 12; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &rubber );
        TS_ASSERT_EQUALS( thermalElements.at( i + 12 )->GetMaterial(), &rubber );
    }

    for ( size_t i = 0; i < 4; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 2.0, 1.0 ), 100.0 / 8.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetPowerDissipation( 2.0, 1.0 ), 100.0 / 8.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), 0.03 * 0.03 * Angle<>::pi / 4.0 * 0.1, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetVolume(), 0.03 * 0.03 * Angle<>::pi / 4.0 * 0.1, sDelta );
    }
    for ( size_t i = 4; i < 8; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 2.0, 1.0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetPowerDissipation( 2.0, 1.0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), ( 0.0016 - 0.0009 ) * Angle<>::pi / 4.0 * 0.1, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetVolume(), ( 0.0016 - 0.0009 ) * Angle<>::pi / 4.0 * 0.1, sDelta );
    }
    for ( size_t i = 8; i < 12; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 2.0, 1.0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetPowerDissipation( 2.0, 1.0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), ( 0.1 * 0.1 - 0.0016 * Angle<>::pi ) / 4.0 * 0.1, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetVolume(), ( 0.1 * 0.1 - 0.0016 * Angle<>::pi ) / 4.0 * 0.1, sDelta );
    }

    for ( size_t i = 0; i < 2; ++i )
    {
        TS_ASSERT( thermalElements.at( 0 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.015, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 1 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, 0.015, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 2 + 12 * i )->GetGridVertex().Equals( Cartesian<>( -0.015, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 3 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, -0.015, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );

        TS_ASSERT( thermalElements.at( 4 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.035, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 5 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, 0.035, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 6 + 12 * i )->GetGridVertex().Equals( Cartesian<>( -0.035, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 7 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, -0.035, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );

        TS_ASSERT( thermalElements.at( 8 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.045, 0.0, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 9 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, 0.045, 0.05 + i * 0.1 ) +
                                                                              Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                             sDelta ) );
        TS_ASSERT( thermalElements.at( 10 + 12 * i )->GetGridVertex().Equals( Cartesian<>( -0.045, 0.0, 0.05 + i * 0.1 ) +
                                                                               Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                              sDelta ) );
        TS_ASSERT( thermalElements.at( 11 + 12 * i )->GetGridVertex().Equals( Cartesian<>( 0.0, -0.045, 0.05 + i * 0.1 ) +
                                                                               Cartesian<>( 0.06, 0.07, 0.03 ),
                                                                              sDelta ) );
    }


    double conductivityRho1 = 0.03 * Angle<>::pi / 2.0 * 0.1 / ( 0.015 / 57.0 + 0.005 / 53.0 );
    double conductivityRho2 = 0.04 * Angle<>::pi / 2.0 * 0.1 / ( 0.005 / 53.0 + 0.005 / 50.0 );
    double conductivityPhi1 = 47.0 * 0.03 * 0.1 / ( 0.015 * Angle<>::pi / 2.0 );
    double conductivityPhi2 = 43.0 * 0.01 * 0.1 / ( 0.035 * Angle<>::pi / 2.0 );
    double conductivityPhi3 = 40.0 * sqrt( ( 0.05 * sqrt( 2.0 ) - 0.04 ) * 0.01 ) * 0.1 / ( 0.045 * Angle<>::pi / 2.0 );
    double conductivityZ1 = 27.0 * 0.03 * 0.03 * Angle<>::pi / 4.0 / 0.1;
    double conductivityZ2 = 23.0 * ( 0.0016 - 0.0009 ) * Angle<>::pi / 4.0 / 0.1;
    double conductivityZ3 = 20.0 * ( 0.1 * 0.1 - 0.0016 * Angle<>::pi ) / 4.0 / 0.1;

    vector< vector< IndexedValue< double > > > checkConductivityMatrix( 24 );
    for ( size_t i = 0; i < 4; ++i )
    {
        size_t j = ( i + 1 ) % 4;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivityPhi1 ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivityPhi1 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( j + 12, conductivityPhi1 ) );
        checkConductivityMatrix.at( j + 12 ).push_back( MakeIndexedValue< double >( i + 12, conductivityPhi1 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 4, conductivityRho1 ) );
        checkConductivityMatrix.at( i + 4 ).push_back( MakeIndexedValue< double >( i, conductivityRho1 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i + 16, conductivityRho1 ) );
        checkConductivityMatrix.at( i + 16 ).push_back( MakeIndexedValue< double >( i + 12, conductivityRho1 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 12, conductivityZ1 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i, conductivityZ1 ) );
    }
    for ( size_t i = 4; i < 8; ++i )
    {
        size_t j = ( i + 1 ) % 4 + 4;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivityPhi2 ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivityPhi2 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( j + 12, conductivityPhi2 ) );
        checkConductivityMatrix.at( j + 12 ).push_back( MakeIndexedValue< double >( i + 12, conductivityPhi2 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 4, conductivityRho2 ) );
        checkConductivityMatrix.at( i + 4 ).push_back( MakeIndexedValue< double >( i, conductivityRho2 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i + 16, conductivityRho2 ) );
        checkConductivityMatrix.at( i + 16 ).push_back( MakeIndexedValue< double >( i + 12, conductivityRho2 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 12, conductivityZ2 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i, conductivityZ2 ) );
    }
    for ( size_t i = 8; i < 12; ++i )
    {
        size_t j = ( i + 1 ) % 4 + 8;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivityPhi3 ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivityPhi3 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( j + 12, conductivityPhi3 ) );
        checkConductivityMatrix.at( j + 12 ).push_back( MakeIndexedValue< double >( i + 12, conductivityPhi3 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 12, conductivityZ3 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i, conductivityZ3 ) );
    }

    for ( size_t i = 0; i < 24; ++i )
    {
        double selfConductivity = 0;
        BOOST_FOREACH ( const IndexedValue< double > &conductivity, checkConductivityMatrix.at( i ) )
            selfConductivity -= conductivity.mValue;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i, selfConductivity ) );
        std::sort( checkConductivityMatrix.at( i ).begin(), checkConductivityMatrix.at( i ).end() );

        TS_ASSERT_EQUALS( checkConductivityMatrix.at( i ).size(), conductivityMatrix.at( i ).size() );
        /*for(size_t j = 0; j < checkConductivityMatrix.at(i).size(); ++j)
        {
            printf("%i, i%:", i, j);
            TS_ASSERT_DELTA(conductivityMatrix.at(i)vertices.at(j), checkConductivityMatrix.at(i)vertices.at(j), sDelta)
        }*/
        TS_ASSERT( boost::algorithm::is_permutation( conductivityMatrix.at( i ).begin(), conductivityMatrix.at( i ).end(),
                                                     checkConductivityMatrix.at( i ).begin(),
                                                     IndexedValuesEqual< double >( sDelta ) ) );
    }


    TS_ASSERT_EQUALS( surfaceAreas.size(), 4 * ( 2 + 2 + 2 * 2 ) );

    vector< IndexedArea< double > >::const_iterator it = surfaceAreas.begin();
    for ( size_t i = 0; i < 12; ++i )
    {
        for ( size_t j = 0; j < 2; ++j )
        {
            TS_ASSERT_EQUALS( it->mIndex, i + j * 12 );
            TS_ASSERT_DELTA( it->mArea.GetDistanceToGridVertex(), 0.05, sDelta );
            if ( i % 12 < 4 )
            {
                TS_ASSERT_EQUALS( it->mArea.GetConductivity(), 27.0 );
            }
            else if ( i % 12 < 8 )
            {
                TS_ASSERT_EQUALS( it->mArea.GetConductivity(), 23.0 );
            }
            else
            {
                TS_ASSERT_EQUALS( it->mArea.GetConductivity(), 20.0 );
            }
            if ( j == 0 )
            {
                TS_ASSERT_EQUALS( it->mArea.GetLocation(), BOTTOM );
            }
            else
            {
                TS_ASSERT_EQUALS( it->mArea.GetLocation(), TOP );
            }

            ++it;
        }
    }
    size_t indices[8] = {8, 9, 10, 11, 20, 21, 22, 23};
    for ( size_t i = 0; i < 8; ++i )
    {
        TS_ASSERT_EQUALS( it->mIndex, indices[i] );
        TS_ASSERT_DELTA( it->mArea.GetDistanceToGridVertex(), 0.005, sDelta );
        TS_ASSERT_EQUALS( it->mArea.GetConductivity(), 50.0 );
        TS_ASSERT_EQUALS( it->mArea.GetLocation(), SIDE );

        ++it;
    }

    it = surfaceAreas.begin();
    for ( size_t i = 0; i < 4; ++i )
        for ( size_t j = 0; j < 2; ++j )
        {
            vector< Cartesian<> > vertices;
            it->mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 3 );
            TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.0, 0.0, j * 0.2 ) + Cartesian<>( 0.06, 0.07, 0.03 ), sDelta ) );
            TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.03 * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.03 * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 0.03 * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.03 * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );

            ++it;
        }
    for ( size_t i = 0; i < 4; ++i )
        for ( size_t j = 0; j < 2; ++j )
        {
            vector< Cartesian<> > vertices;
            it->mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );
            TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.03 * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.03 * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.03 * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.03 * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 0.04 * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 0.04 * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );

            ++it;
        }
    for ( size_t i = 0; i < 4; ++i )
        for ( size_t j = 0; j < 2; ++j )
        {
            vector< Cartesian<> > vertices;
            it->mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );
            TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.04 * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.04 * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.04 * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                              0.05 * sqrt( 2.0 ) * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                  Cartesian<>( 0.06, 0.07, 0.03 ),
                                 sDelta ) );
            TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.05 * sqrt( 2.0 ) * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.2 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );

            ++it;
        }

    for ( size_t j = 0; j < 2; ++j )
        for ( size_t i = 0; i < 4; ++i )
        {
            vector< Cartesian<> > vertices;
            it->mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );
            TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.05 * sqrt( 2.0 ) * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.1 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );
            TS_ASSERT( vertices.at( 1 )
                        .Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                              0.05 * sqrt( 2.0 ) * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), j * 0.1 ) +
                                  Cartesian<>( 0.06, 0.07, 0.03 ),
                                 sDelta ) );
            TS_ASSERT( vertices.at( 2 )
                        .Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ),
                                              0.05 * sqrt( 2.0 ) * sin( ( ( i + 1 ) * 0.5 - 0.25 ) * Angle<>::pi ), ( j + 1 ) * 0.1 ) +
                                  Cartesian<>( 0.06, 0.07, 0.03 ),
                                 sDelta ) );
            TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 0.05 * sqrt( 2.0 ) * cos( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             0.05 * sqrt( 2.0 ) * sin( ( i * 0.5 - 0.25 ) * Angle<>::pi ),
                                                             ( j + 1 ) * 0.1 ) +
                                                 Cartesian<>( 0.06, 0.07, 0.03 ),
                                                sDelta ) );

            ++it;
        }


    double zLower, zUpper;
    const Geometry2D<> *geometry2DPtr = blockGeometry->GetInternalData( zLower, zUpper );
    TS_ASSERT_DELTA( zLower, 0.03, sDelta );
    TS_ASSERT_DELTA( zUpper, 0.23, sDelta );
    TS_ASSERT_DELTA( geometry2DPtr->CalculateAreaValue(), 0.01, sDelta );

    TwoDim<> lowerEdge, higherEdge;
    geometry2DPtr->Envelope( lowerEdge, higherEdge );
    TS_ASSERT_DELTA( lowerEdge.Get1(), 0.01, sDelta );
    TS_ASSERT_DELTA( lowerEdge.Get2(), 0.02, sDelta );
    TS_ASSERT_DELTA( higherEdge.Get1(), 0.11, sDelta );
    TS_ASSERT_DELTA( higherEdge.Get2(), 0.12, sDelta );

    TS_ASSERT_EQUALS( !strstr( blockGeometry->GetDescription(), "descriptionText" ), 0 );

    // test lateral areas
    vector< IndexedInnerArea< double > > innerAreas;
    block.GetInnerSurfaceAreas( innerAreas );
    TS_ASSERT_EQUALS( innerAreas.size(), 8 );
    TS_ASSERT_EQUALS( innerAreas.at( 0 ).mIndex, 4 );
    TS_ASSERT_EQUALS( innerAreas.at( 1 ).mIndex, 16 );
    TS_ASSERT_EQUALS( innerAreas.at( 2 ).mIndex, 5 );
    TS_ASSERT_EQUALS( innerAreas.at( 3 ).mIndex, 17 );
    TS_ASSERT_EQUALS( innerAreas.at( 4 ).mIndex, 6 );
    TS_ASSERT_EQUALS( innerAreas.at( 5 ).mIndex, 18 );
    TS_ASSERT_EQUALS( innerAreas.at( 6 ).mIndex, 7 );
    TS_ASSERT_EQUALS( innerAreas.at( 7 ).mIndex, 19 );

    vector< Cartesian< double > > areaVertices;
    BOOST_FOREACH ( const IndexedInnerArea< double > &indexedArea, innerAreas )
    {
        TS_ASSERT_DELTA( indexedArea.mArea.GetAreaValue(), 0.00565686, sDelta );
        indexedArea.mArea.GetVertices( areaVertices );
        BOOST_FOREACH ( const Cartesian< double > &vertex, areaVertices )
        {
            TS_ASSERT_DELTA( std::pow( ( vertex.GetX() - 0.01 - 0.05 ), 2.0 ) +
                              std::pow( ( vertex.GetY() - 0.02 - 0.05 ), 2.0 ) - std::pow( 0.04, 2.0 ),
                             0.0, sDelta );
        }
    }
}

void TestBlocks::TestHexagonalCellBlock()
{
    //#if defined(__EXCEPTIONS__)
    {
        std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
        thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
        Material<> mElectrolyte( 250.0, 1000.0, 50.0, 40.0, 20.0 );
        Material<> mRubber( 250.0, 1000.0, 50.0, 40.0, 20.0 );

        TS_ASSERT_THROWS_EQUALS( HexagonalCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.0, 0.04, 0.2, 1, 6, 1,
                                                             0.01, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(), "Length must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS(
         HexagonalCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.0, 0.2, 1, 6, 1, 0.01, 0, 0, 0.0, thermalStates ),
         std::runtime_error & e,
         !strstr( e.what(),
                  "RadiusCell must be bigger than zero and less than sqrt(3)/2*length, in order "
                  "that the cell fits into the block" ),
         0 );
        TS_ASSERT_THROWS_EQUALS( HexagonalCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.04, 0.0, 1, 6, 1,
                                                             0.01, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(), "Height must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( HexagonalCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.04, 0.2, 1, 6, 1,
                                                             0.0, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e,
                                 !strstr( e.what(), "ArcPolygonization must be bigger than zero" ), 0 );
        TS_ASSERT_THROWS_EQUALS( HexagonalCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.087, 0.2, 1, 6, 1,
                                                             0.01, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(),
                                                                  "RadiusCell must be bigger than zero and less than "
                                                                  "sqrt(3)/2*length, in order "
                                                                  "that the cell fits into the block" ),
                                 0 );
        TS_ASSERT_THROWS_EQUALS( HexagonalCellBlock<> block( "", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.04, 0.2, 1, 4, 1,
                                                             0.01, 0, 0, 0.0, thermalStates ),
                                 std::runtime_error & e, !strstr( e.what(), "PhiDivision must be divisible by six" ), 0 );
    }
    //#endif


    std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
    thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
    thermalStates.at( 0 )->AddPowerDissipation( 100.0, 0.0 );
    thermalStates.at( 0 )->ResetPowerDissipation();
    Material<> electrolyte( 250.0, 1000.0, 51.0, 41.0, 21.0 );
    Material<> rubber( 250.0, 1000.0, 55.0, 45.0, 25.0 );
    HexagonalCellBlock<> block( "descriptionText", Cartesian<>( 0.01, 0.02, 0.03 ), 0.1, 0.06, 0.2, 1, 6, 2, 0.01,
                                &electrolyte, &rubber, 25.0, thermalStates );

    const Cartesian<> centerOfBaseArea( 0.01 + 0.1, 0.02 + 0.1 * sqrt( 0.75 ), 0.03 );
    vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
    vector< vector< IndexedValue< double > > > conductivityMatrix;
    vector< IndexedArea< double > > surfaceAreas;
    boost::shared_ptr< BlockGeometry<> > blockGeometry;
    block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

    TS_ASSERT_EQUALS( block.GetNumberOfThermalElements(), 24 );
    TS_ASSERT_EQUALS( block.GetThermalStates(), thermalStates );
    TS_ASSERT( block.GetEmplacementPoint().Equals( Cartesian<>( 0.01, 0.02, 0.03 ), sDelta ) );
    TS_ASSERT_EQUALS( !strstr( block.GetDescription(), "descriptionText" ), 0 );


    TS_ASSERT_EQUALS( thermalElements.size(), 24 );
    TS_ASSERT_EQUALS( conductivityMatrix.size(), 24 );
    size_t maxIndex = 0;
    BOOST_FOREACH ( const IndexedArea< double > &area, surfaceAreas )
        if ( maxIndex < area.mIndex )
            maxIndex = area.mIndex;
    TS_ASSERT_LESS_THAN( maxIndex, 24 );


    for ( size_t i = 0; i < 24; ++i )
        TS_ASSERT_EQUALS( thermalElements.at( 0 )->GetTemperature(), 25.0 );

    for ( size_t i = 0; i < 6; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &electrolyte );
        TS_ASSERT_EQUALS( thermalElements.at( i + 12 )->GetMaterial(), &electrolyte );
    }
    for ( size_t i = 6; i < 12; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &rubber );
        TS_ASSERT_EQUALS( thermalElements.at( i + 12 )->GetMaterial(), &rubber );
    }

    for ( size_t i = 0; i < 6; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 2.0, 1.0 ), 100.0 / 12.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetPowerDissipation( 2.0, 1.0 ), 100.0 / 12.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), 0.06 * 0.06 * Angle<>::pi / 6.0 * 0.1, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetVolume(), 0.06 * 0.06 * Angle<>::pi / 6.0 * 0.1, sDelta );
    }
    for ( size_t i = 6; i < 12; ++i )
    {
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 2.0, 1.0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetPowerDissipation( 2.0, 1.0 ), 0.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(),
                         ( 0.1 * 0.1 * sqrt( 3.0 ) / 4.0 - 0.06 * 0.06 * Angle<>::pi / 6.0 ) * 0.1, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i + 12 )->GetVolume(),
                         ( 0.1 * 0.1 * sqrt( 3.0 ) / 4.0 - 0.06 * 0.06 * Angle<>::pi / 6.0 ) * 0.1, sDelta );
    }

    for ( size_t i = 0; i < 2; ++i )
    {
        for ( size_t j = 0; j < 6; ++j )
        {
            Cartesian<> compare( Cartesian<>( 0.03 * cos( Angle<>::pi / 3.0 * ( 0.5 + j ) ),
                                              0.03 * sin( Angle<>::pi / 3.0 * ( 0.5 + j ) ), 0.05 + 0.1 * i ) +
                                 centerOfBaseArea );
            TS_ASSERT( thermalElements.at( j + 12 * i )->GetGridVertex().Equals( compare, sDelta ) );
        }

        for ( size_t j = 0; j < 6; ++j )
        {
            TwoDim<> point1( 0.1, Angle<>::Deg( 60.0 * j ) );
            TwoDim<> point2( 0.1, Angle<>::Deg( 60.0 * ( j + 1 ) ) );
            TwoDim<> point3( 0.06, Angle<>::Deg( 60.0 * ( 0.5 + j ) ) );
            TwoDim<> vertex = TwoDim<>( point1 + point2 + point3 * 2.0 ) / 4.0;
            TS_ASSERT( thermalElements.at( 6 + j + 12 * i )->GetGridVertex().Equals(
             Cartesian<>( vertex.Get1(), vertex.Get2(), 0.05 + i * 0.1 ) + centerOfBaseArea, sDelta ) );
        }
    }


    TwoDim<> point1( 0.1, 0.0 );
    TwoDim<> point2( 0.1, Angle<>::Deg( 60.0 ) );
    TwoDim<> point3( 0.06, Angle<>::Deg( 30.0 ) );
    TwoDim<> vertex = TwoDim<>( point1 + point2 + point3 * 2.0 ) / 4.0;

    double conductivityRho = 0.06 * Angle<>::pi / 3.0 * 0.1 / ( 0.03 / 51.0 + ( vertex.Length() - 0.06 ) / 55.0 );
    double conductivityPhi1 = 41.0 * 0.06 * 0.1 / ( 0.03 * Angle<>::pi / 3.0 );
    double conductivityPhi2 =
     45.0 * sqrt( ( 0.1 - 0.06 ) * ( 0.1 * sqrt( 0.75 ) - 0.06 ) ) * 0.1 / ( vertex.Length() * Angle<>::pi / 3.0 );
    double conductivityZ1 = 21.0 * 0.06 * 0.06 * Angle<>::pi / 6.0 / 0.1;
    double conductivityZ2 = 25.0 * ( 0.1 * 0.1 * sqrt( 3.0 ) / 4.0 - 0.06 * 0.06 * Angle<>::pi / 6.0 ) / 0.1;

    vector< list< IndexedValue< double > > > checkConductivityMatrix( 24 );
    for ( size_t i = 0; i < 6; ++i )
    {
        size_t j = ( i + 1 ) % 6;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivityPhi1 ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivityPhi1 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( j + 12, conductivityPhi1 ) );
        checkConductivityMatrix.at( j + 12 ).push_back( MakeIndexedValue< double >( i + 12, conductivityPhi1 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 6, conductivityRho ) );
        checkConductivityMatrix.at( i + 6 ).push_back( MakeIndexedValue< double >( i, conductivityRho ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i + 18, conductivityRho ) );
        checkConductivityMatrix.at( i + 18 ).push_back( MakeIndexedValue< double >( i + 12, conductivityRho ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 12, conductivityZ1 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i, conductivityZ1 ) );
    }
    for ( size_t i = 6; i < 12; ++i )
    {
        size_t j = ( i + 1 ) % 6 + 6;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( j, conductivityPhi2 ) );
        checkConductivityMatrix.at( j ).push_back( MakeIndexedValue< double >( i, conductivityPhi2 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( j + 12, conductivityPhi2 ) );
        checkConductivityMatrix.at( j + 12 ).push_back( MakeIndexedValue< double >( i + 12, conductivityPhi2 ) );

        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i + 12, conductivityZ2 ) );
        checkConductivityMatrix.at( i + 12 ).push_back( MakeIndexedValue< double >( i, conductivityZ2 ) );
    }

    for ( size_t i = 0; i < 24; ++i )
    {
        double selfConductivity = 0;
        BOOST_FOREACH ( const IndexedValue< double > &conductivity, checkConductivityMatrix.at( i ) )
            selfConductivity -= conductivity.mValue;
        checkConductivityMatrix.at( i ).push_back( MakeIndexedValue< double >( i, selfConductivity ) );
        checkConductivityMatrix.at( i ).sort();

        TS_ASSERT_EQUALS( checkConductivityMatrix.at( i ).size(), conductivityMatrix.at( i ).size() );
        TS_ASSERT( boost::algorithm::is_permutation( conductivityMatrix.at( i ).begin(), conductivityMatrix.at( i ).end(),
                                                     checkConductivityMatrix.at( i ).begin(),
                                                     IndexedValuesEqual< double >( sDelta ) ) );
    }


    TS_ASSERT_EQUALS( surfaceAreas.size(), 12 + 12 + 12 );

    for ( size_t i = 0; i < 24; ++i )
    {
        TS_ASSERT_DELTA( surfaceAreas.at( i ).mArea.GetDistanceToGridVertex(), 0.05, sDelta );
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mIndex, i / 2 + ( i % 2 ) * 12 );
        if ( i < 12 )
        {
            TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetConductivity(), 21.0 );
        }
        else
        {
            TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetConductivity(), 25.0 );
        }
        if ( i % 2 == 0 )
        {
            TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetLocation(), BOTTOM );
        }
        else
        {
            TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetLocation(), TOP );
        }
    }
    for ( size_t i = 24; i < 30; ++i )
    {
        TS_ASSERT_DELTA( surfaceAreas.at( i ).mArea.GetDistanceToGridVertex(), 0.1 * sqrt( 0.75 ) - vertex.Length(), sDelta );
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mIndex, 6 + i % 6 );
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetConductivity(), 55.0 );
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetLocation(), SIDE );
    }
    for ( size_t i = 30; i < 36; ++i )
    {
        TS_ASSERT_DELTA( surfaceAreas.at( i ).mArea.GetDistanceToGridVertex(), 0.1 * sqrt( 0.75 ) - vertex.Length(), sDelta );
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mIndex, 18 + i % 6 );
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetConductivity(), 55.0 );
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mArea.GetLocation(), SIDE );
    }

    for ( size_t i = 0; i < 12; ++i )
    {
        vector< Cartesian<> > vertices;
        surfaceAreas.at( i ).mArea.GetVertices( vertices );
        TS_ASSERT_EQUALS( vertices.size(), 3 );
        TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.0, 0.0, i % 2 ) * 0.2 + centerOfBaseArea, sDelta ) );
        TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.06 * cos( ( i / 2 ) * Angle<>::pi / 3.0 ),
                                                         0.06 * sin( ( i / 2 ) * Angle<>::pi / 3.0 ), 0.2 * ( i % 2 ) ) +
                                             centerOfBaseArea,
                                            sDelta ) );
        TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 0.06 * cos( ( i / 2 + 1 ) * Angle<>::pi / 3.0 ),
                                                         0.06 * sin( ( i / 2 + 1 ) * Angle<>::pi / 3.0 ), 0.2 * ( i % 2 ) ) +
                                             centerOfBaseArea,
                                            sDelta ) );
    }
    for ( size_t i = 12; i < 24; ++i )
    {
        vector< Cartesian<> > vertices;
        surfaceAreas.at( i ).mArea.GetVertices( vertices );
        TS_ASSERT_EQUALS( vertices.size(), 4 );
        TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 0.06 * cos( ( i / 2 ) * Angle<>::pi / 3.0 ),
                                                         0.06 * sin( ( i / 2 ) * Angle<>::pi / 3.0 ), 0.2 * ( i % 2 ) ) +
                                             centerOfBaseArea,
                                            sDelta ) );
        TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.06 * cos( ( i / 2 + 1 ) * Angle<>::pi / 3.0 ),
                                                         0.06 * sin( ( i / 2 + 1 ) * Angle<>::pi / 3.0 ), 0.2 * ( i % 2 ) ) +
                                             centerOfBaseArea,
                                            sDelta ) );
        TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 0.1 * cos( ( i / 2 + 1 ) * Angle<>::pi / 3.0 ),
                                                         0.1 * sin( ( i / 2 + 1 ) * Angle<>::pi / 3.0 ), 0.2 * ( i % 2 ) ) +
                                             centerOfBaseArea,
                                            sDelta ) );
        TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 0.1 * cos( ( i / 2 ) * Angle<>::pi / 3.0 ),
                                                         0.1 * sin( ( i / 2 ) * Angle<>::pi / 3.0 ), 0.2 * ( i % 2 ) ) +
                                             centerOfBaseArea,
                                            sDelta ) );
    }
    for ( size_t j = 0; j < 2; ++j )
        for ( size_t i = 0; i < 6; ++i )
        {
            vector< Cartesian<> > vertices;
            surfaceAreas.at( 24 + i + 6 * j ).mArea.GetVertices( vertices );
            TS_ASSERT_EQUALS( vertices.size(), 4 );

            TS_ASSERT( vertices.at( 0 )
                        .Equals( Cartesian<>( 0.1 * cos( i * Angle<>::pi / 3.0 ), 0.1 * sin( i * Angle<>::pi / 3.0 ), 0.1 * j ) + centerOfBaseArea,
                                 sDelta ) );
            TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 0.1 * cos( ( i + 1 ) * Angle<>::pi / 3.0 ),
                                                             0.1 * sin( ( i + 1 ) * Angle<>::pi / 3.0 ), 0.1 * j ) +
                                                 centerOfBaseArea,
                                                sDelta ) );
            TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 0.1 * cos( ( i + 1 ) * Angle<>::pi / 3.0 ),
                                                             0.1 * sin( ( i + 1 ) * Angle<>::pi / 3.0 ), 0.1 * ( j + 1 ) ) +
                                                 centerOfBaseArea,
                                                sDelta ) );
            TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 0.1 * cos( i * Angle<>::pi / 3.0 ),
                                                             0.1 * sin( i * Angle<>::pi / 3.0 ), 0.1 * ( j + 1 ) ) +
                                                 centerOfBaseArea,
                                                sDelta ) );
        }


    double zLower, zUpper;
    const Geometry2D<> *geometry2DPtr = blockGeometry->GetInternalData( zLower, zUpper );
    TS_ASSERT_DELTA( zLower, 0.03, sDelta );
    TS_ASSERT_DELTA( zUpper, 0.23, sDelta );
    TS_ASSERT_DELTA( geometry2DPtr->CalculateAreaValue(), 1.5 * sqrt( 3.0 ) * 0.1 * 0.1, sDelta );

    vector< TwoDim<> > vertices, checkVertices;
    geometry2DPtr->GetPoints( vertices );
    for ( size_t i = 0; i < 6; ++i )
        checkVertices.push_back( TwoDim<>( 0.1, Angle<>::Rad( Angle<>::pi / 3.0 * i ) ) +
                                 TwoDim<>( centerOfBaseArea.GetX(), centerOfBaseArea.GetY() ) );

    TS_ASSERT_EQUALS( vertices.size(), checkVertices.size() );
    TS_ASSERT( boost::algorithm::is_permutation( vertices.begin(), vertices.end(), checkVertices.begin(), &TwoDimEqual ) );

    TS_ASSERT_EQUALS( !strstr( blockGeometry->GetDescription(), "descriptionText" ), 0 );
}

void TestBlocks::TestTriangularPrismBlock()
{
    //#if defined(__EXCEPTIONS__)
    {
        Material<> mMaterial( 250.0, 1000.0, 50.0, 40.0, 20.0 );

        TS_ASSERT_THROWS_EQUALS( TriangularPrismBlock<> block( "", Cartesian<>( 1.0, 2.0, 3.0 ), TwoDim<>( 0.0, 0.0 ),
                                                               TwoDim<>( 1.0, 0.0 ), TwoDim<>( 0.0, 1.0 ), 0.0, 2, &mMaterial, 25.0 ),
                                 std::runtime_error & e, !strstr( e.what(), "Height must be bigger than zero" ), 0 );
    }
    //#endif


    std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
    thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
    thermalStates.at( 0 )->AddPowerDissipation( 200.0, 0.0 );
    thermalStates.at( 0 )->ResetPowerDissipation();
    Material<> material( 250.0, 1000.0, 50.0, 60.0, 20.0 );
    TriangularPrismBlock<> block( "descriptionText", Cartesian<>( 1.0, 2.0, 3.0 ), TwoDim<>( 0.0, 0.0 ),
                                  TwoDim<>( 1.5, 0.0 ), TwoDim<>( 0.0, 1.5 ), 0.2, 2, &material, 25.0, thermalStates );


    vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
    vector< vector< IndexedValue< double > > > conductivityMatrix;
    vector< IndexedArea< double > > surfaceAreas;
    boost::shared_ptr< BlockGeometry<> > blockGeometry;
    block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

    TS_ASSERT_EQUALS( block.GetNumberOfThermalElements(), 2 );
    TS_ASSERT_EQUALS( block.GetThermalStates(), thermalStates );
    TS_ASSERT( block.GetEmplacementPoint().Equals( Cartesian<>( 1.0, 2.0, 3.0 ), sDelta ) );
    TS_ASSERT_EQUALS( !strstr( block.GetDescription(), "descriptionText" ), 0 );


    TS_ASSERT_EQUALS( thermalElements.size(), 2 );
    TS_ASSERT_EQUALS( conductivityMatrix.size(), 2 );
    size_t maxIndex = 0;
    BOOST_FOREACH ( const IndexedArea< double > &area, surfaceAreas )
        if ( maxIndex < area.mIndex )
            maxIndex = area.mIndex;
    TS_ASSERT_LESS_THAN( maxIndex, 2 );


    for ( size_t i = 0; i < 2; ++i )
    {
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetTemperature(), 25.0 );
        TS_ASSERT_EQUALS( thermalElements.at( i )->GetMaterial(), &material );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetPowerDissipation( 2.0, 1.0 ), 200.0 / 2.0, sDelta );
        TS_ASSERT_DELTA( thermalElements.at( i )->GetVolume(), 0.1 * 1.5 * 1.5 / 2.0, sDelta );
        TS_ASSERT( thermalElements.at( i )->GetGridVertex().Equals( Cartesian<>( 1.0, 2.0, 3.0 ) +
                                                                     Cartesian<>( 1.5 / 3.0, 1.5 / 3.0, 0.05 + 0.1 * i ),
                                                                    sDelta ) );
    }


    double conductivity = 20.0 * ( 1.5 * 1.5 / 2.0 ) / 0.1;

    vector< list< IndexedValue< double > > > checkConductivityMatrix( 24 );
    checkConductivityMatrix.at( 0 ).push_back( MakeIndexedValue< double >( 0, -conductivity ) );
    checkConductivityMatrix.at( 0 ).push_back( MakeIndexedValue< double >( 1, conductivity ) );
    checkConductivityMatrix.at( 1 ).push_back( MakeIndexedValue< double >( 0, conductivity ) );
    checkConductivityMatrix.at( 1 ).push_back( MakeIndexedValue< double >( 1, -conductivity ) );


    TS_ASSERT_EQUALS( conductivityMatrix.at( 0 ).size(), 2 );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 1 ).size(), 2 );
    TS_ASSERT( boost::algorithm::is_permutation( conductivityMatrix.at( 0 ).begin(), conductivityMatrix.at( 0 ).end(),
                                                 checkConductivityMatrix.at( 0 ).begin(), IndexedValuesEqual< double >( sDelta ) ) );
    TS_ASSERT( boost::algorithm::is_permutation( conductivityMatrix.at( 1 ).begin(), conductivityMatrix.at( 1 ).end(),
                                                 checkConductivityMatrix.at( 1 ).begin(), IndexedValuesEqual< double >( sDelta ) ) );


    TS_ASSERT_EQUALS( surfaceAreas.size(), 2 + 2 * 3 );

    for ( size_t i = 0; i < 2; ++i )
    {
        TS_ASSERT_EQUALS( surfaceAreas.at( i ).mIndex, i );
        TS_ASSERT_DELTA( surfaceAreas.at( i ).mArea.GetDistanceToGridVertex(), 0.05, sDelta );
        TS_ASSERT_DELTA( surfaceAreas.at( i ).mArea.GetConductivity(), 20.0, sDelta );

        vector< Cartesian<> > vertices;
        surfaceAreas.at( i ).mArea.GetVertices( vertices );
        TS_ASSERT_EQUALS( vertices.size(), 3 );
        TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.2 * i ), sDelta ) );
        TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.2 * i ) + Cartesian<>( 1.5, 0.0, 0.0 ), sDelta ) );
        TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.2 * i ) + Cartesian<>( 0.0, 1.5, 0.0 ), sDelta ) );
    }
    TS_ASSERT_EQUALS( surfaceAreas.at( 0 ).mArea.GetLocation(), BOTTOM );
    TS_ASSERT_EQUALS( surfaceAreas.at( 1 ).mArea.GetLocation(), TOP );


    for ( size_t i = 0; i < 2; ++i )
    {
        size_t index1 = 2 + i * 3;
        size_t index2 = 3 + i * 3;
        size_t index3 = 4 + i * 3;

        TS_ASSERT_EQUALS( surfaceAreas.at( index1 ).mIndex, i );
        TS_ASSERT_EQUALS( surfaceAreas.at( index2 ).mIndex, i );
        TS_ASSERT_EQUALS( surfaceAreas.at( index3 ).mIndex, i );

        TS_ASSERT_DELTA( surfaceAreas.at( index1 ).mArea.GetConductivity(), 60.0, sDelta );
        TS_ASSERT_DELTA( surfaceAreas.at( index2 ).mArea.GetConductivity(), 55.0, sDelta );
        TS_ASSERT_DELTA( surfaceAreas.at( index3 ).mArea.GetConductivity(), 50.0, sDelta );

        TS_ASSERT_DELTA( surfaceAreas.at( index1 ).mArea.GetDistanceToGridVertex(), 1.5 / 3.0, sDelta );
        TS_ASSERT_DELTA( surfaceAreas.at( index2 ).mArea.GetDistanceToGridVertex(), 0.5 / sqrt( 2.0 ), sDelta );
        TS_ASSERT_DELTA( surfaceAreas.at( index3 ).mArea.GetDistanceToGridVertex(), 1.5 / 3.0, sDelta );

        TS_ASSERT_EQUALS( surfaceAreas.at( index1 ).mArea.GetLocation(), SIDE );
        TS_ASSERT_EQUALS( surfaceAreas.at( index2 ).mArea.GetLocation(), SIDE );
        TS_ASSERT_EQUALS( surfaceAreas.at( index3 ).mArea.GetLocation(), SIDE );


        vector< Cartesian<> > vertices;

        surfaceAreas.at( index1 ).mArea.GetVertices( vertices );
        TS_ASSERT_EQUALS( vertices.size(), 4 );
        TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );
        TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 1.5, 0.0, 0.0 ), sDelta ) );
        TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 1.5, 0.0, 0.1 ), sDelta ) );
        TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 0.0, 0.0, 0.1 ), sDelta ) );

        surfaceAreas.at( index2 ).mArea.GetVertices( vertices );
        TS_ASSERT_EQUALS( vertices.size(), 4 );
        TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 1.5, 0.0, 0.0 ), sDelta ) );
        TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 0.0, 1.5, 0.0 ), sDelta ) );
        TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 0.0, 1.5, 0.1 ), sDelta ) );
        TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 1.5, 0.0, 0.1 ), sDelta ) );

        surfaceAreas.at( index3 ).mArea.GetVertices( vertices );
        TS_ASSERT_EQUALS( vertices.size(), 4 );
        TS_ASSERT( vertices.at( 0 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 0.0, 1.5, 0.0 ), sDelta ) );
        TS_ASSERT( vertices.at( 1 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 0.0, 0.0, 0.0 ), sDelta ) );
        TS_ASSERT( vertices.at( 2 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 0.0, 0.0, 0.1 ), sDelta ) );
        TS_ASSERT( vertices.at( 3 ).Equals( Cartesian<>( 1.0, 2.0, 3.0 + 0.1 * i ) + Cartesian<>( 0.0, 1.5, 0.1 ), sDelta ) );
    }


    double zLower, zUpper;
    const Geometry2D<> *geometry2DPtr = blockGeometry->GetInternalData( zLower, zUpper );
    TS_ASSERT_DELTA( zLower, 3.0, sDelta );
    TS_ASSERT_DELTA( zUpper, 3.2, sDelta );
    TS_ASSERT_DELTA( geometry2DPtr->CalculateAreaValue(), 1.5 * 1.5 / 2.0, sDelta );

    vector< TwoDim<> > vertices, checkVertices;
    geometry2DPtr->GetPoints( vertices );
    checkVertices.push_back( TwoDim<>( 1.0, 2.0 ) );
    checkVertices.push_back( TwoDim<>( 2.5, 2.0 ) );
    checkVertices.push_back( TwoDim<>( 1.0, 3.5 ) );

    TS_ASSERT_EQUALS( vertices.size(), checkVertices.size() );
    TS_ASSERT( boost::algorithm::is_permutation( vertices.begin(), vertices.end(), checkVertices.begin(), &TwoDimEqual ) );

    TS_ASSERT_EQUALS( !strstr( blockGeometry->GetDescription(), "descriptionText" ), 0 );
}

void TestBlocks::TestMultipleThermalStatesInThermalBlock()
{
    {
        std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 24 );
        BOOST_FOREACH ( boost::shared_ptr< ::state::ThermalState<> > &thermalState, thermalStates )
            thermalState.reset( new ::state::ThermalState<> );
        Material<> mMaterial( 250.0, 1000.0, 50.0, 40.0, 20.0 );

        RectangularBlock<> block( "descriptionText", Cartesian<>( 0.0, 0.0, 0.0 ), 0.8, 0.9, 0.6, 8, 9, 6, &mMaterial,
                                  25.0, thermalStates, 4, 3, 2 );

        vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix;
        vector< IndexedArea< double > > surfaceAreas;
        boost::shared_ptr< BlockGeometry<> > blockGeometry;
        block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

        for ( size_t i = 0; i < thermalElements.size(); ++i )
        {
            const geometry::Cartesian<> vertex = thermalElements.at( i )->GetGridVertex();
            const size_t index = static_cast< size_t >( vertex.GetX() / 0.2 ) + 4 * static_cast< size_t >( vertex.GetY() / 0.3 ) +
                                 4 * 3 * static_cast< size_t >( vertex.GetZ() / 0.3 );
            TS_ASSERT_EQUALS( thermalElements.at( i )->mThermalState, thermalStates.at( index ).get() );
        }
    }
    {
        std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 2 );
        thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 1 ).reset( new ::state::ThermalState<> );
        Material<> material( 250.0, 1000.0, 50.0, 50.0, 20.0 );

        TriangularPrismBlock<> block( "descriptionText", Cartesian<>( 0.0, 0.0, 0.0 ), TwoDim<>( 0.0, 0.0 ),
                                      TwoDim<>( 1.5, 0.0 ), TwoDim<>( 0.0, 1.5 ), 0.2, 4, &material, 25.0, thermalStates, 2 );

        vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix;
        vector< IndexedArea< double > > surfaceAreas;
        boost::shared_ptr< BlockGeometry<> > blockGeometry;
        block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

        for ( size_t i = 0; i < thermalElements.size(); ++i )
        {
            const geometry::Cartesian<> vertex = thermalElements.at( i )->GetGridVertex();
            const size_t index = static_cast< size_t >( vertex.GetZ() / 0.1 );
            TS_ASSERT_EQUALS( thermalElements.at( i )->mThermalState, thermalStates.at( index ).get() );
        }
    }
    {
        std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 4 );
        thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 1 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 2 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 3 ).reset( new ::state::ThermalState<> );
        Material<> innerCellMaterial( 250.0, 1000.0, 57.0, 47.0, 27.0 );
        Material<> outerCellMaterial( 250.0, 1000.0, 53.0, 43.0, 23.0 );
        Material<> rubber( 250.0, 1000.0, 50.0, 40.0, 20.0 );

        Supercap<> block( "descriptionText", Cartesian<>( -0.05, -0.05, 0.0 ), 0.1, 0.03, 0.04, 0.2, 4, 2, 0.01,
                          &innerCellMaterial, &outerCellMaterial, &rubber, 25.0, thermalStates, 2, 2 );

        vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix;
        vector< IndexedArea< double > > surfaceAreas;
        boost::shared_ptr< BlockGeometry<> > blockGeometry;
        block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

        for ( size_t i = 0; i < thermalElements.size(); ++i )
        {
            const geometry::Cartesian<> vertex = thermalElements.at( i )->GetGridVertex();
            double angle = geometry::TwoDim<>( vertex.GetX(), vertex.GetY() ).AngleInXY().GetDeg() + 45.0;
            if ( angle >= 360.0 )
                angle -= 360.0;
            else if ( angle < 0.0 )
                angle += 360.0;
            const size_t index = static_cast< size_t >( angle / 180.0 ) + 2 * static_cast< size_t >( vertex.GetZ() / 0.1 );
            if ( vertex.GetX() * vertex.GetX() + vertex.GetY() * vertex.GetY() < 0.03 * 0.03 )
            {
                TS_ASSERT_EQUALS( thermalElements.at( i )->mThermalState, thermalStates.at( index ).get() );
            }
            else
            {
                TS_ASSERT( thermalElements.at( i )->mThermalState == 0 );
            }
        }
    }
    {
        std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 4 );
        thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 1 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 2 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 3 ).reset( new ::state::ThermalState<> );
        Material<> cellMaterial( 250.0, 1000.0, 57.0, 47.0, 27.0 );
        Material<> rubber( 250.0, 1000.0, 50.0, 40.0, 20.0 );

        QuadraticCellBlock<> block( "descriptionText", Cartesian<>( -0.05, -0.05, 0.0 ), 0.1, 0.04, 0.2, 2, 4, 2, 0.01,
                                    &cellMaterial, &rubber, 25.0, thermalStates, 2, 2 );

        vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix;
        vector< IndexedArea< double > > surfaceAreas;
        boost::shared_ptr< BlockGeometry<> > blockGeometry;
        block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

        for ( size_t i = 0; i < thermalElements.size(); ++i )
        {
            const geometry::Cartesian<> vertex = thermalElements.at( i )->GetGridVertex();
            double angle = geometry::TwoDim<>( vertex.GetX(), vertex.GetY() ).AngleInXY().GetDeg() + 45.0;
            if ( angle >= 360.0 )
                angle -= 360.0;
            else if ( angle < 0.0 )
                angle += 360.0;
            const size_t index = static_cast< size_t >( angle / 180.0 ) + 2 * static_cast< size_t >( vertex.GetZ() / 0.1 );
            if ( vertex.GetX() * vertex.GetX() + vertex.GetY() * vertex.GetY() < 0.04 * 0.04 )
            {
                TS_ASSERT_EQUALS( thermalElements.at( i )->mThermalState, thermalStates.at( index ).get() );
            }
            else
            {
                TS_ASSERT( thermalElements.at( i )->mThermalState == 0 );
            }
        }
    }
    {
        std::vector< boost::shared_ptr< ::state::ThermalState<> > > thermalStates( 6 );
        thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 1 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 2 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 3 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 4 ).reset( new ::state::ThermalState<> );
        thermalStates.at( 5 ).reset( new ::state::ThermalState<> );
        Material<> cellMaterial( 250.0, 1000.0, 57.0, 47.0, 27.0 );
        Material<> rubber( 250.0, 1000.0, 50.0, 40.0, 20.0 );

        HexagonalCellBlock<> block( "descriptionText", Cartesian<>( -0.1, -0.1 * sqrt( 0.75 ), 0.0 ), 0.1, 0.04, 0.2, 2,
                                    6, 2, 0.01, &cellMaterial, &rubber, 25.0, thermalStates, 3, 2 );

        vector< boost::shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix;
        vector< IndexedArea< double > > surfaceAreas;
        boost::shared_ptr< BlockGeometry<> > blockGeometry;
        block.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

        for ( size_t i = 0; i < thermalElements.size(); ++i )
        {
            const geometry::Cartesian<> vertex = thermalElements.at( i )->GetGridVertex();
            double angle = geometry::TwoDim<>( vertex.GetX(), vertex.GetY() ).AngleInXY().GetDeg();
            if ( angle >= 360.0 )
                angle -= 360.0;
            else if ( angle < 0.0 )
                angle += 360.0;
            const size_t index = static_cast< size_t >( angle / 120.0 ) + 3 * static_cast< size_t >( vertex.GetZ() / 0.1 );
            if ( vertex.GetX() * vertex.GetX() + vertex.GetY() * vertex.GetY() < 0.04 * 0.04 )
            {
                TS_ASSERT_EQUALS( thermalElements.at( i )->mThermalState, thermalStates.at( index ).get() );
            }
            else
            {
                TS_ASSERT( thermalElements.at( i )->mThermalState == 0 );
            }
        }
    }
}
