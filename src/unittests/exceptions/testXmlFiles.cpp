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
#include "testXmlFiles.h"

// BOOST

#include <boost/shared_ptr.hpp>

// STD

#include <algorithm>
#include <string>
#include <vector>

// ETC

#include "../../xmlparser/tinyxml2/xmlparserimpl.h"
#include "../../misc/tinydir.h"

void TestXmlFiles::TestErrosXmlFiles()
{

    // The Languages:

    std::vector< std::string > languages =
     _tinydir_get_dir_entries( "../src/exceptions/errors", std::string( ".xml" ) );

    // iterators(wird mehrmals benutzt):

    size_t i = 0;
    size_t j = 0;

    // Place params for all xml-files and get all children:

    std::vector< xmlparser::tinyxml2::XmlParserImpl > parsers( languages.size() );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > params( languages.size() );
    std::vector< std::vector< boost::shared_ptr< xmlparser::XmlParameter > > > children( languages.size() );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > getNodeChildren( languages.size() );

    for ( i = 0; i < languages.size(); ++i )
    {
        std::string path = "../src/exceptions/errors/" + languages.at( i );

        parsers.at( i ).ReadFromFile( path.c_str() );
        params.at( i ) = parsers.at( i ).GetRoot();

        getNodeChildren.at( i ) = params.at( i )->GetElementChild( "getchildren" );
        children.at( i ) = params.at( i )->GetElementChildren( "getchildren" );
    }

    size_t anzahlEng = 0;
    size_t anzahlCompare = 0;
    std::string errorMessage;

    for ( j = 1; j < params.size(); ++j )
        for ( i = 0; i < children.at( 0 ).size(); ++i )
        {
            // Control, if the xml, that being compared,has the same element.

            if ( getNodeChildren.at( j )->HasElement( children.at( 0 ).at( i )->GetElementName() ) )
            {
                // EN(default):

                std::string tempErrorMessageEng(
                 getNodeChildren.at( 0 )->GetElementStringValue( children.at( 0 ).at( i )->GetElementName() ) );
                anzahlEng = std::count( tempErrorMessageEng.begin(), tempErrorMessageEng.end(), '%' );

                // Language[j]:

                std::string compareTempErrorMessage(
                 getNodeChildren.at( j )->GetElementStringValue( children.at( 0 ).at( i )->GetElementName() ) );
                anzahlCompare = std::count( compareTempErrorMessage.begin(), compareTempErrorMessage.end(), '%' );

                // Vergleich:

                if ( anzahlCompare != anzahlEng )
                {
                    std::string elementName( children.at( 0 ).at( i )->GetElementName() );

                    if ( anzahlCompare > anzahlEng )
                    {
                        errorMessage = "Missing " + std::to_string( anzahlCompare - anzahlEng ) +
                                       " argument in element : " + elementName + " in EN.xml";
                    }
                    else
                    {
                        errorMessage = "Missing " + std::to_string( anzahlEng - anzahlCompare ) +
                                       " argument in element : " + elementName + " in " + languages.at( j );
                    }

                    TS_FAIL( errorMessage );
                }
            }
            else
            {
                std::string elementName( children.at( 0 ).at( i )->GetElementName() );
                errorMessage = "Missing element: " + elementName + " in " + languages.at( j );
                TS_FAIL( errorMessage );
            }

            // Control, if any element is missing in EN.xml(default).
            if ( i < children.at( j ).size() )
            {
                if ( !getNodeChildren.at( 0 )->HasElement( children.at( j ).at( i )->GetElementName() ) )
                {
                    std::string elementName( children.at( j ).at( i )->GetElementName() );
                    errorMessage = "Missing element: " + elementName + "in" + languages.at( 0 );
                    TS_FAIL( errorMessage );
                }
            }
        }
}
