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
#ifndef _ERRORFUNCTION_
#define _ERRORFUNCTION_

#include "../misc/macros.h"

// BOOST
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

// STD
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdarg>

// ETC
#include "../cstring/strlcpy.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"

extern std::string XML_ERRORS;

template < typename ErrorType >
void ErrorFunction( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... )
{
    va_list args;
    va_start( args, XML_FEHLER_ID );
    const size_t ERRORMESSAGESIZE = 1000;
    char errorMessage[ERRORMESSAGESIZE];

    xmlparser::tinyxml2::XmlParserImpl parser;


    parser.ReadFromMem( XML_ERRORS.c_str() );
    boost::shared_ptr< xmlparser::XmlParameter > param( parser.GetRoot() );

    std::string tempErrorMessageXML =
     param->GetElementChild( "getchildren" )->GetElementStringValue( XML_FEHLER_ID ) + "\n";

    boost::scoped_array< char > errorMessageXML( new char[tempErrorMessageXML.size() + 1] );
    memset( errorMessageXML.get(), 0, sizeof( char ) * ( tempErrorMessageXML.size() + 1 ) );

    strlcpy( errorMessageXML.get(), tempErrorMessageXML.c_str(), tempErrorMessageXML.size() + 1 );

    /* ==================== */

    int x = 0;

#ifdef __DEVELOPER__

    x = snprintf( errorMessage, ERRORMESSAGESIZE, "Error in Funktion:%s, line:%d, file:%s.\n", fct, line, file );

#else

    UNUSED( fct );
    UNUSED( line );
    UNUSED( file );

#endif

    if ( x < 0 )
    {
        printf( "Error Message can't have a negative length!" );
        abort();
    }

    vsnprintf( errorMessage + x, ERRORMESSAGESIZE - x, errorMessageXML.get(), args );

    va_end( args );

#ifdef __EXCEPTIONS__
    throw ErrorType( errorMessage );
#else

    fprintf( stderr, errorMessage );
    abort();

#endif
}

#endif
