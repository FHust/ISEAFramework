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
 * statefactorybuilder.h
 *
 *  Created on: 07.10.2013
 *      Author: chris
 */

#ifndef STATEFACTORYBUILDER_H_
#define STATEFACTORYBUILDER_H_

#include "stateclasswrapper.h"

#include "../factory.h"

namespace factory
{

/// Builds a Factory for ::state
Factory< ::state::Dgl_state, ArgumentTypeState >* BuildStateFactory();

} /* namespace factory */

#endif /* STATEFACTORYBUILDER_H_ */
