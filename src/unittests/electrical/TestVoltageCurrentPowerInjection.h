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
* File Name : TestVoltageCurrentPowerInjection.h
* Creation Date : 07-03-2016
* Last Modified : Mo 07 Mär 2016 18:28:55 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTVOLTAGECURRENTPOWERINJECTION_
#define _TESTVOLTAGECURRENTPOWERINJECTION_

#include <cxxtest/TestSuite.h>

class TestVoltageCurrentPowerInjection : public CxxTest::TestSuite
{
    public:
    void TestTwoArgumentsTwoPorts();
    void TestThreeArgumentsTwoPorts();
    void TestFourArgumentsTwoPorts();
    void TestFiveArgumentsTwoPorts();

    void TestEveryhtingElse();

    template < typename MatrixT, template < typename > class TConcrete >
    void TwoArgs();

    template < typename MatrixT, template < typename > class TConcrete >
    void ThreeArgs();

    template < typename MatrixT, template < typename > class TConcrete >
    void FourArgs();

    template < typename MatrixT, template < typename > class TConcrete >
    void FiveArgs();
};
#endif /* _TESTVOLTAGECURRENTPOWERINJECTION_ */
