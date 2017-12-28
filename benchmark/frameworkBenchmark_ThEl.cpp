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
#include <cstring>
#include "../src/thermal/thermal_simulation.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "../src/misc/StrCont.h"
#include "../src/misc/matrixInclude.h"
#include "../src/thermal/electrical_simulation.h"
#include "../src/factory/observer/createObserver.h"
#include "../src/misc/fast_copy_matrix.h"

#include <fstream>
#include <string>

// BOOST
#include "boost/date_time.hpp"
#include <boost/numeric/odeint.hpp>


// forward declarations
const double thermal::globalMaxValue = 1000000000.0;
const double thermal::globalMinValue = -thermal::globalMaxValue;

// Also adjustable in xml-file
const double socStopCriterion = 5.0;             // percent
const double thermalStateStopCriterion = 5.0;    // degree C


int main( int argc, char *argv[] )
{
    if ( argc != 5 )
    {
        printf(
         "Error, must be called with exactly 4 parameters:\nxml-file\ncurrent value\nstep time\ncycle count time\n" );
        return EXIT_FAILURE;
    }


    // Parameter 1
    boost::scoped_ptr< xmlparser::XmlParser > parser;
    try
    {
        parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromFile( argv[1] );
    }
    catch ( std::exception &e )
    {
        printf( "At 1. parameter: xml-file\n" );
        printf( "%s\n", e.what() );
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        printf( "At 1. parameter: xml-file\n" );
        printf( "Unidentified error\n" );
        return EXIT_FAILURE;
    }

    // Parameter 2
    double current;
    {
        std::stringstream ss( argv[2] );
        ss >> current;
        current *= -1.0;
        if ( ss.fail() )
        {
            printf( "At 2. parameter: current\n" );
            printf( "Error: Could not be read in as double\n" );
            return EXIT_FAILURE;
        }
    }

    // Parameter 3
    double stepTime;
    {
        std::stringstream ss( argv[3] );
        ss >> stepTime;
        if ( ss.fail() )
        {
            printf( "At 3. parameter: step time\n" );
            printf( "Error: Could not be read in as double\n" );
            return EXIT_FAILURE;
        }
    }

    // Paramter 4
    size_t cycleCount;
    {
        std::stringstream ss( argv[4] );
        ss >> cycleCount;
        if ( ss.fail() )
        {
            printf( "At 4. parameter: cycle count\n" );
            printf( "Error: Could not be read in as unsigned int\n" );
            return EXIT_FAILURE;
        }
    }

    typedef boost::shared_ptr< electrical::TwoPort< myMatrixType > > TwoPort_t;


    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double > > electricalSimulation;
    boost::scoped_ptr< simulation::ThermalSimulation< myMatrixType, double, true > > thermalSimulation;
    boost::scoped_ptr< observer::Observer< myMatrixType, electrical::TwoPort > > electricalObserver;
    boost::scoped_ptr< observer::ThermalObserver< double > > thermalVisualizer;

    try
    {
        boost::posix_time::time_duration creationDuration;
        boost::posix_time::ptime creationStart = boost::posix_time::microsec_clock::local_time();

        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double >( rootXmlNode, stepTime, stepTime * cycleCount,
                                                                       socStopCriterion, &cells ) );
        electricalObserver.reset( CreateTwoPortObserver< std::vector< TwoPort_t >, myMatrixType, true >( cells, rootXmlNode.get() ) );
        electricalObserver->AddFilter( new observer::CsvFilterTwoPort< myMatrixType >( "electricalStates.csv" ) );

        std::vector< std::vector< boost::shared_ptr< ThermalState< double > > > > thermalStatesOfCellBlocks;
        thermalSimulation.reset( new simulation::ThermalSimulation< myMatrixType, double, true >(
         rootXmlNode, stepTime, stepTime * cycleCount, thermalStateStopCriterion, &thermalVisualizer,
         &electricalSimulation->mThermalStates, &thermalStatesOfCellBlocks ) );

        parser.reset();

        creationDuration = boost::posix_time::microsec_clock::local_time() - creationStart;
        std::cout << "Creating equation systems took "
                  << static_cast< double >( creationDuration.total_microseconds() ) / 1000000.0 << " sec" << std::endl;
    }
    catch ( std::exception &e )
    {
        const char *message = e.what();
        printf( "Error while creating the equation systems\n" );
        printf( "%s", message );
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        printf( "Unknown error while creating the equation systems\n" );
        return EXIT_FAILURE;
    }


    // try
    //{
    // Output conductivity matrix
    std::ofstream fileConduction( "Conductivity.csv" );
    BOOST_FOREACH ( const std::vector< thermal::IndexedValue< double > > &line,
                    thermalSimulation->mThermalSystem->GetA_th_Conductivity() )
    {
        BOOST_FOREACH ( const thermal::IndexedValue< double > &indexedValue, line )
            fileConduction << indexedValue.mIndex << ", " << indexedValue.mValue << "; ";
        fileConduction << std::endl;
    }

    // Output finite volumes coordinates
    ofstream AllGridVerticesXYZCoordinates( "AllGridVerticesXYZCoordinates.csv" );
    BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalElement<> > &elem,
                    thermalSimulation->mThermalSystem->GetThermalElements() )
    {
        AllGridVerticesXYZCoordinates << elem->GetGridVertex().GetX() << ", " << elem->GetGridVertex().GetY() << ", "
                                      << elem->GetGridVertex().GetZ() << std::endl;
    }
    ofstream AllGridVerticesTemperatures( "AllGridVerticesTemperatures.csv" );

    // Equation solvers
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > > >::type stepperThermal =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > >() );
#if defined( _ARMADILLO_ ) && not defined( SPARSE_MATRIX_FORMAT )
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType > >::type stepperElectrical =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType >() );
#else
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > > >::type stepperElectrical =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > >() );
    std::vector< double > tmpStateVector( electricalSimulation->mStateSystemGroup.mStateVector.n_rows, 0.0 );
