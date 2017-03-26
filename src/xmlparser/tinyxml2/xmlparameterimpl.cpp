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

#include "xmlparameterimpl.h"
#include <cstring>
#include "../../misc/charArrayCmp.h"
#include "../../cstring/strtok_rbsd.h"
#include "../../cstring/strlcpy.h"

#include <boost/scoped_array.hpp>
#include <boost/algorithm/string.hpp>

namespace xmlparser
{
namespace tinyxml2
{

XmlParameterImpl::XmlParameterImpl( XMLElement* nodePtr )
    : mNodePtr( nodePtr )
{
}

XmlParameterImpl::~XmlParameterImpl() {}

boost::shared_ptr< XmlParameter > XmlParameterImpl::GetConfigurationRoot()
{
    XMLElement* root = mNodePtr->GetDocument()->FirstChildElement();
    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( root ) );
}

boost::shared_ptr< XmlParameter > XmlParameterImpl::GetElementChild( const char* elementName ) const
{
    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( GetRawElement( elementName ) ) );
}

std::vector< boost::shared_ptr< XmlParameter > > XmlParameterImpl::GetElementChildren( const char* elementName ) const
{
    std::vector< boost::shared_ptr< XmlParameter > > vec;

    std::vector< XMLElement* > children = GetRawElementChildren( elementName );
    typedef std::vector< XMLElement* >::iterator it_t;

    for ( it_t it = children.begin(); it != children.end(); ++it )
        vec.push_back( boost::shared_ptr< XmlParameter >( new XmlParameterImpl( *it ) ) );

    return vec;
}

std::vector< boost::shared_ptr< XmlParameter > > XmlParameterImpl::GetUnreferencedElementChildren( const char* elementName ) const
{
    XMLElement* node = GetRawElement( elementName )->FirstChildElement();

    std::vector< XMLElement* > children;
    while ( node )
    {
        children.push_back( node );
        node = node->NextSiblingElement();
    }


    std::vector< boost::shared_ptr< XmlParameter > > vec;

    typedef std::vector< XMLElement* >::iterator it_t;
    for ( it_t it = children.begin(); it != children.end(); ++it )
        vec.push_back( boost::shared_ptr< XmlParameter >( new XmlParameterImpl( *it ) ) );

    return vec;
}

bool XmlParameterImpl::HasElement( const char* elementName ) const { return GetRawElement( elementName, false ) != 0; }

bool XmlParameterImpl::GetElementBoolValue( const char* elementName, bool defaultValue ) const
{
    XMLElement* node = GetRawElement( elementName, false );
    if ( !node )
        return defaultValue;

    bool res;
    XMLError err = node->QueryBoolText( &res );

    if ( err )
    {
#ifdef __EXCEPTIONS__
        throw( CouldNotReadElementContentException( elementName, mNodePtr->Name(), mNodePtr->GetLineNumber() ) );
#endif
        abort();
    }

    return res;
}

int XmlParameterImpl::GetElementIntValue( const char* elementName, int defaultValue ) const
{
    XMLElement* node = GetRawElement( elementName );

    int res;
    XMLError err = node->QueryIntText( &res );

    if ( err )
    {

        return defaultValue;

#ifdef __EXCEPTIONS__
        throw( CouldNotReadElementContentException( elementName, mNodePtr->Name(), mNodePtr->GetLineNumber() ) );
#endif
        abort();
    }

    return res;
}

std::vector< size_t > XmlParameterImpl::GetElementUnsignedIntVecValue( const char* elementName ) const
{
    XMLElement* node = GetRawElement( elementName );

    const char* ctmp = node->GetText();
    if ( ctmp == 0 )
    {
#ifdef __EXCEPTIONS__
        throw( CouldNotReadElementContentException( elementName, mNodePtr->Name(), mNodePtr->GetLineNumber() ) );
#endif
        abort();
    }

    std::vector< size_t > res;
    const size_t sizeOfString = strlen( ctmp ) + 1;
    boost::scoped_array< char > input( new char[sizeOfString] );
    strcpy( input.get(), ctmp );

    char* save_ptr;
    char* split = strtok_rbsd( input.get(), ",", &save_ptr );

    char* endOfString = 0;
    while ( split )
    {

        size_t value = std::abs( strtol( split, &endOfString, 10 ) );    // signed long integer, stored as unsigned by
                                                                         // abs, this way -2 = 2, valid numbers are
                                                                         // positive long ints;
        if ( endOfString == split && value == 0 )
        {
#ifdef __EXCEPTIONS__
            throw( CouldNotReadElementContentException( split, mNodePtr->Name(), mNodePtr->GetLineNumber() ) );
#endif
            abort();
        }

        if ( value == 0 && !TestStringHasChar( split, '0' ) )
            break;

        res.push_back( value );

        split = strtok_rbsd( 0, ",", &save_ptr );
    }

    return res;
}

