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
* File Name : TestStateSystem.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 24 Mar 2014 16:46:23 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestStateSystem.h"
//BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../../system/stateSystemGroup.h"

#include "../../electrical/capacity.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/voltagesource.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/parallelrc.h"


void TestStateSystem::testSystemWithCapacity()
{

    double Resistance(10.0);
    boost::scoped_ptr<electrical::Capacity< myMatrixType > > res( new electrical::Capacity< myMatrixType >(new object::ConstObj<double>( Resistance )));

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    res->SetSystem(&stateSystemGroup);
    TS_ASSERT_EQUALS(stateSystemGroup.mDglStateSystem.GetEquationCount(), 1)
}

void TestStateSystem::testSystemWithParallelPort()
{
    std::vector<electrical::TwoPort<myMatrixType>* > twoPortVector;
    boost::scoped_ptr< electrical::TwoPortWithChild< myMatrixType > > rootPort(CreateSampleNetwork< electrical::Capacity< myMatrixType > >(twoPortVector));
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootPort->SetSystem(&stateSystemGroup);
    stateSystemGroup.Initialize();
    rootPort->SetInitialCurrent();
    rootPort->UpdateStateSystemGroup();

    TS_ASSERT_EQUALS(stateSystemGroup.mDglStateSystem.GetEquationCount(), 6);
    TS_ASSERT_EQUALS(stateSystemGroup.mAlgStateSystem.GetEquationCount(), 5);

    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().n_cols,  stateSystemGroup.mDglStateSystem.GetEquationCount() + stateSystemGroup.mAlgStateSystem.GetEquationCount());
    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().n_rows,  stateSystemGroup.mDglStateSystem.GetEquationCount() );
    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationSystemAMatrix().n_rows,  stateSystemGroup.mAlgStateSystem.GetEquationCount() );
}

void TestStateSystem::testSystemWithSerialPort()
{

    double Resistance(10.0);

    boost::scoped_ptr<electrical::SerialTwoPort< myMatrixType > > serial( new electrical::SerialTwoPort< myMatrixType > );

    electrical::Capacity< myMatrixType >* res( new electrical::Capacity<myMatrixType >(new object::ConstObj<double>( Resistance )));

    electrical::Capacity< myMatrixType >* res1( new electrical::Capacity<myMatrixType >(new object::ConstObj<double>( Resistance )));

    electrical::Capacity< myMatrixType >* res2( new electrical::Capacity<myMatrixType >(new object::ConstObj<double>( Resistance )));

    serial->AddChild(res);
    serial->AddChild(res1);
    serial->AddChild(res2);



    electrical::SerialTwoPort< myMatrixType > *serial2( new electrical::SerialTwoPort< myMatrixType >() );

    res  =  new electrical::Capacity<myMatrixType >(new object::ConstObj<double>( Resistance ));
    res1 =  new electrical::Capacity<myMatrixType >(new object::ConstObj<double>( Resistance ));
    res2 =  new electrical::Capacity<myMatrixType >(new object::ConstObj<double>( Resistance ));

    serial2->AddChild(res);
    serial2->AddChild(res1);
    serial2->AddChild(res2);

    serial->AddChild(serial2);

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    serial->SetSystem(&stateSystemGroup);
    stateSystemGroup.Initialize();

    TS_ASSERT_EQUALS(stateSystemGroup.mDglStateSystem.GetEquationCount(), 6);
    TS_ASSERT_EQUALS(stateSystemGroup.mAlgStateSystem.GetEquationCount(), 0);
}

