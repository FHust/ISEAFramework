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
* File Name : TestCartesian.cpp
* Creation Date : 04-03-2015
* Last Modified : Mi 04 Mär 2015 17:42:31 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestCartesian.h"

void TestCartesian::TestCartesianDistance()
{
    geometry::Cartesian<> zero;
    TS_ASSERT_EQUALS(zero.GetX(), 0.0);
    TS_ASSERT_EQUALS(zero.GetY(), 0.0);
    TS_ASSERT_EQUALS(zero.GetZ(), 0.0);

    geometry::Cartesian<> cartesian(1.2, 2.0, 3.0);
    TS_ASSERT_DELTA( zero.Distance(cartesian), cartesian.Distance(zero), 0.00001);
    TS_ASSERT_DELTA( zero.Distance(cartesian), 3.8, 0.00001);
    TS_ASSERT_DELTA( cartesian.Distance(zero), 3.8, 0.00001);

    geometry::Cartesian<> cartesian10(10, 10, 10);
    TS_ASSERT_DELTA( cartesian.Distance(cartesian10), 13.8, 0.00001);
    TS_ASSERT_DELTA( zero.Distance(cartesian10), 17.3205, 0.00001);


}
