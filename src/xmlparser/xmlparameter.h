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
 * xmlparameter.h
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#ifndef XMLPARAMETER_H_
#define XMLPARAMETER_H_

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#ifdef __EXCEPTIONS__
#include "exceptions/couldnotreadelementcontentexception.h"
#include "exceptions/attributenotfoundexception.h"
#include "exceptions/elementnotfoundexception.h"
#include "exceptions/configfilenotfoundexception.h"
#include "exceptions/couldnotparseconfigexception.h"
#endif

namespace xmlparser
{
class XmlParameter;

/// Interface for a parameter node
class XmlParameter
{
    public:
    virtual ~XmlParameter() {}

    /// Finds a node with the given name. Throws ParameterNotFoundException
    virtual boost::shared_ptr< XmlParameter > GetElementChild( const char* elementName ) const = 0;

    /// Returns all children of a node with the given name. Throws ParameterNotFoundException
    virtual std::vector< boost::shared_ptr< XmlParameter > > GetElementChildren( const char* elementName ) const = 0;

    /// Returns all children of a node without referencing functionality with the given name. Throws
    /// ParameterNotFoundException
    virtual std::vector< boost::shared_ptr< XmlParameter > > GetUnreferencedElementChildren( const char* elementName ) const = 0;

    /// Checks if a particular node does exist
    virtual bool HasElement( const char* elementName ) const = 0;

    /// Finds a node with the given name and returns its value as bool
    virtual bool GetElementBoolValue( const char* elementName, bool defaultValue = false ) const = 0;

    /// Finds a node with the given name and returns its value as int. Throws ParameterNotFoundException
    virtual int GetElementIntValue( const char* elementName, int defaultValue = 0 ) const = 0;

    /// Finds a node with the given name and returns its value as std::vector of int. Throws ParameterNotFoundException
    virtual std::vector< size_t > GetElementUnsignedIntVecValue( const char* elementName ) const = 0;

    /// Finds a node with the given name and returns it's value as double. Throws ParameterNotFoundException
    virtual double GetElementDoubleValue( const char* elementName, double defaultValue = 0 ) const = 0;

    /// Finds a node with the given name and returns its value as std::vector of double. Throws
    /// ParameterNotFoundException
    virtual std::vector< double > GetElementDoubleVecValue( const char* elementName ) const = 0;

    /// Finds a node with the given name and returns its value as std::vector of std::vector of double. Throws
    /// ParameterNotFoundException
    virtual std::vector< std::vector< double > > GetElementDoubleVecVecValue( const char* elementName ) const = 0;

    /// Finds a node with the given name and returns its value as std::string and trimmed. Throws
    /// ParameterNotFoundException
    virtual std::string GetElementStringValue( const char* elementName ) const = 0;

    /// Finds a node with the given name and returns its value as std::string and trimmedconst char*,  otherwise the
    /// defaultValue will be returned
    virtual std::string GetElementStringValueWithDefaultValue( const char* elementName, const char* defaultValue ) const = 0;

    /// Returns it's name
    virtual const char* GetElementName() const = 0;

    /// Finds an attribute with the given name and return its value. Throws AttributeNotFoundException
    virtual const char* GetElementAttribute( const char* attributeName ) const = 0;

    /// Checks if it has an attribute with the given name
    virtual bool HasElementAttribute( const char* attributeName ) const = 0;

    /// Finds an attribute with the given name and return its value as bool
    virtual bool GetElementAttributeBoolValue( const char* attributeName, bool defaultValue = false ) const = 0;

    /// Finds an attribute with the given name and return its value as int
    virtual int GetElementAttributeIntValue( const char* attributeName, int defaultValue = 0 ) const = 0;

    /// Returns the documents root element
    virtual boost::shared_ptr< XmlParameter > GetConfigurationRoot() const = 0;

    /// Finds an attribute with the given name and return its value as double
    virtual double GetElementAttributeDoubleValue( const char* attributeName, double defaultValue = 0 ) const = 0;

    /// Gets line number of this parameter in xml-file
    virtual size_t GetLineNumber() const = 0;
};

} /* namespace xmlfactory */
#endif /* XMLPARAMETER_H_ */
