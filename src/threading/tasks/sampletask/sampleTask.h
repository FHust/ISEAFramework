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
 * sampleTask.h
 *
 *  Created on: 08.08.2013
 *      Author: chris
 */

#ifndef SAMPLETask_H_
#define SAMPLETask_H_

#include "../../task.h"
#include "sampleTaskInputData.h"
#include "sampleTaskOutputData.h"
#if defined( BOOST_MPI ) || defined( BOOST_THREAD )
#include "boost/date_time.hpp"
#endif

namespace threading
{
namespace sampletask
{

template < class CommunicatorType = DummyCommunicator >
class SampleTask : public Task< CommunicatorType >
{
    public:
    void DoWork() {}

    protected:
    void Init() {}
};

#ifdef BOOST_MPI

template <>
class SampleTask< MPICommunicator > : public Task< MPICommunicator >
{
    public:
    SampleTask()
        : Task< MPICommunicator >()
        , mCountOfOptionalRuns( 0 )
        , mOptionalWorkStarted( false )
    {
    }

    SampleTask( const size_t minimalNumberOfWorker, const size_t maximalNumberOfWorker )
        : Task< MPICommunicator >( minimalNumberOfWorker, maximalNumberOfWorker )
        , mCountOfOptionalRuns( 0 )
        , mOptionalWorkStarted( false )
    {
    }

    virtual ~SampleTask() {}

    std::vector< boost::shared_ptr< TaskData > > GetPreparedDataForThreads()
    {
        std::vector< boost::shared_ptr< TaskData > > dataVec;

        for ( size_t workerID = 0; workerID < this->mNumberOfWorker; ++workerID )
        {
            SampleTaskInputData* inputData = new SampleTaskInputData();
            inputData->mysecondtestvar = workerID * 2.13;
            dataVec.push_back( boost::shared_ptr< TaskData >( inputData ) );
        }

        return dataVec;
    }

    void UseProcessedDataFromThreads( const std::vector< boost::shared_ptr< TaskData > >& dataVec )
    {
        for ( size_t workerID = 0; workerID < this->mNumberOfWorker; ++workerID )
        {
            boost::shared_ptr< SampleTaskOutputData > outputData =
             boost::static_pointer_cast< SampleTaskOutputData >( dataVec.at( workerID ) );
            std::cout << "SampleTask (worker " << workerID << ") has been dooing optional work for "
                      << outputData->mMiliSecsOfOptionalWork << " milliseconds" << std::endl;
        }
    }

    boost::shared_ptr< TaskData > DoNecessaryWork( size_t workerID, boost::shared_ptr< TaskData > data )
    {
        boost::shared_ptr< SampleTaskInputData > datain = boost::static_pointer_cast< SampleTaskInputData >( data );

        boost::shared_ptr< SampleTaskOutputData > outputData( new SampleTaskOutputData() );
        outputData->mytestvar = datain->mysecondtestvar * 1.1;

        boost::posix_time::time_duration elapsed = mFinishOfOptionalWork - mStartOfOptionalWork;
        outputData->mMiliSecsOfOptionalWork = elapsed.total_milliseconds();

        mCountOfOptionalRuns = 0;    // RESET
        mOptionalWorkStarted = false;

        return outputData;
    }

    // return value
    bool DoOptionalWork( size_t workerID )
    {
        mCountOfOptionalRuns++;

        if ( !mOptionalWorkStarted )
        {
            mOptionalWorkStarted = true;
            mStartOfOptionalWork = boost::posix_time::microsec_clock::local_time();
            mFinishOfOptionalWork = mStartOfOptionalWork;
        }
        else
        {
            mFinishOfOptionalWork = boost::posix_time::microsec_clock::local_time();
        }

        return false;
    }

    protected:
    void Init()
    {
        boost::posix_time::ptime t = boost::posix_time::microsec_clock::local_time();
        mStartOfOptionalWork = t;
        mFinishOfOptionalWork = t;
    }

    private:
    size_t mCountOfOptionalRuns;
    bool mOptionalWorkStarted;
    boost::posix_time::ptime mStartOfOptionalWork;
    boost::posix_time::ptime mFinishOfOptionalWork;
};

#endif    // BOOST_MPI

#ifdef BOOST_THREAD

template <>
class SampleTask< ThreadCommunicator > : public Task< ThreadCommunicator >
{
    public:
    SampleTask()
        : Task< ThreadCommunicator >()
    {
    }

    SampleTask( const size_t minimalNumberOfWorker, const size_t maximalNumberOfWorker )
        : Task< ThreadCommunicator >( minimalNumberOfWorker, maximalNumberOfWorker )
    {
    }

    virtual ~SampleTask() {}

    void DoNecessaryWork( size_t workerID ) {}

    bool DoOptionalWork( size_t workerID )
    {
        if ( !mOptionalWorkStarted[workerID] )
        {
            mOptionalWorkStarted[workerID] = true;
            mStartOfOptionalWork[workerID] = boost::posix_time::microsec_clock::local_time();
            mFinishOfOptionalWork[workerID] = mStartOfOptionalWork[workerID];
        }
        else
        {
            mFinishOfOptionalWork[workerID] = boost::posix_time::microsec_clock::local_time();
        }

        return false;
    }

    void PrepareDataForThreads() {}

    void UseDataFromThreads()
    {
        for ( size_t workerID = 0; workerID < this->mNumberOfWorker; ++workerID )
        {
            boost::posix_time::time_duration elapsed = mFinishOfOptionalWork[workerID] - mStartOfOptionalWork[workerID];
            size_t msecs = elapsed.total_milliseconds();
            std::cout << "SampleTask (worker " << workerID << ") has been doing optional work for " << msecs
                      << " milliseconds" << std::endl;
            mOptionalWorkStarted[workerID] = false;
        }
    }

    protected:
    void Init()
    {
        mOptionalWorkStarted.resize( this->mNumberOfWorker, false );

        boost::posix_time::ptime t = boost::posix_time::microsec_clock::local_time();

        mStartOfOptionalWork.resize( this->mNumberOfWorker, t );
        mFinishOfOptionalWork.resize( this->mNumberOfWorker, t );
    }

    private:
    std::vector< bool > mOptionalWorkStarted;
    std::vector< boost::posix_time::ptime > mStartOfOptionalWork;
    std::vector< boost::posix_time::ptime > mFinishOfOptionalWork;
};

#endif    // BOOST_THREAD
}
} /* namespace threading */

#endif /* SAMPLETask_H_ */
