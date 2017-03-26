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
 * xmlparserimpl.h
 *
 *  Created on: 18.09.2013
 *      Author: chris
 */

#ifndef XMLPARAMETERRIMPLTINY_H_
#define XMLPARAMETERRIMPLTINY_H_

#include "../xmlparameter.h"
#include "tinyxml2.h"

namespace xmlparser
{
namespace tinyxml2
{

/// Implementation of XMLParameter based on libxml2
class XmlParameterImpl : public xmlparser::XmlParameter
{
    public:
    XmlParameterImpl( XMLElement* nodePtr );

    virtual ~XmlParameterImpl();

    boost::shared_ptr< XmlParameter > GetConfigurationRoot();
    boost::shared_ptr< XmlParameter > GetConfigurationRoot() const;

    boost::shared_ptr< XmlParameter > GetElementChild( const char* elementName ) const;

    std::vector< boost::shared_ptr< XmlParameter > > GetElementChildren( const char* elementName ) const;

    std::vector< boost::shared_ptr< XmlParameter > > GetUnreferencedElementChildren( const char* elementName ) const;

    bool HasElement( const char* elementName ) const;

    bool GetElementBoolValue( const char* elementName, bool defaultValue = false ) const;

    int GetElementIntValue( const char* elementName, int defaultValue = 0 ) const;

    std::vector< size_t > GetElementUnsignedIntVecValue( const char* elementName ) const;

    double GetElementDoubleValue( const char* elementName, double defaultValue ) const;

    std::vector< double > GetElementDoubleVecValue( const char* elementName ) const;

    std::vector< std::vector< double > > GetElementDoubleVecVecValue( const char* elementName ) const;

    std::string GetElementStringValue( const char* elementName ) const;

    std::string GetElementStringValueWithDefaultValue( const char* elementName, const char* defaultValue ) const;

    const char* GetElementName() const;

    const char* GetElementAttribute( const char* attributeName ) const;

    bool HasElementAttribute( const char* attributeName ) const;

    bool GetElementAttributeBoolValue( const char* attributeName, bool defaultValue = false ) const;

    int GetElementAttributeIntValue( const char* attributeName, int defaultValue = 0 ) const;

    double GetElementAttributeDoubleValue( const char* attributeName, double defaultValue = 0 ) const;

    size_t GetLineNumber() const;

    private:
    XMLElement* GetRawElement( const char* elementName, bool throwOnMiss = true ) const;

    std::vector< XMLElement* > GetRawElementChildren( const char* elementName ) const;

    bool CheckNodeName( XMLElement* node, const char* name ) const;

    XMLElement* GetReferencedRawElement( XMLElement* node ) const;

    bool IsReference( XMLElement* node ) const;

    bool HasAttribute( XMLElement* param, const char* attrName ) const;

    const char* GetAttribute( XMLElement* param, const char* attrName, bool throwOnMiss = true ) const;

    bool TestStringHasChar( const char* str, const char c ) const;

    XMLElement* mNodePtr;
};
}
} /* namespace factory */
#endif /* XMLPARAMETERRIMPL_H_ */
