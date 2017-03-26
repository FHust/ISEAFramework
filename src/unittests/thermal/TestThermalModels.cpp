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
#include "TestThermalModels.h"
#include "exception_tester.h"

using namespace thermal;
static const double sDelta = 0.0001;


bool TaylorDataAreaEqual( const TaylorData< double > &area1, const TaylorData< double > &area2 )
{
    return fabs( area1.mA_cool - area2.mA_cool ) < sDelta;
}

bool TaylorDataConducitivityEqual( const TaylorData< double > &area1, const TaylorData< double > &area2 )
{
    return area1.mConductivity == area2.mConductivity;
}

bool TaylorDataDistanceEqual( const TaylorData< double > &area1, const TaylorData< double > &area2 )
{
    return fabs( area1.mDistanceToGridVertex - area2.mDistanceToGridVertex ) < sDelta;
}

bool TaylorDataLocationEqual( const TaylorData< double > &area1, const TaylorData< double > &area2 )
{
    return area1.mLocation == area2.mLocation;
}

bool TaylorDataCharEqual( const TaylorData< double > &area1, const TaylorData< double > &area2 )
{
    return fabs( area1.mCharacteristicLength - area2.mCharacteristicLength ) < sDelta;
}

bool TaylorDataCoolingEqual( const TaylorData< double > &area1, const TaylorData< double > &area2 )
{
    return area1.mCooling == area2.mCooling;
}

bool TaylorDataAllEqual( const TaylorData< double > &area1, const TaylorData< double > &area2 )
{
    return fabs( area1.mA_cool - area2.mA_cool ) < sDelta && area1.mConductivity == area2.mConductivity &&
           fabs( area1.mDistanceToGridVertex - area2.mDistanceToGridVertex ) < sDelta && area1.mLocation == area2.mLocation &&
           fabs( area1.mCharacteristicLength - area2.mCharacteristicLength ) < sDelta && area1.mCooling == area2.mCooling;
}

