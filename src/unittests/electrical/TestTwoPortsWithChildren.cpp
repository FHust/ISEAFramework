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
* File Name : TestTwoPortsWithChildren.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 24 Mar 2014 16:43:56 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestTwoPortsWithChildren.h"

#include "../../misc/matrixInclude.h"

#include "../../electrical/paralleltwoport.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/capacity.h"
#include "../../electrical/ohmicresistance.h"

#include "../../object/const_obj.h"


void TestTwoPortsWithChildren::testParallelPort()
{
    electrical::ParallelTwoPort< myMatrixType > pport;
}

void TestTwoPortsWithChildren::testSerialPort()
{
    electrical::SerialTwoPort< myMatrixType > sport;
}

void TestTwoPortsWithChildren::testParallelPortOperation()
{
    electrical::ParallelTwoPort< myMatrixType > pport;
    TS_ASSERT_EQUALS(pport.GetParallelChildren(), 0);
    TS_ASSERT_EQUALS(pport.HasChildren(), false);
    TS_ASSERT_THROWS(pport.at(0), std::out_of_range);
    TS_ASSERT_EQUALS(pport.HasSystem(), false);
    TS_ASSERT_EQUALS(pport.size(), 0);
}

void TestTwoPortsWithChildren::testSerialPortOperation()
{
    electrical::SerialTwoPort< myMatrixType > sport;
    TS_ASSERT_EQUALS(sport.HasChildren(), false);
    TS_ASSERT_THROWS(sport.at(0), std::out_of_range);
    TS_ASSERT_EQUALS(sport.HasSystem(), false);
    TS_ASSERT_EQUALS(sport.size(), 0);
}

void TestTwoPortsWithChildren::testParallelPortChildren()
{
    electrical::ParallelTwoPort< myMatrixType > pport;
    electrical::Capacity< myMatrixType > *res = new electrical::Capacity< myMatrixType >(new object::ConstObj<double>( 10 ));
    pport.AddChild(new electrical::Capacity< myMatrixType >(new object::ConstObj<double>( 10 )));
    pport.AddChild(new electrical::Capacity< myMatrixType >(new object::ConstObj<double>( 10 )));
    pport.AddChild(new electrical::Capacity< myMatrixType >(new object::ConstObj<double>( 10 )));
    pport.AddChild(res);

    TS_ASSERT_EQUALS(pport.size(), 4);
    TS_ASSERT_EQUALS(pport.GetParallelChildren(), 4);

#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS_NOTHING(pport.at(0));
    TS_ASSERT_THROWS_NOTHING(pport.at(1));
    TS_ASSERT_THROWS_NOTHING(pport.at(2));
    TS_ASSERT_THROWS_NOTHING(pport.at(3));
#endif

    TS_ASSERT_THROWS(pport.at(5), std::out_of_range);

}

void TestTwoPortsWithChildren::testSerialPortChildren()
{
    electrical::SerialTwoPort< myMatrixType > pport;
    electrical::Capacity< myMatrixType > *res = new electrical::Capacity< myMatrixType >(new object::ConstObj<double>( 10 ));
    pport.AddChild(new electrical::OhmicResistance< myMatrixType >(new object::ConstObj<double>( 10 )));
    pport.AddChild(new electrical::OhmicResistance< myMatrixType >(new object::ConstObj<double>( 10 )));
    pport.AddChild(new electrical::OhmicResistance< myMatrixType >(new object::ConstObj<double>( 10 )));
    pport.AddChild(res);

    TS_ASSERT_EQUALS(pport.size(), 4);

#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS_NOTHING(pport.at(0));
    TS_ASSERT_THROWS_NOTHING(pport.at(1));
    TS_ASSERT_THROWS_NOTHING(pport.at(2));
    TS_ASSERT_THROWS_NOTHING(pport.at(3));
#endif

    TS_ASSERT_THROWS(pport.at(5), std::out_of_range);
}
