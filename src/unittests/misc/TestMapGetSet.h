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
* File Name : TestMapGetSet.h
* Creation Date : 26-08-2013
* Last Modified : Fr 21 Feb 2014 11:55:40 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMAPGETSET_
#define _TESTMAPGETSET_
#include <cxxtest/TestSuite.h>

class TestMapGetSet : public CxxTest::TestSuite
{
    public:
    void TestCreate();
    void TestAddGet();
};

#endif /* _TESTMAPGETSET_ */
