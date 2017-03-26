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
 * ThreadCommunicator.cpp
 *
 *  Created on: 13.08.2013
 *      Author: chris
 */

#ifdef BOOST_MPI

#include "mpiCommunicator.h"
#include <mpi.h>

namespace threading
{

MPICommunicator::MPICommunicator( int programArgc, char *programArgv[] )
    : mEnvironment( new boost::mpi::environment( programArgc, programArgv ) )
    , mWorldMPICommunicator( new boost::mpi::communicator() )
    , mReadyRemoteTasks( 0 )
    , mWorkerID( 0 )
    , mIsStop( false )
    , mIsCycleFinishRequestActive( false )
    , mIsWorkerReadyRequestActive( false )
{
}

MPICommunicator::~MPICommunicator()
{
    //        if(!mEnvironment->finalized())
    //            mEnvironment->abort( 0 );
}

size_t MPICommunicator::GetSystemThreadCount() const { return mWorldMPICommunicator->size() - 1; }

void MPICommunicator::RegisterWorker( size_t workerID, RemoteTaskExecutionHandler handler )
{
    mWorkerID = workerID;
    mRemoteHandler = handler;
}

void MPICommunicator::InitCommunicationGroups( std::vector< size_t > groupArrangementVector )
{

    size_t currentRank = 1;
    for ( size_t groupID = 0; groupID < groupArrangementVector.size(); ++groupID )
    {

        std::vector< int > rankIDVec( groupArrangementVector.at( groupID ) + 1, 0 );
        for ( size_t i = 1; i <= groupArrangementVector.at( groupID ); ++i )
            rankIDVec.at( i ) = currentRank++;

        boost::mpi::group myGroup = mWorldMPICommunicator->group().include( rankIDVec.begin(), rankIDVec.end() );

        mRemoteTaskMPICommunicatorVec.push_back( boost::mpi::communicator( *mWorldMPICommunicator, myGroup ) );
    }
}

void MPICommunicator::RunThreads()
{
    const size_t myRang = mWorldMPICommunicator->rank();
    //        const size_t mySize = (size_t)mWorldMPICommunicator->size();

    mWorldMPICommunicator->barrier();

    if ( myRang == 0 )
    {
        return;
    }
    else
    {
        mRemoteHandler( mWorkerID );
        mEnvironment.reset();
        exit( 0 );    // is this right?
    }
}

void MPICommunicator::SendDataToAllRemoteTasks( TaskGroupData &data )
{
    for ( size_t i = 0; i < mRemoteTaskMPICommunicatorVec.size(); ++i )
    {
        std::vector< TaskGroupData > datavec( mRemoteTaskMPICommunicatorVec.at( i ).size(), data );
        boost::mpi::scatter( mRemoteTaskMPICommunicatorVec.at( i ), datavec, data, 0 );
    }
}

void MPICommunicator::SendDataToRemoteTasks( const size_t groupID, const std::vector< TaskGroupData > &datavec )
{
    mReadyRemoteTasks = 0;
    TaskGroupData data;
    try
    {
        boost::mpi::scatter( mRemoteTaskMPICommunicatorVec.at( groupID ), datavec, data, 0 );
    }
    catch ( std::exception &ex )
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             ( "SendDataToRemoteTasks: " + ex.what() ).c_str() );
    }
}

void MPICommunicator::ReceiveDataFromRemoteTasks( const size_t groupID, std::vector< TaskGroupData > &datavec )
{
    TaskGroupData data;
    try
    {
        boost::mpi::gather( mRemoteTaskMPICommunicatorVec.at( groupID ), data, datavec, 0 );
    }
    catch ( std::exception &ex )
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             ( "ReceiveDataFromRemoteTasks: " + ex.what() ).c_str() );
    }
}

void MPICommunicator::SendDataToRoot( const size_t groupID, const TaskGroupData &data )
{
    mCycleFinishRequest.wait();
    mIsCycleFinishRequestActive = false;

    boost::mpi::gather( mRemoteTaskMPICommunicatorVec.at( groupID ), data, 0 );
}

TaskGroupData MPICommunicator::ReceiveDataFromRoot( const size_t groupID )
{
    TaskGroupData data;
    boost::mpi::scatter( mRemoteTaskMPICommunicatorVec.at( groupID ), data, 0 );
    mIsStop = data.mIsStop;

    return data;
}

void MPICommunicator::SetReady()
{
    bool ready = true;
    boost::mpi::request req = mWorldMPICommunicator->isend( 0, 0, ready );
    req.wait();
}

bool MPICommunicator::IsReady()
{
    bool ready = false;
    bool isFirst = true;

    while ( ready || isFirst )    // Mindestens ein  Durchlauf.
    {
        if ( !mIsWorkerReadyRequestActive )
        {
            mWorkerReadyRequest = mWorldMPICommunicator->irecv( boost::mpi::any_source, 0, ready );
            mIsWorkerReadyRequestActive = true;
        }

        boost::optional< boost::mpi::status > status = mWorkerReadyRequest.test();

        if ( status )    // Wenn empfangen, dann Request freigeben!
        {
            mWorkerReadyRequest.wait();
            mIsWorkerReadyRequestActive = false;
        }

        if ( ready )
            ++mReadyRemoteTasks;

        isFirst = false;
    }

    return mReadyRemoteTasks == (size_t)mWorldMPICommunicator->size() - 1;
}

void MPICommunicator::FinishThisCycle()
{
    bool ready = false;

    while ( mReadyRemoteTasks < (size_t)mWorldMPICommunicator->size() - 1 )
    {
        if ( !mIsWorkerReadyRequestActive )
        {
            mWorkerReadyRequest = mWorldMPICommunicator->irecv( boost::mpi::any_source, 0, ready );
            mIsWorkerReadyRequestActive = true;
        }

        mWorkerReadyRequest.wait();
        mIsWorkerReadyRequestActive = false;

        if ( ready )
            ++mReadyRemoteTasks;
    }

    bool finish = true;
    std::vector< boost::mpi::request > requestVec;
    for ( int rankID = 1; rankID < mWorldMPICommunicator->size(); ++rankID )
        requestVec.push_back( mWorldMPICommunicator->isend( rankID, 1, finish ) );
    boost::mpi::wait_all( &requestVec[0], &requestVec[0] + mWorldMPICommunicator->size() - 1 );
}

bool MPICommunicator::IsCycleFinished()
{
    bool finish = false;

    if ( !mIsCycleFinishRequestActive )
    {
        mCycleFinishRequest = mWorldMPICommunicator->irecv( 0, 1, finish );
        mIsCycleFinishRequestActive = true;
    }

    mCycleFinishRequest.test();

    return finish;
}

void MPICommunicator::StopAllProcesses()
{
    if ( mWorldMPICommunicator->rank() != 0 )
        return;

    TaskGroupData data;
    data.mIsStop = true;
    SendDataToAllRemoteTasks( data );
}

bool MPICommunicator::IsStop() const { return mIsStop; }

size_t MPICommunicator::GetThreadID() const { return mWorldMPICommunicator->rank(); }

} /* namespace threading */

#endif /* BOOST_MPI */
