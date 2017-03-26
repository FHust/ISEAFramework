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
* File Name : TestZarc.h
* Creation Date : 11-11-2012
* Last Modified : Fr 21 Feb 2014 10:45:57 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTZARC_
#define _TESTZARC_
#include <cxxtest/TestSuite.h>

class TestZarc : public CxxTest::TestSuite
{
    public:
    void testZarcCreation();
    void testZarcOperations();
};
#endif /* _TESTZARC_ */
