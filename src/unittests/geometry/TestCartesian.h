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
* File Name : TestCartesian.h
* Creation Date : 04-03-2015
* Last Modified : Mi 04 Mär 2015 17:41:11 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTCARTESIAN_
#define _TESTCARTESIAN_

#include <cxxtest/TestSuite.h>
#include "../../geometry/cartesian.h"

class TestCartesian : public CxxTest::TestSuite
{
    public:
    void TestCartesianDistance();
};


#endif /* _TESTCARTESIAN_ */
