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
* File Name : TestConstObj.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 24 Nov 2014 18:58:05 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestConstObj.h"

//ETC
#include "../../object/const_obj.h"

void TestConstObj::testConstObjectCreate()
{
    object::ConstObj<double> test( 1.0 );
    TS_ASSERT_EQUALS( strcmp(test.GetName(), "ConstObject"), 0);
}

void TestConstObj::testConstObjectGetValue()
{
    float Z = 1.0;
    object::ConstObj<float> test( Z );
    TS_ASSERT_EQUALS(Z, test.GetValue());
}