#endif

    boost::posix_time::ptime electricalStart;
    boost::posix_time::time_duration electricalDuration;
    boost::posix_time::ptime thermalStart;
    boost::posix_time::time_duration thermalDuration;
    double sumOfThermalDuration = 0.0;
    double sumOfElectricalDuration = 0.0;
    boost::posix_time::ptime start =
     boost::posix_time::microsec_clock::local_time();    // second temp var for global time spent


    // Run simulation
    size_t timeCounter = 0;
    double currentChangeTime = 0.0;
    ( *thermalVisualizer )( thermalSimulation->mTime );
    while ( electricalSimulation->CheckIfSumlationTimeHasNotEndedAndSetStepStartTime() )
    {
        // Reset Current at every cycle
        current *= -1.0;
        currentChangeTime += stepTime;

        electricalStart = boost::posix_time::microsec_clock::local_time();


        // Electrical simulation
        electricalSimulation->UpdateSystem();
        // Initialize block Observer
        electricalSimulation->UpdateSystemValues();
        electricalSimulation->ResetAllThermalStatesPowerDissipation();
        electricalSimulation->InitializeStopCriterion();
        while ( electricalSimulation->CheckLoopConditionAndSetDeltaTime( currentChangeTime ) )
        {
            // Handle stop criterion
            if ( electricalSimulation->IsStopCriterionFulfilled() )
            {
                printf( "SoC stop criterion triggered at %.0f sec\n", electricalSimulation->mTime );
                break;
            }

#if defined( _ARMADILLO_ ) && !defined( SPARSE_MATRIX_FORMAT )
            // Run electrical equation solver
            electricalSimulation->mStateSystemGroup.mDt = electricalSimulation->mDeltaTime;
            while ( stepperElectrical.try_step( boost::ref( *electricalSimulation->mEqSystem ),
                                                electricalSimulation->mStateSystemGroup.mStateVector, electricalSimulation->mTime,
                                                electricalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
            {
                electricalSimulation->mStateSystemGroup.mDt = electricalSimulation->mDeltaTime;
            }
#else
            // Run electrical equation solver
            misc::FastCopyMatrix( &tmpStateVector[0], electricalSimulation->mStateSystemGroup.mStateVector, tmpStateVector.size() );
            electricalSimulation->mStateSystemGroup.mDt = electricalSimulation->mDeltaTime;
            while ( stepperElectrical.try_step( boost::ref( *electricalSimulation->mEqSystem ), tmpStateVector,
                                                electricalSimulation->mTime,
                                                electricalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
            {
                electricalSimulation->mStateSystemGroup.mDt = electricalSimulation->mDeltaTime;
            }
            misc::FastCopyMatrix( electricalSimulation->mStateSystemGroup.mStateVector, &tmpStateVector[0], tmpStateVector.size() );
#endif

            electricalSimulation->UpdateSystemValues();
            electricalSimulation->UpdateAllThermalStatesPowerDissipation();
            //(*electricalObserver)(electricalSimulation->mTime);
            electricalSimulation->SaveStatesForLaterReset();
        }


        electricalDuration = boost::posix_time::microsec_clock::local_time() - electricalStart;
        double elDur = electricalDuration.total_microseconds();
        sumOfElectricalDuration += elDur;
        std::cout << "electrical:" << elDur / 1000.0 << " msec, ";
        thermalStart = boost::posix_time::microsec_clock::local_time();


        // Thermal simulation
        thermalSimulation->InitializeStopCriterion();
        while ( thermalSimulation->CheckLoopConditionAndSetDeltaTime( electricalSimulation->mTime ) )
        {
            // Handle stop criterion
            if ( thermalSimulation->IsStopCriterionFulfilled() )
            {
                printf( "Temperature stop criterion triggered at %.0f sec, electrical states reset\n", thermalSimulation->mTime );
                electricalSimulation->ResetStatesToPointOfTime( thermalSimulation->mTime );
                break;
            }

            // Run thermal equation solver
            while ( stepperThermal.try_step( boost::ref( *thermalSimulation->mThermalSystem ),
                                             thermalSimulation->mTemperatures, thermalSimulation->mTime,
                                             thermalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
            {
            }

            thermalSimulation->UpdateSystem();

            if ( thermalSimulation->mTime >= electricalSimulation->mSimulationDuration / 1000.0 * timeCounter )
            {
                // Write simulation data to file
                ++timeCounter;
                ( *thermalVisualizer )( thermalSimulation->mTime );

                // Output finite volumes temperatures
                AllGridVerticesTemperatures << thermalSimulation->mTime;
                BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalElement<> > &elem,
                                thermalSimulation->mThermalSystem->GetThermalElements() )
                {
                    AllGridVerticesTemperatures << ", " << elem->GetTemperature();
                }
                AllGridVerticesTemperatures << std::endl;

                // Output simulation progress to console during program is running
                printf( "%.0f, ", electricalSimulation->mTime );
            }

            thermalSimulation->UpdateAllThermalStatesTemperatures();
        }

        thermalDuration = boost::posix_time::microsec_clock::local_time() - thermalStart;
        double thDur = thermalDuration.total_microseconds();
        sumOfThermalDuration += thDur;
        std::cout << "thermal:" << thDur / 1000.0 << " msec" << endl;

        // Finish step
        electricalSimulation->FinshStep();
    }
    //}
    // catch(std::exception &e)
    //{
    //    const char *message = e.what();
    //    printf("Error while running simulation\n");
    //    printf(message);
    //    return EXIT_FAILURE;
    //}
    // catch( ... )
    //{
    //    printf("Unknown error while running simulation\n");
    //    return EXIT_FAILURE;
    //}


    boost::posix_time::time_duration elapsed = boost::posix_time::microsec_clock::local_time() - start;
    double elapsedDur = elapsed.total_microseconds();
    double electricalPart = sumOfElectricalDuration / elapsedDur;
    double thermalPart = sumOfThermalDuration / elapsedDur;
    std::cout << "Total time elapsed:" << elapsedDur / 10000000.0 << " sec" << endl;
    std::cout << "Thermal simulation run time:" << thermalPart * 100.0 << " %" << endl;
    std::cout << "Electrical simulation run time:" << electricalPart * 100.0 << " %" << endl;

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
