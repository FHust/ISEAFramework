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

#ifdef BOOST_THREAD
#include "threadCommunicator.h"


namespace threading
{
    ThreadCommunicator::ThreadCommunicator()
            : mReadyRemoteTasks( 0 ),
              mNeedFinish( false ),
              mThreadCount( boost::thread::hardware_concurrency() - 1 )
    {
    }

    ThreadCommunicator::ThreadCommunicator(size_t threadCount)
            : mReadyRemoteTasks( 0 ),
              mNeedFinish( false ),
              mThreadCount( threadCount )
    {
    }

    ThreadCommunicator::~ThreadCommunicator()
    {
    }

    size_t
    ThreadCommunicator::GetSystemThreadCount() const
    {
        return mThreadCount;
    }

    void ThreadCommunicator::RegisterWorker(size_t numberOfWorkers, RemoteTaskExecutionHandler handler)
    {
        for(size_t workerID = 0; workerID < numberOfWorkers; ++workerID)
            mRemoteHandlerVec.push_back(handler);
    }

    void
    ThreadCommunicator::RunThreads()
    {
        typedef std::vector<boost::shared_ptr<boost::thread> >::iterator handlerIt_t;

        size_t totalRemoteThreadCount = mRemoteHandlerVec.size();

        mSendBarrier.reset( new boost::barrier( totalRemoteThreadCount + 1 ) );
        mRecvBarrier.reset( new boost::barrier( totalRemoteThreadCount + 1 ) );

        for(size_t i = 0; i < mRemoteHandlerVec.size(); ++i)
        {
            mThreadVec.push_back( boost::shared_ptr<boost::thread>( new boost::thread( mRemoteHandlerVec.at(i), i ) ) );
        }
    }

    void ThreadCommunicator::SyncBeforeThreadWork()
    {
        mRecvBarrier->wait();
    }

    void ThreadCommunicator::SyncAfterThreadWork()
    {
        mSendBarrier->wait();
        mNeedFinish = false;
        mReadyRemoteTasks = 0;
    }

    void
    ThreadCommunicator::SetReady()
    {
        ++mReadyRemoteTasks;
    }

    bool
    ThreadCommunicator::IsReady() const
    {
        return mReadyRemoteTasks == mRemoteHandlerVec.size();
    }

    void
    ThreadCommunicator::FinishThisCycle()
    {
        mNeedFinish = true;
    }

    bool
    ThreadCommunicator::IsCycleFinished() const
    {
        return mNeedFinish && IsReady();
    }

    void
    ThreadCommunicator::StopAllProcesses()
    {
        mNeedStop = true;
        mRecvBarrier->wait();
        typedef std::vector<boost::shared_ptr<boost::thread> >::iterator threadit_t;
        for( threadit_t it = mThreadVec.begin(); it != mThreadVec.end(); ++it)
        {
            it->get()->join();
        }
    }

    bool
    ThreadCommunicator::IsStop() const
    {
        return mNeedStop;
    }

    size_t
    ThreadCommunicator::GetThreadID() const
    {
        //Boost does not provide a number as an id. This is not an issue right now, but is not soved yet.
//        return boost::this_thread::get_id();
        return 0;
    }

} /* namespace threading */

#endif /* BOOST_THREAD */
