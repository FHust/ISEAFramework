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
* File Name : TestSlice.h
* Creation Date : 08-04-2015
* Last Modified : Mi 15 Apr 2015 11:34:17 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSLICE_
#define _TESTSLICE_
#include <cxxtest/TestSuite.h>

/// DESCRIPTION
class TestSlice : public CxxTest::TestSuite
{
    public:
    void TestSliceCreation();
    void TestSliceEditing();
};
#endif /* _TESTSLICE_ */
