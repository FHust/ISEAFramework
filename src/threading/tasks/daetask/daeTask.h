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
 * LookupTask.h
 *
 *  Created on: 08.08.2013
 *      Author: chris
 */

#ifndef DAETASK_H_
#define DAETASK_H_

#include "../../task.h"
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "../../../electrical/serialtwoport.h"
#include "../../../electrical/paralleltwoport.h"
#include "../../../electrical/twoport.h"
#include "../../../electrical/twoport_withchild.h"
#include <boost/shared_ptr.hpp>

#include "virtualTwoPort.h"
#include "esbPartitioner.h"
#include "daeInputData.h"
#include "daeOutputData.h"
#include "../../../misc/cellelementCounter.h"

namespace threading
{
/// Parallelization of the electrical simulation
namespace daetask
{
/// Task for the parallel calculation of voltage equations
template < class CommunicatorType = DummyCommunicator, class MatrixType = myMatrixType >
class DAETask : public Task< CommunicatorType >
{
    public:
    DAETask( boost::shared_ptr< electrical::TwoPort< MatrixType > > rootPort, systm::StateSystemGroup< MatrixType >* stateSystemGroup );

    virtual ~DAETask() {}

    boost::shared_ptr< electrical::TwoPort< MatrixType > > GetNewRootPort();

    std::vector< size_t > GetWorkloadIndices() const;

    void DoWork() {}

    protected:
    void Init();

    private:
    boost::shared_ptr< electrical::TwoPort< MatrixType > > mRootTwoPort;
    systm::StateSystemGroup< MatrixType >* mStateSystemGroup;
    std::vector< size_t > mWorkloadIndices;
};

template < class CommunicatorType, class MatrixType >
DAETask< CommunicatorType, MatrixType >::DAETask( boost::shared_ptr< electrical::TwoPort< MatrixType > > rootPort,
                                                  systm::StateSystemGroup< MatrixType >* stateSystemGroup )
    : Task< CommunicatorType >()
    , mRootTwoPort( rootPort )
    , mStateSystemGroup( stateSystemGroup )
{
}

template < class CommunicatorType, class MatrixType >
boost::shared_ptr< electrical::TwoPort< MatrixType > > DAETask< CommunicatorType, MatrixType >::GetNewRootPort()
{
    return mRootTwoPort;
}

template < class CommunicatorType, class MatrixType >
void DAETask< CommunicatorType, MatrixType >::Init()
{
    mRootTwoPort->SetSystem( mStateSystemGroup );
    mStateSystemGroup->Initialize();
    mRootTwoPort->SetInitialCurrent();
    mRootTwoPort->UpdateStateSystemGroup();

    mWorkloadIndices.push_back( CountCellelements( mRootTwoPort ) );
}

template < class CommunicatorType, class MatrixType >
std::vector< size_t > DAETask< CommunicatorType, MatrixType >::GetWorkloadIndices() const
{
    return mWorkloadIndices;
}


#ifdef BOOST_MPI

/// Task for the parallel calculation of voltage equations
template < class MatrixType >
class DAETask< MPICommunicator, MatrixType > : public Task< MPICommunicator >
{
    public:
    DAETask( boost::shared_ptr< electrical::TwoPort< MatrixType > > rootPort, systm::StateSystemGroup< MatrixType >* stateSystemGroup );

    virtual ~DAETask() {}

    boost::shared_ptr< electrical::TwoPort< MatrixType > > GetNewRootPort();

    std::vector< size_t > GetWorkloadIndices() const;

    /// Method for sending data from rootprocess to the other worker processes of this Task
    std::vector< boost::shared_ptr< TaskData > > GetPreparedDataForThreads();

    /// Method for receiving data from the other worker processes of this Task
    void UseProcessedDataFromThreads( const std::vector< boost::shared_ptr< TaskData > >& dataVec );

    boost::shared_ptr< TaskData > DoNecessaryWork( size_t workerID, boost::shared_ptr< TaskData > inputData );

    //        boost::shared_ptr<TaskData> DoOptionalWork(size_t workerID, boost::shared_ptr<TaskData> inputData);

    protected:
    void Init();

    private:
    /// Default constructor disabled
    DAETask() {}

