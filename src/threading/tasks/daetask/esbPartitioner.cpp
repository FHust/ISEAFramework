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
 * esbPartitioner.cpp
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#if defined(BOOST_MPI) || defined(BOOST_THREAD)
#include "esbPartitioner.h"

#include "../../../misc/matrixInclude.h"
#include "../../communicators/communicatorInclude.h"

namespace threading { namespace daetask
{

#ifdef BOOST_MPI
    template class ESBPartitioner< ::threading::MPICommunicator, myMatrixType >;
#endif

#ifdef BOOST_THREAD
    template class ESBPartitioner< ::threading::ThreadCommunicator, myMatrixType >;
#endif


} } /* namespace threading */

#endif /* defined(BOOST_MPI) || defined(BOOST_THREAD) */