double XmlParameterImpl::GetElementDoubleValue( const char* elementName, double /* defaultValue */ ) const
{
    XMLElement* node = GetRawElement( elementName );

    double res;
    XMLError err = node->QueryDoubleText( &res );

    if ( err )
    {
#ifdef __EXCEPTIONS__
        throw( CouldNotReadElementContentException( elementName, mNodePtr->Name(), mNodePtr->GetLineNumber() ) );
#endif
        abort();
    }

    return res;
}

bool XmlParameterImpl::TestStringHasChar( const char* str, const char c ) const
{
    if ( !str )
        return false;
    for ( const char* cptr = str; *cptr != 0; ++cptr )
        if ( *cptr == c )
            return true;
    return false;
}

std::vector< double > XmlParameterImpl::GetElementDoubleVecValue( const char* elementName ) const
{
    XMLElement* node = GetRawElement( elementName );
    std::vector< double > res;

    const char* ctmp = node->GetText();
    if ( ctmp == 0 )
    {
#ifdef __EXCEPTIONS__
        throw( CouldNotReadElementContentException( elementName, mNodePtr->Name(), mNodePtr->GetLineNumber() ) );
#endif
        abort();
    }

    const size_t sizeOfString = strlen( ctmp ) + 1;
    boost::scoped_array< char > input( new char[sizeOfString] );
    strlcpy( input.get(), ctmp, sizeOfString );

    char* save_ptr;
    char* split = strtok_rbsd( input.get(), ",", &save_ptr );

    char* endOfString = 0;
    while ( split )
    {

        double value = strtod( split, &endOfString );    // atof( split );
        if ( endOfString == split && value == 0 )
        {
#ifdef __EXCEPTIONS__
            throw( CouldNotReadElementContentException( split, mNodePtr->Name(), mNodePtr->GetLineNumber() ) );
#endif
            abort();
        }

        if ( value == 0 && !TestStringHasChar( split, '0' ) )
            break;

        res.push_back( value );

        split = strtok_rbsd( 0, ",", &save_ptr );
    }

    return res;
}

std::vector< std::vector< double > > XmlParameterImpl::GetElementDoubleVecVecValue( const char* elementName ) const
{
    XMLElement* node = GetRawElement( elementName );
    std::vector< std::vector< double > > res;

    const char* ctmp = node->GetText();
    if ( ctmp == 0 )
    {
#ifdef __EXCEPTIONS__
        throw( CouldNotReadElementContentException( elementName, node->Name(), node->GetLineNumber() ) );
#endif
        abort();
    }

    const size_t sizeOfString = strlen( ctmp ) + 1;
    boost::scoped_array< char > input( new char[sizeOfString] );
    strlcpy( input.get(), ctmp, sizeOfString );

    char* outer_save_ptr;
    char* inner_save_ptr;
    char* outersplit = strtok_rbsd( input.get(), ";", &outer_save_ptr );

    while ( outersplit )
    {
        res.push_back( std::vector< double >() );

        char* innersplit = strtok_rbsd( outersplit, ",", &inner_save_ptr );

        while ( innersplit )
        {
            char* endOfString = 0;
            double value = strtod( innersplit, &endOfString );    // atof( split );
            if ( endOfString == innersplit && value == 0 )
            {
#ifdef __EXCEPTIONS__
                throw( CouldNotReadElementContentException( innersplit, node->Name(), node->GetLineNumber() ) );
#endif
                abort();
            }

            if ( value == 0 && !TestStringHasChar( innersplit, '0' ) )
                break;
            // double value = atof( innersplit );
            res.back().push_back( value );

            innersplit = strtok_rbsd( 0, ",", &inner_save_ptr );
        }
        if ( !TestStringHasChar( outer_save_ptr, ';' ) && !TestStringHasChar( outer_save_ptr, ',' ) )
            break;
        outersplit = strtok_rbsd( 0, ";", &outer_save_ptr );
    }

    return res;
}

std::string XmlParameterImpl::GetElementStringValueWithDefaultValue( const char* elementName, const char* defaultValue ) const
{
    XMLElement* node = GetRawElement( elementName );

    const char* res = node->GetText();

    if ( res == 0 )
    {
        res = defaultValue;
    }

    std::string returnValue( res );
    boost::trim( returnValue );

    return returnValue;
}

