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
* File Name : TestLookupObj.h
* Creation Date : 31-10-2012
* Last Modified : Fr 21 Feb 2014 12:03:07 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTLOOKUPOBJ_
#define _TESTLOOKUPOBJ_
#include <cxxtest/TestSuite.h>

class TestLookupObj : public CxxTest::TestSuite
{
    public:
    void testLookupObj1dCreate();
    void testLookupObj1dOperator_linearInterpolation();
    void testLookupObj1dOperator_nearestNeighbour();
    void testLookupObj1dOperator_constantUntilThreshold();
    void testLookupObj2dCreate();
    void testLookupObj2dOperator_linearInterpolation();
    void testLookupObj2dOperator_nearestNeighbour();
    void testLookupObj2dOperator_constantUntilThreshold();
    void testLookupObj1WithStatedCreate();
    void testLookupObj1WithStatedOperations();
    void testLookupObj2WithStatedCreate();
};
#endif /* _TESTLOOKUPOBJ_ */
