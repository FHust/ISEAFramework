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
* File Name : TestMapGetSet.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:54:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestMapGetSet.h"

#include <vector>
#include "../../misc/mapGetSet.h"

void TestMapGetSet::TestCreate()
{
    MapGetSet<double, double > test;
}

void TestMapGetSet::TestAddGet()
{
    MapGetSet<double, double > test;
    test.Add(4.0, 123.0);
    test.Add(5.0, 66.0);
    test.Add(5.0, 77.0);
    TS_ASSERT_EQUALS(test.Get(4.0)[0], 123.0);
    TS_ASSERT_EQUALS(test.Get(5.0)[0], 66.0);
    TS_ASSERT_EQUALS(test.Get(5.0)[1], 77.0);

    TS_ASSERT_EQUALS(test.Get(0).empty(), true);
}


