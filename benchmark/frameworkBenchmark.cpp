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
/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : main.cpp
* Creation Date : 31-10-2012
* Last Modified : Di 23 Feb 2016 15:34:00 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "../src/misc/matrixInclude.h"
#include "../src/misc/cellelementCounter.h"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time.hpp>
#include <string>
#include <iterator>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "../src/misc/charArrayCmp.h"
#include "../src/observer/observer.h"
#include "../src/factory/observer/createBenchmarkFilter.h"
#include "../src/factory/electrical/electricalfactorybuilder.h"
#include "../src/factory/object/objectfactorybuilder.h"
#include "../src/factory/state/statefactorybuilder.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "../src/system/system.h"
#include "../src/electrical/cellelement.h"
#include "../src/export/esbVisualizer.h"
#include "../src/export/spiceExport.h"
#include "../src/factory/observer/createObserver.h"
#include "../src/time_series/time_series.h"

//######################################################################
#include "../src/threading/communicators/communicatorInclude.h"
#include "../src/threading/taskData.h"
#include "../src/threading/threadManager.h"
#include "../src/threading/tasks/daetask/daeTask.h"
#include "../src/threading/tasks/sampletask/sampleTask.h"
#include "../src/threading/taskDataRegistration.txt"
#include "../src/threading/tasks/daetask/daeTaskDataRegistration.txt"
#include "../src/threading/tasks/sampletask/sampleTaskDataRegistration.txt"
using namespace threading;

//######################################################################

template < class MatrixType >
boost::shared_ptr< electrical::TwoPort< MatrixType > >
GenerateNetworkFromFile( const char *fileName, std::vector< boost::shared_ptr< electrical::TwoPort< MatrixType > > > &tp_vec )
{
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state, factory::ArgumentTypeState > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > > objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< MatrixType >, factory::ArgumentTypeElectrical > > electricalFactory(
     factory::BuildElectricalFactory< MatrixType, double >( objectFactory.get(), stateFactory.get() ) );

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromFile( fileName );

    boost::shared_ptr< xmlparser::XmlParameter > rootParam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    boost::shared_ptr< electrical::TwoPort< MatrixType > > rootPort( electricalFactory->CreateInstance( rootParam ) );
    std::vector< boost::shared_ptr< electrical::TwoPort< MatrixType > > > allTp;
    electricalFactory->GetObjects( tp_vec );
    tp_vec.erase( std::remove_if( tp_vec.begin(), tp_vec.end(), []( const boost::shared_ptr< electrical::TwoPort< MatrixType > > &o )
                                  {
                      return !o->IsObservable();
                  } ),
                  tp_vec.end() );
    return rootPort;
}

template < class CommunicatorType, class MatrixType, class TimeSeries >
void performTest( CommunicatorType *communicator, const char *fileName, size_t cycleCount, TimeSeries *currentProfile,
                  bool doLogging = false )
{

    ThreadManager< CommunicatorType > tm( communicator );
    systm::StateSystemGroup< MatrixType > stateSystemGroup;

    std::vector< boost::shared_ptr< electrical::TwoPort< MatrixType > > > tp_vec;
    boost::shared_ptr< electrical::TwoPort< MatrixType > > tmprootPort( GenerateNetworkFromFile< MatrixType >( fileName, tp_vec ) );

    daetask::DAETask< CommunicatorType, MatrixType > *myDaeTask =
     new daetask::DAETask< CommunicatorType, MatrixType >( tmprootPort, &stateSystemGroup );

    TaskGroup< CommunicatorType > *taskGroup = new TaskGroup< CommunicatorType >();
    taskGroup->AddTask( myDaeTask );
    taskGroup->AddTask( new sampletask::SampleTask< CommunicatorType >() );

    tm.AddTaskGroup( taskGroup );
    tm.CreateAndRunThreads();

    //      ########### Now only RootProcess / Thread

    const double dt = 0.001;    // variabler Solver
    systm::System< MatrixType > system( &stateSystemGroup, dt );

    typedef boost::shared_ptr< electrical::TwoPort< MatrixType > > TwoPort_t;
    TwoPort_t rootPort( myDaeTask->GetNewRootPort() );


// Netzwerkausgabe in dot-Datei
#ifndef __NO_STRING__
    if ( doLogging )
    {
        std::ofstream ostream( "esb.dot" );
        visualizer::EsbVisualizer< MatrixType > testVisualizer( rootPort.get(), &ostream );
    }


    boost::scoped_ptr< observer::Observer< myMatrixType, electrical::TwoPort > > k(
     CreateTwoPortObserver< std::vector< TwoPort_t >, MatrixType, true >( tp_vec, nullptr ) );
    if ( k )
        AddBenchmarkFilter( k.get() );
#endif

#ifndef __NO_STRING__
    double t = 0.0;
#endif
    rootPort->SetCurrent( 0.0 );

    //        bool ready = false;
    for ( size_t i = 0; i < cycleCount; i++ )
    {
        currentProfile->SetTimeAndTriggerEvaluation( t );

        // Calculate all the Voltages, Currents, Powers and also integrate the Socs
        rootPort->CalculateStateDependentValues();    // Erst jetzt, da Power berechnet werden muss!!

        rootPort->SetCurrent( currentProfile->GetValue() );

        tm.SendDataToThreads();
        tm.ReceiveDataFromThreads();

        if ( rootPort->HasChildren() )
            boost::static_pointer_cast< electrical::TwoPortWithChild< MatrixType > >( rootPort )->ReCalculatePower();

        rootPort->UpdateStateSystemGroup();

#ifndef __NO_STRING__

        ( *k )( t );

#endif

#ifndef __NO_STRING__
        t = system.Solve();
#else
        system.Solve();
#endif
    }
    int rowcount = stateSystemGroup.mDglStateSystem.GetEquationCount() + stateSystemGroup.mAlgStateSystem.GetEquationCount();
    int colcount = stateSystemGroup.mStateVector.n_rows + 1;
    std::cout << rowcount << " rows, " << colcount << " cols" << std::endl;

    std::cout << "WorkloadIndices ";
    const std::vector< size_t > workload = myDaeTask->GetWorkloadIndices();
    for ( size_t i = 0; i < workload.size(); ++i )
    {
        if ( i )
            std::cout << ",";
        std::cout << workload.at( i );
    }
    std::cout << std::endl;
}

