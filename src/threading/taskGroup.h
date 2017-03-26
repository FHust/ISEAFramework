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
 * taskGroup.h
 *
 *  Created on: 17.01.2014
 *      Author: chris
 */

#ifndef TASKGROUP_H_
#define TASKGROUP_H_

// STD
#include <vector>
#include <cstdlib>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "task.h"
#include "communicators/communicatorInclude.h"
#include "../exceptions/error_proto.h"

namespace threading
{
/// TaskGruppe. Mehrere task können einer Gruppe zugewiesen werden. Die Min- und MaxWorker Angaben der einzelnen Tasks
/// werden eingehalten.
template < class CommunicatorType = DummyCommunicator >
class TaskGroup
{
    public:
    TaskGroup();

    virtual ~TaskGroup();

    /// Register a new Task in the ThreadManager.
    void AddTask( Task< CommunicatorType > *t );

    /// This method is used for datatransfer and synchronization of the threads with the rootthread
    void SendDataToThreads() const;

    /// This method is used for datatransfer and synchronization of the threads with the rootthread
    void ReceiveDataFromThreads() const;

    /// Returns the maximal allowed number of worker
    size_t GetMaximalNumberOfWorker();

    /// Returns the minimal allowed number of worker
    size_t GetMinimalNumberOfWorker();

    /// This method initializes the Tasks with its GroupID and the assigned number of workers. This method is called by
    /// the ThreadManager
    void Init( CommunicatorType *communicator, size_t groupIndex, size_t numberOfWorker );

    /// This method registers a worker for the remote process.
    void RegisterWorker( size_t workerID = 0 );

    private:
    /// This method runs periodically the DoWork methods
    void RunTasks( size_t workerID ) const;

    std::vector< boost::shared_ptr< Task< CommunicatorType > > > mTaskVec;
    CommunicatorType *mCommunicator;
    size_t mNumberOfWorker;
    size_t mGroupIndex;
    bool mIsInitialized;
};

template < class CommunicatorType >
TaskGroup< CommunicatorType >::TaskGroup()
    : mCommunicator( 0 )
    , mNumberOfWorker( 0 )
    , mGroupIndex( 0 )
    , mIsInitialized( false )
{
}

template < class CommunicatorType >
TaskGroup< CommunicatorType >::~TaskGroup()
{
}

template < class CommunicatorType >
void TaskGroup< CommunicatorType >::Init( CommunicatorType *communicator, size_t groupIndex, size_t numberOfWorker )    /// Initializes this Tasks.
{
    mCommunicator = communicator;
    mGroupIndex = groupIndex;
    mNumberOfWorker = numberOfWorker;

    for ( size_t taskIndex = 0; taskIndex < mTaskVec.size(); ++taskIndex )
    {
        size_t assignedWorker = numberOfWorker;
        size_t maximumWorker = mTaskVec.at( taskIndex )->GetMaximalNumberOfWorker();

        if ( assignedWorker > maximumWorker )
            assignedWorker = maximumWorker;

        mTaskVec.at( taskIndex )->Init( assignedWorker );
    }

    mIsInitialized = true;
}

template < class CommunicatorType >
void TaskGroup< CommunicatorType >::AddTask( Task< CommunicatorType > *t )
{
    if ( mIsInitialized )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlreadyInitialized", "TaskGroup" );

    mTaskVec.push_back( boost::shared_ptr< Task< CommunicatorType > >( t ) );
}

template < class CommunicatorType >
void TaskGroup< CommunicatorType >::SendDataToThreads() const
{
}

template < class CommunicatorType >
void TaskGroup< CommunicatorType >::RunTasks( size_t /* workerID */ ) const
{
    for ( size_t taskIndex = 0; taskIndex < mTaskVec.size(); ++taskIndex )
    {
        mTaskVec.at( taskIndex )->DoWork();
    }
}

template < class CommunicatorType >
void TaskGroup< CommunicatorType >::ReceiveDataFromThreads() const
{
}

template < class CommunicatorType >
void TaskGroup< CommunicatorType >::RegisterWorker( size_t )
{
}

#ifdef BOOST_MPI

template <>
void TaskGroup< MPICommunicator >::SendDataToThreads() const;

template <>
void TaskGroup< MPICommunicator >::RunTasks( size_t workerID ) const;

template <>
void TaskGroup< MPICommunicator >::ReceiveDataFromThreads() const;

template <>
void TaskGroup< MPICommunicator >::RegisterWorker( size_t workerID );

#endif    // BOOST_MPI

#ifdef BOOST_THREAD

template <>
void TaskGroup< ThreadCommunicator >::SendDataToThreads() const;

template <>
void TaskGroup< ThreadCommunicator >::RunTasks( size_t workerID ) const;

template <>
void TaskGroup< ThreadCommunicator >::ReceiveDataFromThreads() const;

template <>
void TaskGroup< ThreadCommunicator >::RegisterWorker( size_t );

#endif    // BOOST_THREAD

template < class CommunicatorType >
size_t TaskGroup< CommunicatorType >::GetMaximalNumberOfWorker()
{
    size_t number = 0;
    for ( size_t i = 0; i < mTaskVec.size(); ++i )
    {
        size_t maxnumber = mTaskVec.at( i )->GetMaximalNumberOfWorker();
        if ( maxnumber > number )
            number = maxnumber;
    }
    return number;
}

template < class CommunicatorType >
size_t TaskGroup< CommunicatorType >::GetMinimalNumberOfWorker()
{
    size_t number = 0;
    for ( size_t i = 0; i < mTaskVec.size(); ++i )
    {
        size_t minnumber = mTaskVec.at( i )->GetMinimalNumberOfWorker();
        if ( minnumber > number )
            number = minnumber;
    }
    return number;
}

} /* namespace threading */

#endif /* TASKGROUP_H_ */
