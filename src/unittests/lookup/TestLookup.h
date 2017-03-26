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
* File Name : TestLookup.h
* Creation Date : 05-05-2012
* Last Modified : Fr 21 Feb 2014 11:28:37 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTLOOKUP_
#define _TESTLOOKUP_
#include <cxxtest/TestSuite.h>

class TestLookup : public CxxTest::TestSuite
{
    public:
    void testLookup();
    void testLookupTestMinMax();
    void testLookupNonAcendingOrder();
    void testLookupInterpolate();
    void testSplineInterpolation1D_testAuxiliaryVariables();
    void testSplineInterpolation1D_testCoefficients();
    void testSplineInterpolation1D_testResults();
    void test2DLookup();
    void test2DLookupInterpolate();
    void test2DLookupInterpolateCorner1();
    void test2DLookupInterpolateCorner2();
    void test2DLookupInterpolateCorner3();
    void test2DLookupInterpolateCorner4();
    void test2DLookupInterpolate1d();
    void test2DLookupInterpolatePreCalc();
    void test2DLookupInterpolate3Point();
    void test2DLookupTestMinMax();
    void test2DLookupCreateExtendedData();
    void test1DLookupConstantUntilThreshold();
    void test2DLookupConstantUntilThreshold();
    void test2DCubicInterpolationTestBeta();
    void test2DCubicInterpolationTestAlpha();
    void test2DCubicInterpolationTestLookup();
};

#endif /* _TESTLOOKUP_ */
