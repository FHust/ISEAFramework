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
 * ThreadManager.h
 *
 *  Created on: 08.08.2013
 *      Author: chris
 */

#ifndef THREADMANAGER_H_
#define THREADMANAGER_H_

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "taskGroup.h"
#include "communicators/communicatorInclude.h"


/// All kind of code for parallel computing
namespace threading
{

/// This class is designed for controlling the different TaskGroups used in parallel computing.
template < class CommunicatorType = DummyCommunicator >
class ThreadManager
{

    public:
    /// Constructor of ThreadManager. An instance of a derived class of CommunicatorBase has to be provided as an
    /// argument
    ThreadManager( CommunicatorType *communicator );

    virtual ~ThreadManager();

    /// Register a new TaskGroup in the ThreadManager.
    void AddTaskGroup( TaskGroup< CommunicatorType > *t );

    /// After all TaskGroups are registered, this method has to be called. It creates and runs the threads
    void CreateAndRunThreads();

    /// This method is used for datatransfer and synchronization of the threads with the rootthread
    void SendDataToThreads() const;

    /// This method is used for datatransfer and synchronization of the threads with the rootthread
    void ReceiveDataFromThreads() const;

    /// It can be tested, if the necessary parts of all TaskGroups are finished in the current cycle
    bool IsReadyForReceiveData() const;

    private:
    /// Default constructor disabled
    ThreadManager() {}

    /// This is a helper method for assigning all available threads or processes to the registered TaskGroup. The number
    /// of workers for each TaskGroup is set here.
    std::vector< size_t > AssignThreadsToTasks( const size_t totalThreadCount );

    std::vector< boost::shared_ptr< TaskGroup< CommunicatorType > > > mTaskGroupVec;
    boost::scoped_ptr< CommunicatorType > mCommunicator;
};

template < class CommunicatorType >
ThreadManager< CommunicatorType >::ThreadManager( CommunicatorType *communicator )
    : mCommunicator( communicator )
{
}

template < class CommunicatorType >
ThreadManager< CommunicatorType >::~ThreadManager()
{
    mCommunicator->StopAllProcesses();
}

template < class CommunicatorType >
void ThreadManager< CommunicatorType >::AddTaskGroup( TaskGroup< CommunicatorType > *t )
{
    mTaskGroupVec.push_back( boost::shared_ptr< TaskGroup< CommunicatorType > >( t ) );
}

/// Dummy method
template < class CommunicatorType >
void ThreadManager< CommunicatorType >::CreateAndRunThreads()
{
    for ( size_t groupID = 0; groupID < mTaskGroupVec.size(); ++groupID )
        mTaskGroupVec.at( groupID )->Init( mCommunicator.get(), groupID, 1 );
}

#ifdef BOOST_MPI

/// After all ThreadedTask are registered, this method has to be called. It creates and runs the threads
template <>
void ThreadManager< MPICommunicator >::CreateAndRunThreads();

#endif    // BOOST_MPI

#ifdef BOOST_THREAD

/// After all ThreadedTask are registered, this method has to be called. It creates and runs the threads
template <>
void ThreadManager< ThreadCommunicator >::CreateAndRunThreads();

#endif    // BOOST_THREAD

template < class CommunicatorType >
void ThreadManager< CommunicatorType >::SendDataToThreads() const
{
    for ( size_t groupID = 0; groupID < mTaskGroupVec.size(); ++groupID )
    {
        mTaskGroupVec.at( groupID )->SendDataToThreads();
    }
}

template < class CommunicatorType >
void ThreadManager< CommunicatorType >::ReceiveDataFromThreads() const
{
    mCommunicator->FinishThisCycle();
    for ( size_t groupID = 0; groupID < mTaskGroupVec.size(); ++groupID )
    {
        mTaskGroupVec.at( groupID )->ReceiveDataFromThreads();
    }
}

template < class CommunicatorType >
bool ThreadManager< CommunicatorType >::IsReadyForReceiveData() const
{
    return mCommunicator->IsReady();
}

template < class CommunicatorType >
std::vector< size_t > ThreadManager< CommunicatorType >::AssignThreadsToTasks( const size_t totalThreadCount )
{
    size_t minThreadCount = 0;
    size_t maxThreadCount = 0;

    for ( size_t groupID = 0; groupID < mTaskGroupVec.size(); ++groupID )
    {
        minThreadCount += mTaskGroupVec.at( groupID )->GetMinimalNumberOfWorker();
        maxThreadCount += mTaskGroupVec.at( groupID )->GetMaximalNumberOfWorker();
    }
    if ( totalThreadCount < minThreadCount )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TooFewThreads", totalThreadCount, minThreadCount );

    if ( maxThreadCount && totalThreadCount > maxThreadCount )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TooFewWorker", totalThreadCount, maxThreadCount );

    std::vector< size_t > groupArrangementVector( mTaskGroupVec.size(), 0 );

    size_t createdThreads = 0;
    // Create the neccessary threads for each threadedtask.
    while ( createdThreads < totalThreadCount )
    {
        size_t previouslyCreatedThreads = createdThreads;
        for ( size_t groupID = 0; groupID < mTaskGroupVec.size() && createdThreads < totalThreadCount; ++groupID )
        {
            TaskGroup< CommunicatorType > *taskGroup = mTaskGroupVec.at( groupID ).get();
            if ( groupArrangementVector.at( groupID ) < taskGroup->GetMinimalNumberOfWorker() )
            {
                ++groupArrangementVector.at( groupID );
                ++createdThreads;
            }
        }
        if ( previouslyCreatedThreads == createdThreads )
            break;
    }
    // distribute the unused thread
    while ( createdThreads < totalThreadCount )
    {
        size_t previouslyCreatedThreads = createdThreads;
        for ( size_t groupID = 0; groupID < mTaskGroupVec.size() && createdThreads < totalThreadCount; ++groupID )
        {
            TaskGroup< CommunicatorType > *taskGroup = mTaskGroupVec.at( groupID ).get();
            if ( groupArrangementVector.at( groupID ) < taskGroup->GetMaximalNumberOfWorker() )
            {
                ++groupArrangementVector.at( groupID );
                ++createdThreads;
            }
        }
        if ( previouslyCreatedThreads == createdThreads )
            break;
    }

    return groupArrangementVector;
}

} /* namespace threading */

#endif /* THREADMANAGER_H_ */
