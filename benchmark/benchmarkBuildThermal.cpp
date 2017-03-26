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
#include <boost/date_time.hpp>
#include <iostream>
#include "../src/thermal/thermal_simulation.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "../src/misc/matrixInclude.h"


// forward declarations
const double thermal::globalMaxValue = 1000000000.0;
const double thermal::globalMinValue = -thermal::globalMaxValue;

int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        printf( "Error, must be called with exactly one parameters:\nxml-file" );
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

    boost::scoped_ptr< simulation::ThermalSimulation< myMatrixType, double, true > > thermalSimulation;

    try
    {
        boost::posix_time::time_duration creationDuration;
        boost::posix_time::ptime creationStart = boost::posix_time::microsec_clock::local_time();

        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();
        thermalSimulation.reset(
         new simulation::ThermalSimulation< myMatrixType, double, true >( rootXmlNode, 10.0, 100.0, 5.0, 0, 0, 0 ) );

        creationDuration = boost::posix_time::microsec_clock::local_time() - creationStart;
        std::cout << "Creating thermal system took "
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

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
