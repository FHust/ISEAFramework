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
* File Name : TestDaeSystem.cpp
* Creation Date : 21-02-2014
* Last Modified : Sa 25 Mär 2017 18:31:27 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestDaeSystem.h"
#include "../../misc/matrixInclude.h"

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

// STD
#include <string>
#include <iostream>

#include "../../container/armadillo_wrapper.h"
#include "../../electrical/capacity.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/voltagesource.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/paralleltwoport.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/zarc.h"
#include "../../object/const_obj.h"
#include "../../states/soc.h"
#include "../../states/thermal_state.h"
#include "../../system/stateSystemGroup.h"
#include "../../system/system.h"

std::vector< std::vector< double > > TestDaeSystem::CopyToVector( const double data[7][4] )
{
    std::vector< std::vector< double > > returnvector( 7, std::vector< double >( 4, 0 ) );
    for ( size_t i = 0; i < 7; ++i )
        returnvector.at( i ).assign( data[i], data[i] + 4 );
    return returnvector;
}


void TestDaeSystem::testDaeSystemMixedSystemCurrentUnsolveable()
{
#if not defined( _EIGEN_ )    // or  EIGEN
    boost::shared_ptr< electrical::ParallelTwoPort<> > parallel( new electrical::ParallelTwoPort<>() );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p1( new electrical::ParallelTwoPort<>() );
    parallel->AddChild( p1 );
    p1->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 10 ) ) );
    p1->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 5 ) ) );

    boost::shared_ptr< electrical::SerialTwoPort<> > p2( new electrical::SerialTwoPort<>() );
    parallel->AddChild( p2 );
    p2->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 3 ) ) );
    p2->AddChild( new electrical::VoltageSource<>( new object::ConstObj< double >( 2.5 ) ) );

    double Current = 4.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent( Current );
    parallel->UpdateStateSystemGroup();

    // TS_ASSERT_THROWS_ANYTHING(  )
    systm::DifferentialAlgebraicSystem< myMatrixType > test(
     &stateSystemGroup );    // <-- behavior changed is not throwing anymore
//   0        0  -0.1000  -0.1000   0.4000
//   0        0   0.2000        0        0
//   -1.0000   1.0000        0        0        0
//   0  -1.0000        0   3.0000   2.5000

#endif /*_EIGEN_*/
}

void TestDaeSystem::testDaeSystemMixedSystemCurrentSolveable()
{
    // Attention: This must look like TestStateSystem::TestMixedSystem()
    boost::scoped_ptr< electrical::ParallelTwoPort<> > parallel( new electrical::ParallelTwoPort<>() );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p1( new electrical::ParallelTwoPort<>() );
    parallel->AddChild( p1 );

    boost::shared_ptr< electrical::SerialTwoPort<> > s11( new electrical::SerialTwoPort<>() );
    boost::shared_ptr< electrical::SerialTwoPort<> > s12( new electrical::SerialTwoPort<>() );
    p1->AddChild( s11 );
    p1->AddChild( s12 );

    s11->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 10 ) ) );
    s11->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 2 ) ) );
    s12->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 5 ) ) );
    s12->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 2 ) ) );

    boost::shared_ptr< electrical::SerialTwoPort<> > s2( new electrical::SerialTwoPort<>() );
    parallel->AddChild( s2 );

    s2->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 3 ) ) );
    s2->AddChild( new electrical::VoltageSource<>( new object::ConstObj< double >( 2.5 ) ) );

    double Current = 4.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent( Current );
    parallel->UpdateStateSystemGroup();

    systm::DifferentialAlgebraicSystem< myMatrixType > test( &stateSystemGroup );
    const myMatrixType &dglMat = stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix();
    const myMatrixType &dglVec = stateSystemGroup.mDglStateSystem.GetEquationSystemCVector();
    const myMatrixType &algMat = stateSystemGroup.mAlgStateSystem.GetEquationSystemAMatrix();
    const myMatrixType &algVec = stateSystemGroup.mAlgStateSystem.GetEquationSystemCVector();

    TS_ASSERT_DELTA( dglMat( 0, 0 ), 0.0, 0.0000001 );
    TS_ASSERT_DELTA( dglMat( 1, 0 ), 0.0, 0.0000001 );
    TS_ASSERT_DELTA( algMat( 0, 0 ), -1.0, 0.000001 );
    TS_ASSERT_DELTA( algMat( 1, 0 ), 0.0, 0.000001 );

    TS_ASSERT_DELTA( dglMat( 0, 1 ), 0, 0.000001 );
    TS_ASSERT_DELTA( dglMat( 1, 1 ), 0, 0.000001 );
    TS_ASSERT_DELTA( algMat( 0, 1 ), 1.0, 0.000001 );
    TS_ASSERT_DELTA( algMat( 1, 1 ), -1, 0.000001 );

    TS_ASSERT_DELTA( dglVec( 0, 0 ), 0.4, 0.000001 );
    TS_ASSERT_DELTA( dglVec( 1, 0 ), 0, 0.000001 );
    TS_ASSERT_DELTA( algVec( 0, 0 ), -8, 0.000001 );
    TS_ASSERT_DELTA( algVec( 1, 0 ), 2.5, 0.000001 );

    TS_ASSERT_DELTA( dglMat( 0, 2 ), -0.1, 0.000001 );
    TS_ASSERT_DELTA( dglMat( 1, 2 ), 0.2, 0.000001 );
    TS_ASSERT_DELTA( algMat( 0, 2 ), 4.0, 0.000001 );
    TS_ASSERT_DELTA( algMat( 1, 2 ), -2.0, 0.000001 );

    TS_ASSERT_DELTA( dglMat( 0, 3 ), -0.1, 0.000001 );
    TS_ASSERT_DELTA( dglMat( 1, 3 ), 0.0, 0.000001 );
    TS_ASSERT_DELTA( algMat( 0, 3 ), 2.0, 0.000001 );
    TS_ASSERT_DELTA( algMat( 1, 3 ), 3.0, 0.000001 );

    try
    {
        test.PrepareEquationSystem();
    }
    catch ( std::exception &e )
    {
        std::cout << e.what() << std::endl;
        TS_FAIL( true );
        return;
    }

