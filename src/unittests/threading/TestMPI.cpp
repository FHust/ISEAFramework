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
* File Name : TestMPI.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 15:11:05 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestMPI.h"

#ifdef BOOST_MPI
#include "../../misc/matrixInclude.h"

//BOOST
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "../../factory/electrical/electricalfactorybuilder.h"
#include "../../factory/object/objectfactorybuilder.h"
#include "../../factory/state/statefactorybuilder.h"

#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

#include "../../threading/communicators/mpiCommunicator.h"

#include "../../threading/taskData.h"
#include "../../threading/threadManager.h"

#include "../../threading/tasks/daetask/daeThreadedTask.h"
#include "../../threading/tasks/daetask/daeInputData.h"
#include "../../threading/tasks/daetask/daeOutputData.h"
#include "../../system/dae_sys.h"
#include <iostream>

using namespace threading;

void TestMPI::testMPITestParallelResistors()
{
#ifdef BOOST_MPI
    boost::scoped_ptr< factory::Factory< ::state::Dgl_state > > stateFactory( factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory<object::Object<double> > > objectFactory( factory::BuildObjectFactory<double>( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory<electrical::TwoPort< myMatrixType > > > electricalFactory( factory::BuildElectricalFactory<myMatrixType, double>( objectFactory.get(), stateFactory.get() ) );

    const char *xmlConfig = "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             </CustomDefinitions>\
                             <RootElement class='ParallelTwoPort'>\
                             <Children count='2'>\
                             <MySerial class='SerialTwoPort'>\
                             <Children count='5'>\
                             <MyParallel class='ParallelTwoPort'>\
                             <Children count='2'>\
                             <MyRes class='OhmicResistance'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </MyRes>\
                             </Children>\
                             </MyParallel>\
                             </Children>\
                             </MySerial>\
                             </Children>\
                             </RootElement>\
                             </Configuration>";

    boost::scoped_ptr<xmlparser::XmlParser> parser(new xmlparser::tinyxml2::XmlParserImpl());
    parser->ReadFromMem(xmlConfig);

    boost::shared_ptr<xmlparser::XmlParameter> rootparam( parser->GetRoot()->GetElementChild("RootElement") );
    boost::shared_ptr< electrical::TwoPort< myMatrixType > > tmpRootPort( electricalFactory->CreateInstance(rootparam) );

    ThreadManager<MPICommunicator> tm( new MPICommunicator(0, 0) );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;

    daetask::DAEThreadedTask<MPICommunicator, myMatrixType> *myDaeTask=new daetask::DAEThreadedTask<MPICommunicator, myMatrixType>(tmpRootPort, &stateSystemGroup);

    tm.AddThreadedTask(myDaeTask);
    tm.CreateAndRunThreads();

    boost::shared_ptr< electrical::TwoPort< myMatrixType > > rootPort( myDaeTask->GetNewRootPort() );
    systm::DifferentialAlgebraicSystem< myMatrixType > daeSystem(&stateSystemGroup);


    double dt = 0.001;
    double Current = 10.0;

    stateSystemGroup.mDt = dt;

    rootPort->SetCurrent( Current );

    tm.SendDataToThreads();

    tm.ReceiveDataFromThreads();

    rootPort->UpdateStateSystemGroup();

    //        daeSystem.ODEifyEquations();
    //
    //        std::cout << std::endl;
    //        std::cout << "Threading" << std::endl;
    //        std::cout << daeSystem.GetA() << std::endl;
#endif
}
#else

void TestMPI::testMPITestParallelResistors()
{
}
#endif
