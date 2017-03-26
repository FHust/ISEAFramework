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
* File Name : TestView.h
* Creation Date : 09-04-2015
* Last Modified : Mo 06 Jul 2015 11:10:43 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTVIEW_
#define _TESTVIEW_

#include <cxxtest/TestSuite.h>

/// Testsuite for the View class
class TestView : public CxxTest::TestSuite
{
    public:
    void TestViewCreation();
    void TestViewCreationFromArray();
    void TestFill();
    void TestViewView();
    void TestCorruptView();
};
#endif /* _TESTVIEW_ */
