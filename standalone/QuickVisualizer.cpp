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
#include "../src/thermal/simulation_data_outline.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "../src/misc/matrixInclude.h"


// forward declarations
const double thermal::globalMaxValue = 1000000000.0;
const double thermal::globalMinValue = -thermal::globalMaxValue;


int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        printf( "Error, must be called with exactly 1 parameter:\nxml-file\n" );
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

    // Run program
    try
    {
        simulation::QuickVisualizeThermalModel< double >( parser->GetRoot() );
    }
    catch ( std::exception &e )
    {
        printf( "%s\n", e.what() );
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        printf( "Unidentified error\n" );
        return EXIT_FAILURE;
    }

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