void TestStateSystem::testDaeSystemWithParallelPortCurrentOhmic()
{
    std::vector<electrical::TwoPort<myMatrixType>* > twoPortVector;
    boost::scoped_ptr< electrical::TwoPortWithChild< myMatrixType > > rootPort(CreateSampleNetwork< electrical::OhmicResistance< myMatrixType > >(twoPortVector));
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootPort->SetSystem(&stateSystemGroup);
    stateSystemGroup.Initialize();

#ifndef _SYMBOLIC_
    double Current(10.0);
    rootPort->SetInitialCurrent(Current);
#else
    rootPort->SetInitialCurrent( symbolic::Symbolic("InputCurrent") );
#endif
    rootPort->UpdateStateSystemGroup();

    for( size_t i = 0 ; i < twoPortVector.size()  ; ++i )
    {
        TS_ASSERT_EQUALS( twoPortVector.at(i)->mCurrent.n_cols, (stateSystemGroup.GetStateCount() + 1) );
        TS_ASSERT_EQUALS( twoPortVector.at(i)->mCurrent.n_rows, 1 );
    }
    /*
       -1 -1 -1 0 0 10
       1 0 0 0 0 0
       0 1 0 0 0 0
       0 0 1 -1 -1 0
       0 0 0 1 0 0
       0 0 0 0 1 0
       0 0 1 0 0 0
       0 0 0 0 0 10
       */


#ifndef _SYMBOLIC_
    for( size_t i = 0 ; i < 3 ; ++i )
        TS_ASSERT_EQUALS(twoPortVector.at(0)->mCurrent(0,i), -1);
    TS_ASSERT_EQUALS(twoPortVector.at(0)->mCurrent(0,5), 10);


    TS_ASSERT_EQUALS(twoPortVector.at(1)->mCurrent(0,0), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(2)->mCurrent(0,1), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,2), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,3), -1);
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,4), -1);

    TS_ASSERT_EQUALS(twoPortVector.at(4)->mCurrent(0,3), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(5)->mCurrent(0,4), 1);

    TS_ASSERT_EQUALS(twoPortVector.at(6)->mCurrent(0,2), 1);

    TS_ASSERT_EQUALS(twoPortVector.at(7)->mCurrent(0,5), Current);
#else
    for( size_t i = 0 ; i < 3 ; ++i )
        TS_ASSERT_EQUALS(twoPortVector.at(0)->mCurrent(0,i), symbolic::Symbolic("-1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(0)->mCurrent(0,5), symbolic::Symbolic("InputCurrent") );


    TS_ASSERT_EQUALS(twoPortVector.at(1)->mCurrent(0,0), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(2)->mCurrent(0,1), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,2), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,3), symbolic::Symbolic("-1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,4), symbolic::Symbolic("-1.000000") );

    TS_ASSERT_EQUALS(twoPortVector.at(4)->mCurrent(0,3), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(5)->mCurrent(0,4), symbolic::Symbolic("1.000000") );

    TS_ASSERT_EQUALS(twoPortVector.at(6)->mCurrent(0,2), symbolic::Symbolic("1.000000") );

    TS_ASSERT_EQUALS(twoPortVector.at(7)->mCurrent(0,5), symbolic::Symbolic("InputCurrent"));
#endif
}

void TestStateSystem::testDaeSystemWithParallelPortCurrentCapacity()
{
    std::vector<electrical::TwoPort<myMatrixType>* > twoPortVector;
    boost::scoped_ptr< electrical::TwoPortWithChild< myMatrixType > > rootPort(CreateSampleNetwork< electrical::Capacity< myMatrixType > >(twoPortVector));
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootPort->SetSystem(&stateSystemGroup);
    stateSystemGroup.Initialize();

#ifndef _SYMBOLIC_
    double Current(10.0);
    rootPort->SetInitialCurrent(Current);
#else
    rootPort->SetInitialCurrent( symbolic::Symbolic("InputCurrent") );
#endif
    rootPort->UpdateStateSystemGroup();

    for( size_t i = 0 ; i < twoPortVector.size()  ; ++i )
    {
        TS_ASSERT_EQUALS( twoPortVector.at(i)->mCurrent.n_cols, stateSystemGroup.GetStateCount() + 1);
        TS_ASSERT_EQUALS( twoPortVector.at(i)->mCurrent.n_rows, 1);
    }

#ifndef _SYMBOLIC_
    TS_ASSERT_EQUALS(twoPortVector.at(1)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 0), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(2)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 1), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 2), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 3), -1);
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 4), -1);

    TS_ASSERT_EQUALS(twoPortVector.at(4)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 3), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(5)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 4), 1);

    TS_ASSERT_EQUALS(twoPortVector.at(6)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 2), 1);

    TS_ASSERT_EQUALS(twoPortVector.at(7)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 5), Current);
