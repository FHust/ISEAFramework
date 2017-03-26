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
* File Name : TestTwoPortsWithChildren.h
* Creation Date : 02-11-2012
* Last Modified : Fr 21 Feb 2014 10:33:30 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTTWOPORTSWIILDREN_
#define _TESTTWOPORTSWIILDREN_
#include <cxxtest/TestSuite.h>

class TestTwoPortsWithChildren : public CxxTest::TestSuite
{
    public:
    void testParallelPort();
    void testSerialPort();
    void testParallelPortOperation();
    void testSerialPortOperation();
    void testParallelPortChildren();
    void testSerialPortChildren();
};
#endif /* _TESTTWOPORTSWIILDREN_ */
