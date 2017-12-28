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
* File Name : verification.cpp
* Creation Date : 14-11-2016
* Last Modified : Mo 14 Nov 2016 16:36:55 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include <cstring>

#include "../src/misc/fast_copy_matrix.h"
#include "../src/misc/matrixInclude.h"
#include "../src/misc/StrCont.h"
#include "../src/thermal/electrical_simulation.h"
#include "../src/thermal/thermal_simulation.h"

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
    if ( argc != 2 )
    {
        printf( "Error, must be called with exactly 1 parameters:\nxml-file\n" );
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

    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double > > electricalSimulation;
    boost::scoped_ptr< simulation::ThermalSimulation< myMatrixType, double, true > > thermalSimulation;
    boost::scoped_ptr< observer::ThermalObserver< double > > thermalVisualizer;

    boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode;
    std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;

    bool electricalSimulationFound = false;
    bool thermalSimulationFound = false;
    const double stepTime = 0.1;

    try
    {
        rootXmlNode = parser->GetRoot();
    }
    catch ( std::exception &e )
    {
    }
    catch ( ... )
    {
    }

    try
    {
        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double >( rootXmlNode, stepTime, 0.0, 0.1, &cells ) );
        electricalSimulationFound = true;
    }
    catch ( std::exception &e )
    {
    }
    catch ( ... )
    {
    }

    try
    {
        std::vector< std::vector< boost::shared_ptr< ThermalState< double > > > > thermalStatesOfCellBlocks;
        if ( electricalSimulation )
        {
            thermalSimulation.reset(
             new simulation::ThermalSimulation< myMatrixType, double, true >( rootXmlNode, stepTime, 0, thermalStateStopCriterion,
                                                                              &thermalVisualizer, &electricalSimulation->mThermalStates,
                                                                              &thermalStatesOfCellBlocks ) );
        }
        else
        {
            thermalSimulation.reset( new simulation::ThermalSimulation< myMatrixType, double, true >(
             rootXmlNode, stepTime, 0, thermalStateStopCriterion, &thermalVisualizer, 0, &thermalStatesOfCellBlocks ) );
        }
        thermalSimulationFound = true;
    }
    catch ( std::exception &e )
    {
    }
    catch ( ... )
    {
    }

    std::cout << "electrical ";
    if ( electricalSimulationFound )
    {
        std::cout << "passed" << std::endl;
    }
    else
    {
        std::cout << "failed" << std::endl;
    }

    std::cout << "thermal ";
    if ( thermalSimulationFound )
    {
        std::cout << "passed" << std::endl;
    }
    else
    {
        std::cout << "failed" << std::endl;
    }


    return EXIT_SUCCESS;
}
