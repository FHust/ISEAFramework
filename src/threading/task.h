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
 * Thread.h
 *
 *  Created on: 08.08.2013
 *      Author: chris
 */

#ifndef THREAD_H_
#define THREAD_H_


#include "communicators/dummyCommunicator.h"
#include "taskBase.h"
#include <boost/bind.hpp>
#include <vector>

namespace threading
{

/// Communicator specific base class for all tasks
template < class CommunicatorType = DummyCommunicator >
class Task : public TaskBase< CommunicatorType >
{
    public:
    /// Constructor. The minimal and the maximal workernumber can be declared with both parameters
    Task( const size_t minimalNumberOfWorker, const size_t maximalNumberOfWorker )
        : TaskBase< CommunicatorType >( minimalNumberOfWorker, maximalNumberOfWorker )
    {
    }

    /// Constructor. Minimal worker:1 Maximal worker:Inf
    Task()
        : TaskBase< CommunicatorType >()
    {
    }

    /// The work of a Task will be done in here.
    virtual void DoWork() = 0;
};

#ifdef BOOST_MPI

/// MPI specialized base class for all tasks
template <>
class Task< MPICommunicator > : public TaskBase< MPICommunicator >
{
    public:
    /// Constructor. The minimal and the maximal workernumber can be declared with both parameters
    Task( const size_t minimalNumberOfWorker, const size_t maximalNumberOfWorker );

    /// Constructor. Minimal worker:1 Maximal worker:Inf
    Task();

    /// Method for preparing data that will be sent to the other worker processes of this Task
    virtual std::vector< boost::shared_ptr< TaskData > > GetPreparedDataForThreads() = 0;

    /// Method for processing data received from the other worker processes of this Task
    virtual void UseProcessedDataFromThreads( const std::vector< boost::shared_ptr< TaskData > >& dataVec ) = 0;

    /// The work of a Task will be done in here. Abstract method which is run by the remote processes (rank>0).
    virtual boost::shared_ptr< TaskData > DoNecessaryWork( size_t workerID, boost::shared_ptr< TaskData > data ) = 0;

    /// The optional work of a Task will be done in here. If the work is done in a cylce, then return true. Abstract
    /// method which is run by the remote processes (rank>0).
    virtual bool DoOptionalWork( size_t workerID );
};

#endif    // BOOST_MPI

#ifdef BOOST_THREAD

/// Threading specialized base class for all tasks
template <>
class Task< ThreadCommunicator > : public TaskBase< ThreadCommunicator >
{
    public:
    /// Constructor. The minimal and the maximal workernumber can be declared with both parameters
    Task( const size_t minimalNumberOfWorker, const size_t maximalNumberOfWorker );

    /// Constructor. Minimal worker:1 Maximal worker:Inf
    Task();

    /// The optional work of a Task will be done in here. Abstract method which is run by the workerthreads.
    virtual void DoNecessaryWork( size_t workerID ) = 0;

    /// The optional work of a Task will be done in here. If the work is done in a cylce, then return true. Abstract
    /// method which is run by the workerthreads.
    virtual bool DoOptionalWork( size_t workerID );

    /// Abstract method for preprocessing data for the workerthreads
    virtual void PrepareDataForThreads() = 0;

    /// Abstract method for postprocessing data from the workerthreads
    virtual void UseDataFromThreads() = 0;
};

#endif    // BOOST_THREAD

} /* namespace threading */

//#endif /* defined(BOOST_MPI) || defined(BOOST_THREAD) */

#endif /* THREAD_H_ */
