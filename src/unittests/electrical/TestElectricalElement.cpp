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
* File Name : TestElectricalElement.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 01 Sep 2014 14:47:40 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestElectricalElement.h"


#include "../../electrical/capacity.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/voltagesource.h"
#include "../../electrical/cellelement.h"

#include "../../object/const_obj.h"
#include "../../object/lookup_obj1d.h"
#include "../../object/lookup_obj2d.h"

static const double sDelta = 0.000001;


void TestElectricalElement::testElectricalElementOhmicConst()
{
    double Resistance( 10.0 );

    boost::shared_ptr< object::Object< double > > obj( new object::ConstObj< double >( Resistance ) );
    electrical::OhmicResistance< myMatrixType > test( obj );
    myMatrixType current;
    current.zeros( 1, 2 );

#ifndef _SYMBOLIC_
    current( 0, 1 ) = 10;
    test.SetCurrent( current );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 1 ), current( 0, 1 ) * Resistance );
    TS_ASSERT( obj.get() == test() );
    TS_ASSERT( obj.get() == test.GetObject() );
#else
    current( 0, 1 ) = symbolic::Symbolic( "InputCurrent" );
    test.SetCurrent( current );
    TS_ASSERT( ( *test.GetVoltage() )( 0, 0 ).IsEmpty() );

    symbolic::Symbolic left = ( *test.GetVoltage() )( 0, 1 );
    symbolic::Symbolic right = current( 0, 1 ) * symbolic::Symbolic( "ID0_ObjBase" );
    TS_ASSERT( left == right );
    TS_ASSERT( obj.get() == test() );
    TS_ASSERT( obj.get() == test.GetObject() );
#endif
}

void TestElectricalElement::testElectricalElementOhmic1DLookup()
{
    const double mPointer[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector< double > measurementpoint;
    measurementpoint.insert( measurementpoint.begin(), mPointer, mPointer + 9 );

    const double dPointer[] = {0.1, 1.2, 9.3, 1.4, 5, 6, 7, 8, 9};
    std::vector< double > data;
    data.insert( data.begin(), dPointer, dPointer + 9 );

    electrical::OhmicResistance< myMatrixType > test( new object::LookupObj1D< double >( data, measurementpoint ) );
    myMatrixType current;
    current.zeros( 1, 2 );

#ifndef _SYMBOLIC_
    current( 0, 1 ) = 10;
    test.SetCurrent( current );
    test()->GetValue( 1.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 0, 0.00001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 1 ), current( 0, 1 ) * 0.1, 0.00001 );

    test()->GetValue( 2.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 0, 0.00001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 1 ), current( 0, 1 ) * 1.2, 0.000001 );

    test()->GetValue( 8.5 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 0, 0.00001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 1 ), current( 0, 1 ) * 8.5, 0.00001 );
#else
    test.SetID( 23 );
    current( 0, 1 ) = symbolic::Symbolic( "InputCurrent" );
    test.SetCurrent( current );
    TS_ASSERT( ( *test.GetVoltage() )( 0, 0 ).IsEmpty() );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 1 ), current( 0, 1 ) * symbolic::Symbolic( "ID23_ObjBase" ) );
#endif
}

void TestElectricalElement::testElectricalElementVoltageSourceConst()
{
    double voltage = 10;
    electrical::VoltageSource< myMatrixType > test( new object::ConstObj< double >( voltage ) );
    myMatrixType current;
    current.zeros( 1, 10 );

#ifndef _SYMBOLIC_
    current( 0, 1 ) = 10;
#else
    current( 0, 1 ) = symbolic::Symbolic( "InputCurrent" );
#endif
    test.SetCurrent( current );
}

