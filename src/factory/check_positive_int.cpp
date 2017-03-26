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
#include "check_positive_int.h"

//#include "../thermal/exceptions/parametrization_error.h"
#include "../exceptions/error_proto.h"
#include "../misc/macros.h"
#include <stdexcept>


namespace factory
{

size_t CheckPositiveInt( const int &val, const boost::shared_ptr< xmlparser::XmlParameter > &param, const char name[] )
{
    if ( val < 1 )
    {
        if ( param.get() )
        {
            const size_t MAXSIZE = 1024;
            char description[MAXSIZE] = {0};
            sprintf( description, "xml-file line %i", static_cast< int >( param->GetLineNumber() ) );
            char stringbuffer[MAXSIZE] = {0};
            real_snprintf( stringbuffer, MAXSIZE, "Integer %s must not be negative or zero", name );
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error56", stringbuffer, description );
        }
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedXmlParameterCheckPositiveInt" );
    }
    return val;
}
}