#ifndef _EIGEN_
    const myMatrixType &matrixA = test.GetA();
    const myMatrixType &vectorC = test.GetC();

    TS_ASSERT_EQUALS( matrixA( 0, 0 ), 0 );
    TS_ASSERT_EQUALS( matrixA( 1, 0 ), 0 );
    TS_ASSERT_EQUALS( matrixA( 2, 0 ), 0 );
    TS_ASSERT_EQUALS( matrixA( 3, 0 ), 0 );

    TS_ASSERT_EQUALS( matrixA( 0, 1 ), 0 );
    TS_ASSERT_EQUALS( matrixA( 1, 1 ), 0 );
    TS_ASSERT_EQUALS( matrixA( 2, 1 ), 0 );
    TS_ASSERT_EQUALS( matrixA( 3, 1 ), 0 );

    TS_ASSERT_DELTA( vectorC( 0, 0 ), 0.4, 0.000001 );
    TS_ASSERT_DELTA( vectorC( 1, 0 ), 0, 0.000001 );
    TS_ASSERT_DELTA( vectorC( 2, 0 ), 0.075, 0.000001 );
    TS_ASSERT_DELTA( vectorC( 3, 0 ), 0.05, 0.000001 );

    TS_ASSERT_DELTA( matrixA( 0, 2 ), -0.1, 0.000001 );
    TS_ASSERT_DELTA( matrixA( 1, 2 ), 0.2, 0.000001 );
    TS_ASSERT_DELTA( matrixA( 2, 2 ), -0.08125, 0.000001 );
    TS_ASSERT_DELTA( matrixA( 3, 2 ), 0.0125, 0.000001 );

    TS_ASSERT_DELTA( matrixA( 0, 3 ), -0.1, 0.000001 );
    TS_ASSERT_DELTA( matrixA( 1, 3 ), 0, 0.000001 );
    TS_ASSERT_DELTA( matrixA( 2, 3 ), -0.01875, 0.000001 );
    TS_ASSERT_DELTA( matrixA( 3, 3 ), -0.0125, 0.000001 );
#endif

    // 0 0 -0.1 -0.1 0.4
    // 0 0 0.2 0 0
    //-1 1 4 2 -8
    // 0 -1 -2 3 2.5

    // ODEIFIED
    // 0 0 -0.1 -0.1 0.4
    // 0 0 0.2 0 0
    // 0 0 -0.08125 -0.01875 0.075
    // 0 0 0.0125 -0.0125 0.05
}

void TestDaeSystem::testDaeSystemMixedSystemCurrentSolveableIntegrate()
{
    boost::shared_ptr< electrical::ParallelTwoPort<> > parallel( new electrical::ParallelTwoPort<>() );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p1( new electrical::ParallelTwoPort<>() );
    parallel->AddChild( p1 );

    boost::shared_ptr< electrical::SerialTwoPort<> > s11( new electrical::SerialTwoPort<>() );
    boost::shared_ptr< electrical::SerialTwoPort<> > s12( new electrical::SerialTwoPort<>() );

    p1->AddChild( s11 );
    p1->AddChild( s12 );

    s11->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 0.01 ) ) );
    s11->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 2 ) ) );
    s12->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 0.01 ) ) );
    s12->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 2 ) ) );

    boost::shared_ptr< electrical::SerialTwoPort<> > s2( new electrical::SerialTwoPort<>() );
    parallel->AddChild( s2 );
    s2->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 3 ) ) );
    s2->AddChild( new electrical::VoltageSource<>( new object::ConstObj< double >( 2.5 ) ) );

    double t = 0.0;
    double Current = 4.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent( Current );
    parallel->UpdateStateSystemGroup();

    systm::ConstantStepDglSystemSolver< myMatrixType > solver( &stateSystemGroup, 0.01 );

    while ( t < 10.0 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }
}