void TestElectricalElement::testElectricalElementOhmic2DLookup()
{
    double const mpRow[] = {1, 2, 3};
    std::vector< double > measurementpointRow( mpRow, mpRow + 3 );

    double const mpCol[] = {1, 2, 3};
    std::vector< double > measurementpointColumn( mpCol, mpCol + 3 );

    double const mpData[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    std::vector< std::vector< double > > data( 3, std::vector< double >( 3, 0 ) );
    for ( size_t i = 0; i < 3; ++i )
        data.at( i ).assign( mpData[i], mpData[i] + 3 );

    electrical::OhmicResistance< myMatrixType > test(
     new object::LookupObj2D< double >( data, measurementpointRow, measurementpointColumn ) );
    myMatrixType current;
    current.zeros( 1, 2 );

#ifndef _SYMBOLIC_
    current( 0, 1 ) = 10;
    test.SetCurrent( current );
    test()->GetValue( 1.0, 1.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 1 ), current( 0, 1 ) * 1 );

    test()->GetValue( 2.0, 2.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 1 ), current( 0, 1 ) * 5.0 );

    test()->GetValue( 3.0, 3.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), 0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 1 ), current( 0, 1 ) * 9.0 );
#endif
}

void TestElectricalElement::testElectricalElementVoltageSource1DLookup()
{
    const double mp[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector< double > measurementpoint( mp, mp + 9 );
    const double mpData[] = {0.1, 1.2, 9.3, 1.4, 5, 6, 7, 8, 9};
    std::vector< double > data( mpData, mpData + 9 );

    electrical::VoltageSource< myMatrixType > test( new object::LookupObj1D< double >( data, measurementpoint ) );
    myMatrixType current;
    current.zeros( 1, 10 );

#ifndef _SYMBOLIC_
    current( 0, 1 ) = 10;
    test.SetCurrent( current );
    test()->GetValue( 1.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 9 ), 0.1, 0.001 );

    test()->GetValue( 2.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 9 ), 1.2, 0.001 );

    test()->GetValue( 8.5 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 9 ), 8.5, 0.001 );
#else
    test.SetID( 12 );
    current( 0, 1 ) = symbolic::Symbolic( "InputCurrent" );
    test.SetCurrent( current );
    TS_ASSERT( ( *test.GetVoltage() )( 0, 0 ).IsEmpty() );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 9 ), symbolic::Symbolic( "ID12_ObjBase" ) );
#endif
}

void TestElectricalElement::testElectricalElementVoltageSource2DLookup()
{
    double const mpRow[] = {1, 2, 3};
    std::vector< double > measurementpointRow( mpRow, mpRow + 3 );

    double const mpCol[] = {1, 2, 3};
    std::vector< double > measurementpointColumn( mpCol, mpCol + 3 );

    double const mpData[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    std::vector< std::vector< double > > data( 3, std::vector< double >( 3, 0 ) );
    for ( size_t i = 0; i < 3; ++i )
        data.at( i ).assign( mpData[i], mpData[i] + 3 );

    electrical::VoltageSource< myMatrixType > test(
     new object::LookupObj2D< double >( data, measurementpointRow, measurementpointColumn ) );
    myMatrixType current;
    current.zeros( 1, 10 );

#ifndef _SYMBOLIC_
    current( 0, 1 ) = 10;
    test.SetCurrent( current );
    test()->GetValue( 1.0, 1.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 9 ), 1, 0.001 );

    test()->GetValue( 2.0, 2.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 9 ), 5.0, 0.001 );

    test()->GetValue( 3.0, 3.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 9 ), 9.0, 0.001 );
#endif
}

void TestElectricalElement::testElectricalParllelRCCreation()
{
    double Resistance( 10.0 );
    double Capacity( 0.1 );

    boost::shared_ptr< object::Object< double > > rObj( new object::ConstObj< double >( Resistance ) );
    boost::shared_ptr< object::Object< double > > tauObj( new object::ConstObj< double >( Capacity ) );
    electrical::ParallelRC< myMatrixType > test( rObj, tauObj );

    TS_ASSERT( rObj.get() == test() );
    TS_ASSERT( tauObj.get() == test.GetTauObject() );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    test.SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( 9 );


#ifndef _SYMBOLIC_
    myMatrixType current;
    current.zeros( 1, 10 );
    current( 0, 1 ) = 5.4;

    test.SetID( 12 );
    test.SetCurrent( current );
    TS_ASSERT_DELTA( ( *test.GetVoltage() )( 0, 0 ), 1.0, sDelta );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix()( 0, 1 ), 5.4 * Resistance / Capacity, sDelta );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix()( 0, 0 ), -1.0 / Capacity, sDelta );

    TS_ASSERT_EQUALS( test.GetTauValue(), Capacity );
