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
#ifndef _PARAMETER_DESCRIPTION_
#define _PARAMETER_DESCRIPTION_

#include "../../xmlparser/xmlparameter.h"
#include "../../misc/StrCont.h"
#include "../../geometry/cartesian.h"


namespace factory
{

/**
 * Gets the xml-parameter and the class name and returns this information as a misc::StrCont
 */
misc::StrCont ParameterDescription( const xmlparser::XmlParameter *param, const char *className );

/**
 * Gets the xml-parameter and the class name and returns this information as a misc::StrCont
 * @param[in] x,y,z Global coordinates of emplacement point
 */
template < typename T >
misc::StrCont ParameterDescription( const xmlparser::XmlParameter *param, const char *className, geometry::Cartesian< T > emplacementPoint )
{
    return misc::StrCont( param->GetElementName() ) + misc::StrCont( " of class " ) + misc::StrCont( className ) +
           misc::StrCont( " at (" ) + misc::StrCont( emplacementPoint.GetX() ) + misc::StrCont( "," ) +
           misc::StrCont( emplacementPoint.GetY() ) + misc::StrCont( "," ) + misc::StrCont( emplacementPoint.GetZ() ) +
           misc::StrCont( ")" ) + misc::StrCont( " in xml-file line " ) + misc::StrCont( param->GetLineNumber() );
}
}

#endif