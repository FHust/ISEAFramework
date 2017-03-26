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

#include "../src/container/matio_file.h"
#include "../src/misc/fast_copy_matrix.h"
#include "../src/misc/matrixInclude.h"
#include "../src/misc/StrCont.h"
#include "../src/thermal/electrical_simulation.h"
#include "../src/thermal/block_observer.h"
#include "../src/thermal/thermal_simulation.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"

#include <fstream>
#include <string>

// BOOST
#include <boost/numeric/odeint.hpp>


// forward declarations
const double thermal::globalMaxValue = 1000000000.0;
const double thermal::globalMinValue = -thermal::globalMaxValue;

// Also adjustable in xml-file
const double socStopCriterion = 5.0;             // percent
const double thermalStateStopCriterion = 5.0;    // degree C


int main( int argc, char *argv[] )
{
    if ( argc != 4 )
    {
        printf( "Error, must be called with exactly 3 parameters:\nxml-file\ncurrent profile\nstep time\n" );
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
    boost::scoped_ptr< electrical::TimeSeries< double, electrical::EvalNoInterpolation > > currentProfile;
    try
    {
        std::string filename( argv[2] );
        if ( filename.rfind( ".mat" ) != std::string::npos )
        {
            matlab::MatFile matFile( filename );
            matlab::MatioData &matStruct = matFile["diga"]["daten"];
            std::vector< double > time = matStruct["Programmdauer"].operator()< matlab::MatioData::X_DIM >( 0 );
            std::vector< double > current = matStruct["Strom"].operator()< matlab::MatioData::X_DIM >( 0 );
            currentProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( time, current ) );
        }
        else
        {
            std::ifstream currentProfileFile( argv[2] );
            currentProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( currentProfileFile ) );
        }
    }
    catch ( std::exception &e )
    {
        printf( "At 2. parameter: current profile\n" );
        printf( "%s\n", e.what() );
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        printf( "At 2. parameter: current profile\n" );
        printf( "Unidentified error\n" );
        return EXIT_FAILURE;
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

    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double > > electricalSimulation;
    boost::scoped_ptr< simulation::ThermalSimulation< myMatrixType, double, true > > thermalSimulation;
    boost::scoped_ptr< thermal::BlockObserver< myMatrixType, double > > blockObserver;
    boost::scoped_ptr< observer::ThermalObserver< double > > thermalVisualizer;

    try
    {
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double >( rootXmlNode, stepTime, currentProfile->GetMaxTime(),
                                                                       socStopCriterion, &cells ) );

        std::vector< std::vector< boost::shared_ptr< ThermalState< double > > > > thermalStatesOfCellBlocks;
        thermalSimulation.reset( new simulation::ThermalSimulation< myMatrixType, double, true >(
         rootXmlNode, stepTime, currentProfile->GetMaxTime(), thermalStateStopCriterion, &thermalVisualizer,
         &electricalSimulation->mThermalStates, &thermalStatesOfCellBlocks ) );
        blockObserver.reset( new thermal::BlockObserver< myMatrixType, double >( cells, thermalStatesOfCellBlocks ) );

        parser.reset();
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
#if defined( _ARMADILLO_ ) && !defined( SPARSE_MATRIX_FORMAT )
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType > >::type stepperElectrical =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType >() );
#else
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > > >::type stepperElectrical =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > >() );
    std::vector< double > tmpStateVector( electricalSimulation->mStateSystemGroup.mStateVector.n_rows, 0.0 );
#endif


    // Run simulation
    size_t timeCounter = 0;

    electricalSimulation->mRootTwoPort->SetCurrent( 0.0 );
    electricalSimulation->UpdateSystem();
    electricalSimulation->UpdateSystemValues();
    ( *electricalSimulation->mObserver )( electricalSimulation->mTime );

    double currentChangeTime = 0.0;
    ( *thermalVisualizer )( thermalSimulation->mTime );
    while ( electricalSimulation->CheckIfSumlationTimeHasNotEndedAndSetStepStartTime() )
    {
        // Current profile
        if ( electricalSimulation->mTime >= currentChangeTime )
        {
            currentProfile->SetTimeAndTriggerEvaluation( electricalSimulation->mTime );
            electricalSimulation->mRootTwoPort->SetCurrent( currentProfile->GetValue() );
            currentChangeTime = currentProfile->GetTimeUntilMaxValueDeviation( 0.0 );
        }


        // Initialize block Observer
        blockObserver->Initialize( electricalSimulation->mTime );
        electricalSimulation->ResetAllThermalStatesPowerDissipation();
        electricalSimulation->InitializeStopCriterion();
        while ( electricalSimulation->CheckLoopConditionAndSetDeltaTime( currentChangeTime ) &&
                !electricalSimulation->IsStopCriterionFulfilled() )
        {
            electricalSimulation->UpdateSystem();
#if defined( _ARMADILLO_ ) && !defined( SPARSE_MATRIX_FORMAT )
            // Run electrical equation solver
            myMatrixType state = electricalSimulation->mStateSystemGroup.mStateVector.submat(
             0, 0, electricalSimulation->mStateSystemGroup.mStateVector.n_rows - 2, 0 );
            electricalSimulation->mStateSystemGroup.mDt = electricalSimulation->mDeltaTime;
            while ( stepperElectrical.try_step( boost::ref( *electricalSimulation->mEqSystem ), state, electricalSimulation->mTime,
                                                electricalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
            {
                electricalSimulation->mStateSystemGroup.mStateVector.submat(
                 0, 0, electricalSimulation->mStateSystemGroup.mStateVector.n_rows - 2, 0 ) = state;
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
            ( *electricalSimulation->mObserver )( electricalSimulation->mTime );
            electricalSimulation->SaveStatesForLaterReset();
        }


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
                printf( "%.2f, ", std::ceil( static_cast< int >( electricalSimulation->mTime * 100.0 + 0.5 ) ) / 100.0 );
            }

            thermalSimulation->UpdateAllThermalStatesTemperatures();
        }

        // Finish step
        electricalSimulation->FinshStep();
        ( *blockObserver )( electricalSimulation->mTime );
    }

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