void TestDaeSystem::testRC()
{
    boost::shared_ptr< electrical::ParallelTwoPort<> > parallel( new electrical::ParallelTwoPort<>() );

    parallel->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );
    parallel->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 0.1 ) ) );

    double t = 0.0;    // 15.5;//0.000;
    double Current = 2.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent( Current );
    parallel->UpdateStateSystemGroup();

    systm::ConstantStepDglSystemSolver< myMatrixType > solver( &stateSystemGroup, 0.001 );

    myMatrixType &x = stateSystemGroup.mStateVector;

    while ( t < 10.00 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }

    TS_ASSERT_DELTA( x( 0, 0 ), 20, 0.001 )
    TS_ASSERT_DELTA( x( 1, 0 ), 0, 0.001 )
    TS_ASSERT_DELTA( x( 2, 0 ), 1, 0.001 )

    Current = -3;
    parallel->SetCurrent( Current );

    while ( t < 20.00 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }

    Current = 0;
    parallel->SetCurrent( Current );

    while ( t < 30.00 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }
}

void TestDaeSystem::testRCParallel()
{
    boost::shared_ptr< electrical::ParallelTwoPort<> > parallel( new electrical::ParallelTwoPort<>() );

    boost::shared_ptr< electrical::SerialTwoPort<> > p1( new electrical::SerialTwoPort<>() );
    parallel->AddChild( p1 );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p11( new electrical::ParallelTwoPort<>() );
    p1->AddChild( p11 );

    p11->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 0.1 ) ) );
    p11->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );
    p1->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 1.0 ) ) );


    boost::shared_ptr< electrical::SerialTwoPort<> > p2( new electrical::SerialTwoPort<>() );
    parallel->AddChild( p2 );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p21( new electrical::ParallelTwoPort<>() );
    p2->AddChild( p21 );

    p21->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 0.1 ) ) );
    p21->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );
    p2->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 1.0 ) ) );


    boost::shared_ptr< electrical::SerialTwoPort<> > p3( new electrical::SerialTwoPort<>() );
    parallel->AddChild( p3 );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p31( new electrical::ParallelTwoPort<>() );
    p3->AddChild( p31 );

    p31->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 0.1 ) ) );
    p31->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );
    p3->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 1.0 ) ) );


    boost::shared_ptr< electrical::SerialTwoPort<> > p4( new electrical::SerialTwoPort<>() );
    parallel->AddChild( p4 );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p41( new electrical::ParallelTwoPort<>() );
    p4->AddChild( p41 );

    p41->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 0.1 ) ) );
    p41->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );
    p4->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10000.0 ) ) );

    double t = 0.000;
    double Current = 4.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent( Current );
    parallel->UpdateStateSystemGroup();

    systm::ConstantStepDglSystemSolver< myMatrixType > solver( &stateSystemGroup, 0.01 );

    while ( t < 15.50 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }

    Current = -4;
    parallel->SetCurrent( Current );

    while ( t < 31.00 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }
}

void TestDaeSystem::testRCSerial()
{
    boost::shared_ptr< electrical::SerialTwoPort<> > parallel( new electrical::SerialTwoPort<>() );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p1( new electrical::ParallelTwoPort<>() );
    parallel->AddChild( p1 );
    p1->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );
    p1->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 0.5 ) ) );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p2( new electrical::ParallelTwoPort<>() );
    parallel->AddChild( p2 );
    p2->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );
    p2->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 0.1 ) ) );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p3( new electrical::ParallelTwoPort<>() );
    parallel->AddChild( p3 );
    p3->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 27 ) ) );
    p3->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 1.0 ) ) );
    p3->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );

    double t = 0.000;
    double Current = 4.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent( Current );
    parallel->UpdateStateSystemGroup();

    systm::ConstantStepDglSystemSolver< myMatrixType > solver( &stateSystemGroup, 0.01 );

    while ( t < 15.50 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }

    Current = -8;
    parallel->SetCurrent( Current );

    while ( t < 31.00 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }

    Current = 0;
    parallel->SetCurrent( Current );

    while ( t < 37.00 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }
}

void TestDaeSystem::testParallelRCElement()
{
    double Resistance( 10.0 );
    double Capacity( 0.1 );

    boost::shared_ptr< electrical::ParallelRC<> > parallel(
     new electrical::ParallelRC<>( new object::ConstObj< double >( Resistance ), new object::ConstObj< double >( Capacity ) ) );

    double t = 0.000;
    double Current = 4.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent( Current );
    parallel->UpdateStateSystemGroup();

    systm::ConstantStepDglSystemSolver< myMatrixType > solver( &stateSystemGroup, 0.001 );

    myMatrixType &x = stateSystemGroup.mStateVector;

    while ( t < 15.50 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }

    TS_ASSERT_DELTA( x( 0, 0 ), 40, 0.001 );
    parallel->SetCurrent( 0 );
    parallel->UpdateStateSystemGroup();

    while ( t < 15.50 + 15.50 )
    {
        parallel->UpdateStateSystemGroup();
        t = solver.Solve();
        parallel->CalculateStateDependentValues();
    }

    TS_ASSERT( x( 0, 0 ) < 1.24e-05 );
}

