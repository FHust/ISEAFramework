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
#ifndef _GET_ELECTRICAL_DISCRETIZATION_
#define _GET_ELECTRICAL_DISCRETIZATION_

#include "../xmlparser/xmlparameter.h"


namespace factory
{

/// Returns the number of cell elements in a cell/thermal block
size_t GetElectricalDiscretization( const boost::shared_ptr< xmlparser::XmlParameter > &thermalBlockNode );
}

#endif