int main( int argc, char *argv[] )
{
    int debugParamterNumber = 3;
#ifndef USE_BOOST_THREADS
#ifndef USE_BOOST_MPI
#define COMTYPE DummyCommunicator
    if ( argc < 4 )
    {
        std::cout << "Command [xmlfile] [current profile] [cycles]" << std::endl;
        return 1;
    }

    COMTYPE *communicator = new DummyCommunicator();
    std::cout << "Performing test with Single Thread" << std::endl;
#endif
#endif

#ifdef USE_BOOST_THREADS
#define COMTYPE ThreadCommunicator
    COMTYPE *communicator;

    if ( argc < 4 )
    {
        std::cout << "Command [xmlfile] [current profile] [cycles] [workercount]" << std::endl;
        return 1;
    }

    if ( argc >= 5 )
    {
        size_t worker = atoi( argv[3] );
        communicator = new ThreadCommunicator( worker );
        debugParamterNumber = 4;
    }
    else
        communicator = new ThreadCommunicator();

    std::cout << "Performing test with Threading and " << communicator->GetSystemThreadCount() << " worker" << std::endl;

#elif USE_BOOST_MPI
#define COMTYPE MPICommunicator
    if ( argc < 4 )
    {
        std::cout << "Command [xmlfile] [current profile] [cycles]" << std::endl;
        return 1;
    }

#ifdef USE_ZLIB
    std::cout << "Using ZLib" << std::endl;
#endif

    COMTYPE *communicator = new MPICommunicator( argc, argv );
    std::cout << "Performing test with MPI" << std::endl;
#endif

    // Parameter 2
    boost::scoped_ptr< electrical::TimeSeries< double, electrical::EvalNoInterpolation > > currentProfile;
    try
    {
        std::string filename( argv[2] );

        if ( filename.rfind( ".mat" ) != std::string::npos )
        {
            matlab::MatFile matFile( filename );
            matlab::MatioData &matStruct = matFile["diga"]["daten"];
            std::vector< double > time = matStruct["Programmdauer"].operator()< matlab::MatioData::X_DIM >( 0 );
            std::vector< double > current = matStruct["Strom"].operator()< matlab::MatioData::X_DIM >( 0 );
            currentProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( time, current ) );
        }
        else
        {
            std::ifstream currentProfileFile( argv[2] );
            currentProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( currentProfileFile ) );
        }
    }
    catch ( std::exception &e )
    {
        printf( "At 2. parameter: current profile\n" );
        printf( "%s\n", e.what() );
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        printf( "At 2. parameter: current profile\n" );
        printf( "Unidentified error\n" );
        return EXIT_FAILURE;
    }

    bool debug =
     ( argc > debugParamterNumber ) && ( misc::CaseInsensitiveStringCompare( argv[debugParamterNumber], "debug" ) == 0 );

    performTest< COMTYPE, myMatrixType, electrical::TimeSeries< double, electrical::EvalNoInterpolation > >(
     communicator, argv[1], atoi( argv[3] ), currentProfile.get(), debug );

    return 0;
}