void TestDaeSystem::testConstantZarcSynthetic()
{
    double Resistance( 10.0 );
    double Capacity( 0.1 );

    boost::shared_ptr< electrical::SerialTwoPort<> > serial( new electrical::SerialTwoPort<>() );
    serial->AddChild( new electrical::ParallelRC<>( new object::ConstObj< double >( Resistance * 3 ),
                                                    new object::ConstObj< double >( Capacity / 3 * 0.408379079339744 ) ) );
    serial->AddChild( new electrical::ParallelRC<>( new object::ConstObj< double >( Resistance * 3 ),
                                                    new object::ConstObj< double >( Capacity / 3 * 0.939 ) ) );
    serial->AddChild( new electrical::ParallelRC<>( new object::ConstObj< double >( Resistance * 3 ),
                                                    new object::ConstObj< double >( Capacity / 3 / 0.408379079339744 ) ) );

    double t = 0.000;
    double Current = 4.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    serial->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    serial->SetInitialCurrent( Current );
    serial->UpdateStateSystemGroup();

    systm::ConstantStepDglSystemSolver< myMatrixType > solver( &stateSystemGroup, 0.01 );

    while ( t < 15.50 )
    {
        t = solver.Solve();
        serial->CalculateStateDependentValues();
    }
    Current = -8;
    serial->SetCurrent( Current );

    while ( t < 31.00 )
    {
        serial->UpdateStateSystemGroup();
        t = solver.Solve();
        serial->CalculateStateDependentValues();
    }

    Current = 0;
    serial->SetCurrent( Current );

    while ( t < 40.00 )
    {
        serial->UpdateStateSystemGroup();
        t = solver.Solve();
        serial->CalculateStateDependentValues();
    }
}
void TestDaeSystem::testRealZarc()
{
    const double col[] = {5, 20, 35, 50, 65, 80, 90};
    std::vector< double > vecCol( col, col + 7 );

    const double row[] = {0, 10, 23, 50};
    std::vector< double > vecRow( row, row + 4 );

    boost::shared_ptr< ::state::ThermalState< double > > temp( new ::state::ThermalState< double >( 23 ) );
    boost::shared_ptr< electrical::state::Soc > soc( new electrical::state::Soc( 10, 0.5, vecCol ) );


    boost::shared_ptr< electrical::SerialTwoPort<> > serial( new electrical::SerialTwoPort<>() );


    const double tau[7][4] = {{1.12980227037822e-01, 1.71749789914134e-01, 4.27878577005388e-01, 4.70584769553997e-01},
                              {2.88029269499999e-01, 2.03069924403468e-01, 7.59434298204209e-02, 5.38248547348156e-02},
                              {1.80149147116020e-01, 1.12630469688383e-01, 3.52185854766679e-02, 2.98230802877780e-02},
                              {1.47909096582298e-01, 8.33921914682953e-02, 2.57060565921076e-02, 2.22501291807582e-02},
                              {1.32900242338032e-01, 7.22938740353130e-02, 2.05480235442034e-02, 9.95886997764575e-02},
                              {1.19852623435413e-01, 6.56657395160509e-02, 2.36971844841798e-02, 1.02914420936082e-01},
                              {1.18273000586515e-01, 6.60491071602289e-02, 3.25921294643322e-02, 1.09197268465146e-01}};


    const double ohm[7][4] = {{0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788},
                              {0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630},
                              {0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295},
                              {0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453},
                              {0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745},
                              {0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180},
                              {0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105}};

    const double phi[7][4] = {{0.574590000000000, 0.516891694022437, 0.408379079339744, 0.110443124808620},
                              {0.574590000000000, 0.516891694022437, 0.568073405078093, 0.566435038531618},
                              {0.616859078283881, 0.579111165549142, 0.662374262351899, 0.620812008168125},
                              {0.625502091112064, 0.610693210867254, 0.697319797270455, 0.671812046180353},
                              {0.621586176902636, 0.620972340223097, 0.721715387830978, 0.323239015612757},
                              {0.620607929391110, 0.630419920219851, 0.671002168217511, 0.371661290927877},
                              {0.620534462894923, 0.628465004550853, 0.585312180039373, 0.410302314031108}};

    boost::shared_ptr< object::LookupObj2dWithState< double > > tauObj(
     new object::LookupObj2dWithState< double >( CopyToVector( tau ), vecRow, vecCol, soc, temp ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > ohmObj(
     new object::LookupObj2dWithState< double >( CopyToVector( ohm ), vecRow, vecCol, soc, temp ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > phiObj(
     new object::LookupObj2dWithState< double >( CopyToVector( phi ), vecRow, vecCol, soc, temp ) );

    boost::shared_ptr< electrical::Zarc<> > zarc( new electrical::Zarc<>( tauObj, ohmObj, phiObj, 0.0000001, false ) );

    serial->AddChild( zarc );


    double t = 0.000;
    double Current = 4.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    serial->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    serial->SetInitialCurrent( Current );
    serial->UpdateStateSystemGroup();

    systm::ConstantStepDglSystemSolver< myMatrixType > solver( &stateSystemGroup, 0.01 );

    while ( t < 15.50 )
    {
        serial->UpdateStateSystemGroup();
        t = solver.Solve();
        serial->CalculateStateDependentValues();
    }

    Current = -8;
    serial->SetCurrent( Current );

    while ( t < 31.00 )
    {
        serial->UpdateStateSystemGroup();
        t = solver.Solve();
        serial->CalculateStateDependentValues();
    }

    Current = 0;
    serial->SetCurrent( Current );

    while ( t < 40.00 )
    {
        serial->UpdateStateSystemGroup();
        t = solver.Solve();
        serial->CalculateStateDependentValues();
    }
}


void TestDaeSystem::testSingleCellellement()
{
    const double row[] = {0, 10, 23, 50};
    std::vector< double > vecRow( row, row + 4 );

    const double col[] = {5, 20, 35, 50, 65, 80, 90};
    std::vector< double > vecCol( col, col + 7 );

    const double ocvColData[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    std::vector< double > ocvCol( ocvColData, ocvColData + 10 );

    const double tau[7][4] = {{1.12980227037822e-01, 1.71749789914134e-01, 4.27878577005388e-01, 4.70584769553997e-01},
                              {2.88029269499999e-01, 2.03069924403468e-01, 7.59434298204209e-02, 5.38248547348156e-02},
                              {1.80149147116020e-01, 1.12630469688383e-01, 3.52185854766679e-02, 2.98230802877780e-02},
                              {1.47909096582298e-01, 8.33921914682953e-02, 2.57060565921076e-02, 2.22501291807582e-02},
                              {1.32900242338032e-01, 7.22938740353130e-02, 2.05480235442034e-02, 9.95886997764575e-02},
                              {1.19852623435413e-01, 6.56657395160509e-02, 2.36971844841798e-02, 1.02914420936082e-01},
                              {1.18273000586515e-01, 6.60491071602289e-02, 3.25921294643322e-02, 1.09197268465146e-01}};

    const double ohm[7][4] = {{0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788},
                              {0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630},
                              {0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295},
                              {0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453},
                              {0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745},
                              {0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180},
                              {0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105}};

    const double phi[7][4] = {{0.574590000000000, 0.516891694022437, 0.408379079339744, 0.110443124808620},
                              {0.574590000000000, 0.516891694022437, 0.568073405078093, 0.566435038531618},
                              {0.616859078283881, 0.579111165549142, 0.662374262351899, 0.620812008168125},
                              {0.625502091112064, 0.610693210867254, 0.697319797270455, 0.671812046180353},
                              {0.621586176902636, 0.620972340223097, 0.721715387830978, 0.323239015612757},
                              {0.620607929391110, 0.630419920219851, 0.671002168217511, 0.371661290927877},
                              {0.620534462894923, 0.628465004550853, 0.585312180039373, 0.410302314031108}};

    const double tau2[7][4] = {{0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788},
                               {0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630},
                               {0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295},
                               {0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453},
                               {0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745},
                               {0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180},
                               {0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105}};

    const double ohm2[7][4] = {{0.0422632965521782, 0.0422632965521782, 0.0422632965521782, 0.0646154311756893},
                               {0.0122909999999999, 0.00820305593966980, 0.00584662021670523, 0.00383743519303631},
                               {0.0322225070656886, 0.0289438148360227, 0.0225495908462913, 0.00599634893454420},
                               {0.0257481760483287, 0.0186632656230284, 0.00993329376244036, 0.00582987109816202},
                               {0.0157426548912461, 0.0171684201770746, 0.0134953332473813, 0.00457332312382527},
                               {0.0218280618476754, 0.0131225853688649, 0.00741527720123879, 0.00329020951414971},
                               {0.0153521701301020, 0.00846967601820038, 0.00563955480281332, 0.00315825717209717}};

    const double phi2[7][4] = {{1, 0.957683980942010, 0.762474540690386, 0.810166837787303},
                               {1, 0.957683980942010, 0.837851589303327, 0.826206919812180},
                               {0.806534226878276, 0.755415801951905, 0.630228467428149, 0.686969202518359},
                               {0.708526377255388, 0.661628608281274, 0.630894213874104, 0.675415879938948},
                               {0.791964147404381, 0.697353484425094, 0.628110619592649, 0.801166605326404},
                               {0.759343341643463, 0.720390317831926, 0.755733232495091, 0.938336421996388},
                               {0.790468798628622, 0.797350100822233, 0.852903199927494, 0.986742576117900}};

    const double ocv[4][10] = {{3.47980625000000, 3.56020000000000, 3.61080000000000, 3.64033750000000, 3.67903750000000,
                                3.74791250000000, 3.84935000000000, 3.94420000000000, 4.05327500000000, 4.18230000000000},
                               {3.48386250000000, 3.56396250000000, 3.61388750000000, 3.64350000000000, 3.68183750000000,
                                3.75231250000000, 3.85146875000000, 3.94563750000000, 4.05550000000000, 4.18410000000000},
                               {3.48390000000000, 3.56456250000000, 3.61843125000000, 3.64930000000000, 3.68808750000000,
                                3.76500000000000, 3.85450000000000, 3.94680000000000, 4.05477500000000, 4.17770000000000},
                               {3.47570000000000, 3.55060000000000, 3.60896250000000, 3.64975000000000, 3.68677500000000,
                                3.75358750000000, 3.84280000000000, 3.93626250000000, 4.04295000000000, 4.16480000000000}};

    const double rser[7][4] = {{0.0288200000000001, 0.0273753907300395, 0.0264344729259093, 0.0135127660821939},
                               {0.0288200000000001, 0.0273753907300395, 0.0270767085789135, 0.0275280515729565},
                               {0.0290500142586083, 0.0277525443197526, 0.0275261486868454, 0.0276368633915343},
                               {0.0288630838514731, 0.0278576890117617, 0.0275537618404411, 0.0277933657755191},
                               {0.0286781206123875, 0.0277738617773271, 0.0275453220226692, 0.0259269199477185},
                               {0.0285331061111646, 0.0277867915477187, 0.0268358840093433, 0.0267808653711796},
                               {0.0286358289536196, 0.0277803384908296, 0.0257328031445100, 0.0272959924289106}};


    const double soc[] = {5, 10, 20, 30, 35, 40, 50, 60, 65, 70, 80, 90, 100};
    std::vector< double > socData( soc, soc + 13 );

    std::vector< std::vector< double > > ocvLookupData( 4, std::vector< double >( 10, 0 ) );
    for ( size_t i = 0; i < 4; ++i )
        ocvLookupData.at( i ).assign( ocv[i], ocv[i] + 10 );


    double HUGEsampleTime = 0.00000001;
    boost::scoped_ptr< electrical::SerialTwoPort<> > basePort( new electrical::SerialTwoPort<> );

    boost::shared_ptr< ::state::ThermalState< double > > tempState( new ::state::ThermalState< double >( 23 ) );
    boost::shared_ptr< electrical::state::Soc > socState( new electrical::state::Soc( 2.05, 20, socData ) );

    boost::shared_ptr< electrical::Cellelement<> > cellelem( new electrical::Cellelement<>( tempState, socState ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > tauObj1(
     new object::LookupObj2dWithState< double >( CopyToVector( tau ), vecRow, vecCol, tempState, socState ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > ohmObj1(
     new object::LookupObj2dWithState< double >( CopyToVector( ohm ), vecRow, vecCol, tempState, socState ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > phiObj1(
     new object::LookupObj2dWithState< double >( CopyToVector( phi ), vecRow, vecCol, tempState, socState ) );

    cellelem->AddChild( new electrical::Zarc<>( tauObj1, ohmObj1, phiObj1, HUGEsampleTime, false ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > tauObj2(
     new object::LookupObj2dWithState< double >( CopyToVector( tau2 ), vecRow, vecCol, tempState, socState ) );


    boost::shared_ptr< object::LookupObj2dWithState< double > > ohmObj2(
     new object::LookupObj2dWithState< double >( CopyToVector( ohm2 ), vecRow, vecCol, tempState, socState ) );


    boost::shared_ptr< object::LookupObj2dWithState< double > > phiObj2(
     new object::LookupObj2dWithState< double >( CopyToVector( phi2 ), vecRow, vecCol, tempState, socState ) );

    cellelem->AddChild( new electrical::Zarc<>( tauObj2, ohmObj2, phiObj2, HUGEsampleTime, false ) );


    boost::shared_ptr< object::LookupObj2dWithState< double > > ocvLookup(
     new object::LookupObj2dWithState< double >( ocvLookupData, ocvCol, vecRow, socState, tempState ) );

    boost::shared_ptr< object::LookupObj2dWithState< double > > rserLookup(
     new object::LookupObj2dWithState< double >( CopyToVector( rser ), vecRow, vecCol, tempState, socState ) );


    boost::shared_ptr< electrical::VoltageSource<> > voltSource( new electrical::VoltageSource<>( ocvLookup ) );
    boost::shared_ptr< electrical::OhmicResistance<> > ohmResistance( new electrical::OhmicResistance<>( rserLookup ) );
    cellelem->AddChild( voltSource );
    cellelem->AddChild( ohmResistance );

    basePort->AddChild( cellelem );

    double t = 0.0;
    double dt = 100;
    double Current = 2.05;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    basePort->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    basePort->SetInitialCurrent( Current );
    basePort->UpdateStateSystemGroup();
    stateSystemGroup.mDt = dt;    // erst jetzt Dt setzen, damit wir den InitialState der Socs testen können

    systm::DifferentialAlgebraicSystem< myMatrixType > test( &stateSystemGroup );

    myMatrixType &x = stateSystemGroup.mStateVector;

    const int REALSTATESIZE = 6;
    double initialoldstate[] = {0, 0, 0, 0, 0, 0};
    for ( size_t i = 0; i < REALSTATESIZE; ++i )
    {
        TS_ASSERT_DELTA( x( i, 0 ), initialoldstate[i], 0.001 );
    }

    TS_ASSERT_DELTA( socState->GetCapacity(), 1476, 0.001 );

    myMatrixType dxdt( REALSTATESIZE + 1, 1 );

    basePort->UpdateStateSystemGroup();    // Nochmal die Spannungen berechnen. Diesesmal mit Dt und einer Aenderung des
                                           // Soc
    test.PrepareEquationSystem();

#ifdef _EIGEN_
    test.operator()( x.topRows( REALSTATESIZE ), dxdt, t );
    x += dxdt * dt;
#else
    test.operator()( x.submat( 0, 0, REALSTATESIZE - 1, 0 ), dxdt, t );
    x.submat( 0, 0, REALSTATESIZE - 1, 0 ) += dxdt * dt;
#endif
    basePort->CalculateStateDependentValues();

    double intialdxdt[] = {0.0053, 0.1648, 1.4069, 0.0238, 0.6528, 0.8343};
    double newoldstate[] = {0.5333, 16.4840, 140.6924, 2.3802, 65.2889, 83.4350};
    for ( size_t i = 0; i < 6; ++i )
    {
        TS_ASSERT_DELTA( dxdt( i, 0 ), intialdxdt[i], 0.001 );
        TS_ASSERT_DELTA( x( i, 0 ), newoldstate[i], 0.001 );
    }
    TS_ASSERT_DELTA( socState->GetCapacity(), 1681, 0.001 );
}

void TestDaeSystem::testSingleCellellementBalancing()
{
    const double socData[] = {5, 10, 20, 30, 35, 40, 50, 60, 65, 70, 80, 90, 100};
    std::vector< double > soc( socData, socData + 13 );
    boost::shared_ptr< ::state::ThermalState< double > > tempState( new ::state::ThermalState< double >( 23 ) );
    boost::shared_ptr< electrical::state::Soc > socState( new electrical::state::Soc( 2.05, 20, soc ) );

    boost::shared_ptr< electrical::Cellelement<> > cellelem( new electrical::Cellelement<>( tempState, socState ) );
    cellelem->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 3 ) ) );
    cellelem->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 3 ) ) );

    double t = 0.0;
    double dt = 0.001;
    double Current = 10;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    cellelem->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    cellelem->SetInitialCurrent( Current );
    cellelem->UpdateStateSystemGroup();
    stateSystemGroup.mDt = dt;    // erst jetzt Dt setzen, damit wir den InitialState der Socs testen können

    systm::DifferentialAlgebraicSystem< myMatrixType > test( &stateSystemGroup );

    myMatrixType &x = stateSystemGroup.mStateVector;
    myMatrixType dxdt = myMatrixType( 1, 1 );


    cellelem->SetCurrentFromActiveBalancer( -5 );
    cellelem->UpdateStateSystemGroup();    // Nochmal die Spannungen berechnen. Diesesmal mit Dt und einer Aenderung des
                                           // Soc
    test.PrepareEquationSystem();
#ifdef _EIGEN_
    test( x.topRows( dxdt.n_rows ), dxdt, t );    // TODO: FHU topRows wrapper
#else
    test( x.submat( 0, 0, dxdt.n_rows - 1, 0 ), dxdt, t );    // TODO: FHU topRows wrapper
#endif
    cellelem->CalculateStateDependentValues();
    TS_ASSERT_EQUALS( cellelem->GetCurrentValue(), 5 )        // 10 - 5
    TS_ASSERT_EQUALS( cellelem->GetVoltageValue(), 5 * 3 )    // 10 - 5

#ifdef _EIGEN_
    x.topRows( dxdt.n_rows ) += dxdt * dt;    // TODO: FHU topRows wrapper
#else
    x.submat( 0, 0, dxdt.n_rows - 1, 0 ) += dxdt * dt;        // TODO: FHU topRows wrapper
#endif
    cellelem->SetCurrent( Current );

    cellelem->SetCurrentFromActiveBalancer( -5 );
    cellelem->UpdateStateSystemGroup();    // Nochmal die Spannungen berechnen. Diesesmal mit Dt und einer Aenderung des
                                           // Soc
    test.PrepareEquationSystem();
#ifdef _EIGEN_
    test( x.topRows( dxdt.n_rows ), dxdt, t );    // TODO: FHU topRows wrapper
#else
    test( x.submat( 0, 0, dxdt.n_rows - 1, 0 ), dxdt, t );    // TODO: FHU topRows wrapper
#endif
    cellelem->CalculateStateDependentValues();

    TS_ASSERT_EQUALS( cellelem->GetCurrentValue(), 5 )                          // 10 - 5
    TS_ASSERT_EQUALS( cellelem->GetVoltageValue(), 5 * 3 + 0.001 / 3.0 * 5 )    // 10 - 5
}

void TestDaeSystem::testMultiCellellementBalancing()
{

    const double socData[] = {5, 10, 20, 30, 35, 40, 50, 60, 65, 70, 80, 90, 100};
    std::vector< double > soc( socData, socData + 13 );

    boost::shared_ptr< ::state::ThermalState< double > > tempState( new ::state::ThermalState< double >( 23 ) );
    boost::shared_ptr< electrical::state::Soc > socState( new electrical::state::Soc( 2.05, 20, soc ) );

    boost::shared_ptr< electrical::ParallelTwoPort<> > para( new electrical::ParallelTwoPort<>() );

    boost::shared_ptr< electrical::Cellelement<> > cellelem( new electrical::Cellelement<>( tempState, socState ) );
    cellelem->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );
    cellelem->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 3 ) ) );

    boost::shared_ptr< electrical::Cellelement<> > cellelem2( new electrical::Cellelement<>( tempState, socState ) );
    cellelem2->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 10 ) ) );
    cellelem2->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 3 ) ) );

    para->AddChild( cellelem );
    para->AddChild( cellelem2 );
    double dt = 0.001;
    double Current = 10;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    para->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    para->SetInitialCurrent( Current );
    para->UpdateStateSystemGroup();
    stateSystemGroup.mDt = dt;    // erst jetzt Dt setzen, damit wir den InitialState der Socs testen können

    systm::DifferentialAlgebraicSystem< myMatrixType > test( &stateSystemGroup );

    myMatrixType dxdt = myMatrixType( 3, 1 );

    para->SetCurrent( Current );
    cellelem->SetCurrentFromActiveBalancer( 2 );
    cellelem2->SetCurrentFromActiveBalancer( 0 );

    para->UpdateStateSystemGroup();    // Nochmal die Spannungen berechnen. Diesesmal mit Dt und einer Aenderung des Soc

    systm::ConstantStepDglSystemSolver< myMatrixType > solver( &stateSystemGroup, 0.01 );

    para->UpdateStateSystemGroup();
    solver.Solve();
    para->CalculateStateDependentValues();
    cellelem->CalculateStateDependentValues();
    cellelem2->CalculateStateDependentValues();
    TS_ASSERT_DELTA( cellelem->GetCurrentValue(), 6, 0.001 )
    TS_ASSERT_DELTA( cellelem2->GetCurrentValue(), 6, 0.001 )

    para->SetCurrent( Current );
    para->UpdateStateSystemGroup();
    solver.Solve();
    para->CalculateStateDependentValues();
    cellelem->CalculateStateDependentValues();
    cellelem2->CalculateStateDependentValues();

    TS_ASSERT_DELTA( cellelem->GetCurrentValue(), 5, 0.001 )
    TS_ASSERT_DELTA( cellelem2->GetCurrentValue(), 5, 0.001 )


    para->UpdateStateSystemGroup();
    solver.Solve();
    para->SetCurrent( Current );

    cellelem->SetCurrentFromActiveBalancer( 1 );
    cellelem2->SetCurrentFromActiveBalancer( 1 );

    para->UpdateStateSystemGroup();
    solver.Solve();
    para->CalculateStateDependentValues();
    cellelem->CalculateStateDependentValues();
    cellelem2->CalculateStateDependentValues();
    TS_ASSERT_DELTA( cellelem->GetCurrentValue(), 6, 0.001 )
    TS_ASSERT_DELTA( cellelem2->GetCurrentValue(), 6, 0.001 )
}

