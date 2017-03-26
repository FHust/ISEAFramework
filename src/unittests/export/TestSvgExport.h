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
#ifndef _TESTSVGEXPORT_
#define _TESTSVGEXPORT_

#include <cxxtest/TestSuite.h>
#include <string>
#include <boost/shared_ptr.hpp>

#include "../../electrical/twoport_withchild.h"

class TestSvgExport : public CxxTest::TestSuite
{
    public:
    TestSvgExport();

    void testSvgExportFailTwoPort();
    void testSvgExportFailStream();
    void testSvgExportStopEvaluateDepth();
    void testSvgExportGetHeightOfTwoPort();
    void testPreamble();
    void testBody();
    void testEnding();

    private:
    std::stringstream mDummyOutstream;

    boost::shared_ptr< electrical::TwoPortWithChild< myMatrixType > > mTestDummy;
};

#endif /* _TESTSVGEXPORT_ */
