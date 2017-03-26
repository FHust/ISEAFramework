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
#ifndef _THREADED_FOR_LOOP_
#define _THREADED_FOR_LOOP_
#ifdef BOOST_THREAD

#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include "../misc/aligned_to_cache_line.h"


namespace threading
{

class ThreadedForLoop : boost::noncopyable
{
    public:
    struct LoopFunctorInterface
    {
        virtual void Iterate( size_t i ) = 0;
    };

    private:
    class ThreadFunction
    {
        public:
        /**
        * @param[in] rangeModifier ThreadFunction instances are stored in a vector and rangeModifier is the index of the
        * position of this instance in the vector
        * @param[in] numberOfThreads number of threads (and so ThreadedForLoop) used by ThreadedForLoop instance that
        * contains them
        * @param[in] numberOfLoops,loopFunctor these are altered from outside before new loop is done
        * @param[in] synchronizeLoop Synchronizes before and after a loop is triggered
        * @param[in] terminate Causes operator() to end when set to true
        */
        ThreadFunction( size_t rangeModifier, size_t numberOfThreads, const size_t &numberOfLoops,
                        LoopFunctorInterface *&loopFunctor, boost::barrier *synchronizeLoop = 0
                                                            //, boost::condition_variable *cvStartOfLoop = 0
                                                            //, boost::mutex *mutexStartOfLoop = 0
                        ,
                        const boost::atomic< bool > *terminate = 0 );
        ~ThreadFunction();
        /// Actual loop is executed inside this function
        void DoLoop();
        /// The interface function used by std::thread and boost::thread
        void operator()();

        private:
        const size_t mRangeModifier;
        const size_t mNumberOfThreads;
        const size_t &mNumberOfLoops;
        LoopFunctorInterface *&mLoopFunctor;
        boost::barrier *mSynchronizeLoop;
        // boost::condition_variable *mCvStartOfLoop;
        // boost::mutex *mMutexStartOfLoop;
        const boost::atomic< bool > *mTerminate;
    };

    public:
    explicit ThreadedForLoop( size_t numberOfThreads = boost::thread::hardware_concurrency() );
    ~ThreadedForLoop();
    void DoLoop( LoopFunctorInterface &loopFunctor, size_t numberOfLoops );

    private:
    ALIGNED_TO_64_BYTE size_t mNumberOfLoops;
    ALIGNED_TO_64_BYTE LoopFunctorInterface *mLoopFunctor;
    const size_t mNumberOfThreads;
    boost::barrier mSynchronizeLoop;
    // boost::condition_variable mCvStartOfLoop;
    // boost::mutex mMutexStartOfLoop;
    boost::atomic< bool > mTerminate;
    std::vector< boost::shared_ptr< boost::thread > > mThreads;
    ThreadFunction mThreadFunctionOfRootThread;
};

}    // namespace threading
#endif    /* BOOST_THREAD */
#endif    // Include guard
