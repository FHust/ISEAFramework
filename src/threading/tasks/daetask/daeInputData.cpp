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
 * DAEInputData.cpp
 *
 *  Created on: 14.08.2013
 *      Author: chris
 */

#ifdef BOOST_MPI
#include "daeInputData.h"
#include "../../../misc/matrixInclude.h"


namespace threading { namespace daetask
{

template class DAEInputData<myMatrixType>;

} }/* namespace threading */

#endif /* BOOST_MPI */