#else
    TS_ASSERT_EQUALS(twoPortVector.at(1)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 0), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(2)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 1), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 2), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 3), symbolic::Symbolic("-1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 4), symbolic::Symbolic("-1.000000") );

    TS_ASSERT_EQUALS(twoPortVector.at(4)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 3), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(5)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 4), symbolic::Symbolic("1.000000") );

    TS_ASSERT_EQUALS(twoPortVector.at(6)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 2), symbolic::Symbolic("1.000000") );

    TS_ASSERT_EQUALS(twoPortVector.at(7)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 5), symbolic::Symbolic("InputCurrent") );
#endif
}

void TestStateSystem::testDaeSystemWithParallelPortCurrentVoltage()
{
    std::vector<electrical::TwoPort<myMatrixType>* > twoPortVector;
    boost::scoped_ptr< electrical::TwoPortWithChild< myMatrixType > > rootPort(CreateSampleNetwork< electrical::VoltageSource< myMatrixType > >(twoPortVector));
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootPort->SetSystem(&stateSystemGroup);
    stateSystemGroup.Initialize();

#ifndef _SYMBOLIC_
    double Current(10.0);
    rootPort->SetInitialCurrent(Current);
#else
    rootPort->SetInitialCurrent( symbolic::Symbolic("InputCurrent") );
#endif
    rootPort->UpdateStateSystemGroup();

for( size_t i = 0 ; i < twoPortVector.size()  ; ++i )
    {
        TS_ASSERT_EQUALS( twoPortVector.at(i)->mCurrent.n_cols, stateSystemGroup.GetStateCount() + 1);
        TS_ASSERT_EQUALS( twoPortVector.at(i)->mCurrent.n_rows, 1);
    }

#ifndef _SYMBOLIC_
    TS_ASSERT_EQUALS(twoPortVector.at(1)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 0), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(2)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 1), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 2), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 3), -1);
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 4), -1);

    TS_ASSERT_EQUALS(twoPortVector.at(4)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 3), 1);
    TS_ASSERT_EQUALS(twoPortVector.at(5)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 4), 1);

    TS_ASSERT_EQUALS(twoPortVector.at(6)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 2), 1);

    TS_ASSERT_EQUALS(twoPortVector.at(7)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 5), Current);
#else
    TS_ASSERT_EQUALS(twoPortVector.at(1)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 0), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(2)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 1), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 2), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 3), symbolic::Symbolic("-1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(3)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 4), symbolic::Symbolic("-1.000000") );

    TS_ASSERT_EQUALS(twoPortVector.at(4)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 3), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(twoPortVector.at(5)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 4), symbolic::Symbolic("1.000000") );

    TS_ASSERT_EQUALS(twoPortVector.at(6)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 2), symbolic::Symbolic("1.000000") );

    TS_ASSERT_EQUALS(twoPortVector.at(7)->mCurrent(0,stateSystemGroup.mDglStateSystem.GetEquationCount() + 5), symbolic::Symbolic("InputCurrent") );
#endif
}

