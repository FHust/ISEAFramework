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
#include "TestOdeSystemThermal.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4018 )
#endif
#include <boost/numeric/odeint.hpp>
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#include <boost/scoped_ptr.hpp>
#include <boost/ref.hpp>
#include <cmath>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include "../../thermal/ode_system_thermal.h"
#include "../../thermal/blocks/rectangular_block.h"
#include "../../thermal/thermal_model.h"
#include "../../observer/thermal_observer.h"
#include "../../observer/filter/csvfilter.h"
#include "../../thermal/block_observer.h"
#include "exception_tester.h"


using namespace thermal;
static const double sDelta = 0.000001;


void TestOdeSystemThermal::TestOdeSystem2RectangularBlocks()
{
    //#if defined(__EXCEPTIONS__)
    {
        vector< shared_ptr< DefaultConvection<> > > convection;
        shared_ptr< Radiation<> > radiation;
        vector< shared_ptr< ThermalElement<> > > thermalElements;
        vector< vector< IndexedValue< double > > > conductivityMatrix( 1 );
        vector< vector< TaylorData< double > > > coolingDataVector;
        vector< vector< TaylorData< double > > > dirichletDataVector( 1 );
        vector< shared_ptr< ::state::ThermalState< double > > > thermalStates;

        TS_ASSERT_THROWS_EQUALS( OdeSystemThermal<> system( thermalElements, conductivityMatrix, coolingDataVector,
                                                            dirichletDataVector, convection, radiation, 25.0, thermalStates ),
                                 std::runtime_error & e,
                                 strcmp( e.what(),
                                         "size() of thermalElements, a_th_Conductivity, coolingDataVector and "
                                         "dirchletDataVector must be equal." ),
                                 0 );
        /*
                TS_ASSERT_THROWS_EQUALS(OdeSystemThermal<> system(thermalElements, conductivityMatrix,
           coolingDataVector, dirichletDataVector, convection, radiation, 25.0, thermalStates), const std::runtime_error
           &e,
                                        CmpInit(e, "size() of thermalElements, a_th_Conductivity, coolingDataVector and
           dirchletDataVector must be equal", "OdeSystemThermal"), 0);
        */
        conductivityMatrix.clear();
        coolingDataVector.resize( 1 );

        TS_ASSERT_THROWS_EQUALS( OdeSystemThermal<> system( thermalElements, conductivityMatrix, coolingDataVector,
                                                            dirichletDataVector, convection, radiation, 25.0, thermalStates ),
                                 std::runtime_error & e,
                                 strcmp( e.what(),
                                         "size() of thermalElements, a_th_Conductivity, coolingDataVector and "
                                         "dirchletDataVector must be equal." ),
                                 0 );
        /*
                TS_ASSERT_THROWS_EQUALS(OdeSystemThermal<> system(thermalElements, conductivityMatrix,
           coolingDataVector, dirichletDataVector, convection, radiation, 25.0, thermalStates), const std::runtime_error
           &e,
                                        CmpInit(e, "size() of thermalElements, a_th_Conductivity, coolingDataVector and
           dirchletDataVector must be equal", "OdeSystemThermal"), 0);
        */
        dirichletDataVector.clear();
        conductivityMatrix.resize( 1 );

        TS_ASSERT_THROWS_EQUALS( OdeSystemThermal<> system( thermalElements, conductivityMatrix, coolingDataVector,
                                                            dirichletDataVector, convection, radiation, 25.0, thermalStates ),
                                 std::runtime_error & e,
                                 strcmp( e.what(),
                                         "size() of thermalElements, a_th_Conductivity, coolingDataVector and "
                                         "dirchletDataVector must be equal." ),
                                 0 );
        /*
                TS_ASSERT_THROWS_EQUALS(OdeSystemThermal<> system(thermalElements, conductivityMatrix,
           coolingDataVector, dirichletDataVector, convection, radiation, 25.0, thermalStates), const std::runtime_error
           &e,
                                        CmpInit(e, "size() of thermalElements, a_th_Conductivity, coolingDataVector and
           dirchletDataVector must be equal", "OdeSystemThermal"), 0);
        */
    }
    //#endif
    {
        vector< shared_ptr< ThermalElement<> > > emptyThermalElements;
        vector< vector< IndexedValue< double > > > emptyConductivityMatrix;
        vector< vector< TaylorData< double > > > emptyCoolingDataVector;
        vector< vector< TaylorData< double > > > emptyDirichletDataVector;
        vector< shared_ptr< DefaultConvection<> > > emptyDefualtConvection;
        shared_ptr< Radiation<> > radiation;


        vector< shared_ptr< ::state::ThermalState<> > > thermalStates = vector< shared_ptr< ::state::ThermalState<> > >( 1 );
        thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
        vector< shared_ptr< ::state::ThermalState<> > > unconnectedThermalStates =
         vector< shared_ptr< ::state::ThermalState<> > >( 1 );
        unconnectedThermalStates.at( 0 ).reset( new ::state::ThermalState<> );
        vector< shared_ptr< Material<> > > materials = vector< shared_ptr< Material<> > >( 1 );
        materials.at( 0 ).reset( new Material<>( 1.0, 1.0, 1.0, 1.0, 1.0 ) );
        vector< shared_ptr< Cooling<> > > coolings = vector< shared_ptr< Cooling<> > >( 1 );
        coolings.at( 0 ).reset( new Radiation<> );

        OdeSystemThermal<> testPreservingData( emptyThermalElements, emptyConductivityMatrix, emptyCoolingDataVector,
                                               emptyDirichletDataVector, emptyDefualtConvection, radiation, 25.0,
                                               thermalStates, unconnectedThermalStates, materials, coolings );

        TS_ASSERT( testPreservingData.mThermalStates == thermalStates );
        TS_ASSERT( testPreservingData.mUnconnectedThermalStates == unconnectedThermalStates );
        TS_ASSERT( testPreservingData.mMaterials == materials );
        TS_ASSERT( testPreservingData.mCoolings == coolings );
    }

    vector< shared_ptr< ::state::ThermalState<> > > thermalStates( 1 );
    thermalStates.at( 0 ).reset( new ::state::ThermalState<> );
    thermalStates.at( 0 )->AddPowerDissipation( 1000.0, 0.0 );
    Material<> material( 250.0, 1000.0, 50.0, 50.0, 50.0 );
    RectangularBlock<> block1( "descriptionText", Cartesian<>( 0.0, 0.0, 0.0 ), 0.8, 0.6, 0.2, 4, 3, 2, &material, 27.0 );
    RectangularBlock<> block2( "descriptionText", Cartesian<>( 0.1, 0.1, 0.2 ), 0.4, 0.2, 0.2, 2, 1, 1, &material, 27.0, thermalStates );


    ThermalModel<> thermalModel( Tolerance<>( 0.000001, Angle<>::Deg( 0.001 ), 0.1 ), ThermalModel<>::AGGREGATE_BY_PLANE_AND_BLOCKS );

    vector< shared_ptr< ThermalElement<> > > thermalElements;
    vector< vector< IndexedValue< double > > > conductivityMatrix;
    vector< IndexedArea< double > > surfaceElements;
    shared_ptr< BlockGeometry<> > blockGeometry;

    block1.CreateData( thermalElements, conductivityMatrix, surfaceElements, blockGeometry );
    thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceElements, blockGeometry );

    block2.CreateData( thermalElements, conductivityMatrix, surfaceElements, blockGeometry );
    thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceElements, blockGeometry );

    vector< vector< TaylorData< double > > > coolingDataVector;
    vector< vector< TaylorData< double > > > dirichletDataVector;
    thermalModel.CreateDataByFusingBlocks( thermalElements, conductivityMatrix, coolingDataVector, dirichletDataVector );


    vector< shared_ptr< DefaultConvection<> > > convection( 3 );
    convection.at( TOP ) = shared_ptr< DefaultConvection<> >( new ConvectionByFormula<>( 0.71 ) );
    convection.at( SIDE ) = shared_ptr< DefaultConvection<> >( new ConvectionByFormula<>( 0.548 ) );
    shared_ptr< Radiation<> > radiation( new Radiation<> );


    thermalStates = block1.GetThermalStates();
    thermalStates.insert( thermalStates.end(), block2.GetThermalStates().begin(), block2.GetThermalStates().end() );


    OdeSystemThermal<> system( thermalElements, conductivityMatrix, coolingDataVector, dirichletDataVector, convection,
                               radiation, 23.0, thermalStates );

    vector< double > tempVec;
    system.GetTemperatureVector( tempVec );
    system.SetTemperatureVector( tempVec );
    system.GetTemperatureVector( tempVec );
    TS_ASSERT_DELTA( system.mAirTemperature, 23.0, sDelta );
    system.ResetAirTemperature( 25.0 );
    TS_ASSERT_DELTA( system.mAirTemperature, 25.0, sDelta );


    TS_ASSERT_EQUALS( system.GetAirTemperature(), system.mAirTemperature );
    TS_ASSERT_EQUALS( &system.GetThermalElementFactors(), &system.mThermalElementFactors );
    TS_ASSERT_EQUALS( &system.GetConvection(), &system.mConvection );
    TS_ASSERT_EQUALS( &system.GetRadiation(), &system.mRadiation );
    TS_ASSERT_EQUALS( &system.GetMatrixDirichlet(), &system.mMatrixDirichlet );
    TS_ASSERT_EQUALS( &system.GetCoolingDataVector(), &system.mCoolingDataVector );


    double t = 0.000;
    double dt = 0.1;


    vector< ThermalElement< double > * > thermalElementsOfAreas;
    vector< vector< size_t > > areas;
    vector< vector< size_t > > volumes;
    vector< misc::StrCont > volumeNames;
    vector< Cartesian< double > > vertices;
    thermalModel.CreateDataForVisualization( thermalElementsOfAreas, areas, volumes, volumeNames, vertices );
    observer::ThermalObserver< double > thermaVisualizer( thermalElementsOfAreas, areas, volumes, volumeNames, vertices );
    thermaVisualizer.AddFilter( new observer::CsvFilterThermal< double >( "./Patch_Temperatures.csv", true ) );
    thermaVisualizer( 0.0 );
