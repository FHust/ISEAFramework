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
/*
 * xmlparserimpl.cpp
 *
 *  Created on: 18.09.2013
 *      Author: chris
 */

#include "xmlparserimpl.h"
#include "xmlparameterimpl.h"

namespace xmlparser
{
namespace tinyxml2
{

XmlParserImpl::XmlParserImpl()
{ }

XmlParserImpl::~XmlParserImpl()
{
    mDoc.Clear();
}

void XmlParserImpl::ReadFromFile( const char* fileName )
{
    if( !fileName )
    {
#ifdef __EXCEPTIONS__
        throw( ConfigFileNotFoundException( "Memory" ) );
#endif
        abort();
    }

    XMLError err = mDoc.LoadFile( fileName );

#ifdef __EXCEPTIONS__
    switch( err )
    {
        case XML_NO_ERROR:
            break;

        case XML_ERROR_FILE_NOT_FOUND:
        case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
        case XML_ERROR_FILE_READ_ERROR:
            throw( ConfigFileNotFoundException( fileName ) );

        case XML_ERROR_ELEMENT_MISMATCH:
            throw( CouldNotParseConfigException( "ELEMENT_MISMATCH", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_ELEMENT:
            throw( CouldNotParseConfigException( "PARSING_ELEMENT", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_ATTRIBUTE:
            throw( CouldNotParseConfigException( "PARSING_ATTRIBUTE", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_IDENTIFYING_TAG:
            throw( CouldNotParseConfigException( "IDENTIFYING_TAG", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_TEXT:
            throw( CouldNotParseConfigException( "PARSING_TEXT", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_CDATA:
            throw( CouldNotParseConfigException( "PARSING_CDATA", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_COMMENT:
            throw( CouldNotParseConfigException( "PARSING_COMMENT", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_DECLARATION:
            throw( CouldNotParseConfigException( "PARSING_DECLARATION", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_UNKNOWN:
            throw( CouldNotParseConfigException( "PARSING_UNKNOWN", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_EMPTY_DOCUMENT:
            throw( CouldNotParseConfigException( "EMPTY_DOCUMENT" ) );
        case XML_ERROR_MISMATCHED_ELEMENT:
            throw( CouldNotParseConfigException( "MISMATCHED_ELEMENT", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING:
            throw( CouldNotParseConfigException( "PARSING", mDoc.GetErrorLineNumber() ) );

        default:
            {
                throw( CouldNotParseConfigException() );
            }
    }
#else
    if( err )
        abort();
#endif

}

void XmlParserImpl::ReadFromMem( const char* xmlData )
{
    if( !xmlData )
    {
#ifdef __EXCEPTIONS__
        throw( ConfigFileNotFoundException( "Memory" ) );
#endif
        abort();
    }

    XMLError err = mDoc.Parse( xmlData, strlen( xmlData ) );

#ifdef __EXCEPTIONS__
    switch( err )
    {
        case XML_NO_ERROR:
            break;

        case XML_ERROR_FILE_NOT_FOUND:
        case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
        case XML_ERROR_FILE_READ_ERROR:
            throw( ConfigFileNotFoundException( "Memory" ) );

        case XML_ERROR_ELEMENT_MISMATCH:
            throw( CouldNotParseConfigException( "ELEMENT_MISMATCH", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_ELEMENT:
            throw( CouldNotParseConfigException( "PARSING_ELEMENT", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_ATTRIBUTE:
            throw( CouldNotParseConfigException( "PARSING_ATTRIBUTE", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_IDENTIFYING_TAG:
            throw( CouldNotParseConfigException( "IDENTIFYING_TAG", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_TEXT:
            throw( CouldNotParseConfigException( "PARSING_TEXT", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_CDATA:
            throw( CouldNotParseConfigException( "PARSING_CDATA", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_COMMENT:
            throw( CouldNotParseConfigException( "PARSING_COMMENT", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_DECLARATION:
            throw( CouldNotParseConfigException( "PARSING_DECLARATION", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING_UNKNOWN:
            throw( CouldNotParseConfigException( "PARSING_UNKNOWN", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_EMPTY_DOCUMENT:
            throw( CouldNotParseConfigException( "EMPTY_DOCUMENT" ) );
        case XML_ERROR_MISMATCHED_ELEMENT:
            throw( CouldNotParseConfigException( "MISMATCHED_ELEMENT", mDoc.GetErrorLineNumber() ) );
        case XML_ERROR_PARSING:
            throw( CouldNotParseConfigException( "PARSING", mDoc.GetErrorLineNumber() ) );

        default:
            {
                throw( CouldNotParseConfigException() );
            }
    }
#else
    if( err )
        abort();
#endif

}

    boost::shared_ptr< XmlParameter >
XmlParserImpl::GetRoot()
{
    XMLElement* root = mDoc.FirstChildElement();

    if( !root )
    {

#ifdef __EXCEPTIONS__
        throw( CouldNotParseConfigException() );
#endif
        abort();

    }

    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( root ) );
}

}
} /* namespace xmlparser */