void TestStateSystem::TestMixedSystem()
{
#ifndef _SYMBOLIC_
#else
    //Attention: This must look like TestDaeSystem::testDaeSystemMixedSystemCurrentSolveable()
    boost::scoped_ptr< electrical::ParallelTwoPort<> > parallel( new electrical::ParallelTwoPort<>() );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p1( new electrical::ParallelTwoPort<>() );
    parallel->AddChild( p1 );

    boost::shared_ptr < electrical::SerialTwoPort<> > s11( new electrical::SerialTwoPort<>() );
    boost::shared_ptr < electrical::SerialTwoPort<> > s12( new electrical::SerialTwoPort<>() );
    p1->AddChild( s11 );
    p1->AddChild( s12 );

    s11->AddChild(new electrical::Capacity<>(new object::ConstObj<double>( 10 )));
    s11->AddChild(new electrical::OhmicResistance<>(new object::ConstObj<double>( 2 )));
    s12->AddChild(new electrical::Capacity<>(new object::ConstObj<double>( 5 )));
    s12->AddChild(new electrical::OhmicResistance<>(new object::ConstObj<double>( 2 )));

    boost::shared_ptr <electrical::SerialTwoPort<> >s2( new electrical::SerialTwoPort<>() );
    parallel->AddChild( s2 );

    s2->AddChild(new electrical::OhmicResistance<>(new object::ConstObj<double>( 3 )));
    s2->AddChild(new electrical::VoltageSource<>(new object::ConstObj<double>( 2.5 )));

    parallel->SetID(0);
    p1->SetID(1);
    s11->SetID(2);
    s11->at(0)->SetID(3);
    s11->at(1)->SetID(4);
    s12->SetID(5);
    s12->at(0)->SetID(6);
    s12->at(1)->SetID(7);
    s2->SetID(8);
    s2->at(0)->SetID(9);
    s2->at(1)->SetID(10);

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem(&stateSystemGroup);
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent( symbolic::Symbolic("InputCurrent") );
    parallel->UpdateStateSystemGroup();

    //systm::DifferentialAlgebraicSystem< myMatrixType > test(&stateSystemGroup);
    const myMatrixType &dglMat = stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix();
    const myMatrixType &dglVec = stateSystemGroup.mDglStateSystem.GetEquationSystemCVector();
    const myMatrixType &algMat = stateSystemGroup.mAlgStateSystem.GetEquationSystemAMatrix();
    const myMatrixType &algVec = stateSystemGroup.mAlgStateSystem.GetEquationSystemCVector();

    TS_ASSERT( dglMat(0,0).IsEmpty() );
    TS_ASSERT(dglMat(1,0).IsEmpty() );
    TS_ASSERT_EQUALS(algMat(0,0), symbolic::Symbolic("NEG(1.000000)") );
    TS_ASSERT(algMat(1,0).IsEmpty() );

    TS_ASSERT(dglMat(0,1).IsEmpty() );
    TS_ASSERT(dglMat(1,1).IsEmpty() );
    TS_ASSERT_EQUALS(algMat(0,1), symbolic::Symbolic("1.000000") );
    TS_ASSERT_EQUALS(algMat(1,1), symbolic::Symbolic("NEG(1.000000)") );

    TS_ASSERT_EQUALS(dglVec(0,0), symbolic::Symbolic("DIV(InputCurrent,ID3_ObjBase)") );
    TS_ASSERT(dglVec(1,0).IsEmpty() );
    TS_ASSERT_EQUALS(algVec(0,0), symbolic::Symbolic("NEG(MUL(InputCurrent,ID4_ObjBase))") );
    TS_ASSERT_EQUALS(algVec(1,0), symbolic::Symbolic("ID10_ObjBase") );

    TS_ASSERT_EQUALS(dglMat(0,2), symbolic::Symbolic("DIV(-1.000000,ID3_ObjBase)") );
    TS_ASSERT_EQUALS(dglMat(1,2), symbolic::Symbolic("DIV(1.000000,ID6_ObjBase)") );
    TS_ASSERT_EQUALS(algMat(0,2), symbolic::Symbolic("SUB(ID7_ObjBase,MUL(-1.000000,ID4_ObjBase))") );
    TS_ASSERT_EQUALS(algMat(1,2), symbolic::Symbolic("NEG(ID7_ObjBase)") );

    TS_ASSERT_EQUALS(dglMat(0,3), symbolic::Symbolic( "DIV(-1.000000,ID3_ObjBase)") );
    TS_ASSERT(dglMat(1,3).IsEmpty() );
    TS_ASSERT_EQUALS(algMat(0,3), symbolic::Symbolic("NEG(MUL(-1.000000,ID4_ObjBase))") );
    TS_ASSERT_EQUALS(algMat(1,3), symbolic::Symbolic("ID9_ObjBase") );
#endif
}