std::string XmlParameterImpl::GetElementStringValue( const char* elementName ) const
{
    XMLElement* node = GetRawElement( elementName );

    const char* res = node->GetText();

    if ( res == 0 )
    {
#ifdef __EXCEPTIONS__
        throw( CouldNotReadElementContentException( elementName, mNodePtr->Name(), mNodePtr->GetLineNumber() ) );
#endif
        abort();
    }

    std::string returnValue( res );
    boost::trim( returnValue );

    return returnValue;
}

const char* XmlParameterImpl::GetElementName() const { return (const char*)mNodePtr->Name(); }

const char* XmlParameterImpl::GetElementAttribute( const char* attributeName ) const
{
    return GetAttribute( mNodePtr, attributeName );
}

bool XmlParameterImpl::HasElementAttribute( const char* attributeName ) const
{
    return HasAttribute( mNodePtr, attributeName );
}

bool XmlParameterImpl::GetElementAttributeBoolValue( const char* attributeName, bool defaultValue ) const
{
    const char* ctmp = GetAttribute( mNodePtr, attributeName, false );
    if ( !ctmp )
        return defaultValue;
    return !misc::CaseInsensitiveStringCompare( (const char*)ctmp, "TRUE" );
}

int XmlParameterImpl::GetElementAttributeIntValue( const char* attributeName, int defaultValue ) const
{
    const char* ctmp = GetAttribute( mNodePtr, attributeName, false );
    if ( !ctmp )
        return defaultValue;

    return atoi( GetAttribute( mNodePtr, attributeName ) );
}

double XmlParameterImpl::GetElementAttributeDoubleValue( const char* attributeName, double defaultValue ) const
{
    const char* ctmp = GetAttribute( mNodePtr, attributeName, false );
    if ( !ctmp )
        return defaultValue;

    return atof( GetAttribute( mNodePtr, attributeName ) );
}

size_t XmlParameterImpl::GetLineNumber() const { return mNodePtr->GetLineNumber(); }

XMLElement* XmlParameterImpl::GetRawElement( const char* elementName, bool throwOnMiss ) const
{
    XMLElement* node = mNodePtr->FirstChildElement( elementName );

    if ( !node && throwOnMiss )
    {
#ifdef __EXCEPTIONS__
        throw( ElementNotFoundException( elementName, mNodePtr->Name(), mNodePtr->GetLineNumber() ) );
#endif
        abort();
    }

    if ( node && IsReference( node ) )
        node = GetReferencedRawElement( node );

    return node;
}

std::vector< XMLElement* > XmlParameterImpl::GetRawElementChildren( const char* elementName ) const
{
    XMLElement* node = GetRawElement( elementName )->FirstChildElement();

    std::vector< XMLElement* > vec;
    while ( node )
    {
        if ( IsReference( node ) )
            vec.push_back( GetReferencedRawElement( node ) );
        else
            vec.push_back( node );

        node = node->NextSiblingElement();
    }
    return vec;
}

bool XmlParameterImpl::CheckNodeName( XMLElement* node, const char* name ) const
{
    return !misc::CaseInsensitiveStringCompare( (const char*)node->Name(), name );
}

XMLElement* XmlParameterImpl::GetReferencedRawElement( XMLElement* node ) const
{
    const char* referencename = GetAttribute( node, "ref" );

    XMLElement* root = node->GetDocument()->FirstChildElement();
    std::vector< XMLElement* > customelems = XmlParameterImpl( root ).GetRawElementChildren( "CustomDefinitions" );

    typedef std::vector< XMLElement* >::iterator it_t;

    for ( it_t it = customelems.begin(); it != customelems.end(); ++it )
        if ( !misc::CaseInsensitiveStringCompare( (const char*)( *it )->Name(), referencename ) )
            return *it;

#ifdef __EXCEPTIONS__
    throw( ElementNotFoundException( referencename, node->Name(), node->GetLineNumber() ) );
#endif
    abort();

    return 0;
}

bool XmlParameterImpl::IsReference( XMLElement* node ) const { return HasAttribute( node, "ref" ); }

bool XmlParameterImpl::HasAttribute( XMLElement* node, const char* attrName ) const
{
    const char* attr = node->Attribute( attrName );
    return attr != 0;
}

const char* XmlParameterImpl::GetAttribute( XMLElement* node, const char* attrName, bool throwOnMiss ) const
{
    const char* attr = node->Attribute( attrName );

    if ( !attr )
    {
        if ( throwOnMiss )
        {

#ifdef __EXCEPTIONS__
            throw( AttributeNotFoundException( attrName, node->Name(), node->GetLineNumber() ) );
#endif
            abort();
        }
        return 0;
    }
    return attr;
}

boost::shared_ptr< XmlParameter > XmlParameterImpl::GetConfigurationRoot() const
{
    XMLElement* root = mNodePtr->GetDocument()->FirstChildElement();
    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( root ) );
}
}
} /* namespace factory */