#ifndef __NO_STRING__
    std::ifstream fileVertices( "./Patch_Vertices.csv" );
    std::ifstream fileAreas( "./Patch_Areas.csv" );
    std::ifstream fileTemperatures( "./Patch_Temperatures.csv" );
    std::ifstream fileVerticesSave( "./Patch_Vertices_Save.csv" );
    std::ifstream fileAreasSave( "./Patch_Areas_Save.csv" );
    std::ifstream fileTemperaturesSave( "./Patch_Temperatures_Save.csv" );

    std::istreambuf_iterator< char > itEnd;
    std::string stringVertices( std::istreambuf_iterator< char >( fileVertices ), itEnd );
    std::string stringAreas( std::istreambuf_iterator< char >( fileAreas ), itEnd );
    std::string stringTemperatures( std::istreambuf_iterator< char >( fileTemperatures ), itEnd );
    std::string stringVerticesSave( std::istreambuf_iterator< char >( fileVerticesSave ), itEnd );
    std::string stringAreasSave( std::istreambuf_iterator< char >( fileAreasSave ), itEnd );
    std::string stringTemperaturesSave( std::istreambuf_iterator< char >( fileTemperaturesSave ), itEnd );

#if not defined( _MSC_VER ) && not defined( __MINGW32__ )
    std::string::iterator itNewEnd;
    itNewEnd = std::remove( stringVerticesSave.begin(), stringVerticesSave.end(), '\r' );
    stringVerticesSave.assign( stringVerticesSave.begin(), itNewEnd );
    itNewEnd = std::remove( stringAreasSave.begin(), stringAreasSave.end(), '\r' );
    stringAreasSave.assign( stringAreasSave.begin(), itNewEnd );
    itNewEnd = std::remove( stringTemperaturesSave.begin(), stringTemperaturesSave.end(), '\r' );
    stringTemperaturesSave.assign( stringTemperaturesSave.begin(), itNewEnd );
#else
#endif

    TS_ASSERT_EQUALS( stringVertices, stringVerticesSave );
    TS_ASSERT_EQUALS( stringAreas, stringAreasSave );
    TS_ASSERT_EQUALS( stringTemperatures, stringTemperaturesSave );
#endif

    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > > >::type stepper =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > >() );

    /*
    z=1:    ---------------
            | 20 21 22 23 |
            | 16 17 18 19 |
            | 12 13 14 15 |
            ---------------

    z=0:    ---------------
            |  8  9 10 11 |
            |  4  5  6  7 |
            |  0  1  2  3 |
            ---------------
    Heat production in 17
    Coordinates:
    17       1,1,1
    5        1,1,0
    13,16    1,0,1; 0,1,1
    1,4      1,0,0; 0,1,0
    6        2,1,0
    */


    while ( t < 00000.0 )
    {
        if ( stepper.try_step( boost::ref( system ), tempVec, t, dt ) == boost::numeric::odeint::success )
        {
            system.ResetAirTemperature( 25.0 + t * 0.0001 );
            system.Update( t, dt );
            thermaVisualizer( t );
            system.SetTemperatureVector( tempVec );
        }
    }
}
