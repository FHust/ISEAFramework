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
* File Name : TestSpiceExport.h
* Creation Date : 14-01-2014
* Last Modified : Fr 21 Feb 2014 10:50:07 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSPICEEXPORT_
#define _TESTSPICEEXPORT_
#include <cxxtest/TestSuite.h>


/// This class is testing the creation of a Spice Network
class TestSpiceExport : public CxxTest::TestSuite
{
    public:
    void testStreamConstructor();
};


#endif /* _TESTSPICEEXPORT_ */
