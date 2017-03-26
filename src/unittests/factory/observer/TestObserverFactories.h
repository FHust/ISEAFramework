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
* File Name : TestObserverFactories.h
* Creation Date : 10-08-2015
* Last Modified : Di 11 Aug 2015 14:34:31 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTOBSERVERFACTORIES_
#define _TESTOBSERVERFACTORIES_
/// DESCRIPTION
#include <cxxtest/TestSuite.h>

class TestObserverFactories : public CxxTest::TestSuite
{
    public:
    void TestObserverFactoryCSVCreation();
    void TestObserverFactorySTDCreation();
    void TestObserverFactoryDecimationFilterCreation();
    void TestObserverFactoryMatlabFilterCreation();
};
#endif /* _TESTOBSERVERFACTORIES_ */