void TestThermalModels::TestThermalModel()
{
    //#if defined(__EXCEPTIONS__)
    {
        ThermalModel<> thermalModel( Tolerance<>( 0.0, Angle<>::Deg( 0.0 ), 0.0 ), ThermalModel<>::AGGREGATE_BY_PLANE_AND_BLOCKS );
        vector< shared_ptr< ThermalElement<> > > thermalElements( 1 );
        vector< vector< IndexedValue< double > > > conductivityMatrix( 2 );
        vector< IndexedArea< double > > surfaceAreas;
        shared_ptr< BlockGeometry<> > blockGeometry;
        TS_ASSERT_THROWS_EQUALS( thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry ),
                                 std::runtime_error & e,
                                 strcmp( e.what(), "size() of thermalElements and conductivityMatrix must be equal." ), 0 );
        conductivityMatrix.pop_back();
        surfaceAreas.push_back(
         IndexedArea< double >( 2, Area<>( vector< TwoDim<> >( 2 ),
                                           Plane<>( Cartesian<>(), UnitVector<>( 1.0, 0.0, 0.0 ), UnitVector<>( 0.0, 1.0, 0.0 ) ),
                                           0.0, 0.0, TOP ) ) );
        TS_ASSERT_THROWS_EQUALS(
         thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry ), std::out_of_range & e,
         strcmp( e.what(), "Biggest mIndex in surfaceAreas must be smaller than size() of thermalElements." ), 0 );


        vector< vector< TaylorData< double > > > dataVector;
        vector< vector< TaylorData< double > > > dirichletDataVector;
        ThermalModel<> thermalModel2( Tolerance<>( 0.001, Angle<>::Deg( 0.1 ), 1.0 ), ThermalModel<>::AGGREGATE_BY_PLANE_AND_BLOCKS );

        vector< TwoDim<> > vertices( 3 );
        vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
        vertices.at( 1 ) = TwoDim<>( 1.0, 0.0 );
        vertices.at( 2 ) = TwoDim<>( 0.5, 0.000001 );
        blockGeometry.reset( new BlockGeometry<>( Geometry2D<>( vertices ), 0.0, 0.00001, "Block1" ) );
        thermalModel2.mBlocks.push_back( ThermalModel<>::BlockData() );
        thermalModel2.mBlocks.back().mBlockGeometry = blockGeometry;

        TS_ASSERT_THROWS_EQUALS( thermalModel2.CreateDataByFusingBlocks( thermalElements, conductivityMatrix, dataVector, dirichletDataVector ),
                                 std::runtime_error & e,
                                 strcmp( e.what(), "Dimension of thermal block is smaller than tolerance in Block1." ), 0 );


        ThermalModel<> thermalModel3( Tolerance<>( 0.001, Angle<>::Deg( 0.1 ), 1.0 ), ThermalModel<>::AGGREGATE_BY_PLANE_AND_BLOCKS );

        vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
        vertices.at( 1 ) = TwoDim<>( 1.0, 0.0 );
        vertices.at( 2 ) = TwoDim<>( 0.5, 1.0 );
        blockGeometry.reset( new BlockGeometry<>( Geometry2D<>( vertices ), 0.0, 1.0, "Block2" ) );
        thermalModel3.mBlocks.push_back( ThermalModel<>::BlockData() );
        thermalModel3.mBlocks.back().mBlockGeometry = blockGeometry;
        thermalModel3.mBlocks.push_back( ThermalModel<>::BlockData() );
        thermalModel3.mBlocks.back().mBlockGeometry = blockGeometry;

        TS_ASSERT_THROWS_EQUALS( thermalModel3.CreateDataByFusingBlocks( thermalElements, conductivityMatrix, dataVector, dirichletDataVector ),
                                 std::runtime_error & e,
                                 strcmp( e.what(), "Collision between thermal block Block2 and thermal block Block2." ), 0 );
    }
    //#endif


    vector< shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
    thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
    thermalStates.at( 0 )->AddPowerDissipation( 20000.0, 0.0 );
    Material<> material( 250.0, 1000.0, 47.0, 50.0, 53.0 );
    /*
         z                 __
        |          Block1:/__/|      __
        |                /__/|/  <> /__/|:Block2
        |______          |__|/      |__|/
       /       y           ^
      /                 BlockTop and BlockMidlle
     x                  are under and over (^)

    */
    RectangularBlock<> block1( "AtOrigin", Cartesian<>( 0.0, 0.0, 0.0 ), 0.4, 0.2, 0.2, 2, 1, 1, &material, 27.0 );
    RectangularBlock<> block2( "0.2 in Y", Cartesian<>( 0.05, 0.2, 0.05 ), 0.2, 0.1, 0.1, 1, 1, 1, &material, 27.0, thermalStates );
    RectangularBlock<> blockBottom( "Under the middle thermal element", Cartesian<>( 0.3, 0.1, -0.08 ), 0.2, 0.15, 0.08,
                                    1, 1, 1, &material, 27.0 );
    RectangularBlock<> blockTop( "Over the most middle thermal element", Cartesian<>( 0.3, 0.1, 0.2 ), 0.2, 0.15, 0.08,
                                 1, 1, 1, &material, 27.0 );
    shared_ptr< Cooling<> > cooling( new CoolingByConstantValue<>( 25.6 ) );
    vector< TwoDim<> > vertices( 3 );
    vertices.at( 0 ) = TwoDim<>( 0.0, 0.0 );
    vertices.at( 1 ) = TwoDim<>( 0.2, 0.0 );
    vertices.at( 2 ) = TwoDim<>( 0.2, -1.0 );
    CoolingPrismatic<> coolingBlock( "CoolingDescription", vertices, 0.0, 1.0, cooling );
    shared_ptr< Cooling<> > dirichlet( new DirichletBoundaryCondition<>( 10.0 ) );
    vertices.resize( 4 );
    vertices.at( 0 ) = TwoDim<>( 0.3, 0.1 );
    vertices.at( 1 ) = TwoDim<>( 0.5, 0.1 );
    vertices.at( 2 ) = TwoDim<>( 0.5, 0.25 );
    vertices.at( 3 ) = TwoDim<>( 0.3, 0.25 );
    CoolingPrismatic<> dirichletBlock( "DirichletDescription", vertices, 0.28, 0.38, dirichlet );

    ThermalModel<> thermalModel( Tolerance<>( 0.000001, Angle<>::Deg( 0.001 ), 0.1 ), ThermalModel<>::AGGREGATE_BY_PLANE_AND_BLOCKS );

    vector< shared_ptr< ThermalElement<> > > thermalElements;
    vector< vector< IndexedValue< double > > > conductivityMatrix;
    vector< IndexedArea< double > > surfaceAreas;
    shared_ptr< BlockGeometry<> > blockGeometry;
    vector< Area<> > coolingAreas;

    thermalModel.ClearAndSetNumberOfBlocksAndCoolings( 4, 1 );
    TS_ASSERT_EQUALS( thermalModel.mBlocks.capacity(), 4 );
    TS_ASSERT_EQUALS( thermalModel.mCoolings.capacity(), 1 );

    block1.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
    thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
    block2.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
    thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
    blockBottom.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
    thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
    blockTop.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
    thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

    coolingBlock.GetCoolingAreas( coolingAreas );
    coolingBlock.GetBlockGeometry( blockGeometry );
    shared_ptr< Cooling<> > moveCooling = coolingBlock.GetCooling();
    thermalModel.AddCooling( coolingAreas, blockGeometry, moveCooling );
    moveCooling = dirichletBlock.GetCooling();
    dirichletBlock.GetCoolingAreas( coolingAreas );
    dirichletBlock.GetBlockGeometry( blockGeometry );
    thermalModel.AddCooling( coolingAreas, blockGeometry, moveCooling );


    vector< vector< TaylorData< double > > > dataVector;
    vector< vector< TaylorData< double > > > dirichletDataVector;
    TS_ASSERT_THROWS_NOTHING(
     thermalModel.CreateDataByFusingBlocks( thermalElements, conductivityMatrix, dataVector, dirichletDataVector ) );


    TS_ASSERT_EQUALS( thermalElements.size(), 5 );
    TS_ASSERT_EQUALS( conductivityMatrix.size(), 5 );


    for ( size_t i = 0; i < conductivityMatrix.size(); ++i )
    {
        double positive = 0, negative = 0;
        bool selfConductivityFound = false, selfConductivityFoundMoreThanOnce = false;

        BOOST_FOREACH ( const IndexedValue< double > &conductivity, conductivityMatrix.at( i ) )
        {
            if ( conductivity.mValue > 0.0 )
            {
                positive += conductivity.mValue;
            }
            else
            {
                if ( selfConductivityFound )
                {
                    selfConductivityFoundMoreThanOnce = true;
                    break;
                }
                else
                {
                    selfConductivityFound = true;
                    negative += -conductivity.mValue;
                }
            }
        }

        TS_ASSERT_EQUALS( selfConductivityFoundMoreThanOnce, false );
        TS_ASSERT( selfConductivityFound );
        TS_ASSERT_DELTA( negative, positive, sDelta );
    }

    TS_ASSERT_EQUALS( conductivityMatrix.at( 0 ).size(), 3 );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 0 ).at( 1 ).mIndex, 1 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 0 ).at( 1 ).mValue, 47.0 * 0.2, sDelta );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 0 ).at( 2 ).mIndex, 2 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 0 ).at( 2 ).mValue, 50.0 * 0.1 * 0.15 / ( 0.5 * 0.2 + 0.5 * 0.1 ), sDelta );

    TS_ASSERT_EQUALS( conductivityMatrix.at( 1 ).size(), 5 );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 1 ).at( 0 ).mIndex, 0 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 1 ).at( 0 ).mValue, 47.0 * 0.2, sDelta );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 1 ).at( 2 ).mIndex, 2 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 1 ).at( 2 ).mValue, 50.0 * 0.1 * 0.05 / ( 0.5 * 0.2 + 0.5 * 0.1 ), sDelta );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 1 ).at( 3 ).mIndex, 3 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 1 ).at( 3 ).mValue, 53.0 * 0.1 * 0.1 / ( 0.5 * 0.2 + 0.5 * 0.08 ), sDelta );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 1 ).at( 4 ).mIndex, 4 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 1 ).at( 4 ).mValue, 53.0 * 0.1 * 0.1 / ( 0.5 * 0.2 + 0.5 * 0.08 ), sDelta );

    TS_ASSERT_EQUALS( conductivityMatrix.at( 2 ).size(), 3 );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 2 ).at( 0 ).mIndex, 0 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 2 ).at( 0 ).mValue, 50.0 * 0.1 * 0.15 / ( 0.5 * 0.2 + 0.5 * 0.1 ), sDelta );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 2 ).at( 1 ).mIndex, 1 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 2 ).at( 1 ).mValue, 50.0 * 0.1 * 0.05 / ( 0.5 * 0.2 + 0.5 * 0.1 ), sDelta );

    TS_ASSERT_EQUALS( conductivityMatrix.at( 3 ).size(), 2 );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 3 ).at( 0 ).mIndex, 1 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 3 ).at( 0 ).mValue, 53.0 * 0.1 * 0.1 / ( 0.5 * 0.2 + 0.5 * 0.08 ), sDelta );

    TS_ASSERT_EQUALS( conductivityMatrix.at( 4 ).size(), 2 );
    TS_ASSERT_EQUALS( conductivityMatrix.at( 4 ).at( 0 ).mIndex, 1 );
    TS_ASSERT_DELTA( conductivityMatrix.at( 4 ).at( 0 ).mValue, 53.0 * 0.1 * 0.1 / ( 0.5 * 0.2 + 0.5 * 0.08 ), sDelta );


    TS_ASSERT_EQUALS( dataVector.size(), 5 );
    vector< list< TaylorData< double > > > checkVector( 5 );

    TaylorData< double > data;
    data.mA_cool = 0.04;
    data.mLocation = TOP;
    data.mConductivity = 53.0;
    data.mDistanceToGridVertex = 0.1;
    data.mCharacteristicLength = 0.5 * 0.2 * 0.4 / ( 0.2 + 0.4 );
    data.mCooling = 0;

    // Element 0 & 1 uncut areas and areas cut by blockBottom and blocktop
    checkVector.at( 0 ).push_back( data );
    checkVector.at( 1 ).push_back( data );
    checkVector.at( 1 ).back().mA_cool = 0.04 - 0.01;

    data.mLocation = BOTTOM;
    checkVector.at( 0 ).push_back( data );
    checkVector.at( 1 ).push_back( data );
    checkVector.at( 1 ).back().mA_cool = 0.04 - 0.01;

    data.mLocation = SIDE;
    data.mCharacteristicLength = 0.2;
    data.mConductivity = 47.0;
    checkVector.at( 0 ).push_back( data );
    checkVector.at( 1 ).push_back( data );
    data.mConductivity = 50.0;
    checkVector.at( 1 ).push_back( data );

    // Element 0 cooled area
    data.mCooling = cooling.get();
    data.mCharacteristicLength = -1.0;
    checkVector.at( 0 ).push_back( data );

    // Element 2 uncut areas
    data.mCooling = 0;
    data.mLocation = SIDE;
    data.mCharacteristicLength = 0.1;
    data.mA_cool = 0.02;
    data.mConductivity = 50.0;
    data.mDistanceToGridVertex = 0.05;
    checkVector.at( 2 ).push_back( data );

    data.mLocation = TOP;
    data.mCharacteristicLength = 0.5 * 0.1 * 0.2 / ( 0.1 + 0.2 );
    data.mConductivity = 53.0;
    checkVector.at( 2 ).push_back( data );

    data.mLocation = BOTTOM;
    checkVector.at( 2 ).push_back( data );

    data.mLocation = SIDE;
    data.mCharacteristicLength = 0.1;
    data.mA_cool = 0.01;
    data.mConductivity = 47.0;
    data.mDistanceToGridVertex = 0.1;
    checkVector.at( 2 ).push_back( data );
    checkVector.at( 2 ).push_back( data );

    // Element 0 & 1 areas cut by block 2
    data.mA_cool = 0.04 - 0.1 * 0.15;
    data.mLocation = SIDE;
    data.mCharacteristicLength = 0.2;
    data.mConductivity = 50.0;
    data.mDistanceToGridVertex = 0.1;
    checkVector.at( 0 ).push_back( data );

    data.mA_cool = 0.04 - 0.1 * 0.05;
    checkVector.at( 1 ).push_back( data );

    // Element 3 & 4 areas
    data.mA_cool = 0.15 * 0.08;
    data.mLocation = SIDE;
    data.mConductivity = 47.0;
    data.mDistanceToGridVertex = 0.1;
    data.mCharacteristicLength = 0.08;
    checkVector.at( 3 ).push_back( data );
    checkVector.at( 3 ).push_back( data );
    checkVector.at( 4 ).push_back( data );
    checkVector.at( 4 ).push_back( data );

    data.mA_cool = 0.2 * 0.08;
    data.mConductivity = 50.0;
    data.mDistanceToGridVertex = 0.075;
    checkVector.at( 3 ).push_back( data );
    checkVector.at( 3 ).push_back( data );
    checkVector.at( 4 ).push_back( data );
    checkVector.at( 4 ).push_back( data );

    data.mA_cool = 0.2 * 0.15;
    data.mLocation = BOTTOM;
    data.mConductivity = 53.0;
    data.mDistanceToGridVertex = 0.04;
    data.mCharacteristicLength = 0.5 * 0.2 * 0.15 / ( 0.2 + 0.15 );
    checkVector.at( 3 ).push_back( data );
    checkVector.at( 4 ).push_back( data );
    checkVector.at( 4 ).back().mA_cool = 0.03 - 0.01;

    data.mLocation = TOP;
    checkVector.at( 3 ).push_back( data );
    checkVector.at( 3 ).back().mA_cool = 0.03 - 0.01;

    for ( size_t i = 0; i < dataVector.size(); ++i )
    {
        TS_ASSERT_EQUALS( dataVector.at( i ).size(), checkVector.at( i ).size() );

        if ( dataVector.at( i ).size() == checkVector.at( i ).size() )
        {
            TS_ASSERT( boost::algorithm::is_permutation( dataVector.at( i ).begin(), dataVector.at( i ).end(),
                                                         checkVector.at( i ).begin(), TaylorDataAreaEqual ) );
            TS_ASSERT( boost::algorithm::is_permutation( dataVector.at( i ).begin(), dataVector.at( i ).end(),
                                                         checkVector.at( i ).begin(), TaylorDataConducitivityEqual ) );
            TS_ASSERT( boost::algorithm::is_permutation( dataVector.at( i ).begin(), dataVector.at( i ).end(),
                                                         checkVector.at( i ).begin(), TaylorDataDistanceEqual ) );
            TS_ASSERT( boost::algorithm::is_permutation( dataVector.at( i ).begin(), dataVector.at( i ).end(),
                                                         checkVector.at( i ).begin(), TaylorDataLocationEqual ) );
            TS_ASSERT( boost::algorithm::is_permutation( dataVector.at( i ).begin(), dataVector.at( i ).end(),
                                                         checkVector.at( i ).begin(), TaylorDataCharEqual ) );
            TS_ASSERT( boost::algorithm::is_permutation( dataVector.at( i ).begin(), dataVector.at( i ).end(),
                                                         checkVector.at( i ).begin(), TaylorDataCoolingEqual ) );

            TS_ASSERT( boost::algorithm::is_permutation( dataVector.at( i ).begin(), dataVector.at( i ).end(),
                                                         checkVector.at( i ).begin(), TaylorDataAllEqual ) );
        }
    }


    TS_ASSERT_EQUALS( dirichletDataVector.size(), 5 );
    for ( size_t i = 0; i < dirichletDataVector.size(); ++i )
        if ( i == 4 )
        {
            TS_ASSERT_EQUALS( dirichletDataVector.at( i ).size(), 1 );
        }
        else
        {
            TS_ASSERT_EQUALS( dirichletDataVector.at( i ).size(), 0 );
        }
    TS_ASSERT_DELTA( dirichletDataVector.at( 4 ).at( 0 ).mA_cool, 0.2 * 0.15, sDelta );
    TS_ASSERT_EQUALS( dirichletDataVector.at( 4 ).at( 0 ).mConductivity, 53.0 );
    TS_ASSERT_DELTA( dirichletDataVector.at( 4 ).at( 0 ).mDistanceToGridVertex, 0.04, sDelta );
    TS_ASSERT_EQUALS( dirichletDataVector.at( 4 ).at( 0 ).mLocation, TOP );
    TS_ASSERT_DELTA( dirichletDataVector.at( 4 ).at( 0 ).mCharacteristicLength, -1.0, sDelta );
    TS_ASSERT_EQUALS( dirichletDataVector.at( 4 ).at( 0 ).mCooling, dirichlet.get() );
}