    boost::shared_ptr< electrical::TwoPort< MatrixType > > mRootTwoPort;
    boost::scoped_ptr< ESBPartitioner< MPICommunicator, MatrixType > > mESBPartitioner;
    systm::StateSystemGroup< MatrixType >* mStateSystemGroup;
    std::vector< size_t > mWorkloadIndices;
};

template < class MatrixType >
DAETask< MPICommunicator, MatrixType >::DAETask( boost::shared_ptr< electrical::TwoPort< MatrixType > > rootPort,
                                                 systm::StateSystemGroup< MatrixType >* stateSystemGroup )
    : Task< MPICommunicator >()
    , mRootTwoPort( rootPort )
    , mStateSystemGroup( stateSystemGroup )
{
}

template < class MatrixType >
boost::shared_ptr< electrical::TwoPort< MatrixType > > DAETask< MPICommunicator, MatrixType >::GetNewRootPort()
{
    return mESBPartitioner->GetRoot();
}

template < class MatrixType >
void DAETask< MPICommunicator, MatrixType >::Init()
{
    mESBPartitioner.reset( new ESBPartitioner< MPICommunicator, MatrixType >( mRootTwoPort, this->mNumberOfWorker ) );
    mESBPartitioner->GetRoot()->SetSystem( mStateSystemGroup );
    mStateSystemGroup->Initialize();
    mESBPartitioner->GetRoot()->SetInitialCurrent();
    mESBPartitioner->GetRoot()->UpdateStateSystemGroup();

    const std::vector< boost::shared_ptr< VirtualTwoPort< MPICommunicator, MatrixType > > >& threadTPVec =
     mESBPartitioner->GetVirtualTwoPortVector();
    for ( size_t i = 0; i < threadTPVec.size(); ++i )
    {
        mWorkloadIndices.push_back( CountCellelements< MatrixType >( threadTPVec.at( i ) ) );
    }
}

template < class MatrixType >
std::vector< boost::shared_ptr< TaskData > > DAETask< MPICommunicator, MatrixType >::GetPreparedDataForThreads()
{
    std::vector< boost::shared_ptr< TaskData > > dataVec;

    for ( size_t workerID = 0; workerID < this->mNumberOfWorker; ++workerID )
    {
        VirtualTwoPort< MPICommunicator, MatrixType >* tp = mESBPartitioner->GetVirtualTwoPort( workerID );

        DAEInputData< MatrixType >* inputData = new DAEInputData< MatrixType >();
        inputData->CurrentValue = tp->GetCurrent();
        inputData->StateVector = mStateSystemGroup->mStateVector;
        inputData->Dt = mStateSystemGroup->mDt;

        dataVec.push_back( boost::shared_ptr< TaskData >( inputData ) );
    }

    return dataVec;
}

template < class MatrixType >
boost::shared_ptr< TaskData >
DAETask< MPICommunicator, MatrixType >::DoNecessaryWork( size_t workerID, boost::shared_ptr< TaskData > inputData )
{
    boost::shared_ptr< DAEInputData< MatrixType > > datain = boost::static_pointer_cast< DAEInputData< MatrixType > >( inputData );
    boost::shared_ptr< DAEOutputData< MatrixType > > outputData( new DAEOutputData< MatrixType >() );

    VirtualTwoPort< MPICommunicator, MatrixType >* tps = mESBPartitioner->GetVirtualTwoPort( workerID );
    SystemCopyInfo info = tps->GetSystemCopyInfo();

    mStateSystemGroup->mStateVector = datain->StateVector;
    mStateSystemGroup->mDt = datain->Dt;


    // BEGIN: alten zyklus abschließen

    tps->CalculateStateDependentValuesOfSubNetwork();

    typedef std::vector< boost::shared_ptr< electrical::TwoPort< MatrixType > > > tpVector_t;
    tpVector_t& observableTwoPortVector = tps->GetObservableTwoPorts();
    for ( typename tpVector_t::iterator it = observableTwoPortVector.begin(); it != observableTwoPortVector.end(); ++it )
        ( *it )->SaveInternalData( outputData->ObservableTwoportDataVector );
    tps->SaveInternalData( outputData->ObservableTwoportDataVector );

    // END: alten zyklus abschließen

    mStateSystemGroup->mDglStateSystem.ResetSystem();
    mStateSystemGroup->mAlgStateSystem.ResetSystem();

    tps->SetSubNetworkCurrent( datain->CurrentValue );

    outputData->Voltage = *tps->GetSubNetworkVoltage();

    size_t cols = outputData->Voltage.n_cols;
    if ( info.DglRows.FirstRowToCopy <= info.DglRows.LastRowToCopy )
    {
#ifndef _EIGEN_
        outputData->DglSubMat =
         mStateSystemGroup->mDglStateSystem.GetEquationSystemAMatrix().submat( info.DglRows.FirstRowToCopy, 0,
                                                                               info.DglRows.LastRowToCopy, cols - 2 );
        outputData->DglSubVectorC =
         mStateSystemGroup->mDglStateSystem.GetEquationSystemCVector().submat( info.DglRows.FirstRowToCopy, 0,
                                                                               info.DglRows.LastRowToCopy, 0 );
#else
        outputData->DglSubMat =
         mStateSystemGroup->mDglStateSystem.GetEquationSystemAMatrix().block( info.DglRows.FirstRowToCopy, 0,
                                                                              info.DglRows.LastRowToCopy, cols - 2 );
        outputData->DglSubVectorC =
         mStateSystemGroup->mDglStateSystem.GetEquationSystemCVector().block( info.DglRows.FirstRowToCopy, 0,
                                                                              info.DglRows.LastRowToCopy, 0 );
#endif
    }

    if ( info.AlgRows.FirstRowToCopy <= info.AlgRows.LastRowToCopy )
    {
#ifndef _EIGEN_
        outputData->AlgSubMat =
         mStateSystemGroup->mAlgStateSystem.GetEquationSystemAMatrix().submat( info.AlgRows.FirstRowToCopy, 0,
                                                                               info.AlgRows.LastRowToCopy, cols - 2 );
        outputData->AlgSubVectorC =
         mStateSystemGroup->mAlgStateSystem.GetEquationSystemCVector().submat( info.AlgRows.FirstRowToCopy, 0,
                                                                               info.AlgRows.LastRowToCopy, 0 );
#else
        outputData->AlgSubMat =
         mStateSystemGroup->mAlgStateSystem.GetEquationSystemAMatrix().block( info.AlgRows.FirstRowToCopy, 0,
                                                                              info.AlgRows.LastRowToCopy, cols - 2 );
        outputData->AlgSubVectorC =
         mStateSystemGroup->mAlgStateSystem.GetEquationSystemCVector().block( info.AlgRows.FirstRowToCopy, 0,
                                                                              info.AlgRows.LastRowToCopy, 0 );
#endif
    }

    return outputData;
}

template < class MatrixType >
void DAETask< MPICommunicator, MatrixType >::UseProcessedDataFromThreads( const std::vector< boost::shared_ptr< TaskData > >& dataVec )
{
    for ( size_t workerID = 0; workerID < this->mNumberOfWorker; ++workerID )
    {
        boost::shared_ptr< DAEOutputData< MatrixType > > outputData =
         boost::static_pointer_cast< DAEOutputData< MatrixType > >( dataVec.at( workerID ) );

        VirtualTwoPort< MPICommunicator, MatrixType >* tp = mESBPartitioner->GetVirtualTwoPort( workerID );
        SystemCopyInfo info = tp->GetSystemCopyInfo();

        mStateSystemGroup->mDglStateSystem.AddEquations( info.DglRows.FirstRowToCopy, outputData->DglSubMat, outputData->DglSubVectorC );
        mStateSystemGroup->mAlgStateSystem.AddEquations( info.AlgRows.FirstRowToCopy, outputData->AlgSubMat, outputData->AlgSubVectorC );

        tp->SetVoltage( outputData->Voltage );

        typedef std::vector< boost::shared_ptr< electrical::TwoPort< MatrixType > > > tpVector_t;
        tpVector_t& observableTwoPortVector = tp->GetObservableTwoPorts();
        tp->LoadInternalData( outputData->ObservableTwoportDataVector );
        for ( typename tpVector_t::reverse_iterator it = observableTwoPortVector.rbegin();
              it != observableTwoPortVector.rend(); ++it )
            ( *it )->LoadInternalData( outputData->ObservableTwoportDataVector );
    }
}

template < class MatrixType >
std::vector< size_t > DAETask< MPICommunicator, MatrixType >::GetWorkloadIndices() const
{
    return mWorkloadIndices;
}

#endif    // BOOST_MPI

#ifdef BOOST_THREAD

/// Task for the parallel calculation of voltage equations
template < class MatrixType >
class DAETask< ThreadCommunicator, MatrixType > : public Task< ThreadCommunicator >
{
    public:
    DAETask( boost::shared_ptr< electrical::TwoPort< MatrixType > > rootPort, systm::StateSystemGroup< MatrixType >* stateSystemGroup );

