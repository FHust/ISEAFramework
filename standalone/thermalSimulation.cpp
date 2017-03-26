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
* File Name : thermalSimulation.cpp
* Creation Date : 14-08-2015
* Last Modified : Di 23 Feb 2016 16:19:54 CET
* Created By : Manoel Brunnen
_._._._._._._._._._._._._._._._._._._._._.*/

#include "../src/misc/matrixInclude.h"
#include "../src/thermal/thermal_simulation.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "../src/thermal/block_observer.h"
#include "../src/time_series/time_series.h"
#include "../src/time_series/eval_linear_interpolation.h"
#include "../src/container/matio_file.h"
#include "../src/observer/thermal_observer.h"


#include <iostream>
using namespace matlab;
int main( int argc, char *argv[] )
{
    if ( argc != 3 )
    {
        std::cout << "Error, must be called with exactly 3 parameters:\n"
                     "xml-file\n"
                     "dissipation power\n" << std::endl;
        return EXIT_FAILURE;
    }

    double stepTime = 0.1, threshholdSOC = 0.0;

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
        printf( "At 1st parameter: xml-file\n" );
        printf( "Unidentified error\n" );
        return EXIT_FAILURE;
    }

    // Parameter 2
    boost::scoped_ptr< electrical::TimeSeries< double, electrical::EvalNoInterpolation > > powerProfile;
    try
    {
        std::string filename( argv[2] );

        if ( filename.rfind( ".mat" ) != std::string::npos )
        {
            MatFile matFile( filename );
            MatioData &matStruct = matFile["diga"]["daten"];
            std::vector< double > time = matStruct["Programmdauer"].operator()< MatioData::X_DIM >( 0 );
            std::vector< double > current = matStruct["Strom"].operator()< MatioData::X_DIM >( 0 );
            powerProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( time, current ) );
        }
        else
        {
            std::ifstream powerProfileFile( argv[2] );
            powerProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( powerProfileFile ) );
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


    // for the cellelements containing the thermalstates
    boost::scoped_ptr< simulation::ThermalSimulation< myMatrixType, double, true > > thermalSimulation;
    boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();
    boost::scoped_ptr< observer::ThermalObserver< double > > thermalVisualizer;
    boost::scoped_ptr< thermal::BlockObserver< myMatrixType, double > > blockObserver;

    try
    {
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

        thermalSimulation.reset(
         new simulation::ThermalSimulation< myMatrixType, double, true >( rootXmlNode, stepTime, powerProfile->GetMaxTime(),
                                                                          threshholdSOC, &thermalVisualizer, NULL, NULL ) );

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
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > > >::type stepperThermal =
     make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > >() );

    // Set fixed power dissipation
    powerProfile->SetTimeAndTriggerEvaluation( thermalSimulation->mTime );
    std::cout << "Thermal Size: " << thermalSimulation->mThermalStates.size() << std::endl;

    std::cout << "Thermal(unconnected) Size: " << thermalSimulation->mUnconnectedThermalStates.size() << std::endl;
    std::cout << "Thermal(connected) Size: " << thermalSimulation->mConnectedThermalElements.size() << std::endl;

    thermalSimulation->mThermalStates.at( 0 )->SetFixedPowerDissipation( powerProfile->GetValue() );
    std::ofstream fileVolumeDissapation( "Patches_Dissapation.csv" );
    fileVolumeDissapation << powerProfile->GetValue() << "\n";

    ( *thermalVisualizer )( thermalSimulation->mTime );

    // Thermal simulation
    double currentChangeTime = 0.0;
    while ( thermalSimulation->CheckIfSumlationTimeHasNotEnded() )
    {

        if ( thermalSimulation->mTime >= currentChangeTime )
        {
            powerProfile->SetTimeAndTriggerEvaluation( thermalSimulation->mTime );
            thermalSimulation->mThermalStates.at( 0 )->SetFixedPowerDissipation( powerProfile->GetValue() );
            currentChangeTime = powerProfile->GetTimeUntilMaxValueDeviation( 0.0 );
        }


        thermalSimulation->InitializeStopCriterion();
        while ( thermalSimulation->CheckLoopConditionAndSetDeltaTime( currentChangeTime ) )
        {

            if ( thermalSimulation->IsStopCriterionFulfilled() )
            {
                thermalSimulation->UpdateSystem();

                thermalSimulation->UpdateAllThermalStatesTemperatures();


                break;
            }

            std::cout << "time: " << thermalSimulation->mTime << "\t Temperature: "
                      << thermalSimulation->mThermalStates[thermalSimulation->mThermalStates.size() - 1]->GetValue()
                      << std::endl;
            // Run thermal equation solver
            while ( stepperThermal.try_step( boost::ref( *thermalSimulation->mThermalSystem ),
                                             thermalSimulation->mTemperatures, thermalSimulation->mTime,
                                             thermalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
            {
            }

            thermalSimulation->UpdateSystem();


            thermalSimulation->UpdateAllThermalStatesTemperatures();

            ( *thermalVisualizer )( thermalSimulation->mTime );
            fileVolumeDissapation << powerProfile->GetValue() << "\n";
        }
    }

    // Sucessful exit
    std::cout << "Run was successful" << std::endl;
    return EXIT_SUCCESS;
}
