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
* File Name : TestBoolVectorOper.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:37:52 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestBoolVectorOper.h"
#include <vector>

#include "../../misc/boolVecOper.h"

void TestBoolVectorOperation::TestOperation()
{
    {
        std::vector<bool> test;
        test.push_back(true);
        test.push_back(true);
        test.push_back(true);
        TS_ASSERT_EQUALS(Any(test), true);
        TS_ASSERT_EQUALS(All(test), true);
    }

    {
        std::vector<bool> test;
        test.push_back(false);
        test.push_back(true);
        test.push_back(true);
        TS_ASSERT_EQUALS(Any(test), true);
        TS_ASSERT_EQUALS(All(test), false);
    }

    {
        std::vector<bool> test;
        test.push_back(false);
        test.push_back(false);
        test.push_back(false);
        TS_ASSERT_EQUALS(Any(test), false);
        TS_ASSERT_EQUALS(All(test), false);

    }
}


