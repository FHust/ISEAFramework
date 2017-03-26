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
* File Name : TestMatioWriteTree.h
* Creation Date : 03-08-2015
* Last Modified : Do 13 Aug 2015 10:07:05 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMATIOWRITETREE_
#define _TESTMATIOWRITETREE_
#include <cxxtest/TestSuite.h>

/// DESCRIPTION
class TestMatioWriteTree : public CxxTest::TestSuite
{
    public:
    void TestMatioData();
    void TestTree();
    void TestTreeOtherOrder();
};
#endif /* _TESTMATIOWRITETREE_ */
