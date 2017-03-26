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
* File Name : TestDaeSystem.h
* Creation Date : 03-11-2012
* Last Modified : Mi 13 Aug 2014 09:55:57 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTDAESYSTEM_
#define _TESTDAESYSTEM_
#include <cxxtest/TestSuite.h>
#include <vector>

class TestDaeSystem : public CxxTest::TestSuite
{
    public:
    void testDaeSystemMixedSystemCurrentUnsolveable();
    void testDaeSystemMixedSystemCurrentSolveable();
    void testDaeSystemMixedSystemCurrentSolveableIntegrate();
    void testRC();
    void testRCParallel();
    void testRCSerial();
    void testParallelRCElement();
    void testConstantZarcSynthetic();
    void testRealZarc();
    void testSingleCellellement();
    void testSingleCellellementBalancing();
    void testMultiCellellementBalancing();
    void testVariableStepSolverWithParallelRCMindingResults();

    private:
    std::vector< std::vector< double > > CopyToVector( const double data[7][4] );
};
#endif /* _TESTDAESYSTEM_ */
