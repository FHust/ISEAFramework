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
* File Name : TestFuncObj.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 24 Nov 2014 18:57:44 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestFuncObj.h"

//STD
#include <cmath>

//ETC
#include "../../object/function_obj1d.h"

void TestFunctObj::testFuncObjectCreate()
{
    object::FunctionObj<double, ::sin> obj;
    TS_ASSERT_EQUALS( strcmp(obj.GetName(), "FunctionObj"), 0);
}

void TestFunctObj::testFuncObjectEvaluation()
{
    object::FunctionObj<double, ::sin> obj;
    double value(0);
    for( size_t i = 0 ; i < 360 ; ++i ) // Walk in a circle
    {
        value = static_cast<double>(i) / 360 * 2 * pi;
        TS_ASSERT_EQUALS( obj(value), ::sin(value));
        TS_ASSERT_EQUALS( obj(), ::sin(value));
    }
}

