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
 * taskGroup.cpp
 *
 *  Created on: 17.01.2014
 *      Author: chris
 */

#include "taskGroup.h"

namespace threading
{

#ifdef BOOST_MPI

template <>
void TaskGroup< MPICommunicator >::SendDataToThreads() const
{
    std::vector< TaskGroupData > groupDataVec( mNumberOfWorker + 1, TaskGroupData( mTaskVec.size() ) );

    for ( size_t taskIndex = 0; taskIndex < mTaskVec.size(); ++taskIndex )
    {
        std::vector< boost::shared_ptr< TaskData > > dataVec = mTaskVec.at( taskIndex )->GetPreparedDataForThreads();

        for ( size_t workerIndex = 0; workerIndex < dataVec.size(); ++workerIndex )
            groupDataVec.at( workerIndex + 1 ).mDataVec.at( taskIndex ) = dataVec.at( workerIndex );
    }

    // SEND
    mCommunicator->SendDataToRemoteTasks( mGroupIndex, groupDataVec );
}

template <>
void TaskGroup< MPICommunicator >::RunTasks( size_t workerID ) const
{
    std::vector< boost::shared_ptr< Task< MPICommunicator > > > tasksWithUncompletedOptionalWork;

    while ( true )
    {
        // RECEIVE
        TaskGroupData groupData = mCommunicator->ReceiveDataFromRoot( mGroupIndex );

        if ( mCommunicator->IsStop() )
            break;

        // Necessary Work
        for ( size_t taskIndex = 0; taskIndex < mTaskVec.size(); ++taskIndex )
        {
            boost::shared_ptr< Task< MPICommunicator > > task = mTaskVec.at( taskIndex );

            if ( workerID >= task->GetNumberOfWorker() )
                continue;

            boost::shared_ptr< TaskData > datain = groupData.mDataVec.at( taskIndex );
            groupData.mDataVec.at( taskIndex ) = task->DoNecessaryWork( workerID, datain );

            tasksWithUncompletedOptionalWork.push_back( task );
        }

        mCommunicator->SetReady();

        // Optional Work
        size_t index = 0;
        while ( !mCommunicator->IsCycleFinished() && tasksWithUncompletedOptionalWork.size() )
        {
            const bool isCompleted = tasksWithUncompletedOptionalWork.at( index )->DoOptionalWork( workerID );

            if ( isCompleted )
                tasksWithUncompletedOptionalWork.erase( tasksWithUncompletedOptionalWork.begin() + index );
            else
                ++index;

            if ( index >= tasksWithUncompletedOptionalWork.size() )
                index = 0;
        }
        tasksWithUncompletedOptionalWork.clear();

        // SEND
        mCommunicator->SendDataToRoot( mGroupIndex, groupData );
    }
}

template <>
void TaskGroup< MPICommunicator >::ReceiveDataFromThreads() const
{
    // RECEIVE
    std::vector< TaskGroupData > groupDataVec;
    mCommunicator->ReceiveDataFromRemoteTasks( mGroupIndex, groupDataVec );

    for ( size_t taskIndex = 0; taskIndex < mTaskVec.size(); ++taskIndex )
    {
        std::vector< boost::shared_ptr< TaskData > > dataVec;

        for ( size_t workerIndex = 0; workerIndex < mTaskVec.at( taskIndex )->GetNumberOfWorker(); ++workerIndex )
            dataVec.push_back( groupDataVec.at( workerIndex + 1 ).mDataVec.at( taskIndex ) );

        mTaskVec.at( taskIndex )->UseProcessedDataFromThreads( dataVec );
    }
}

template <>
void TaskGroup< MPICommunicator >::RegisterWorker( size_t workerID )
{
    if ( !this->mIsInitialized )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "StillNotInitialized", "TaskGroup" );

    RemoteTaskExecutionHandler handler = boost::bind( &TaskGroup< MPICommunicator >::RunTasks, this, _1 );
    this->mCommunicator->RegisterWorker( workerID, handler );
}

#endif    // BOOST_MPI

#ifdef BOOST_THREAD

template <>
void TaskGroup< ThreadCommunicator >::SendDataToThreads() const
{
    for ( size_t taskIndex = 0; taskIndex < mTaskVec.size(); ++taskIndex )
        mTaskVec.at( taskIndex )->PrepareDataForThreads();

    mCommunicator->SyncBeforeThreadWork();
}

template <>
void TaskGroup< ThreadCommunicator >::RunTasks( size_t workerID ) const
{
    std::vector< boost::shared_ptr< Task< ThreadCommunicator > > > tasksWithUncompletedOptionalWork;

    while ( true )
    {
        mCommunicator->SyncBeforeThreadWork();

        if ( mCommunicator->IsStop() )
            break;

        // Necessary Work
        for ( size_t taskIndex = 0; taskIndex < mTaskVec.size(); ++taskIndex )
        {
            boost::shared_ptr< Task< ThreadCommunicator > > task = mTaskVec.at( taskIndex );

            if ( workerID >= task->GetNumberOfWorker() )
                continue;

            task->DoNecessaryWork( workerID );

            tasksWithUncompletedOptionalWork.push_back( task );
        }

        mCommunicator->SetReady();

        // Optional Work
        size_t index = 0;
        while ( !mCommunicator->IsCycleFinished() && tasksWithUncompletedOptionalWork.size() )
        {
            const bool isCompleted = tasksWithUncompletedOptionalWork.at( index )->DoOptionalWork( workerID );

            if ( isCompleted )
                tasksWithUncompletedOptionalWork.erase( tasksWithUncompletedOptionalWork.begin() + index );
            else
                ++index;

            if ( index >= tasksWithUncompletedOptionalWork.size() )
                index = 0;
        }
        tasksWithUncompletedOptionalWork.clear();

        mCommunicator->SyncAfterThreadWork();
    }
}

template <>
void TaskGroup< ThreadCommunicator >::ReceiveDataFromThreads() const
{
    mCommunicator->SyncAfterThreadWork();

    for ( size_t taskIndex = 0; taskIndex < mTaskVec.size(); ++taskIndex )
        mTaskVec.at( taskIndex )->UseDataFromThreads();
}

template <>
void TaskGroup< ThreadCommunicator >::RegisterWorker( size_t )
{
    if ( !this->mIsInitialized )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "StillNotInitialized", "TaskGroup" );

    RemoteTaskExecutionHandler handler = boost::bind( &TaskGroup< ThreadCommunicator >::RunTasks, this, _1 );
    this->mCommunicator->RegisterWorker( this->mNumberOfWorker, handler );
}

#endif    // BOOST_THREAD


} /* namespace threading */
