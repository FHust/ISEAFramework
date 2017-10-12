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
* File Name : TestVoltageCurrentPowerInjection.cpp
* Creation Date : 07-03-2016
* Last Modified : Di 08 Mär 2016 16:03:59 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestVoltageCurrentPowerInjection.h"

// STD
#include <memory>
// BOOST
#include <boost/make_shared.hpp>

// ETC
#include "../../object/const_obj.h"

#include "../../electrical/twoport.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/capacity.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/constantphaseelement.h"
#include "../../electrical/inductance.h"
#include "../../electrical/voltagesource.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/paralleltwoport.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/parallelRCAlg.h"
#include "../../electrical/warburgTanh.h"
#include "../../electrical/warburgCotanh.h"
#include "../../electrical/zarc.h"
#include "../../electrical/zarcalg.h"

void Asserter( ElectricalDataStruct< double > *data )
{
    TS_ASSERT_EQUALS( data->mCurrentValue, 1.0 )
    TS_ASSERT_EQUALS( data->mVoltageValue, 2.0 )
    TS_ASSERT_EQUALS( data->mPowerValue, 3.0 )
}

template < typename MatrixT, template < typename > class TConcrete >
void TestVoltageCurrentPowerInjection::TwoArgs()
{
    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );
    TConcrete< MatrixT > tester( false, dataStruct );
    Asserter( tester.mDataStruct.get() );
}

template < typename MatrixT, template < typename > class TConcrete >
void TestVoltageCurrentPowerInjection::ThreeArgs()
{
    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );
    boost::shared_ptr< object::Object< double > > obj( 0 );
    TConcrete< MatrixT > tester( obj, false, dataStruct );
    Asserter( tester.mDataStruct.get() );
}

template < typename MatrixT, template < typename > class TConcrete >
void TestVoltageCurrentPowerInjection::FourArgs()
{
    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );
    boost::shared_ptr< object::Object< double > > obj( 0 );
    TConcrete< MatrixT > tester( obj, obj, false, dataStruct );
    Asserter( tester.mDataStruct.get() );
}

template < typename MatrixT, template < typename > class TConcrete >
void TestVoltageCurrentPowerInjection::FiveArgs()
{
    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );
    boost::shared_ptr< object::Object< double > > obj( new object::ConstObj< double >( 10.0 ) );
    TConcrete< MatrixT > tester( obj, obj, obj, 0.001, false, dataStruct );
    Asserter( tester.mDataStruct.get() );
}

void TestVoltageCurrentPowerInjection::TestTwoArgumentsTwoPorts()

{    // Two arguments

    this->TwoArgs< myMatrixType, electrical::TwoPort >();
    this->TwoArgs< myMatrixType, electrical::SerialTwoPort >();
    this->TwoArgs< myMatrixType, electrical::ParallelTwoPort >();
    this->TwoArgs< myMatrixType, electrical::WarburgTanh >();
    this->TwoArgs< myMatrixType, electrical::WarburgCotanh >();
}

void TestVoltageCurrentPowerInjection::TestThreeArgumentsTwoPorts()
{
    // Three arguments
    this->ThreeArgs< myMatrixType, electrical::OhmicResistance >();
    this->ThreeArgs< myMatrixType, electrical::Capacity >();
    this->ThreeArgs< myMatrixType, electrical::VoltageSource >();
    this->ThreeArgs< myMatrixType, electrical::ConstantPhaseElement >();
    this->ThreeArgs< myMatrixType, electrical::Inductance >();
}

void TestVoltageCurrentPowerInjection::TestFourArgumentsTwoPorts()
{
    // Four arguments
    this->FourArgs< myMatrixType, electrical::ParallelRC >();
    this->FourArgs< myMatrixType, electrical::ParallelRCAlg >();
}

void TestVoltageCurrentPowerInjection::TestFiveArgumentsTwoPorts()
{
    this->FiveArgs< myMatrixType, electrical::Zarc >();
    this->FiveArgs< myMatrixType, electrical::ZarcAlg >();
}

void TestVoltageCurrentPowerInjection::TestEveryhtingElse()
{
    boost::shared_ptr< ::state::ThermalState< double > > temp;
    boost::shared_ptr< electrical::state::Soc > socState;

    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );

    electrical::Cellelement< myMatrixType > tester( temp, socState, false, dataStruct );
    Asserter( tester.mDataStruct.get() );
}
