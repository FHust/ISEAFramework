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
 * MPICommunicator.h
 *
 *  Created on: 16.08.2013
 *      Author: chris
 */

#ifndef MPICOMMUNICATOR_H_
#define MPICOMMUNICATOR_H_

#ifdef BOOST_MPI

#include "communicatorBase.h"
#include <vector>
#include <boost/mpi.hpp>
#include "../taskGroupData.h"

namespace threading
{
/// MPI communication class
class MPICommunicator
{

    public:
    /// Constructor. The main methods arguments have to be supplied
    MPICommunicator( int programArgc, char *programArgv[] );
    /// Destructor
    virtual ~MPICommunicator();

    /// Get the number of threads or processes that are able to run wokers
    size_t GetSystemThreadCount() const;

    /// Register the workers functor. Only if not rootprocess (rank>0)
    void RegisterWorker( size_t workerID, RemoteTaskExecutionHandler handler );

    /// Initializes the different communication groups. One group for each ThreadedTask. The woker->process assignment
    /// is given via parameter
    void InitCommunicationGroups( std::vector< size_t > groupArrangementVector );

    /// Executes the functors
    void RunThreads();

    /// Send a DTO to all processes (Partner method: ReceiveDataFromRoot)
    void SendDataToAllRemoteTasks( TaskGroupData & );

    /// Send a vector of DTO to the workerprocesses (Partner method: ReceiveDataFromRoot)
    void SendDataToRemoteTasks( const size_t groupID, const std::vector< TaskGroupData > &datavec );

    /// Receive a vector of DTO from the workerprocesses (Partner method: SendDataToRoot)
    void ReceiveDataFromRemoteTasks( const size_t groupID, std::vector< TaskGroupData > &datavec );

    /// Send a DTO from worker to rootprocess (Partner method: ReceiveDataFromRemoteTasks)
    void SendDataToRoot( const size_t groupID, const TaskGroupData &data );

    /// Receive a DTO from rootprocess (Partner method: SendDataToRemoteTasks)
    TaskGroupData ReceiveDataFromRoot( const size_t groupID );

    /// Tell the master thread/process, that a worker has finished its necessary workpart
    void SetReady();

    /// Checks, if a workcycle can be finished
    bool IsReady();

    /// Make the worker finish their workcycle
    void FinishThisCycle();

    /// Checks, if a worker needs to finish
    bool IsCycleFinished();

    /// Stops all threads/processes. This method will be used for exiting the program
    void StopAllProcesses();

    /// Checks, if a stop command is send
    bool IsStop() const;

    /// Get the ID of the callers thread/process
    size_t GetThreadID() const;

    private:
    boost::shared_ptr< boost::mpi::environment > mEnvironment;
    boost::shared_ptr< boost::mpi::communicator > mWorldMPICommunicator;
    std::vector< boost::mpi::communicator > mRemoteTaskMPICommunicatorVec;
    size_t mReadyRemoteTasks;
    RemoteTaskExecutionHandler mRemoteHandler;
    size_t mWorkerID;
    bool mIsStop;
    boost::mpi::request mCycleFinishRequest;
    bool mIsCycleFinishRequestActive;
    boost::mpi::request mWorkerReadyRequest;
    bool mIsWorkerReadyRequestActive;
};

} /* namespace threading */

#endif /* BOOST_MPI */

#endif /* MPICOMMUNICATOR_H_ */
