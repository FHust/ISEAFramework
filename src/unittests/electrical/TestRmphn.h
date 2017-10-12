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
* File Name : TestRmphn.h
* Creation Date : 29-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTRMN_
#define _TESTRMN_
#include <cxxtest/TestSuite.h>

/// DESCRIPTION
class TestRmphn : public CxxTest::TestSuite
{
    public:
    void testRmphnCreation();
    void testRmphnRCDifference();
    void testRmphnFactoryCreation();
    void testRmphnFactoryCreationSimplification();
};

#endif /* _TESTRMN_ */
