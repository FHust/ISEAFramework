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
 * xmlparser.h
 *
 *  Created on: 18.09.2013
 *      Author: chris
 */

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include "xmlparameter.h"


/// This namespace contains XML related classes.
namespace xmlparser
{

/// Interface for a parser
class XmlParser
{
    public:
    virtual ~XmlParser() {}

    /// Read XML document from file
    virtual void ReadFromFile( const char* fileName ) = 0;

    /// Read XML document from memory
    virtual void ReadFromMem( const char* xmldata ) = 0;

    /// Returns the documents root.
    virtual boost::shared_ptr< XmlParameter > GetRoot() = 0;
};
}

#endif /* XMLPARSER_H_ */
