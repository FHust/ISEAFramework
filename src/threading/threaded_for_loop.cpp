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
#include "threaded_for_loop.h"
#ifdef BOOST_THREAD
#include <boost/foreach.hpp>
//#include <boost/utility.hpp>


namespace threading
{

ThreadedForLoop::ThreadFunction::ThreadFunction( size_t rangeModifier, size_t numberOfThreads, const size_t &numberOfLoops,
                                                 LoopFunctorInterface *&loopFunctor, boost::barrier *synchronizeLoop
                                                 //, boost::condition_variable *cvStartOfLoop
                                                 //, boost::mutex *mutexStartOfLoop
                                                 ,
                                                 const boost::atomic< bool > *terminate )
    : mRangeModifier( rangeModifier )
    , mNumberOfThreads( numberOfThreads )
    , mNumberOfLoops( numberOfLoops )
    , mLoopFunctor( loopFunctor )
    , mSynchronizeLoop( synchronizeLoop )
    //, mCvStartOfLoop(cvStartOfLoop)
    //, mMutexStartOfLoop(mutexStartOfLoop)
    , mTerminate( terminate )
{
}

ThreadedForLoop::ThreadFunction::~ThreadFunction() {}

void ThreadedForLoop::ThreadFunction::DoLoop()
{
    const size_t start = ( mNumberOfLoops * mRangeModifier ) / mNumberOfThreads;
    const size_t end = ( mNumberOfLoops * ( mRangeModifier + 1 ) ) / mNumberOfThreads;
    for ( size_t i = start; i < end; ++i )
        mLoopFunctor->Iterate( i );
}

void ThreadedForLoop::ThreadFunction::operator()()
{
    for ( ;; )
    {
        // boost::unique_lock<boost::mutex> lock(*mMutexStartOfLoop);
        // mCvStartOfLoop->wait(lock);
        mSynchronizeLoop->wait();
        if ( mTerminate->load() == true )
            break;
        DoLoop();
        mSynchronizeLoop->wait();
    }
}


ThreadedForLoop::ThreadedForLoop( size_t numberOfThreads )
    : mNumberOfLoops( 0 )
    , mLoopFunctor( 0 )
    , mNumberOfThreads( ( numberOfThreads == 0 ) ? 8 : numberOfThreads )
    , mSynchronizeLoop( mNumberOfThreads )
    , mTerminate( false )
    , mThreadFunctionOfRootThread( mNumberOfThreads - 1, mNumberOfThreads, mNumberOfLoops, mLoopFunctor )
{
    mThreads.resize( mNumberOfThreads - 1 );
    for ( size_t i = 0; i < mNumberOfThreads - 1; ++i )
        mThreads[i].reset( new boost::thread( ThreadFunction( i, mNumberOfThreads, mNumberOfLoops, mLoopFunctor,
                                                              &mSynchronizeLoop
                                                              //, &mCvStartOfLoop, &mMutexStartOfLoop
                                                              ,
                                                              &mTerminate ) ) );
}

ThreadedForLoop::~ThreadedForLoop()
{
    mTerminate.store( true );
    // boost::unique_lock<boost::mutex> lock(mMutexStartOfLoop);
    // mCvStartOfLoop.notify_all();
    mSynchronizeLoop.wait();
    BOOST_FOREACH ( boost::shared_ptr< boost::thread > &thread, mThreads )
        thread->join();
}

void ThreadedForLoop::DoLoop( LoopFunctorInterface &loopFunctor, size_t numberOfLoops )
{
    mNumberOfLoops = numberOfLoops;
    mLoopFunctor = &loopFunctor;

    // boost::unique_lock<boost::mutex> lock(mMutexStartOfLoop);
    // mCvStartOfLoop.notify_all();
    mSynchronizeLoop.wait();
    mThreadFunctionOfRootThread.DoLoop();
    mSynchronizeLoop.wait();
}


}    // namespace threading
#endif /* BOOST_THREAD */
