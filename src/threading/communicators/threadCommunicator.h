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
 * ThreadCommunicator.h
 *
 *  Created on: 13.08.2013
 *      Author: chris
 */

#ifndef THREADCOMMUNICATOR_H_
#define THREADCOMMUNICATOR_H_

#ifdef BOOST_THREAD

#include "communicatorBase.h"
#include <map>
#include <vector>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/atomic.hpp>

namespace threading
{

/// Thread communication class
class ThreadCommunicator
{

    public:
    /// Constructor. There will be as many workerthreads as cores
    ThreadCommunicator();

    /// Constructor. The number of workerthreads can be supplied via parameter and can be greater than the cores in the
    /// system
    ThreadCommunicator( size_t threadCount );

    /// Destructor
    virtual ~ThreadCommunicator();

    /// Get the number of threads or processes that are able to run wokers
    size_t GetSystemThreadCount() const;

    /// Register the workers functor and prepares for numberOfWorkers workerthreads
    void RegisterWorker( size_t numberOfWorkers, RemoteTaskExecutionHandler handler );

    /// Executes the functors
    void RunThreads();

    /// Rise barrier for synchronization
    void SyncBeforeThreadWork();

    /// Rise barrier for synchronization
    void SyncAfterThreadWork();

    /// Tell the master thread/process, that a worker has finished its necessary workpart
    void SetReady();

    /// Checks, if a workcycle can be finished
    bool IsReady() const;

    /// Make the worker finish their workcycle
    void FinishThisCycle();

    /// Checks, if a worker needs to finish
    bool IsCycleFinished() const;

    /// Stops all threads/processes. This method will be used for exiting the program
    void StopAllProcesses();

    /// Checks, if a stop command is send
    bool IsStop() const;

    /// Get the ID of the callers thread/process
    size_t GetThreadID() const;

    private:
    std::vector< boost::shared_ptr< boost::thread > > mThreadVec;
    std::vector< RemoteTaskExecutionHandler > mRemoteHandlerVec;

    boost::atomic< size_t > mReadyRemoteTasks;
    boost::atomic< bool > mNeedFinish;
    boost::atomic< bool > mNeedStop;
    boost::scoped_ptr< boost::barrier > mSendBarrier;
    boost::scoped_ptr< boost::barrier > mRecvBarrier;
    size_t mThreadCount;
};

} /* namespace threading */

#endif /* BOOST_THREAD */
#endif /* THREADCOMMUNICATOR_H_ */