void TestThermalModels::TestCharacteristicLengthCalculation()
{
    Material<> material( 250.0, 1000.0, 50.0, 50.0, 50.0 );
    RectangularBlock<> block0( "AtOrigin", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.1, 0.1, 1, 1, 1, &material, 27.0 );
    RectangularBlock<> blockX( "Next to block0 in x-direction", Cartesian<>( 0.1, 0.0, 0.0 ), 0.1, 0.1, 0.1, 1, 1, 1,
                               &material, 27.0 );
    RectangularBlock<> blockXX( "Distant to block0 in x-direction", Cartesian<>( 0.2, 0.0, 0.0 ), 0.1, 0.1, 0.1, 1, 1,
                                1, &material, 27.0 );
    RectangularBlock<> blockZ( "Next to block0 in z-direction", Cartesian<>( 0.0, 0.0, 0.1 ), 0.1, 0.1, 0.1, 1, 1, 1,
                               &material, 27.0 );
    RectangularBlock<> blockZZ( "Distant to block0 in z-direction", Cartesian<>( 0.0, 0.0, 0.2 ), 0.1, 0.1, 0.1, 1, 1,
                                1, &material, 27.0 );


    {
        vector< shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix;
        vector< IndexedArea< double > > surfaceAreas;
        shared_ptr< BlockGeometry<> > blockGeometry;
        vector< Area<> > coolingAreas;
        vector< vector< TaylorData< double > > > coolingDataVector;
        vector< vector< TaylorData< double > > > dirichletDataVector;

        ThermalModel<> thermalModel( Tolerance<>( 0.000001, Angle<>::Deg( 0.001 ), 0.1 ), ThermalModel<>::AGGREGATE_BY_PLANE );
        thermalModel.ClearAndSetNumberOfBlocksAndCoolings( 3, 0 );

        block0.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        blockXX.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        blockZZ.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

        TS_ASSERT_THROWS_NOTHING( thermalModel.CreateDataByFusingBlocks( thermalElements, conductivityMatrix,
                                                                         coolingDataVector, dirichletDataVector ) );
        TS_ASSERT_EQUALS( coolingDataVector.size(), 3 );


        vector< vector< TaylorData< double > > > checkVector( 3 );
        TaylorData< double > data;
        data.mA_cool = 0.01;
        data.mConductivity = 50.0;
        data.mDistanceToGridVertex = 0.05;
        data.mCooling = 0;

        data.mLocation = TOP;
        // Single area
        data.mCharacteristicLength = 0.5 * 0.1 * 0.1 / ( 0.1 + 0.1 );
        checkVector.at( 2 ).push_back( data );
        // Aggregated areas
        data.mCharacteristicLength = 0.5 * 0.1 * 0.3 / ( 0.1 + 0.3 );
        checkVector.at( 0 ).push_back( data );
        checkVector.at( 1 ).push_back( data );

        data.mLocation = BOTTOM;
        // Single area
        data.mCharacteristicLength = 0.5 * 0.1 * 0.1 / ( 0.1 + 0.1 );
        checkVector.at( 2 ).push_back( data );
        // Aggregated areas
        data.mCharacteristicLength = 0.5 * 0.1 * 0.3 / ( 0.1 + 0.3 );
        checkVector.at( 0 ).push_back( data );
        checkVector.at( 1 ).push_back( data );

        data.mLocation = SIDE;
        // Single area
        data.mCharacteristicLength = 0.1;
        for ( size_t i = 0; i < 2; ++i )
            checkVector.at( 1 ).push_back( data );
        // Aggregated areas
        data.mCharacteristicLength = 0.3;
        for ( size_t i = 0; i < 2; ++i )
            checkVector.at( 1 ).push_back( data );
        for ( size_t i = 0; i < 4; ++i )
        {
            checkVector.at( 0 ).push_back( data );
            checkVector.at( 2 ).push_back( data );
        }


        for ( size_t i = 0; i < coolingDataVector.size(); ++i )
        {
            TS_ASSERT_EQUALS( coolingDataVector.at( i ).size(), checkVector.at( i ).size() );
            if ( coolingDataVector.at( i ).size() == checkVector.at( i ).size() )
            {
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataAreaEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataConducitivityEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataDistanceEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataLocationEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataCharEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataCoolingEqual ) );

                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataAllEqual ) );
            }
        }
    }


    {
        vector< shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix;
        vector< IndexedArea< double > > surfaceAreas;
        shared_ptr< BlockGeometry<> > blockGeometry;
        vector< Area<> > coolingAreas;
        vector< vector< TaylorData< double > > > coolingDataVector;
        vector< vector< TaylorData< double > > > dirichletDataVector;

        ThermalModel<> thermalModel( Tolerance<>( 0.000001, Angle<>::Deg( 0.001 ), 0.1 ), ThermalModel<>::AGGREGATE_BY_PLANE );
        thermalModel.ClearAndSetNumberOfBlocksAndCoolings( 3, 0 );

        block0.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        blockX.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        blockZ.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

        TS_ASSERT_THROWS_NOTHING( thermalModel.CreateDataByFusingBlocks( thermalElements, conductivityMatrix,
                                                                         coolingDataVector, dirichletDataVector ) );
        TS_ASSERT_EQUALS( coolingDataVector.size(), 3 );


        vector< vector< TaylorData< double > > > checkVector( 3 );
        TaylorData< double > data;
        data.mA_cool = 0.01;
        data.mConductivity = 50.0;
        data.mDistanceToGridVertex = 0.05;
        data.mCooling = 0;

        data.mLocation = TOP;
        data.mCharacteristicLength = 0.5 * 0.1 * 0.1 / ( 0.1 + 0.1 );
        checkVector.at( 1 ).push_back( data );
        checkVector.at( 2 ).push_back( data );

        data.mLocation = BOTTOM;
        data.mCharacteristicLength = 0.5 * 0.2 * 0.1 / ( 0.2 + 0.1 );
        checkVector.at( 0 ).push_back( data );
        checkVector.at( 1 ).push_back( data );

        data.mLocation = SIDE;
        data.mCharacteristicLength = 0.1;
        checkVector.at( 1 ).push_back( data );
        checkVector.at( 2 ).push_back( data );
        data.mCharacteristicLength = 0.2;
        checkVector.at( 1 ).push_back( data );
        checkVector.at( 1 ).push_back( data );
        for ( size_t i = 0; i < 3; ++i )
        {
            checkVector.at( 0 ).push_back( data );
            checkVector.at( 2 ).push_back( data );
        }
        // checkVector.at(1).push_back(data);
        // checkVector.at(1).push_back(data);


        for ( size_t i = 0; i < coolingDataVector.size(); ++i )
        {
            TS_ASSERT_EQUALS( coolingDataVector.at( i ).size(), checkVector.at( i ).size() );
            if ( coolingDataVector.at( i ).size() == checkVector.at( i ).size() )
            {
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataAreaEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataConducitivityEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataDistanceEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataLocationEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataCharEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataCoolingEqual ) );

                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataAllEqual ) );
            }
        }
    }
}