void TestDaeSystem::testVariableStepSolverWithParallelRCMindingResults()
{
#if defined( _EIGEN_ ) || defined( _ARMADILLO_ )
    boost::shared_ptr< electrical::SerialTwoPort<> > serial( new electrical::SerialTwoPort<>() );

    boost::shared_ptr< object::Object< double > > objR( new object::ConstObj< double >( 0.6 ) );
    boost::shared_ptr< object::Object< double > > objTau( new object::ConstObj< double >( 0.6 * 0.5 ) );
    boost::shared_ptr< electrical::ParallelRC<> > parallelRC( new electrical::ParallelRC<>( objR, objTau, true ) );

    serial->AddChild( parallelRC );


    double t = 0.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    serial->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    serial->SetInitialCurrent( 2.0 );
    serial->UpdateStateSystemGroup();

    systm::VariableStepDglSystemSolver< myMatrixType > solver(
     &stateSystemGroup );    // systm::ConstantStepDglSystemSolver< myMatrixType > solver(&stateSystemGroup, 0.001);

    while ( t < 1.0 )
    {
        serial->UpdateStateSystemGroup();
        t = solver.Solve();
        serial->CalculateStateDependentValues();
        TS_ASSERT_DELTA( serial->GetVoltageValue(), 2.0 * 0.6 * ( 1 - exp( -( t - 0.0 ) / 0.3 ) ), 0.01 );
    }
    double lastCurrentSwitchTime = t;
    double lastVoltageValue = serial->GetVoltageValue();

    serial->SetCurrent( -0.4 );
    while ( t < 3.0 )
    {
        serial->UpdateStateSystemGroup();
        t = solver.Solve();
        serial->CalculateStateDependentValues();
        TS_ASSERT_DELTA( serial->GetVoltageValue(),
                         ( lastVoltageValue + 0.4 * 0.6 ) * exp( -( t - lastCurrentSwitchTime ) / 0.3 ) - 0.4 * 0.6, 0.01 );
    }
    lastCurrentSwitchTime = t;
    lastVoltageValue = serial->GetVoltageValue();

    serial->SetCurrent( 0.0 );
    while ( t < 6.0 )
    {
        serial->UpdateStateSystemGroup();
        t = solver.Solve();
        serial->CalculateStateDependentValues();
        TS_ASSERT_DELTA( serial->GetVoltageValue(), lastVoltageValue * exp( -( t - lastCurrentSwitchTime ) / 0.3 ), 0.01 );
    }
#endif
}
