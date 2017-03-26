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

#ifndef XMLPARSERIMPLTINY_H_
#define XMLPARSERIMPLTINY_H_

#include "../xmlparser.h"
#include "tinyxml2.h"

namespace xmlparser
{
namespace tinyxml2
{

/// Implementation of XmlParser based on libxml2
class XmlParserImpl : public xmlparser::XmlParser
{
    public:
    XmlParserImpl();

    virtual ~XmlParserImpl();

    void ReadFromFile( const char* fileName );

    void ReadFromMem( const char* xmldata );

    boost::shared_ptr< XmlParameter > GetRoot();

    private:
    XMLDocument mDoc;
};
}
} /* namespace xmlparser */
#endif /* XMLPARSERIMPL_H_ */
