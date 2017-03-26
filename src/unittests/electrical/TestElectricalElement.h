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
* File Name : TestElectricalElement.h
* Creation Date : 01-11-2012
* Last Modified : Fr 21 Feb 2014 10:29:44 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTELECTRICALELEMENT_
#define _TESTELECTRICALELEMENT_
#include <cxxtest/TestSuite.h>

class TestElectricalElement : public CxxTest::TestSuite
{
    public:
    void testElectricalElementOhmicConst();
    void testElectricalElementOhmic1DLookup();
    void testElectricalElementVoltageSourceConst();
    void testElectricalElementOhmic2DLookup();
    void testElectricalElementVoltageSource1DLookup();
    void testElectricalElementVoltageSource2DLookup();
    void testElectricalParllelRCCreation();
    void testElectricalElementOhmPower();
    void testElectricalElementCapPower();
    void testElectricalElementParallelRCPower();
    void testCellelement();
};
#endif /* _TESTELECTRICALELEMENT_ */