void TestThermalModels::TestCharacteristicLengthCalculationAdvancedAggregation()
{
    Material<> material( 250.0, 1000.0, 50.0, 50.0, 50.0 );
    RectangularBlock<> block0( "AtOrigin", Cartesian<>( 0.0, 0.0, 0.0 ), 0.1, 0.1, 0.1, 1, 1, 1, &material, 27.0 );
    RectangularBlock<> blockX( "Next to block0 in x-direction", Cartesian<>( 0.1, 0.0, 0.0 ), 0.1, 0.1, 0.1, 1, 1, 1,
                               &material, 27.0 );
    RectangularBlock<> blockXX( "Distant to block0 in x-direction", Cartesian<>( 0.2, 0.0, 0.0 ), 0.1, 0.1, 0.1, 1, 1,
                                1, &material, 27.0 );
    RectangularBlock<> blockZ( "Next to block0 in z-direction", Cartesian<>( 0.0, 0.0, 0.1 ), 0.1, 0.1, 0.1, 1, 1, 1,
                               &material, 27.0 );
    RectangularBlock<> blockZZ( "Distant to block0 in z-direction", Cartesian<>( 0.0, 0.0, 0.2 ), 0.1, 0.1, 0.1, 1, 1,
                                1, &material, 27.0 );


    {
        vector< shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix;
        vector< IndexedArea< double > > surfaceAreas;
        shared_ptr< BlockGeometry<> > blockGeometry;
        vector< Area<> > coolingAreas;
        vector< vector< TaylorData< double > > > coolingDataVector;
        vector< vector< TaylorData< double > > > dirichletDataVector;

        ThermalModel<> thermalModel( Tolerance<>( 0.000001, Angle<>::Deg( 0.001 ), 0.1 ), ThermalModel<>::AGGREGATE_BY_PLANE_AND_BLOCKS );
        thermalModel.ClearAndSetNumberOfBlocksAndCoolings( 3, 0 );

        block0.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        blockXX.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        blockZZ.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

        TS_ASSERT_THROWS_NOTHING( thermalModel.CreateDataByFusingBlocks( thermalElements, conductivityMatrix,
                                                                         coolingDataVector, dirichletDataVector ) );
        TS_ASSERT_EQUALS( coolingDataVector.size(), 3 );


        vector< vector< TaylorData< double > > > checkVector( 3 );
        TaylorData< double > data;
        data.mA_cool = 0.01;
        data.mConductivity = 50.0;
        data.mDistanceToGridVertex = 0.05;
        data.mCooling = 0;

        data.mLocation = TOP;
        data.mCharacteristicLength = 0.5 * 0.1 * 0.1 / ( 0.1 + 0.1 );
        checkVector.at( 0 ).push_back( data );
        checkVector.at( 1 ).push_back( data );
        checkVector.at( 2 ).push_back( data );

        data.mLocation = BOTTOM;
        data.mCharacteristicLength = 0.5 * 0.1 * 0.1 / ( 0.1 + 0.1 );
        checkVector.at( 0 ).push_back( data );
        checkVector.at( 1 ).push_back( data );
        checkVector.at( 2 ).push_back( data );

        data.mLocation = SIDE;
        data.mCharacteristicLength = 0.1;
        for ( size_t i = 0; i < 4; ++i )
        {
            checkVector.at( 0 ).push_back( data );
            checkVector.at( 1 ).push_back( data );
            checkVector.at( 2 ).push_back( data );
        }


        for ( size_t i = 0; i < coolingDataVector.size(); ++i )
        {
            TS_ASSERT_EQUALS( coolingDataVector.at( i ).size(), checkVector.at( i ).size() );
            if ( coolingDataVector.at( i ).size() == checkVector.at( i ).size() )
            {
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataAreaEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataConducitivityEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataDistanceEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataLocationEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataCharEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataCoolingEqual ) );

                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataAllEqual ) );
            }
        }
    }


    {
        vector< shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix;
        vector< IndexedArea< double > > surfaceAreas;
        shared_ptr< BlockGeometry<> > blockGeometry;
        vector< Area<> > coolingAreas;
        vector< vector< TaylorData< double > > > coolingDataVector;
        vector< vector< TaylorData< double > > > dirichletDataVector;

        ThermalModel<> thermalModel( Tolerance<>( 0.000001, Angle<>::Deg( 0.001 ), 0.1 ), ThermalModel<>::AGGREGATE_BY_PLANE_AND_BLOCKS );
        thermalModel.ClearAndSetNumberOfBlocksAndCoolings( 3, 0 );

        block0.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        blockX.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        blockZ.CreateData( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceAreas, blockGeometry );

        TS_ASSERT_THROWS_NOTHING( thermalModel.CreateDataByFusingBlocks( thermalElements, conductivityMatrix,
                                                                         coolingDataVector, dirichletDataVector ) );
        TS_ASSERT_EQUALS( coolingDataVector.size(), 3 );


        vector< vector< TaylorData< double > > > checkVector( 3 );
        TaylorData< double > data;
        data.mA_cool = 0.01;
        data.mConductivity = 50.0;
        data.mDistanceToGridVertex = 0.05;
        data.mCooling = 0;

        data.mLocation = TOP;
        data.mCharacteristicLength = 0.5 * 0.1 * 0.1 / ( 0.1 + 0.1 );
        checkVector.at( 1 ).push_back( data );
        checkVector.at( 2 ).push_back( data );

        data.mLocation = BOTTOM;
        data.mCharacteristicLength = 0.5 * 0.2 * 0.1 / ( 0.2 + 0.1 );
        checkVector.at( 0 ).push_back( data );
        checkVector.at( 1 ).push_back( data );

        data.mLocation = SIDE;
        data.mCharacteristicLength = 0.1;
        checkVector.at( 1 ).push_back( data );
        checkVector.at( 1 ).push_back( data );
        checkVector.at( 1 ).push_back( data );
        checkVector.at( 2 ).push_back( data );
        data.mCharacteristicLength = 0.2;
        for ( size_t i = 0; i < 3; ++i )
        {
            checkVector.at( 0 ).push_back( data );
            checkVector.at( 2 ).push_back( data );
        }
        // checkVector.at(1).push_back(data);
        // checkVector.at(1).push_back(data);


        for ( size_t i = 0; i < coolingDataVector.size(); ++i )
        {
            TS_ASSERT_EQUALS( coolingDataVector.at( i ).size(), checkVector.at( i ).size() );
            if ( coolingDataVector.at( i ).size() == checkVector.at( i ).size() )
            {
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataAreaEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataConducitivityEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataDistanceEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataLocationEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataCharEqual ) );
                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataCoolingEqual ) );

                TS_ASSERT( boost::algorithm::is_permutation( coolingDataVector.at( i ).begin(), coolingDataVector.at( i ).end(),
                                                             checkVector.at( i ).begin(), TaylorDataAllEqual ) );
            }
        }
    }
}
