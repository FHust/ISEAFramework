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
* File Name : TestXML.h
* Creation Date : 11-11-2012
* Last Modified : Di 16 Feb 2016 14:48:53 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTELECTRICALSTUFF_
#define _TESTELECTRICALSTUFF_
#include <cxxtest/TestSuite.h>

namespace electrical
{
template < typename T >
class TwoPort;
}

class TestXML : public CxxTest::TestSuite
{
    public:
    void TestXmlGetAttributeDoubleGetUnrefrencedChild();
    void testXMLCacheRefInstance();
    void testXMLTestConfigfile();
    void testXMLReferencedCapacitance();
    void testXMLParallelPortChildren();
    void testXMLElectricalElementOhmicConst();
    void testXMLElectricalElementOhmic1DLookup();
    void testXMLParallelRCTestSimplificationFastRCToOhmicResistance();
    void testXMLWarburgTestReductionOfOhmicResistance();
    void testXMLWarburgTestCreationWithFullRCImplementation();
    void testXMLWarburgTestWrongSampleRate();
    void testXMLWarburgCothTestCreationWithFullRCImplementation();
    void testXMLWarburgCothTestCreationWithFullRCImplementationAndLookup1D();
    void testXMLWarburgCothTestCreationWithFullRCImplementationAndLookup2D();
    void testXMLWarburgCotanhTestReductionOfOhmicResistance();
    void testXMLWarburgCotanhTestWrongSampleRate();
    void testXMLWarburgCotanhTestSimplificationOfRCImpedance();
    void testXMLWarburgCotanhTestWithAndWithoutCapacity();
    void testXMLWarburgTanhTestSimplificationOfRCImpedance();
    void testXMLElectricalElementOhmic2DLookup();
    void testXMLTemperatureOperation();
    void testXMLSocOperations();
    void testXMLZarcOperations();
    void testXMLCellelementOperations();
    void testNegativeSampleRateForZarc();

    private:
    template < typename T >
    bool CheckTwoPortTypeAndValue( electrical::TwoPort< T > *testedTwoPort, const char *expectedType, double expectedValue );
};
#endif /* _TESTELECTRICALSTUFF_ */
