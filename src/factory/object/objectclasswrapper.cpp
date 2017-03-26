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
 * objectclasswrapper.cpp
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#include "objectclasswrapper.h"

namespace factory
{
template class Factory< object::Object< double >, ArgumentTypeObject<double> >;

template class ObjectClassWrapper< double, object::ConstObj >;
template class ObjectClassWrapper< double, object::LookupObj1D >;
template class ObjectClassWrapper< double, object::LookupObj2D >;
template class ObjectClassWrapper< double, object::LookupObj1dWithState >;
template class ObjectClassWrapper< double, object::LookupObj2dWithState >;

} /* namespace factory */