#else
    myMatrixType current;
    current.zeros( 1, 10 );
    current( 0, 1 ) = symbolic::Symbolic( "InputCurrent" );

    test.SetID( 12 );
    test.SetCurrent( current );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() )( 0, 0 ), symbolic::Symbolic( 1.0 ) );
    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix()( 0, 1 ),
                      symbolic::Symbolic( "DIV(MUL(InputCurrent,ID12_ObjBase),ID12_ObjTau)" ) );
    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix()( 0, 0 ),
                      symbolic::Symbolic( "NEG(DIV(1.000000,ID12_ObjTau))" ) );

    TS_ASSERT_EQUALS( test.GetTauValue(), symbolic::Symbolic( "ID12_ObjTau" ) );
#endif
}

void TestElectricalElement::testElectricalElementOhmPower()
{
    double Resistance( 10.0 );
    myMatrixType current;
    current.zeros( 1, 2 );
    current( 0, 0 ) = 1;
    current( 0, 1 ) = 1;

    //        myMatrixType state(2,1);
    //        state(0,0) = 2;
    //        state(1,0) = -0.5;

    electrical::OhmicResistance< myMatrixType > test( new object::ConstObj< double >( Resistance ) );
    test.SetCurrent( current );
    test.GetVoltage();
    //        TS_ASSERT_DELTA(test.GetPower(), 22.5, 0.0001)
}

void TestElectricalElement::testElectricalElementCapPower()
{
    double Capacity( 10.0 );
    myMatrixType current;
    current.zeros( 1, 2 );

#ifndef _SYMBOLIC_
    current( 0, 0 ) = 1;
    current( 0, 1 ) = 1;
#else
    current( 0, 0 ) = symbolic::Symbolic( "Var0" );
    current( 0, 1 ) = symbolic::Symbolic( "Var1" );
#endif

    //        myMatrixType state(2,1);
    //        state(0,0) = 2;
    //        state(1,0) = -0.5;

    electrical::Capacity< myMatrixType > test( new object::ConstObj< double >( Capacity ) );
    test.SetCurrent( current );
    test.GetVoltage();
    //        TS_ASSERT_DELTA(test.GetPower(), 0, 0.0001)
}

void TestElectricalElement::testElectricalElementParallelRCPower()
{
    double Resistance( 10.0 );
    myMatrixType current;
    current.zeros( 1, 2 );

#ifndef _SYMBOLIC_
    current( 0, 0 ) = 1;
    current( 0, 1 ) = 1;
#else
    current( 0, 0 ) = symbolic::Symbolic( "Var0" );
    current( 0, 1 ) = symbolic::Symbolic( "Var1" );
#endif

    //        myMatrixType state(2,1);
    //        state(0,0) = 2;
    //        state(1,0) = -0.5;

    electrical::ParallelRC< myMatrixType > test( new object::ConstObj< double >( Resistance ),
                                                 new object::ConstObj< double >( 1.0 / Resistance ) );
    test.SetCurrent( current );
    test.GetVoltage();
    //        TS_ASSERT_DELTA(test.GetPower(), 0.4, 0.0001)
}

void TestElectricalElement::testCellelement()
{
    std::vector< double > measurmentpoints;
    measurmentpoints.push_back( 0.3 );
    measurmentpoints.push_back( 1.35 );
    boost::shared_ptr< ::state::ThermalState< double > > thermalState( new ::state::ThermalState< double >( 47.0 ) );
    boost::shared_ptr< electrical::state::Soc > socState( new electrical::state::Soc( 1.0, 0.5, measurmentpoints ) );
    electrical::Cellelement< myMatrixType > test( thermalState, socState );

    TS_ASSERT_EQUALS( test.GetThermalState().get(), thermalState.get() );
    TS_ASSERT_EQUALS( test.GetSoc().get(), socState.get() );
}
