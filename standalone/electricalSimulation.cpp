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
// STD
#include <fstream>
#include <string>
#include <cstring>
#include <string>

// BOOST
#include <boost/numeric/odeint.hpp>

// ETC
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "../src/misc/StrCont.h"
#include "../src/misc/matrixInclude.h"
#include "../src/misc/fast_copy_matrix.h"
#include "../src/thermal/electrical_simulation.h"
#include "../src/time_series/time_series.h"
#include "../src/time_series/eval_linear_interpolation.h"
#include "../src/container/matio_file.h"


int main( int argc, char *argv[] )
{
    if ( argc != 3 )
    {
        printf( "Error, must be called with exactly 2 parameters:\nxml-file\ncurrent profile\n" );
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
    // boost::scoped_ptr< electrical::TimeSeries<double, electrical::EvalLinearInterpolation> > currentProfile;
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
    catch ( const std::exception &e )
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
    double stepTime = 5.0;

    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double > > electricalSimulation;

    try
    {

        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double >( rootXmlNode, stepTime,
                                                                       currentProfile->GetMaxTime(), 0.1, &cells ) );
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


// Equation solvers
#if defined( _ARMADILLO_ ) && not defined( SPARSE_MATRIX_FORMAT )
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType > >::type stepperElectrical =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType >() );
#else
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< std::vector< double > > >::type stepperElectrical =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< std::vector< double > >() );
    std::vector< double > tmpStateVector( electricalSimulation->mStateSystemGroup.mStateVector.n_rows, 0.0 );
#endif

    // Run simulation
    electricalSimulation->mRootTwoPort->SetCurrent( 0.0 );
    electricalSimulation->UpdateSystem();
    electricalSimulation->UpdateSystemValues();
    ( *electricalSimulation->mObserver )( electricalSimulation->mTime );
    double currentChangeTime = 0.0;
    while ( electricalSimulation->CheckIfSumlationTimeHasNotEndedAndSetStepStartTime() )
    {
        printf( "%f\n", electricalSimulation->mTime );
        // Current profile
        if ( electricalSimulation->mTime >= currentChangeTime )
        {
            currentProfile->SetTimeAndTriggerEvaluation( electricalSimulation->mTime );
            electricalSimulation->mRootTwoPort->SetCurrent( currentProfile->GetValue() );
            currentChangeTime = currentProfile->GetTimeUntilMaxValueDeviation( 0.0 );
        }

        // Electrical simulation
        //// Initialize block Observer
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
            ( *electricalSimulation->mObserver )( electricalSimulation->mTime );
            electricalSimulation->SaveStatesForLaterReset();
        }

        // Finish step
        electricalSimulation->FinshStep();
    }

    // Sucessful exit
    return EXIT_SUCCESS;
}
