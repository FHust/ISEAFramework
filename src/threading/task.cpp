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
 * Task.cpp
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#include "task.h"

namespace threading
{

template class Task< DummyCommunicator >;

#ifdef BOOST_MPI

    Task<MPICommunicator>::Task(const size_t minimalNumberOfWorker,
                 const size_t maximalNumberOfWorker)
        : TaskBase<MPICommunicator>(minimalNumberOfWorker, maximalNumberOfWorker)
    { }

    Task<MPICommunicator>::Task()
        : TaskBase<MPICommunicator>()
    { }

    bool Task< MPICommunicator >::DoOptionalWork(size_t workerID)
    {
        return true; //Nothing todo
    }

#endif //BOOST_MPI

#ifdef BOOST_THREAD

    Task<ThreadCommunicator>::Task(const size_t minimalNumberOfWorker,
                 const size_t maximalNumberOfWorker)
        : TaskBase<ThreadCommunicator>(minimalNumberOfWorker, maximalNumberOfWorker)
    { }

    Task<ThreadCommunicator>::Task()
        : TaskBase<ThreadCommunicator>()
    { }

    bool Task< ThreadCommunicator >::DoOptionalWork(size_t workerID)
    {
        return true; //Nothing todo
    }

#endif //BOOST_THREAD

#ifdef BOOST_MPI
    template class Task< MPICommunicator >;
#endif

#ifdef BOOST_THREAD
    template class Task< ThreadCommunicator >;
#endif


} /* namespace threading */