    virtual ~DAETask() {}

    boost::shared_ptr< electrical::TwoPort< MatrixType > > GetNewRootPort();

    std::vector< size_t > GetWorkloadIndices() const;

    protected:
    void Init();

    void PrepareDataForThreads() {}

    void DoNecessaryWork( size_t workerID );

    void UseDataFromThreads() {}

    private:
    boost::shared_ptr< electrical::TwoPort< MatrixType > > mRootTwoPort;
    boost::scoped_ptr< ESBPartitioner< ThreadCommunicator, MatrixType > > mESBPartitioner;
    systm::StateSystemGroup< MatrixType >* mStateSystemGroup;
    std::vector< size_t > mWorkloadIndices;
};

template < class MatrixType >
DAETask< ThreadCommunicator, MatrixType >::DAETask( boost::shared_ptr< electrical::TwoPort< MatrixType > > rootPort,
                                                    systm::StateSystemGroup< MatrixType >* stateSystemGroup )
    : Task< ThreadCommunicator >()
    , mRootTwoPort( rootPort )
    , mStateSystemGroup( stateSystemGroup )
{
}

template < class MatrixType >
boost::shared_ptr< electrical::TwoPort< MatrixType > > DAETask< ThreadCommunicator, MatrixType >::GetNewRootPort()
{
    return mESBPartitioner->GetRoot();
}

template < class MatrixType >
void DAETask< ThreadCommunicator, MatrixType >::Init()
{
    mESBPartitioner.reset( new ESBPartitioner< ThreadCommunicator, MatrixType >( mRootTwoPort, this->mNumberOfWorker ) );
    mESBPartitioner->GetRoot()->SetSystem( mStateSystemGroup );
    mStateSystemGroup->Initialize();

    std::vector< std::pair< size_t, size_t > > dglPartitions;
    std::vector< std::pair< size_t, size_t > > algPartitions;

    const std::vector< boost::shared_ptr< VirtualTwoPort< ThreadCommunicator, MatrixType > > >& threadTPVec =
     mESBPartitioner->GetVirtualTwoPortVector();
    for ( size_t i = 0; i < threadTPVec.size(); ++i )
    {
        const SystemCopyInfo& copyInfo = threadTPVec.at( i )->GetSystemCopyInfo();

        dglPartitions.push_back(
         std::make_pair< size_t, size_t >( copyInfo.DglRows.FirstRowToCopy, copyInfo.DglRows.LastRowToCopy + 1 ) );
        algPartitions.push_back(
         std::make_pair< size_t, size_t >( copyInfo.AlgRows.FirstRowToCopy, copyInfo.AlgRows.LastRowToCopy + 1 ) );

        mWorkloadIndices.push_back( CountCellelements< MatrixType >( threadTPVec.at( i ) ) );
    }

    mStateSystemGroup->mDglStateSystem.PartitionSystem( dglPartitions );
    mStateSystemGroup->mAlgStateSystem.PartitionSystem( algPartitions );

    mESBPartitioner->GetRoot()->SetInitialCurrent();
    mESBPartitioner->GetRoot()->UpdateStateSystemGroup();
}

template < class MatrixType >
void DAETask< ThreadCommunicator, MatrixType >::DoNecessaryWork( size_t workerID )
{
    VirtualTwoPort< ThreadCommunicator, MatrixType >* tps = mESBPartitioner->GetVirtualTwoPort( workerID );
    tps->CalculateStateDependentValuesOfSubNetwork();
    tps->SetSubNetworkCurrent();
    tps->CalcSubNetworkVoltage();    // problematisch bei linearsystems
}

template < class MatrixType >
std::vector< size_t > DAETask< ThreadCommunicator, MatrixType >::GetWorkloadIndices() const
{
    return mWorkloadIndices;
}

#endif    // BOOST_THREAD
}
} /* namespace threading */

#endif /* DAETASK_H_ */
